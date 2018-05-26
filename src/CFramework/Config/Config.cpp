/*
 * Copyright (C) 2008-2016 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <string>
#include <algorithm>
#include <mutex>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "Config.h"
#include "Errors.h"
#include "Log.h"

#include "Wrappers/Registry.h"

using namespace boost::property_tree;

#ifdef WIN32
System::Wrappers::CRegistry mRegistryMgr(HKEY_CURRENT_USER, "Software\\WMDL\\RtConfig");
#endif
boost::property_tree::ptree _config;

bool ConfigMgr::LoadInitial(std::string const& file, std::vector<std::string> args,
                            std::string& error)
{
    std::lock_guard<std::mutex> lock(_configLock);

    _filename = file;
    _args = args;

    try
    {
        ptree fullTree;
        ini_parser::read_ini(file, fullTree);

        if (fullTree.empty())
        {
            error = "empty file (" + file + ")";
            return false;
        }

        // Since we're using only one section per config file, we skip the section and have direct property access
        _config = fullTree.begin()->second;
    }
    catch (ini_parser::ini_parser_error const& e)
    {
        if (e.line() == 0)
            error = e.message() + " (" + e.filename() + ")";
        else
            error = e.message() + " (" + e.filename() + ":" + std::to_string(e.line()) + ")";
        return false;
    }

    return true;
}

ConfigMgr& ConfigMgr::Instance()
{
    static ConfigMgr m_instance;
    return m_instance;
}

bool ConfigMgr::Reload(std::string& error)
{
    return LoadInitial(_filename, std::move(_args), error);
}

bool ConfigMgr::Reload()
{
    std::string configError;
    if (!Reload(configError))
    {
        printf("!!!! CONFIG FILE ERROR !!!! %s\r\n", configError.c_str());
        return false;
    }
    return true;
}

template<class T>
T ConfigMgr::GetValueDefault(std::string const& name, T def) const
{
    try
    {
        return _config.get<T>(ptree::path_type(name, '/'));
    }
    catch (boost::property_tree::ptree_bad_path)
    {
        System::Console()->WriteErrorLine("server.loading", "Missing name %s in config file %s, add \"%s = %s\" to this file",
            name.c_str(), _filename.c_str(), name.c_str(), std::to_string(def).c_str());
    }
    catch (boost::property_tree::ptree_bad_data)
    {
        System::Console()->WriteErrorLine("server.loading", "Bad value defined for name %s in config file %s, going to use %s instead",
            name.c_str(), _filename.c_str(), std::to_string(def).c_str());
    }

    return def;
}

template<>
std::string ConfigMgr::GetValueDefault<std::string>(std::string const& name, std::string def) const
{
    try
    {
        return _config.get<std::string>(ptree::path_type(name, '/'));
    }
    catch (boost::property_tree::ptree_bad_path)
    {
        System::Console()->WriteErrorLine("server.loading", "Missing name %s in config file %s, add \"%s = %s\" to this file",
            name.c_str(), _filename.c_str(), name.c_str(), def.c_str());
    }
    catch (boost::property_tree::ptree_bad_data)
    {
        System::Console()->WriteErrorLine("server.loading", "Bad value defined for name %s in config file %s, going to use %s instead",
            name.c_str(), _filename.c_str(), def.c_str());
    }

    return def;
}

std::string ConfigMgr::GetStringDefault(std::string const& name, const std::string& def) const
{
    std::string val = GetValueDefault(name, def);
    val.erase(std::remove(val.begin(), val.end(), '"'), val.end());
    return GetDBValue(name.c_str(), val.c_str());
}

bool ConfigMgr::GetBoolDefault(std::string const& name, bool def) const
{
    std::string val = GetValueDefault(name, std::string(def ? "1" : "0"));
    val.erase(std::remove(val.begin(), val.end(), '"'), val.end());
    return GetDBValue(name.c_str(), (val == "1" || val == "true" || val == "TRUE" || val == "yes" || val == "YES"));
}

uint64 ConfigMgr::GetUInt64Default(const char * name, const uint64 def)
{
    return GetDBValue(name, GetValueDefault(name, def));
}

int ConfigMgr::GetIntDefault(std::string const& name, int def) const
{
    return GetDBValue(name.c_str(), GetValueDefault(name, def));
}

float ConfigMgr::GetFloatDefault(std::string const& name, float def) const
{
    return GetDBValue(name.c_str(), GetValueDefault(name, def));
}

void ConfigMgr::SetDBValue(const char * name, const char * value, bool db) 
{
#ifdef WIN32
    if (db)
        mRegistryMgr.WriteStringValue(value, name);
#endif
}

void ConfigMgr::SetDBValue(const char * name, int32 value, bool db)
{
#ifdef WIN32
    if (db)
        mRegistryMgr.WriteDoubleWordValue(value, name);
#endif
}

void ConfigMgr::SetDBValue(const char * name, uint64 value, bool db)
{
#ifdef WIN32
    if (db)
    {
        char buf[64];
        _i64toa(value, buf, 10);
        mRegistryMgr.WriteStringValue(buf, name);
    }
#endif
}

void ConfigMgr::SetDBValue(const char * name, float value, bool db)
{
#ifdef WIN32
    if (db)
    {
        std::ostringstream buff;
        buff << value;
        mRegistryMgr.WriteStringValue(buff.str().c_str(), name);
    }
#endif
}

void ConfigMgr::SetDBValue(const char * name, bool value, bool db)
{
#ifdef WIN32
    if (db)
    {
        mRegistryMgr.WriteDoubleWordValue(value, name);
    }
#endif
}


std::string ConfigMgr::GetDBValue(const char * name, const char * defvalue) const
{
#ifdef WIN32
    return GetRegistryStringValue(name, defvalue);
#else
    return defvalue;
#endif
}

uint32 ConfigMgr::GetDBValue(const char * name, int32 defvalue) const
{
#ifdef WIN32
    return GetRegistryIntValue(name, defvalue);
#else
    return defvalue;
#endif
}

uint64 ConfigMgr::GetDBValue(const char * name, uint64 defvalue) const
{
#ifdef WIN32
    std::string value = GetRegistryStringValue(name, "");
    if (!value.empty())
        defvalue = _atoi64(value.c_str());
    return defvalue;
#else
    return defvalue;
#endif
}

float ConfigMgr::GetDBValue(const char * name, float defvalue) const
{
#ifdef WIN32
    std::string value = GetRegistryStringValue(name, "");
    if (!value.empty())
        defvalue = std::stof(value.c_str());
    return defvalue;
#else
    return defvalue;
#endif
}

bool ConfigMgr::GetDBValue(const char * name, bool defvalue) const
{
#ifdef WIN32
    return GetRegistryIntValue(name, defvalue) != 0;
#else
    return defvalue;
#endif
}


uint ConfigMgr::GetRegistryIntValue(const char * rId, int defaultValue) const
{
#ifdef WIN32
    DWORD value = 0;
    DWORD _ret = mRegistryMgr.ReadDoubleWordValue(&value, rId);
    if (_ret == ERROR_SUCCESS)
        return value;
    return defaultValue;
#else
    return uint();
#endif
}

std::string ConfigMgr::GetRegistryStringValue(const char * rId, const char * defaultValue) const
{
#ifdef WIN32
    std::string value;
    DWORD _ret = mRegistryMgr.ReadStringValue(value, rId);
    if (_ret == ERROR_SUCCESS)
        return value;
    return defaultValue;
#else
    return "";
#endif
}

std::string const& ConfigMgr::GetFilename()
{
    std::lock_guard<std::mutex> lock(_configLock);
    return _filename;
}

std::list<std::string> ConfigMgr::GetKeysByString(std::string const& name)
{
    std::lock_guard<std::mutex> lock(_configLock);

    std::list<std::string> keys;

    for (const ptree::value_type& child : _config)
        if (child.first.compare(0, name.length(), name) == 0)
            keys.push_back(child.first);

    return keys;
}

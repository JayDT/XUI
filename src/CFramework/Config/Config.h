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

#ifndef CONFIG_H
#define CONFIG_H

#include "standard/Platform/Define.h"

#include <string>
#include <list>
#include <vector>
#include <mutex>

class TC_CFRAMEWORK_API ConfigMgr
{
    ConfigMgr() = default;
    ConfigMgr(ConfigMgr const&) = delete;
    ConfigMgr& operator=(ConfigMgr const&) = delete;
    ~ConfigMgr() = default;

public:
    /// Method used only for loading main configuration files (bnetserver.conf and worldserver.conf)
    bool LoadInitial(std::string const& file, std::vector<std::string> args,
        std::string& error);

    static ConfigMgr& Instance();

    bool Reload(std::string& error);
    bool Reload(); // Implement future

    std::string GetStringDefault(std::string const& name, const std::string& def) const;
    bool GetBoolDefault(std::string const& name, bool def) const;
    uint64 GetUInt64Default(const char *name, const uint64 def);
    int GetIntDefault(std::string const& name, int def) const;
    float GetFloatDefault(std::string const& name, float def) const;
    void SetDBValue(const char* name, const char* value, bool db = false);
    void SetDBValue(const char* name, int32 value, bool db = false);
    void SetDBValue(const char* name, uint64 value, bool db = false);
    void SetDBValue(const char* name, float value, bool db = false);
    void SetDBValue(const char* name, bool value, bool db = false);

    std::string GetDBValue(const char* name, const char* defvalue) const;
    uint32 GetDBValue(const char* name, int32 defvalue) const;
    uint64 GetDBValue(const char* name, uint64 defvalue) const;
    float GetDBValue(const char* name, float defvalue) const;
    bool GetDBValue(const char* name, bool defvalue) const;

    uint GetRegistryIntValue(const char* rId, int defaultValue /*= 0*/) const;
    std::string GetRegistryStringValue(const char* rId, const char* defaultValue /*= ""*/) const;

    std::string const& GetFilename();
    std::vector<std::string> const& GetArguments() const { return _args; }
    std::list<std::string> GetKeysByString(std::string const& name);

private:
    std::string _filename;
    std::vector<std::string> _args;
    std::mutex _configLock;

    template<class T>
    T GetValueDefault(std::string const& name, T def) const;
};

#define sConfig ConfigMgr::Instance()

#endif

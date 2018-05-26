/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2016 Austin Brunkhorst, All Rights Reserved.
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

#include "Precompiled.h"

#include "Type.h"

#include "ReflectionModule.h"
#include "ReflectionDatabase.h"

#include <sstream>

namespace System::Reflection
{
    inline namespace meta
    {
        extern Type _InvalidType;

        size_t ReflectionModule::sTypeCounter = 0;
        size_t ReflectionModule::sFieldCounter = 0;
        size_t ReflectionModule::sMethodCounter = 0;
        size_t ReflectionModule::sEnumCounter = 0;

        ReflectionModule::ReflectionModule(const char* modulName, uint64_t modulID)
            : _modulID(modulID)
            , assemblyName(modulName)
            , _db(nullptr)
        {
            _db = ReflectionModulManager::Instance().GetDatabase(modulID, *this);
        }

        void ReflectionModule::AddMetaData(const MetaManager::Initializer & meta)
        {
            MetaManager::Append(meta);
        }

        ///////////////////////////////////////////////////////////////////////

        std::vector<Global*> ReflectionModule::GetGlobals(void)
        {
            std::vector<Global*> globals;

            for (auto &global : Assembly().globals)
                globals.emplace_back(global.second);

            return globals;
        }

        ///////////////////////////////////////////////////////////////////////

        const Global &ReflectionModule::GetGlobal(const std::string &name)
        {
            return *_db->globals[name];
        }

        ///////////////////////////////////////////////////////////////////////

        std::vector<Function*> ReflectionModule::GetGlobalFunctions(void)
        {
            std::vector<Function*> functions;

            for (auto &overload : Assembly().globalFunctions)
            {
                for (auto &function : overload.second)
                {
                    functions.emplace_back( function );
                }
            }

            return functions;
        }

        ///////////////////////////////////////////////////////////////////////

        const Function &ReflectionModule::GetGlobalFunction(const std::string &name)
        {
            return _db->GetGlobalFunction(name);
        }

        ///////////////////////////////////////////////////////////////////////

        const Function &ReflectionModule::GetGlobalFunction(
            const std::string &name,
            const ArgHashStorage & signature
        )
        {
            return _db->GetGlobalFunction(name, signature);
        }

        ///////////////////////////////////////////////////////////////////////

        std::vector<Type const*> const& ReflectionModule::GetTypes(void)
        {
            return (std::vector<Type const*> const&)Assembly().types;
        }

        ///////////////////////////////////////////////////////////////////////

        Type const& ReflectionModule::GetFromName(const std::string &name)
        {
            auto search = Assembly().ids.find(name);

            if (search == Assembly().ids.end())
                return Type::Invalid();

            return *Assembly().types[search->second];
        }

        void ReflectionModule::Initialize()
        {
            _db->Initialize();
        }

        ///////////////////////////////////////////////////////////////////////

        /** @brief Gets the type that this array type holds.
        *  @return Type this array type holds.
        *          ie - Array<double> -> double
        *          Non array types return itself.
        */

        ReflectionModulManager::ReflectionModulManager()
        {
        }

        ReflectionModulManager::RefDatabase& ReflectionModulManager::GetDatabase(uint64_t modulID, ReflectionModule& modul)
        {
            RefDatabase& rdb = m_databases[modulID];
            if (!rdb)
                rdb = new ReflectionDatabase( modul );
            if (!default_db)
                default_db = rdb;
            return rdb;
        }

        ReflectionModule* ReflectionModulManager::GetModul(RefDatabase db)
        {
            if (db)
                return &db->GetReflectionModul();
            return nullptr;
        }

        ReflectionModulManager & ReflectionModulManager::Instance()
        {
            static ReflectionModulManager _instance;
            return _instance;
        }

        std::string ReflectionModulManager::GetStatistic()
        {
            std::ostringstream stat;
            stat << "Types: " << ReflectionModule::sTypeCounter << std::endl;
            stat << "Fields: " << ReflectionModule::sFieldCounter << std::endl;
            stat << "Methods: " << ReflectionModule::sMethodCounter << std::endl;
            stat << "Enums: " << ReflectionModule::sEnumCounter << std::endl;
            return stat.str();
        }

        ReflectionModulManager::AssemblyArray ReflectionModulManager::GetAssemblies()
        {
            ReflectionModulManager::AssemblyArray moduls;
            for (auto const& m : m_moduls)
                moduls.push_back(m.second);
            return moduls;
        }

        ReflectionModulManager::RefModul ReflectionModulManager::GetAssembly(std::string const & assembly_name)
        {
            static RefModul _empty;

            auto iModul = m_moduls.find(assembly_name);
            if (iModul != m_moduls.end())
                return iModul->second;
            return _empty;
        }

        Type const & ReflectionModulManager::GetType(TypeID typeId)
        {
            if (!m_databases.empty())
            {
                if (typeId == InvalidTypeID)
                {
                    return _InvalidType; //Type::Invalid();*m_databases.begin()->second->types[typeId];
                }

                auto IDB = m_databases.find(typeId.ModulID);
                if (IDB != m_databases.end())
                    return *IDB->second->types[typeId.ID];
            }
            return Type::Invalid();
        }

        Type const & ReflectionModulManager::GetType(std::type_info const & rhs)
        {
            size_t hashcode = rhs.hash_code();
            return GetType(hashcode);
        }

        Type const & ReflectionModulManager::GetType(std::size_t rhs)
        {
            for (auto const& db : m_databases)
            {
                TypeID tid = db.second->TranslateNativeTypeIdToManagedTypeID(rhs);
                if (tid != InvalidTypeID)
                {
                    return *db.second->types[tid];
                }
            }
            return Type::Invalid();
        }

        Type const & ReflectionModulManager::GetType(const std::string & fullname)
        {
            for (auto const& db : m_databases)
            {
                Type const& t = db.second->GetReflectionModul().GetType(fullname);
                if (t)
                {
                    return t;
                }
            }
            return Type::Invalid();
        }

        TypeID const & ReflectionModulManager::GetTypeID(std::type_info const & rhs)
        {
            size_t hashcode = rhs.hash_code();
            for (auto const& db : m_databases)
            {
                if (!db.second)
                    continue;

                TypeID const& tid = db.second->TranslateNativeTypeIdToManagedTypeID(hashcode);
                if (tid != InvalidTypeID)
                {
                    return tid;
                }
            }

            return InvalidTypeID;
        }

        TypeID const & ReflectionModulManager::GetTypeID(size_t nativetypeid)
        {
            for (auto const& db : m_databases)
            {
                if (!db.second)
                    continue;

                auto iTypeID = db.second->typeids.find(nativetypeid);
                if (iTypeID != db.second->typeids.end())
                {
                    return iTypeID->second;
                }
            }

            return InvalidTypeID;
        }

        ReflectionModulManager::RefDatabase ReflectionModulManager::GetDatabase(uint64_t modulID)
        {
            ReflectionDataBaseMap::iterator rdb = m_databases.find(modulID);
            if (rdb != m_databases.end())
                return rdb->second;
            return ReflectionModulManager::RefDatabase();
        }

        void ReflectionModulManager::InitializeReflectionModuls()
        {
            std::list<ReflectionModule*> moduls;
            while (!InitReflectionModule.empty())
            {
                moduls.push_back(InitReflectionModule.front());
                InitReflectionModule.pop();
            }

            for (auto const& modul : moduls)
            {
                modul->CreateTypes();
            }

            for (auto const& modul : moduls)
            {
                modul->Initialize();
            }
        }
    }
}

#define sAppDomain System::Reflection::meta::ReflectionModulManager::Instance()

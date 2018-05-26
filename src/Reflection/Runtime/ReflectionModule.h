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

#pragma once

#include "ReflectionDatabase.h"

#include "standard/Platform/Define.h"
#include "standard/misc.h"

#include <memory>
#include <queue>

#define DECLARE_REFLECTION_MODULE(module)                            \
    inline namespace meta_generated                                         \
    {                                                                \
        class Module##module : public System::Reflection::meta::ReflectionModule \
        {                                                            \
        public:                                                      \
             Module##module(System::Reflection::meta::ReflectionDatabase &db);   \
            ~Module##module(void);                                   \
        };                                                           \
    }                                                                \

#define UsingModule(module) meta_generated::Module##module _##module( db );

namespace System::Reflection
{
    inline namespace meta
    {
        class TC_METARUNTIME_API ReflectionModule : public MetaManager
        {
            friend class TC_METARUNTIME_API ReflectionModulManager;
        public:

            static size_t sTypeCounter;
            static size_t sFieldCounter;
            static size_t sMethodCounter;
            static size_t sEnumCounter;

            ReflectionModule(const char* modulName, uint64_t modulID);
            virtual ~ReflectionModule() {}

            void AddMetaData(const MetaManager::Initializer &meta);

            const char* AssemblyName() const { return assemblyName.c_str(); }
            ReflectionDatabase const& Assembly() const { return *_db; }
            uint64_t GetModulID() const { return _modulID; }

            ///////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////

            /** @brief Gets all registered global variables.
            */
            std::vector<Global*> GetGlobals(void);

            /** @brief Gets a global variable with the specified name.
            *  @param name Qualified global name.
            */
            const Global &GetGlobal(const std::string &name);

            /** @brief Gets all registered global functions.
            */
            std::vector<Function*> GetGlobalFunctions(void);

            /** @brief Gets a global function with the specified name, and
            *         first available overload.
            *  @param name Qualified global function name.
            */
            const Function &GetGlobalFunction(const std::string &name);

            /** @brief Gets a global function with the specified name, and
            *         overload signature.
            *  @param name Qualified global function name.
            *  @param signature Signature of the global function.
            */
            const Function &GetGlobalFunction(
                const std::string &name,
                const ArgHashStorage & signature
            );

            ///////////////////////////////////////////////////////////////////

            /** @brief Gets all types registered in the main
            *         reflection database.
            */
            std::vector<Type const*> const& GetTypes(void);

            /** @brief Gets a type based on the qualified string name.
            *  @param name Name of the type.
            */
            Type const& GetFromName(const std::string &name);
            Type const& GetType(const std::string &name)
            {
                return GetFromName(name);
            }

            virtual void Initialize();
            virtual void CreateTypes() = 0;
        protected:
            uint64_t _modulID;
            std::string assemblyName;
            ReflectionDatabase* _db;
        };

        ReflectionModule* __GetCurrent_RunTime__ReflectionModule();

        __forceinline uint64_t GetCurrentModule()
        {
            return System::GetCurrentModule();
        }

        class TC_METARUNTIME_API ReflectionModulManager
        {
        public:
            typedef ReflectionModule* RefModul;
            typedef ReflectionDatabase* RefDatabase;
        private:
            friend class TC_METARUNTIME_API ReflectionModule;

            typedef std::map<std::string, RefModul> ReflectionModuleMap;
            typedef std::map<uint64_t, RefDatabase> ReflectionDataBaseMap;

            ReflectionModuleMap m_moduls;
            ReflectionDataBaseMap m_databases;
            RefDatabase default_db;

            ReflectionModulManager();

            RefDatabase& GetDatabase(uint64_t modulID, ReflectionModule& modul);
            ReflectionModule* GetModul(RefDatabase db);

        public:
            typedef std::vector<RefModul> AssemblyArray;

            uint32 SignitureNextID = 0;

            template<typename MODUL>
            RefModul RegisterAssembly()
            {
                RefModul modul = new MODUL();
                if (m_moduls.find(modul->AssemblyName()) == m_moduls.end())
                    m_moduls[modul->AssemblyName()] = modul;
                else
                {
                    std::abort();
                }
                return modul;
            }

            static ReflectionModulManager& Instance();
            
            static std::string GetStatistic();

            __forceinline ReflectionModule* GetCurrentAssembly(uint64_t modulID = GetCurrentModule())
            {
                auto IDB = m_databases.find(modulID);
                if (IDB != m_databases.end())
                    return GetModul(IDB->second);
                return nullptr;
            }

            AssemblyArray GetAssemblies();
            RefModul GetAssembly(std::string const& assembly_name);

            Type const& GetType(TypeID typeId);
            Type const& GetType(std::type_info const& rhs);
            Type const& GetType(std::size_t rhs);
            Type const& GetType(const std::string &fullname);
            TypeID const& GetTypeID(std::type_info const& rhs);
            TypeID const& GetTypeID(size_t nativetypeid);

            RefDatabase const& GetDefaultDB() const { return default_db; }

            std::queue<ReflectionModule*> InitReflectionModule;

            ReflectionModulManager::RefDatabase GetDatabase(uint64_t modulID);

            void InitializeReflectionModuls();
        };

        //TODO: implement marshal
        inline TC_METARUNTIME_API Type const& ManagedTypeOf(TypeID typeID)
        {
            return System::Reflection::meta::ReflectionModulManager::Instance().GetType(typeID);
        }

        inline TC_METARUNTIME_API Type const& ManagedTypeOf(std::type_info const& rhs)
        {
            return System::Reflection::meta::ReflectionModulManager::Instance().GetType(rhs);
        }

        inline TC_METARUNTIME_API Type const& ManagedTypeOf(std::size_t rhs)
        {
            return System::Reflection::meta::ReflectionModulManager::Instance().GetType(rhs);
        }

        inline TC_METARUNTIME_API TypeID const& ManagedTypeID(std::type_info const& rhs)
        {
            return System::Reflection::meta::ReflectionModulManager::Instance().GetTypeID(rhs);
        }

        inline TC_METARUNTIME_API TypeID const& ManagedTypeID(size_t nativetypeid)
        {
            return System::Reflection::meta::ReflectionModulManager::Instance().GetTypeID(nativetypeid);
        }
    }
}
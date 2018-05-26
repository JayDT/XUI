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

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "standard/Platform/Define.h"

#include "TypeInfo.h"
#include "Global.h"
#include "ArgumentConfig.h"
#include "InvokableConfig.h"

#include "sparsehash/dense_hash_map"

#include "Type.h"

namespace System::Reflection
{
    inline namespace meta
    {
        class Type;
        class Function;
        struct MethodDesc;
        struct FieldDesc;

        class TC_METARUNTIME_API ReflectionModule;
        //class TC_METARUNTIME_API ReflectionModulManager;

        class TC_METARUNTIME_API ReflectionDatabase
        {
        public:
            google::dense_hash_map<size_t /*std::type_info const* hash_code */, TypeID> typeids;
            google::dense_hash_map<std::string, TypeID> ids;
            google::dense_hash_map<std::string, Global*> globals;
            google::dense_hash_map<std::string, std::vector<Function*>> globalFunctions;
            std::vector<Type*> types;

            ReflectionDatabase(ReflectionModule& modul);
            ~ReflectionDatabase(void);


            ////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////

            void Initialize();

            ////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////

            TypeID AllocateType(const char* name, std::type_info const& native_type_info);

            ////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////

            TypeID const& TranslateNativeTypeIdToManagedTypeID(size_t rhs) const;

            // Function Getter, Function Setter
            template<typename GlobalType, typename GetterType, typename SetterType>
            void AddGlobal(
                const std::string &name,
                GetterType getter,
                SetterType setter,
                const MetaManager::Initializer &meta
            );

            // Function Getter, Raw Setter
            template<typename GlobalType, typename GetterType>
            void AddGlobal(
                const std::string &name,
                GetterType getter,
                GlobalType *globalSetter,
                const MetaManager::Initializer &meta
            );

            // Raw Getter, Function Setter
            template<typename GlobalType, typename SetterType>
            void AddGlobal(
                const std::string &name,
                GlobalType *globalGetter,
                SetterType setter,
                const MetaManager::Initializer &meta
            );

            // Raw Getter, Raw Setter
            template<typename GlobalType>
            void AddGlobal(
                const std::string &name,
                GlobalType *globalGetter,
                GlobalType *globalSetter,
                const MetaManager::Initializer &meta
            );

            ////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////

            void AddGlobalFunction(
                const char* name,
                Variant invoker_impl(size_t methodId, Variant &&obj, const ArgumentList &&arguments),
                MethodDesc const& methodinfo_impl(size_t methodId),
                size_t methodid,
                const MetaManager::Initializer &meta,
                CXVisibilityKind visibility
            );

            const Function &GetGlobalFunction(const std::string &name);

            const Function &GetGlobalFunction(
                const std::string &name, 
                const ArgHashStorage & signature
            );

            ReflectionModule& GetReflectionModul() const { return m_reflectionModul; }
        private:
            TypeID m_nextID;
            ReflectionModule& m_reflectionModul;
            bool Initialized;
        };
    }
}

#include "Impl/ReflectionDatabase.hpp"
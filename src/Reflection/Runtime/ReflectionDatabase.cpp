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
#include "standard/Errors.h"

#include "ReflectionModule.h"
#include "ReflectionDatabase.h"

#include "Variant.h"
#include "Type.h"
#include "InvokableConfig.h"

#include "Function.h"
#include "Method.h"
#include "Constructor.h"
#include "Destructor.h"

#include <typeinfo>

//size_t methodId = 65535; // max object per library 65535

namespace System::Reflection
{
    inline namespace meta
    {
        extern Type _InvalidType(nullptr, InvalidTypeID, "UNKNOWN");

        extern std::map<intptr_t, FieldSetterBase*> singletonFieldSetter;
        extern std::map<intptr_t, FieldGetterBase*> singletonFieldGetter;
        extern std::map<intptr_t, FunctionInvokerBase*> singletonFunctionInvoker;
        extern std::map<intptr_t, MethodInvokerBase*> singletonMethodInvoker;

        ReflectionDatabase::ReflectionDatabase(ReflectionModule& modul)
            : types( 1 )
            , m_nextID( 1, modul.GetModulID() )
            , m_reflectionModul(modul)
            , Initialized(false)
        {
            types.reserve(5000);
            ids.set_empty_key(std::string());
            typeids.set_empty_key(0);
            globals.set_empty_key(std::string());
            globalFunctions.set_empty_key(std::string());

            //m_nextID.ModulID = modul.GetModulID();

            EXCEPT(types[InvalidTypeID] == nullptr);
            types[InvalidTypeID] = &_InvalidType; // new Type(this, InvalidTypeID, "UNKNOWN");
            types[InvalidTypeID]->name = "UNKNOWN";
            ids["UNKNOWN"] = InvalidTypeID;
        }

        ///////////////////////////////////////////////////////////////////////

        ReflectionDatabase::~ReflectionDatabase(void) { }

        ///////////////////////////////////////////////////////////////////////

        void ReflectionDatabase::Initialize()
        {
            if (Initialized)
                return;

            Initialized = true;

            static bool BaseTypeInitialized = false;

            if (!BaseTypeInitialized)
                BaseTypeInitialized = true;
        }

        ///////////////////////////////////////////////////////////////////////

        TypeID ReflectionDatabase::AllocateType(const char* name, std::type_info const& native_type_info)
        {
            TypeID _typeId = ReflectionModulManager::Instance().GetTypeID(native_type_info.hash_code());
            if (_typeId != InvalidTypeID)
                return InvalidTypeID; // already Defined

            auto search = ids.find( name );

            ASSERT(search == ids.end() && "Invalid type allocation");

            TypeID id = m_nextID;
            ++m_nextID.ID;

            ids[ name ] = id;
            typeids[ native_type_info.hash_code() ] = id;

            //types.reserve(types.size() + 1);
            types.emplace_back( new Type(this, id, name) );

            return id;
        }

        inline TypeID const& ReflectionDatabase::TranslateNativeTypeIdToManagedTypeID(size_t rhs) const
        {
            auto iNativeIndex = typeids.find(rhs);
            if (iNativeIndex != typeids.end())
                return iNativeIndex->second;

            return InvalidTypeID;
        }

        ///////////////////////////////////////////////////////////////////////

        void ReflectionDatabase::AddGlobalFunction(
            const char* name,
            Variant invoker_impl(size_t methodId, Variant &&obj, const ArgumentList &&arguments), 
            MethodDesc const & methodinfo_impl(size_t methodId), 
            size_t methodid, 
            const MetaManager::Initializer & meta,
            CXVisibilityKind visibility)
        {
            MethodDesc const& mInfo = methodinfo_impl(methodid);
            if (mInfo.m_classType->GetID() == InvalidTypeID)
                return;

            FunctionInvokerBase*& invoker = singletonFunctionInvoker[reinterpret_cast<intptr_t>(invoker_impl)];
            if (!invoker)
            {
                invoker = new FunctionWrapper;
                FunctionWrapper* methodWrapper = (FunctionWrapper*)invoker;
                methodWrapper->invoker = invoker_impl;
                methodWrapper->methodinfo = methodinfo_impl;
            }

            Function* meth = new Function(name, *mInfo.m_classType, invoker);
            meth->m_UID = methodid;
            meth->m_metaData = meta;

            globalFunctions[name].push_back(meth);
        }

        const Function &ReflectionDatabase::GetGlobalFunction(
            const std::string &name
        )
        {
            auto base = globalFunctions.find(name);
            if (base == globalFunctions.end())
                return Function::Invalid();

            return **base->second.begin( );
        }

        ///////////////////////////////////////////////////////////////////////

        const Function &ReflectionDatabase::GetGlobalFunction(
            const std::string &name, 
            const ArgHashStorage & signature
        )
        {
            auto base = globalFunctions.find(name);
            if (base == globalFunctions.end())
                return Function::Invalid();

            for (Function* m : base->second)
                if (m->CanCall(signature))
                    return *m;

            return Function::Invalid();
        }
    }
}

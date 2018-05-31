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
#include "Method.h"
#include "ReflectionModule.h"

#include "Common/Logging.h"

namespace System::Reflection
{
    inline namespace meta
    {
        Method::Method(void)
            : NamedMethod( )
            , m_isConst( true )
            , m_classType( Type::Invalid( ) )
            , m_invoker( nullptr )
            , AccessLevel(CXXAccessSpecifier::Public)
        {
            ++ReflectionModule::sMethodCounter;
        }

        Method::Method(
            const char* name,
            Type const& returnType,
            MethodInvokerBase* invoker,
            bool isConst, 
            CXXAccessSpecifier accessLevel)
            : NamedMethod(name)
            , m_isConst(isConst)
            , m_classType(returnType)
            , m_invoker(invoker)
            , AccessLevel(accessLevel)
        {
            ++ReflectionModule::sMethodCounter;
        }

        Method::~Method()
        {
            --ReflectionModule::sMethodCounter;
        }

        const Method &Method::Invalid(void)
        {
            static Method invalid;

            return invalid;
        }

        Type const& Method::DeclaringType(void) const
        {
            return m_classType;
        }

        bool Method::CanCall(const ArgHashStorage & signature) const
        {
            auto const& mInfo = m_invoker->GetMethodInfo(m_UID);
            if (signature.empty() && mInfo.m_argHashStorage.empty())
                return true;

            if (signature.size() != mInfo.m_argHashStorage.size())
                return false;

            for (size_t i = 0; i != mInfo.m_argHashStorage.size(); ++i)
                if (signature[i] != mInfo.m_argHashStorage[i] && typeof(System::Reflection::Variant).native_hash_code != mInfo.m_argHashStorage[i])
                    return false;

            return true;
        }

        bool Method::CanCall(const InvokableSignature & signature) const
        {
            auto const& mInfo = m_invoker->GetMethodInfo(m_UID);
            if (signature.empty() && mInfo.m_argHashStorage.empty())
                return true;

            if (signature.size() != mInfo.m_argHashStorage.size())
                return false;

            for (size_t i = 0; i != mInfo.GetInvokableSignature().size(); ++i)
                if (!mInfo.GetInvokableSignature()[i]->IsAssignableFrom(*signature[i]))
                    return false;

            return true;
        }

        bool Method::IsValid(void) const
        {
            return m_invoker != nullptr;
        }

        bool Method::IsConst(void) const
        {
            return m_isConst;
        }

        Variant Method::InternalInvoke(
            Variant &instance,
            ArgumentList &arguments
        ) const
        {
        #if defined(_DEBUG)

            UAssert( IsValid( ), 
                "Invalid method invoked." 
            );

            UAssert( !(instance.IsConst( ) && !m_isConst), 
                "Non-const method invoked on const object." 
            );

            UAssert(m_classType.IsAssignableFrom(*instance.GetType( )),
                "Incompatible method invoked with instance." 
            );

        #endif

            return m_invoker->Invoke(m_UID, instance, arguments);
        }
    }
}

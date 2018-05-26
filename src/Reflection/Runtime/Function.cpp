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
#include "Function.h"
#include "ReflectionModule.h"

#include "Common/Logging.h"

namespace System::Reflection
{
    inline namespace meta
    {
        Function::Function(void)
            : NamedMethod( )
            , m_parentType( Type::Invalid( ) )
            , m_invoker( nullptr )
            , AccessLevel(CXXAccessSpecifier::Public)
        {
            ++ReflectionModule::sMethodCounter;
        }

        Function::Function(const char* name, Type const & parentType, FunctionInvokerBase* invoker, CXXAccessSpecifier accessLevel)
            : NamedMethod(name)
            , m_parentType(parentType)
            , m_invoker(invoker)
            , AccessLevel(accessLevel)
        {
            ++ReflectionModule::sMethodCounter;
        }

        Function::~Function()
        {
            --ReflectionModule::sMethodCounter;
        }

        const Function &Function::Invalid(void)
        {
            static Function invalid;

            return invalid;
        }

        Type const& Function::DeclaringType(void) const
        {
            return m_parentType;
        }

        bool Function::CanCall(const ArgHashStorage & signature) const
        {
            auto const& mInfo = m_invoker->GetMethodInfo(m_UID);
            if (signature.empty() && mInfo.m_argHashStorage.empty())
                return true;

            if (signature.size() != mInfo.m_argHashStorage.size())
                return false;

            for (size_t i = 0; i != mInfo.m_argHashStorage.size(); ++i)
                if (signature[i] != mInfo.m_argHashStorage[i])
                    return false;

            return true;
        }

        bool Function::IsValid(void) const
        {
            return m_invoker != nullptr;
        }

        Variant Function::InvokeVariadic(ArgumentList &arguments) const
        {
        #if defined(_DEBUG)

            UAssert( IsValid( ), "Invalid function invocation." );

        #endif
        
            return m_invoker->Invoke(m_UID, arguments );
        }

        Variant Function::InternalInvoke(Variant & /*instance*/, ArgumentList & arguments) const
        {
            return InvokeVariadic(arguments);
        }

        bool Function::IsStatic() const
        {
            return true; // m_parentType.IsValid();
        }
    }
}

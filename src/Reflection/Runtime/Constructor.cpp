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
#include "Constructor.h"
#include "Invokable.h"

#include "Common/Logging.h"

#include "ReflectionModule.h"

namespace System::Reflection
{
    inline namespace meta
    {
        namespace
        {
            const auto kConstructorName = "constructor";
        }

        Constructor::Constructor(void)
            : NamedMethod( )
            , m_isDynamic( false )
            , m_classType( ReflectionModulManager::Instance().GetType(InvalidTypeID) )
            , m_invoker( nullptr ) 
            , AccessLevel(CXXAccessSpecifier::Public)
        {
            ++ReflectionModule::sMethodCounter;
        }

        Constructor::Constructor(const Constructor &rhs)
            : NamedMethod( kConstructorName )
            , m_isDynamic( rhs.m_isDynamic )
            , m_classType( rhs.m_classType )
            , m_invoker( rhs.m_invoker )
            , AccessLevel(CXXAccessSpecifier::Public)
        {
            ++ReflectionModule::sMethodCounter;
        }

        Constructor::Constructor(const Constructor &&rhs)
            : NamedMethod( kConstructorName )
            , m_isDynamic( rhs.m_isDynamic )
            , m_classType( rhs.m_classType )
            , m_invoker( std::move( rhs.m_invoker ) )
            , AccessLevel(CXXAccessSpecifier::Public)
        {
            ++ReflectionModule::sMethodCounter;
        }

        Constructor::Constructor(
            Type const& classType,
            FunctionInvokerBase* invoker,
            bool isDynamic,
            CXXAccessSpecifier accessLevel
        )
            : NamedMethod( kConstructorName )
            , m_isDynamic( isDynamic )
            , m_classType( classType )
            , m_invoker( invoker )
            , AccessLevel(accessLevel)
        {
            ++ReflectionModule::sMethodCounter;
        }

        Constructor::~Constructor()
        {
            --ReflectionModule::sMethodCounter;
        }

        //Constructor &Constructor::operator=(const Constructor &&rhs)
        //{
        //    m_isDynamic = rhs.m_isDynamic;
        //    m_classType= rhs.m_classType;
        //    m_invoker = std::move( rhs.m_invoker );
        //
        //    m_signature = std::move( rhs.m_signature );
        //
        //    return *this;
        //}

        const Constructor &Constructor::Invalid(void)
        {
            static Constructor invalid;

            return invalid;
        }

        Type const& Constructor::DeclaringType(void) const
        {
            return m_classType;
        }

        bool Constructor::IsValid(void) const
        {
            return m_invoker != nullptr;
        }

        bool Constructor::IsDynamic(void) const
        {
            return m_isDynamic;
        }

        Variant Constructor::InvokeVariadic(ArgumentList &arguments) const
        {
            UAssert( IsValid( ), "Invalid constructor invoked." );

            return m_invoker->Invoke(m_UID, arguments );
        }

        bool Constructor::CanCall(const ArgHashStorage & signature) const
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

	    bool Constructor::CanCall(const InvokableSignature& signature) const
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

	    MethodDesc const & Constructor::GetMethodInfo() const
        {
            return m_invoker->GetMethodInfo(m_UID);
        }

        Variant Constructor::InternalInvoke(Variant & /*instance*/, ArgumentList & arguments) const
        {
            return InvokeVariadic(arguments);
        }

        ArgHashStorage const& Constructor::GetParameters() const
        {
            return GetMethodInfo().m_argHashStorage;
        }
    }
}

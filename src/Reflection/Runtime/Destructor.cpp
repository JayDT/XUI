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
#include "Destructor.h"

#include "Variant.h"
#include "ReflectionModule.h"

#include "Common/Logging.h"

namespace System::Reflection
{
    inline namespace meta
    {
        namespace
        {
            const auto kDestructorName = "destructor";
        }

        Destructor::Destructor(void)
            : NamedMethod( )
            , m_classType( Type::Invalid( ) ) 
        {
            ++ReflectionModule::sMethodCounter;
        }

        Destructor::Destructor(Type const& classType, MethodInvokerBase* invoker)
            : NamedMethod( kDestructorName )
            , m_classType( classType )
            , m_invoker( invoker ) 
        {
            ++ReflectionModule::sMethodCounter;
        }

        Destructor::~Destructor()
        {
            --ReflectionModule::sMethodCounter;
        }

        Type const& Destructor::DeclaringType(void) const
        {
            return m_classType;
        }

        void Destructor::Invoke(Variant &instance) const
        {
            UAssert( IsValid( ), 
                "Invalid constructor invoked." 
            );

            UAssert( m_classType == *instance.GetType( ), 
                "Destructor called on incompatible type." 
            );
        
            m_invoker->Invoke(m_UID, instance, ArgumentList() );

            delete instance.m_base;

            instance.m_base = nullptr;
        }

        CXXAccessSpecifier Destructor::GetAccessLevel() const
        {
            return CXXAccessSpecifier::Public; // @ToDo: implement this
        }

        bool Destructor::CanCall(const ArgHashStorage & signature) const
        {
            return signature.empty();
        }

        MethodDesc const & Destructor::GetMethodInfo() const
        {
            return m_invoker->GetMethodInfo(m_UID);
        }

        Variant Destructor::InternalInvoke(Variant & instance, ArgumentList & arguments) const
        {
            Invoke(instance);
            return Variant::Empty;
        }

        const Destructor &Destructor::Invalid(void)
        {
            static Destructor invalid;

            return invalid;
        }

        bool Destructor::IsValid(void) const
        {
            return m_invoker != nullptr;
        }
    }
}
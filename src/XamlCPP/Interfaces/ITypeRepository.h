/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2016 OmniGUI Platform
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


//File: ITypeRepository.h

#pragma once
#include <System/System.h>
#include "Reflection/Runtime/Type.h"
#include "Reflection/Runtime/Field.h"
#include "Reflection/Runtime/Method.h"

namespace XamlCPP::Core
{
    class TypeConverterRegistration;

    namespace Proto
    {
        struct PrefixRegistration;
        class XamlNamespace;
        class XamlType;
        class XamlTypeName;
    }

    namespace ReflEx
    {
        class Member;
        class AttachableMember;
    }
}

namespace XamlCPP::Interfaces
{
    struct INamespaceRegistry;

    struct ITypeRepository
    {
        virtual ~ITypeRepository() {}
        virtual XamlCPP::Core::Proto::XamlType* GetByType(System::Reflection::Type const* type) = 0;
        virtual XamlCPP::Core::Proto::XamlType* GetByQualifiedName(std::string const& qualifiedName) = 0;
        virtual XamlCPP::Core::Proto::XamlType* GetByPrefix(std::string const& prefix, std::string const& typeName) = 0;
        virtual XamlCPP::Core::Proto::XamlType* GetByFullAddress(XamlCPP::Core::Proto::XamlTypeName* xamlTypeName) = 0;
        virtual XamlCPP::Core::ReflEx::Member* GetMember(System::Reflection::meta::Field const* propertyInfo) = 0;
        virtual XamlCPP::Core::ReflEx::AttachableMember* GetAttachableMember(std::string const& name, const System::Reflection::IMethod* getter, const System::Reflection::IMethod* setter) = 0;
        virtual void AddTypeConverter(Core::TypeConverterRegistration const& typeConverterRegistration) = 0;
        virtual Interfaces::INamespaceRegistry * GetNamespaceScope() const = 0;
    };
}

//End of file


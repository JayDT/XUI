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

#pragma once
#include <System/System.h>
#include "Reflection/Runtime/Type.h"
#include "Reflection/Runtime/Field.h"
#include "Interfaces/ITypeFactory.h"
#include "Interfaces/ITypeRepository.h"
#include "Interfaces/ITypeFeatureProvider.h"
#include "Interfaces/INamedScope.h"

namespace XamlCPP::Core
{
    namespace Proto
    {
        struct PrefixRegistration;
        class XamlNamespace;
        class XamlTypeName;
    }

    namespace ReflEx
    {
        struct MemberBase;
        class Member;
        class AttachableMember;
    }
}

namespace XamlCPP::Core::Proto
{
    class XamlType
    {
        Interfaces::ITypeRepository* typeRepository = nullptr;
        Interfaces::ITypeFactory* typeFactory = nullptr;
        Interfaces::ITypeFeatureProvider* featureProvider = nullptr;
        System::Reflection::Type const* underlyingType = nullptr;
        std::string name;

    public:
        bool IsRefPointer = false;

        XamlType(System::Reflection::Type const* type, Interfaces::ITypeRepository* typeRepository, Interfaces::ITypeFactory* typeTypeFactory, Interfaces::ITypeFeatureProvider* featureProvider);
        XamlType(System::Reflection::Type const* type);
        virtual ~XamlType()
        {
        }

        bool operator==(XamlType const& rhs) const
        {
            return underlyingType == rhs.underlyingType && IsRefPointer == rhs.IsRefPointer;
        }

        bool operator!=(XamlType const& rhs) const
        {
            return underlyingType != rhs.underlyingType || IsRefPointer != rhs.IsRefPointer;
        }

        virtual int GetHashCode() { return (underlyingType ? underlyingType->GetHashCode() : 0) * 397; }

        System::Reflection::Type const* UnderlyingType() const { return underlyingType; }
        std::string const& Name() const { return name; }
        bool IsCollection() const;
        bool IsContainer() const;
        bool IsIndexedCollection() const;
        bool NeedsConstructionParameters() const;
        Interfaces::ITypeRepository* getTypeRepository() const { return typeRepository; }
        Interfaces::ITypeFactory* getTypeFactory() const { return typeFactory; }
        Interfaces::ITypeFeatureProvider* getFeatureProvider() const { return featureProvider; }
        ReflEx::Member* getContentProperty();
        Interfaces::ITypeConverter* getTypeConverter();
        ReflEx::Member* getRuntimeNamePropertyMember();
        ReflEx::Member* GetMember(std::string const& name);
        virtual ReflEx::Member* LookupMember(std::string const& name);
        ReflEx::AttachableMember* GetAttachableMember(std::string const& name);
        virtual std::string ToString();
        std::shared_ptr<System::Reflection::Object> CreateInstance(std::vector<System::Reflection::Variant> const& parameters);
        static XamlType* Create(System::Reflection::Type const* underlyingType, Interfaces::ITypeRepository* typeRepository, Interfaces::ITypeFactory* typeFactory, Interfaces::ITypeFeatureProvider* featureProvider);
        static XamlType* CreateForBuiltInType(System::Reflection::Type const* type);
        virtual ReflEx::AttachableMember* LookupAttachableMember(std::string const& name);
        std::vector<ReflEx::MemberBase*> GetAllMembers();
        static bool IsValidMember(System::Reflection::meta::Field const* info);
        virtual Interfaces::INameScope* GetNamescope(System::Reflection::Object* instance);
        virtual void BeforeInstanceSetup(System::Reflection::Object* instance);
        virtual void AfterInstanceSetup(System::Reflection::Object* instance);
        virtual void AfterAssociationToParent(System::Reflection::Object* instance);
    };
}

//End of file


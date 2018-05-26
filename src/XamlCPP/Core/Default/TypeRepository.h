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
#include "MemberBase.h"
#include "Interfaces/INamespaceRegistry.h"
#include "Interfaces/ITypeRepository.h"

#include <unordered_map>

namespace XamlCPP::Core
{
    class TypeRepository : public Interfaces::ITypeRepository
    {
    private:
        Interfaces::INamespaceRegistry *const namespaceRegistry;
        Interfaces::ITypeFactory *const typeTypeFactory;
        Interfaces::ITypeFeatureProvider *const featureProvider;
        std::unordered_map<const System::Reflection::Type*, Proto::XamlType *> XamlTypeMap;

    public:
        virtual ~TypeRepository()
        {
            delete namespaceRegistry;
            delete typeTypeFactory;
            delete featureProvider;
        }

        TypeRepository(Interfaces::INamespaceRegistry *namespaceRegistry, Interfaces::ITypeFactory *typeTypeFactory, Interfaces::ITypeFeatureProvider *featureProvider);

        Interfaces::ITypeFeatureProvider *getFeatureProvider() const;

        Interfaces::ITypeFactory *getTypeFactory() const;

        virtual Proto::XamlType *GetByType(System::Reflection::Type const *type);

        Proto::XamlType *GetByQualifiedName(const std::string &qualifiedName);

        Proto::XamlType *GetByPrefix(const std::string &prefix, const std::string &typeName);

        Proto::XamlType *GetByFullAddress(Proto::XamlTypeName *xamlTypeName);

        ReflEx::Member *GetMember(System::Reflection::meta::Field const *propertyInfo);

        ReflEx::AttachableMember *GetAttachableMember(const std::string &name, const System::Reflection::IMethod* getter, const System::Reflection::IMethod* setter);

        void AddTypeConverter(TypeConverterRegistration const& typeConverterRegistration) override;

        Interfaces::INamespaceRegistry * GetNamespaceScope() const override { return namespaceRegistry; }
    };
}

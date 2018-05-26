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
#ifndef __XAMLCPP_TYPEFEATURE_PROVIDER_H__
#define __XAMLCPP_TYPEFEATURE_PROVIDER_H__

#include <System/System.h>
#include "Interfaces/ITypeFeatureProvider.h"
#include "Interfaces/ITypeConverterProvider.h"
#include "Core/ReflEx/MetadataProvider.h"

namespace XamlCPP::Core::ReflEx
{
    class TypeFeatureProvider : public Interfaces::ITypeFeatureProvider
    {
    private:
        std::shared_ptr<Interfaces::ITypeConverterProvider> privateConverterProvider;

        MetadataProvider metadatas;

    public:
        virtual ~TypeFeatureProvider()
        {
        }

        TypeFeatureProvider(std::shared_ptr<Interfaces::ITypeConverterProvider> const& converterProvider);

        Interfaces::ITypeConverterProvider *getConverterProvider() const;
        Interfaces::ITypeConverter *GetTypeConverter(std::size_t typehash);

        void AddTypeConverter(TypeConverterRegistration const& typeConverterRegistration) override;

        std::string GetContentPropertyName(const System::Reflection::Type* type);

        std::vector<Core::TypeConverterRegistration> getTypeConverters();

        std::shared_ptr<Metadata> GetMetadata(const System::Reflection::Type* type);

        void RegisterMetadata(const System::Reflection::Type* type, Metadata *metadata);

        void FillFromAttributes(std::set<System::Reflection::Type const*> &allExportedTypes);
    };
}
#endif //!__XAMLCPP_TYPEFACTORY_H__
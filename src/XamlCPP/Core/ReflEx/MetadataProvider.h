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
#ifndef __XAMLCPP_METADATA_PROVIDER_H__
#define __XAMLCPP_METADATA_PROVIDER_H__

#include <System/System.h>
#include "Interfaces/ITypeFeatureProvider.h"
#include "Interfaces/ITypeConverterProvider.h"

#include <unordered_map>

namespace XamlCPP::Core::ReflEx
{
    class MetadataProvider
    {
    private:
        //std::unordered_map<std::string, std::map<System::Reflection::Type const*, System::Reflection::Variant>> lookupDictionaries;
        std::unordered_map<System::Reflection::Type const*, std::shared_ptr<XamlCPP::Core::Metadata>> lookupDictionaries;

    public:
        void Register(System::Reflection::Type const*type, Metadata *medataData);

    private:
        std::map<System::Reflection::Type const*, System::Reflection::Variant> const& GetDictionary(const std::string &name);

    public:
        std::shared_ptr<Metadata> Get(System::Reflection::Type const *type);

    private:
        static std::vector<System::Reflection::meta::Field*> getMedataProperties();
    };
}
#endif //!__XAMLCPP_METADATA_PROVIDER_H__£
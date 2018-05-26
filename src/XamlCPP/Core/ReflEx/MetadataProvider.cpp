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

#include "StdAfx.h"
#include "MetadataProvider.h"
#include "Core/Metadata.h"

void XamlCPP::Core::ReflEx::MetadataProvider::Register(System::Reflection::Type const * type, Metadata * medataData)
{
    lookupDictionaries[type] = medataData->shared_from_base_dynamic<Metadata>();
    //for (System::Reflection::meta::Field* field : *medataData->GetType()->GetFields())
    //{
    //    auto dic = GetDictionary(field->GetName());
    //    auto value = field->GetValue(medataData);
    //    if (value)
    //        dic[type] = value;
    //}
}

std::map<System::Reflection::Type const*, System::Reflection::Variant> const & XamlCPP::Core::ReflEx::MetadataProvider::GetDictionary(const std::string & name)
{
    static std::map<System::Reflection::Type const*, System::Reflection::Variant> empty;
    return empty; // lookupDictionaries[name];
}

std::shared_ptr<XamlCPP::Core::Metadata> XamlCPP::Core::ReflEx::MetadataProvider::Get(System::Reflection::Type const * type)
{
    auto iMetaData = lookupDictionaries.find(type);
    if (iMetaData != lookupDictionaries.end())
        return iMetaData->second;
    return nullptr;

    //auto metadata = std::make_shared<Metadata>();
    //for (auto prop : getMedataProperties())
    //{
    //    auto lookupDictionaries_iterator = lookupDictionaries.find(prop->GetName());
    //    auto hadDictionary = lookupDictionaries_iterator != lookupDictionaries.end();
    //    auto dic = lookupDictionaries_iterator->second;
    //    if (hadDictionary)
    //    {
    //        auto ivalue = dic.find(type);
    //        if (ivalue != dic.end())
    //        {
    //            prop->SetGenericValue(metadata.get(), ivalue->second);
    //        }
    //    }
    //}
    //
    //return metadata;
}

std::vector<System::Reflection::meta::Field*> XamlCPP::Core::ReflEx::MetadataProvider::getMedataProperties()
{
    return *typeof(Metadata).GetFields();
}

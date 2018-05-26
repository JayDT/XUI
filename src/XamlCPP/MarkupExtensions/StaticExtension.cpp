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

#include "StaticExtension.h"
#include "Interfaces/ITypeRepository.h"
#include "Proto/XamlType.h"

using namespace XamlCPP;
using namespace XamlCPP::Markup;

StaticExtension::StaticExtension()
{
}

StaticExtension::StaticExtension(const std::string &identifier)
{
    Identifier = identifier;
}

System::Reflection::Variant StaticExtension::ProvideValue(Core::MarkupExtensionContext * markupExtensionContext)
{
    auto typeRepository = markupExtensionContext->getValueContext()->getTypeRepository();
    auto typeAndMember = GetTypeAndMember(Identifier);
    auto prefixAndType = GetPrefixAndType(std::get<0>(typeAndMember));
    auto xamlType = typeRepository->GetByPrefix(std::get<0>(prefixAndType), std::get<1>(prefixAndType));
    return GetValue(xamlType->UnderlyingType(), std::get<1>(typeAndMember));
}

std::tuple<std::string, std::string> StaticExtension::GetTypeAndMember(const std::string &s)
{
    auto parts = System::String(s).Split('.');

    if (parts.size() != 2)
    {
        throw std::invalid_argument("Static member must be in the form Type.Member.");
    }

    return std::make_tuple(parts[0], parts[1]);
}

std::tuple<std::string, std::string> StaticExtension::GetPrefixAndType(const std::string &s)
{
    if (s.find(":") != std::string::npos)
    {
        auto parts = System::String(s).Split(':');
        return std::make_tuple(parts[0], parts[1]);
    }
    return std::make_tuple("", s);
}

System::Reflection::Variant StaticExtension::GetValue(const System::Reflection::Type* type, const std::string &name)
{
    auto t = type;

    if (t->IsEnum())
    {
        return System::Reflection::Enum::Parse(*t, name);
    }
    else
    {
        System::Reflection::IField const* _field = &t->GetRuntimeProperty(name);

        if (_field->IsValid() && _field->GetMethod()->IsStatic())
        {
            return _field->GetValue(nullptr);
        }

        _field = &t->GetStaticField(name);

        if (_field->IsValid() && _field->IsStatic())
        {
            return _field->GetValue(nullptr);
        }
    }

    throw std::invalid_argument(System::String::format("Static member '%s.%s' not found.", type->GetName(), name.c_str()));
}

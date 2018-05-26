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

#include "TypeExtension.h"
#include "ITypeRepository.h"
#include "Proto/XamlType.h"

using namespace XamlCPP;
using namespace XamlCPP::Markup;

TypeExtension::TypeExtension()
{
}

TypeExtension::TypeExtension(const System::Reflection::Type* type)
{
    TypeType = type;
}

const System::Reflection::Type* TypeExtension::ResolveFromString(const std::string &type, Interfaces::ITypeRepository *typeRepository)
{
    System::ThrowIfFailed<System::ArgumentException>(!type.empty(), "type");

    auto split = System::String(type).Split(':');
    auto prefix = (sizeof(split) / sizeof(split[0])) == 1 ? split[0] : nullptr;
    auto typeName = (sizeof(split) / sizeof(split[0])) == 1 ? split[1] : split[0];
    auto xamlType = typeRepository->GetByPrefix(prefix, typeName);
    return xamlType->UnderlyingType();
}

System::Reflection::Variant TypeExtension::ProvideValue(Core::MarkupExtensionContext *markupExtensionContext)
{
    if (TypeType != nullptr)
        return TypeType;

    return ResolveFromString(TypeName, markupExtensionContext->getValueContext()->getTypeRepository());
}


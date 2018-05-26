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

#include "PropertyLocator.h"
#include "XamlQualifiedName.h"

using namespace XamlCPP::Core;
using namespace XamlCPP::Core::ProtoParser;

PropertyLocator::PropertyLocator(std::shared_ptr<Proto::XamlName> owner, const std::string &prefix, const std::string &propertyName)
    : XamlName(propertyName)
    , Owner(owner)
{
    if (owner != nullptr)
    {
        Prefix = owner->Prefix;
    }
    else
    {
        Prefix = prefix;
    }
}

std::string PropertyLocator::GetOwnerName() const
{
    if (!GetIsDotted())
    {
        return "";
    }
    return Owner->PropertyName;
}

bool PropertyLocator::GetIsDotted() const
{
    return Owner != nullptr;
}

bool PropertyLocator::GetIsNsPrefixDefinition() const
{
    return Prefix == "xmlns" || PropertyName == "xmlns";
}

std::shared_ptr<PropertyLocator> PropertyLocator::Parse(const std::string &longName)
{
    if (longName.empty())
    {
        return nullptr;
    }

    std::string prefix;
    std::string qName;

    if (!Proto::XamlQualifiedName::TryParse(longName, prefix, qName))
    {
        return nullptr;
    }

    auto startIndex = 0;
    std::string part1;
    auto length = (int)qName.find('.');
    if (length != -1)
    {
        part1 = qName.substr(startIndex, length);

        if (part1.empty())
        {
            return nullptr;
        }

        startIndex = length + 1;
    }
    auto part2 = startIndex == 0 ? qName : qName.substr(startIndex);
    std::shared_ptr<Proto::XamlName> xamlQualifiedName;
    if (!part1.empty())
    {
        xamlQualifiedName = std::make_shared<Proto::XamlQualifiedName>(prefix, part1);
    }

    return std::make_shared<PropertyLocator>(xamlQualifiedName, prefix, part2);
}

std::shared_ptr<PropertyLocator> PropertyLocator::Parse(const std::string &longName, const std::string &namespaceUri)
{
    auto xamlPropertyName = Parse(longName);
    xamlPropertyName->Namespace = namespaceUri;
    return xamlPropertyName;
}

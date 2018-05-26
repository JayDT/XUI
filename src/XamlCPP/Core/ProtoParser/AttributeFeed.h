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

#ifndef __XAMLCPP_ATTRIBUTEFEED_H__
#define __XAMLCPP_ATTRIBUTEFEED_H__

#include <System/System.h>
#include "Core/ProtoParser/AttributeAssignment.h"
#include "Core/ProtoParser/DirectiveAssignment.h"

namespace XamlCPP::Core
{
    namespace ProtoParser
    {
        class AttributeFeed;
        class AttributeAssignment;
        class DirectiveAssignment;
        class PropertyLocator;
        class UnprocessedAttributeBase;
    }

    namespace Proto
    {
        class XamlName;
    }
}

namespace XamlCPP::Core::ProtoParser
{
    struct NsPrefix
    {
        NsPrefix(std::string prefix, std::string ns)
        {
            Prefix = prefix;
            Namespace = ns;
        }

        std::string Prefix;
        std::string Namespace;
    };

    class AttributeFeed
    {
    private:
        std::vector<NsPrefix> prefixDefinitions;
        const std::vector<NsPrefix>& specialPrefix;
        std::vector<AttributeAssignment> attributes;
        std::vector<DirectiveAssignment> directives;

        std::shared_ptr<DirectiveAssignment> classOverride;

    public:
        AttributeFeed(std::vector<AttributeAssignment>& assignments, const std::vector<NsPrefix>& specialPrefix);

    private:
        bool IsAttribute(AttributeAssignment *assignment);

        void ParseNsDefinition(UnprocessedAttributeBase *assignment);

        bool IsNamespaceDefinition(AttributeAssignment *assignment);

        void ParseDirectives(AttributeAssignment *assignment);

        bool IsDirective(AttributeAssignment *attributeAssignment);

    public:
        std::vector<AttributeAssignment> const& GetRawAttributes() const;

        std::vector<NsPrefix> const& GetPrefixRegistrations() const;

        std::vector<DirectiveAssignment> const& GetDirectives() const;

        DirectiveAssignment* GetClassOverride() { return classOverride.get(); }
    };
}


#endif	//#ifndef __XAMLCPP_ATTRIBUTEFEED_H__

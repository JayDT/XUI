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

#ifndef __XAMLCPP_PROPERTYLOCATOR_H__
#define __XAMLCPP_PROPERTYLOCATOR_H__

#include <System/System.h>
#include "Core/Proto/XamlName.h"

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

    namespace ReflEx
    {
        struct Directive;
    }
}

namespace XamlCPP::Core::ProtoParser
{
    class TC_XAMLCPP_API PropertyLocator : public Proto::XamlName
    {
    public:
        std::shared_ptr<Proto::XamlName> Owner;

        virtual ~PropertyLocator()
        {
        }

        PropertyLocator(std::shared_ptr<Proto::XamlName> owner, const std::string &prefix, const std::string &propertyName);

        std::string GetOwnerName() const;

        bool GetIsDotted() const;

        bool GetIsNsPrefixDefinition() const;

        static std::shared_ptr<PropertyLocator> Parse(const std::string &longName);

        static std::shared_ptr<PropertyLocator> Parse(const std::string &longName, const std::string &namespaceUri);
    };
}


#endif	//#ifndef __XAMLCPP_PROPERTYLOCATOR_H__

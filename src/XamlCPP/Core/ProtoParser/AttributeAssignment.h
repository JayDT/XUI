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

#ifndef __XAMLCPP_ATTRIBUTEASSIGNMENT_H__
#define __XAMLCPP_ATTRIBUTEASSIGNMENT_H__

#include <System/System.h>
#include "Core/ProtoParser/UnprocessedAttributeBase.h"

namespace XamlCPP::Core
{
    namespace ProtoParser
    {
        class AttributeFeed;
        class AttributeAssignment;
        class DirectiveAssignment;
        class PropertyLocator;
    }

    namespace Proto
    {
        class XamlName;
    }
}

namespace XamlCPP::Core::ProtoParser
{
    class AttributeAssignment : public UnprocessedAttributeBase
    {
    public:
        AttributeAssignment(std::shared_ptr<PropertyLocator> const& locator, const std::string &value) : UnprocessedAttributeBase(locator, value)
        {
        }

    };
}


#endif	//#ifndef __XAMLCPP_ATTRIBUTEASSIGNMENT_H__

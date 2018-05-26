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

#ifndef __XAMLCPP_PROTOINSTRUCTION_H__
#define __XAMLCPP_PROTOINSTRUCTION_H__

#include <System/System.h>
#include "Core/Proto/XamlType.h"
#include "Core/ReflEx/MemberBase.h"

namespace XamlCPP::Core
{
    enum class NodeType
        Meta(Enable)
    {
        None,
        Element,
        EmptyElement,
        EndTag,
        PrefixDefinition,
        Directive,
        Attribute,
        Text,
        EmptyPropertyElement,
        PropertyElement
    };

    struct ProtoInstruction
    {
        NodeType NodeType = NodeType::None;
        Proto::XamlType* XamlType = nullptr; // not owned
        ReflEx::MemberBase* PropertyAttribute = nullptr;
        ReflEx::MemberBase* PropertyElement = nullptr;
        std::string Namespace;
        std::string Prefix;
        std::string PropertyAttributeText;
        std::string Text;

        ~ProtoInstruction()
        {
            if (PropertyAttribute)
                delete PropertyAttribute;
            if (PropertyElement)
                delete PropertyElement;
        }

        bool operator==(ProtoInstruction const& rhs) const
        {
            return *XamlType == *rhs.XamlType &&
                Namespace == rhs.Namespace &&
                NodeType == rhs.NodeType &&
                PropertyAttribute == rhs.PropertyAttribute &&
                Prefix == rhs.Prefix &&
                PropertyElement == rhs.PropertyElement &&
                PropertyAttributeText == rhs.PropertyAttributeText &&
                Text == rhs.Text;
        }


        size_t GetHashCode()
        {
            std::hash<std::string> str_hasher;

            size_t hashCode = (XamlType != nullptr ? XamlType->GetHashCode() : 0);
            hashCode = (hashCode * 397) ^ (!Namespace.empty() ? str_hasher(Namespace) : 0);
            hashCode = (hashCode * 397) ^ (int)NodeType;
            hashCode = (hashCode * 397) ^ (PropertyAttribute != nullptr ? str_hasher(PropertyAttribute->Name) : 0);
            hashCode = (hashCode * 397) ^ (!Prefix.empty() ? str_hasher(Prefix) : 0);
            hashCode = (hashCode * 397) ^ (PropertyElement != nullptr ? str_hasher(PropertyElement->Name) : 0);
            hashCode = (hashCode * 397) ^ (!PropertyAttributeText.empty() ? str_hasher(PropertyAttributeText) : 0);
            hashCode = (hashCode * 397) ^ (!Text.empty() ? str_hasher(Text) : 0);
            return hashCode;
        }

        std::string ToString();
    };
}

#endif //!__XAMLCPP_PROTOINSTRUCTION_H__

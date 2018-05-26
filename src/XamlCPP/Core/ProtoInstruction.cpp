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

#include "ProtoInstruction.h"

std::string XamlCPP::Core::ProtoInstruction::ToString()
{
    auto str = System::String::format("%u: ", NodeType);
    switch (NodeType)
    {
        case NodeType::EmptyElement:
        case NodeType::Element:
            str += "Element: ";
            str += XamlType->Name();
            break;
        case NodeType::Attribute:
            str += "Attribute: ";
            str += PropertyAttributeText;
            break;
        case NodeType::PropertyElement:
        case NodeType::EmptyPropertyElement:
            str += "Property: ";
            str += PropertyElement->ToString();
            break;
        case NodeType::Directive:
            str += "Directive: ";
            str += PropertyAttribute->ToString();
            break;
        case NodeType::PrefixDefinition:
            str += "PrefixDefinition: ";
            str += Prefix;
            str += ":";
            str += Namespace;
            break;
        case NodeType::None:
            str += "None";
            break;
        case NodeType::EndTag:
            str += "End Tag";
            break;
        case NodeType::Text:
        {
            std::string strRep;
            if (Text.empty())
            {
                strRep = "(empty)";
            }
            else
            {
                strRep = Text;
            }

            str = "Text: ";
            str += strRep;
            break;
        }
        default:
            throw System::NotSupportedException();
    }

    return str;
}

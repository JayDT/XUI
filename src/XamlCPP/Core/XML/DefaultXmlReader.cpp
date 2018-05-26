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

#include "DefaultXmlReader.h"

std::string EmptyIfNull(const char* text)
{
    if (!text)
        return "";

    return text;
}

void XamlCPP::Core::XML::XmlDefaultReader::ReadAttributeData()
{
    name            = EmptyIfNull(TopAttribute ? TopAttribute->name() : "");
    prefix          = "";
    localName       = EmptyIfNull(TopAttribute ? TopAttribute->local_name() : "");
    value           = EmptyIfNull(TopAttribute ? TopAttribute->value() : "");
    nameSpaceURI    = EmptyIfNull(TopAttribute ? TopAttribute->xmlns() : "");

    isEmptyElement = TopAttribute == nullptr || name.empty();

    nodeType = Interfaces::XmlNodeType::Attribute;
}

void XamlCPP::Core::XML::XmlDefaultReader::ReadElementData()
{
    name            = EmptyIfNull(TopNode ? TopNode->name() : "");
    prefix          = EmptyIfNull(TopNode ? TopNode->prefix() : "");
    localName       = name;
    value           = EmptyIfNull(TopNode ? TopNode->value() : "");
    nameSpaceURI    = EmptyIfNull(TopNode ? TopNode->xmlns() : "");

    isEmptyElement = TopNode == nullptr || name.empty();

    if (TopNode)
    {
        switch (TopNode->type())
        {
            case rapidxml::node_type::node_pi:
            case rapidxml::node_type::node_literal:
            case rapidxml::node_type::node_declaration:
            case rapidxml::node_type::node_comment:
                nodeType = Interfaces::XmlNodeType::Whitespace;
                break;
            case rapidxml::node_type::node_cdata:
            case rapidxml::node_type::node_data:
                nodeType = Interfaces::XmlNodeType::Text;
                break;
            case rapidxml::node_type::node_element:
                nodeType = Interfaces::XmlNodeType::Element;
                break;
            default:
                nodeType = Interfaces::XmlNodeType::None;
                break;
        }
    }
    else
        nodeType = Interfaces::XmlNodeType::None;
}

void XamlCPP::Core::XML::XmlDefaultReader::ReadEndElementData()
{
    ReadElementData();

    if (nodeType == Interfaces::XmlNodeType::Element)
        nodeType = Interfaces::XmlNodeType::EndElement;
}

void XamlCPP::Core::XML::XmlDefaultReader::ReadRootElementData()
{
    ReadElementData();
}

XamlCPP::Core::XML::XmlDefaultReader::XmlDefaultReader(const std::ostringstream & stream)
{
    xmlCache = stream.str();
    xmlReader.parse<0>((char*)xmlCache.c_str());
    xmlReader.validate();
}

bool XamlCPP::Core::XML::XmlDefaultReader::HasLineInfo()
{
    return false;
}

XamlCPP::Interfaces::XmlNodeType XamlCPP::Core::XML::XmlDefaultReader::NodeType()
{
    return nodeType;
}

bool XamlCPP::Core::XML::XmlDefaultReader::IsEmptyElement()
{
    return isEmptyElement;
}

std::string const & XamlCPP::Core::XML::XmlDefaultReader::Prefix()
{
    return prefix;
}

std::string const & XamlCPP::Core::XML::XmlDefaultReader::LocalName()
{
    return localName;
}

std::string const & XamlCPP::Core::XML::XmlDefaultReader::Name()
{
    return name;
}

std::string const & XamlCPP::Core::XML::XmlDefaultReader::Value()
{
    return value;
}

std::string const & XamlCPP::Core::XML::XmlDefaultReader::Namespace()
{
    return nameSpaceURI;
}

void XamlCPP::Core::XML::XmlDefaultReader::Read(bool onlyNextSibling)
{
    if (isFirstRead)
    {
        isFirstRead = false;
        RootNode = TopNode = xmlReader.first_node();
        if (TopNode)
            xmlNodeStack.push(TopNode);
    }
    else if (TopNode)
    {
        rapidxml::xml_node<char>* next = !isNextSibling ? TopNode->first_node(nullptr) : nullptr;
        if (next)
        {
            TopNode = next;
            xmlNodeStack.push(next);
        }
        else
        {
            next = TopNode->next_sibling();
            if (next)
            {
                isNextSibling = false;
                // First report element end
                if (nodeType != Interfaces::XmlNodeType::EndElement)
                {
                    ReadEndElementData();
                    return;
                }
                else
                {
                    xmlNodeStack.top() = TopNode = next;
                }
            }
            else if (!onlyNextSibling && !xmlNodeStack.empty())
            {
                xmlNodeStack.pop();
                TopNode = xmlNodeStack.top();
                ReadEndElementData();
                isNextSibling = true;
                return;
            }
            else if (onlyNextSibling)
            {
                ReadEndElementData();
                return;
            }
            else
            {
                TopNode = nullptr;
            }
        }
    }

    ReadElementData();
}

bool XamlCPP::Core::XML::XmlDefaultReader::MoveToFirstAttribute()
{
    if (nodeType == Interfaces::XmlNodeType::EndElement)
        return false;

    TopAttribute = TopNode->first_attribute();
    ReadAttributeData();
    return TopAttribute != nullptr;
}

bool XamlCPP::Core::XML::XmlDefaultReader::MoveToNextAttribute()
{
    if (nodeType == Interfaces::XmlNodeType::EndElement)
        return false;

    TopAttribute = TopAttribute->next_attribute();
    ReadAttributeData();
    return TopAttribute != nullptr;
}

void XamlCPP::Core::XML::XmlDefaultReader::MoveToElement()
{
    if (nodeType == Interfaces::XmlNodeType::EndElement)
        return;

    TopAttribute = nullptr;
    ReadElementData();
}

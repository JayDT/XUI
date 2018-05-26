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

#pragma once

#include <System/System.h>
#include "Interfaces/IXmlReader.h"
#include "standard/xml/rapidxml.h"
#include <stack>

namespace XamlCPP::Core::XML
{
    class XmlDefaultReader : public Interfaces::IXmlReader
    {
    private:
        std::string xmlCache;
        rapidxml::xml_document<char> xmlReader;
        std::stack<rapidxml::xml_node<char>*> xmlNodeStack;
        rapidxml::xml_node<char>* RootNode = nullptr;
        rapidxml::xml_node<char>* TopNode = nullptr;
        rapidxml::xml_attribute<char>* TopAttribute = nullptr;

        std::string prefix;
        std::string localName;
        std::string name;
        std::string value;
        std::string nameSpaceURI;

        Interfaces::XmlNodeType nodeType = Interfaces::XmlNodeType::None;

        bool        isEmptyElement = true;
        bool        isFirstRead = true;
        bool        isNextSibling = false;

        void ReadAttributeData();
        void ReadElementData();
        void ReadEndElementData();
        void ReadRootElementData();
    public:

        XmlDefaultReader(const std::ostringstream& stream);

        // Inherited via IXmlReader
        virtual bool HasLineInfo() override;
        virtual Interfaces::XmlNodeType NodeType() override;
        virtual bool IsEmptyElement() override;
        virtual std::string const & Prefix() override;
        virtual std::string const & LocalName() override;
        virtual std::string const & Name() override;
        virtual std::string const & Value() override;
        virtual std::string const & Namespace() override;
        virtual void Read(bool onlyNextSibling = false) override;
        virtual bool MoveToFirstAttribute() override;
        virtual bool MoveToNextAttribute() override;
        virtual void MoveToElement() override;

        int GetNodeLevel() const override { return xmlNodeStack.size(); }
    };
}

//End of file
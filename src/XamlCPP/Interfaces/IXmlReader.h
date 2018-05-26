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
#include "Reflection/Runtime/Type.h"

namespace XamlCPP::Interfaces
{
    enum class XmlNodeType
    {
        //
        // Summary:
        //     This is returned by the System.Xml.XmlReader if a Read method has not been called.
        None = 0,
        //
        // Summary:
        //     An element (for example, <item> ).
        Element = 1,
        //
        // Summary:
        //     An attribute (for example, id='123' ).
        Attribute = 2,
        //
        // Summary:
        //     The text content of a node.
        Text = 3,
        //
        // Summary:
        //     A CDATA section (for example, <![CDATA[my escaped text]]> ).
        CDATA = 4,
        //
        // Summary:
        //     A reference to an entity (for example, &num; ).
        EntityReference = 5,
        //
        // Summary:
        //     An entity declaration (for example, <!ENTITY...> ).
        Entity = 6,
        //
        // Summary:
        //     A processing instruction (for example, <?pi test?> ).
        ProcessingInstruction = 7,
        //
        // Summary:
        //     A comment (for example, <!-- my comment --> ).
        Comment = 8,
        //
        // Summary:
        //     A document object that, as the root of the document tree, provides access to
        //     the entire XML document.
        Document = 9,
        //
        // Summary:
        //     The document type declaration, indicated by the following tag (for example, <!DOCTYPE...>
        //     ).
        DocumentType = 10,
        //
        // Summary:
        //     A document fragment.
        DocumentFragment = 11,
        //
        // Summary:
        //     A notation in the document type declaration (for example, <!NOTATION...> ).
        Notation = 12,
        //
        // Summary:
        //     White space between markup.
        Whitespace = 13,
        //
        // Summary:
        //     White space between markup in a mixed content model or white space within the
        //     xml:space="preserve" scope.
        SignificantWhitespace = 14,
        //
        // Summary:
        //     An end element tag (for example, </item> ).
        EndElement = 15,
        //
        // Summary:
        //     Returned when XmlReader gets to the end of the entity replacement as a result
        //     of a call to System.Xml.XmlReader.ResolveEntity.
        EndEntity = 16,
        //
        // Summary:
        //     The XML declaration (for example, <?xml version='1.0'?> ).
        XmlDeclaration = 17
    };

    //
    // Summary:
    //     Provides an interface to enable a class to return line and position information.
    struct IXmlLineInfo
    {
        //
        // Summary:
        //     Gets the current line number.
        //
        // Returns:
        //     The current line number or 0 if no line information is available (for example,
        //     System.Xml.IXmlLineInfo.HasLineInfo returns false).
        int LineNumber = 0;
        //
        // Summary:
        //     Gets the current line position.
        //
        // Returns:
        //     The current line position or 0 if no line information is available (for example,
        //     System.Xml.IXmlLineInfo.HasLineInfo returns false).
        int LinePosition = 0;
        //
        // Summary:
        //     Gets a value indicating whether the class can return line information.
        //
        // Returns:
        //     true if System.Xml.IXmlLineInfo.LineNumber and System.Xml.IXmlLineInfo.LinePosition
        //     can be provided; otherwise, false.
        virtual bool HasLineInfo() = 0;
    };

    struct IXmlReader : public IXmlLineInfo
    {
        virtual XmlNodeType NodeType() = 0;
        virtual bool IsEmptyElement() = 0;
        virtual std::string const& Prefix() = 0;
        virtual std::string const& LocalName() = 0;
        virtual std::string const& Name() = 0;
        virtual std::string const& Value() = 0;
        virtual std::string const& Namespace() = 0;
        virtual void Read(bool onlyNextSibling = false) = 0;
        virtual bool MoveToFirstAttribute() = 0;
        virtual bool MoveToNextAttribute() = 0;
        virtual void MoveToElement() = 0;

        virtual int GetNodeLevel() const = 0;
    };
}

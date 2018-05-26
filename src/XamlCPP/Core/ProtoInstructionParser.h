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
#include "Interfaces/IParser.h"
#include "Interfaces/IXmlReader.h"
#include "Core/ProtoParser/AttributeParser.h"
#include "Core/ProtoInstructionBuilder.h"
#include "Core/ProtoParser/AttributeParser.h"

namespace XamlCPP::Core
{
    namespace ProtoParser
    {
        class AttributeFeed;
        class DirectiveAssignment;
        class PropertyLocator;
    }

    namespace Proto
    {
        class XamlName;
    }

    class ProtoInstructionParser : public Interfaces::IProtoParser
    {
    public:

        typedef std::vector<std::shared_ptr<XamlCPP::Core::ProtoInstruction>> ProtoStorage;

    private:
        ProtoInstructionBuilder instructionBuilder;
        ProtoParser::AttributeParser attributeParser;
        Interfaces::IRuntimeTypeSource* typeSource = nullptr;
        Interfaces::IXmlReader* reader = nullptr;

        ProtoStorage protoInstructions;

    public:

        ProtoInstructionParser(Interfaces::IRuntimeTypeSource* typeSource);

        ProtoStorage Parse(Interfaces::IXmlReader* stream) override;

        std::string ToString() const;

    private:
        void ParseEmptyElement(Proto::XamlType *xamlType, ReflEx::NamespaceDeclaration *namespaceDeclaration, ProtoParser::AttributeFeed *attributes);

        void CommonNodesOfElement(Proto::XamlType *owner, std::shared_ptr<ProtoInstruction> const& elementToInject, ProtoParser::AttributeFeed *attributeFeed);

        void ParseExpandedElement(Proto::XamlType *xamlType, ReflEx::NamespaceDeclaration *namespaceDeclaration, ProtoParser::AttributeFeed *attributes);

        void ParseNestedElements(Proto::XamlType *xamlType);

        void ParseNestedProperty(Proto::XamlType *xamlType);

        void ParseElement();

        void InjectPropertyInstruction(Proto::XamlType *xamlType, ProtoParser::PropertyLocator *propertyLocator, ReflEx::NamespaceDeclaration *namespaceDeclaration);

        static bool IsNormalProperty(Proto::XamlType *xamlType, ProtoParser::PropertyLocator *propertyLocator);

        void ParseInnerTextIfAny();

        std::string FormatText(const std::string &rawText);

        void ParseChildren();

        void AssertValidElement();

        // TODO: Refactor this shit.
        void ParseAttributeToNode(Proto::XamlType *containingType, ProtoParser::AttributeAssignment const& rawAttributeAssignment);

        bool IsNameDirective(Proto::XamlName *propertyLocator, Proto::XamlType *ownerType);

        ReflEx::MutableMember *GetMemberForDottedLocator(ProtoParser::PropertyLocator *propertyLocator);

        void RegisterPrefixes(std::vector<ProtoParser::NsPrefix> &prefixRegistrations);

        void SkipWhitespaces();

        void ParseDirective(ProtoParser::DirectiveAssignment const& assignment);

        void ParseAttributeToNsPrefixDefinition(ProtoParser::NsPrefix const& prefix);
    };
}
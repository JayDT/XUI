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

#include "StdAfx.h"
#include "ProtoInstructionParser.h"
#include "Proto/PrefixRegistration.h"
#include "Proto/XamlName.h"
#include "ProtoParser/PropertyLocator.h"
#include "ReflEx/Member.h"
#include "ReflEx/AttachableMember.h"
#include "ProtoInstructionBuilder.h"
#include "CoreTypes.h"
#include "Interfaces/IXamlCPP.h"
#include "Core/ReflEx/NamespaceDeclaration.h"
#include "Runtime/ReflectionModule.h"

#include <regex>

std::string XamlCPP::Core::ProtoInstructionParser::ToString() const
{
    std::ostringstream stream;
    for (auto const& i : this->protoInstructions)
    {
        stream << i->ToString() << std::endl;
    }
    
    return stream.str();
}

XamlCPP::Core::ProtoInstructionParser::ProtoInstructionParser(Interfaces::IRuntimeTypeSource * typeSource)
    : instructionBuilder(typeSource)
{
    this->typeSource = typeSource;
}

std::vector<std::shared_ptr<XamlCPP::Core::ProtoInstruction>> XamlCPP::Core::ProtoInstructionParser::Parse(Interfaces::IXmlReader * stream)
{
    reader = stream;
    reader->Read();
    ParseElement();

    return protoInstructions;
}

void XamlCPP::Core::ProtoInstructionParser::ParseEmptyElement(Proto::XamlType * xamlType, ReflEx::NamespaceDeclaration * namespaceDeclaration, ProtoParser::AttributeFeed * attributes)
{
    auto proto = instructionBuilder.EmptyElement(xamlType->UnderlyingType(), namespaceDeclaration);
    CommonNodesOfElement(xamlType, proto, attributes);
}

void XamlCPP::Core::ProtoInstructionParser::CommonNodesOfElement(Proto::XamlType * owner, std::shared_ptr<ProtoInstruction> const& elementToInject, ProtoParser::AttributeFeed * attributeFeed)
{
    for (auto const& nsprefix : attributeFeed->GetPrefixRegistrations())
    {
        ParseAttributeToNsPrefixDefinition(nsprefix);
    }

    protoInstructions.emplace_back(elementToInject);

    for (auto const& directive : attributeFeed->GetDirectives())
    {
        ParseDirective(directive);
    }

    for (auto const& attribute : attributeFeed->GetRawAttributes())
    {
        ParseAttributeToNode(owner, attribute);
    };
}

void XamlCPP::Core::ProtoInstructionParser::ParseExpandedElement(Proto::XamlType * xamlType, ReflEx::NamespaceDeclaration * namespaceDeclaration, ProtoParser::AttributeFeed * attributes)
{
    auto proto = instructionBuilder.NonEmptyElement(xamlType->UnderlyingType(), namespaceDeclaration);
    CommonNodesOfElement(xamlType, proto, attributes);

    reader->Read(true);

    ParseInnerTextIfAny();
    ParseNestedElements(xamlType);

    protoInstructions.emplace_back(instructionBuilder.EndTag());
}

void XamlCPP::Core::ProtoInstructionParser::ParseNestedElements(Proto::XamlType * xamlType)
{
    int level = reader->GetNodeLevel();
    while (reader->NodeType() != Interfaces::XmlNodeType::EndElement)
    {
        SkipWhitespaces();

        auto isPropertyElement = reader->LocalName().find(".") != std::string::npos;
        if (isPropertyElement)
        {
            ParseNestedProperty(xamlType);
            reader->Read();
        }
        else
        {
            ParseChildren();
        }
    }
}

void XamlCPP::Core::ProtoInstructionParser::ParseNestedProperty(Proto::XamlType * xamlType)
{
    auto propertyLocator = ProtoParser::PropertyLocator::Parse(reader->Name());
    ReflEx::NamespaceDeclaration namespaceDeclaration(reader->Namespace(), reader->Prefix());

    InjectPropertyInstruction(xamlType, propertyLocator.get(), &namespaceDeclaration);

    reader->Read();

    ParseInnerTextIfAny();

    SkipWhitespaces();

    if (reader->NodeType() != Interfaces::XmlNodeType::EndElement)
        ParseChildren();

    protoInstructions.emplace_back(instructionBuilder.EndTag());
    propertyLocator.reset();
}

void XamlCPP::Core::ProtoInstructionParser::ParseElement()
{
    SkipWhitespaces();

    AssertValidElement();

    auto attributes = attributeParser.Read(reader);

    auto prefixRegistrations = attributes->GetPrefixRegistrations();

    RegisterPrefixes(prefixRegistrations);

    auto prefix = reader->Prefix();
    auto ns = reader->Namespace();
    auto typeName = reader->LocalName();

    // ToDo: implement generic way
    if (attributes->GetClassOverride() && typeSource->GetDefaultAssembly())
    {
        std::string ons;
        auto parts = System::String(attributes->GetClassOverride()->GetValue()).Split('.');
        if (parts.size() > 1)
        {
            for (int i = 0; i < parts.size() - 1; ++i)
            {
                if (i != 0)
                    ons += "::";
                ons += parts[i];
            }
        }

        std::string clrNs = System::String::format("clr-namespace:%s", ons.c_str());
        Proto::PrefixRegistration prefixRegistration(ons, clrNs);
        typeSource->RegisterPrefix(prefixRegistration);

        prefix = ons;
        ns = ons;
        typeName = parts.back();
    }

    ReflEx::NamespaceDeclaration namespaceDeclaration(ns, prefix);

    auto childType = typeSource->GetByPrefix(namespaceDeclaration.GetPrefix(), typeName);

    if (reader->IsEmptyElement())
        ParseEmptyElement(childType, &namespaceDeclaration, attributes.get());
    else
        ParseExpandedElement(childType, &namespaceDeclaration, attributes.get());
}

void XamlCPP::Core::ProtoInstructionParser::InjectPropertyInstruction(Proto::XamlType * xamlType, ProtoParser::PropertyLocator * propertyLocator, ReflEx::NamespaceDeclaration * namespaceDeclaration)
{
    if (IsNormalProperty(xamlType, propertyLocator))
    {
        protoInstructions.emplace_back(instructionBuilder.NonEmptyPropertyElement(xamlType->UnderlyingType(), propertyLocator->PropertyName, namespaceDeclaration));
    }
    else
    {
        auto owner = typeSource->GetByPrefix(propertyLocator->Prefix, propertyLocator->GetOwnerName());
        protoInstructions.emplace_back(instructionBuilder.ExpandedAttachedProperty(owner->UnderlyingType(), propertyLocator->PropertyName, namespaceDeclaration));
    }
}

bool XamlCPP::Core::ProtoInstructionParser::IsNormalProperty(Proto::XamlType * xamlType, ProtoParser::PropertyLocator * propertyLocator)
{
    return !propertyLocator->GetIsDotted() || propertyLocator->GetIsDotted() && propertyLocator->GetOwnerName() == xamlType->Name();
}

void XamlCPP::Core::ProtoInstructionParser::ParseInnerTextIfAny()
{
    if (reader->NodeType() == Interfaces::XmlNodeType::Text)
    {
        protoInstructions.emplace_back(instructionBuilder.Text(FormatText(reader->Value())));
        reader->Read();
    }
}

std::string XamlCPP::Core::ProtoInstructionParser::FormatText(const std::string & rawText)
{
    //std::regex regex{ R"(\s+)" };
    //std::smatch match;
    //regex.
    return rawText;
}

void XamlCPP::Core::ProtoInstructionParser::ParseChildren()
{
    int level = reader->GetNodeLevel();
    while (reader->NodeType() != Interfaces::XmlNodeType::EndElement)
    {
        ParseElement();

        protoInstructions.emplace_back(instructionBuilder.Text());

        reader->Read();
        SkipWhitespaces();

        // Property element need switch to ProtoInstructionParser::ParseNestedElements
        if (reader->NodeType() == Interfaces::XmlNodeType::Element && reader->LocalName().find(".") != std::string::npos)
            break;
    }
}

void XamlCPP::Core::ProtoInstructionParser::AssertValidElement()
{
    if (!(reader->NodeType() == Interfaces::XmlNodeType::Element && reader->LocalName().find(".") == std::string::npos))
    {
        throw XamlCPP::ParseException("The root should be an element.");
    }
}

void XamlCPP::Core::ProtoInstructionParser::ParseAttributeToNode(Proto::XamlType * containingType, ProtoParser::AttributeAssignment const& rawAttributeAssignment)
{
    ReflEx::MutableMember *member;

    if (rawAttributeAssignment.GetLocator()->GetIsDotted())
    {
        member = GetMemberForDottedLocator(rawAttributeAssignment.GetLocator());
    }
    else
    {
        if (IsNameDirective(rawAttributeAssignment.GetLocator(), containingType))
        {
            protoInstructions.emplace_back(instructionBuilder.Directive(CoreTypes::getName(), rawAttributeAssignment.GetValue()));
            return;
        }

        member = containingType->GetMember(rawAttributeAssignment.GetLocator()->PropertyName);
    }

    protoInstructions.emplace_back(instructionBuilder.Attribute(member, rawAttributeAssignment.GetValue(), rawAttributeAssignment.GetLocator()->Prefix));
}

bool XamlCPP::Core::ProtoInstructionParser::IsNameDirective(Proto::XamlName * propertyLocator, Proto::XamlType * ownerType)
{
    std::unique_ptr<ReflEx::Member> member(ownerType->getRuntimeNamePropertyMember());
    if (!member)
        return false;
    return propertyLocator->PropertyName == member->Name;
}

XamlCPP::Core::ReflEx::MutableMember * XamlCPP::Core::ProtoInstructionParser::GetMemberForDottedLocator(ProtoParser::PropertyLocator * propertyLocator)
{
    auto ownerName = propertyLocator->Owner->PropertyName;
    auto ownerPrefix = propertyLocator->Owner->Prefix;

    auto owner = typeSource->GetByPrefix(ownerPrefix, ownerName);

    ReflEx::MutableMember *member = owner->GetAttachableMember(propertyLocator->PropertyName);
    return member;
}

void XamlCPP::Core::ProtoInstructionParser::RegisterPrefixes(std::vector<ProtoParser::NsPrefix>& prefixRegistrations)
{
    for (auto prefixRegistration : prefixRegistrations)
    {
        Proto::PrefixRegistration registration(prefixRegistration.Prefix, prefixRegistration.Namespace);
        typeSource->RegisterPrefix(registration);
    }
}

void XamlCPP::Core::ProtoInstructionParser::SkipWhitespaces()
{
    while (reader->NodeType() == Interfaces::XmlNodeType::Whitespace)
    {
        reader->Read();
    }
}

void XamlCPP::Core::ProtoInstructionParser::ParseDirective(ProtoParser::DirectiveAssignment const& assignment)
{
    protoInstructions.emplace_back(instructionBuilder.Directive(assignment.GetDirective(), assignment.GetValue()));
}

void XamlCPP::Core::ProtoInstructionParser::ParseAttributeToNsPrefixDefinition(ProtoParser::NsPrefix const & prefix)
{
    protoInstructions.emplace_back(instructionBuilder.NamespacePrefixDeclaration(prefix.Prefix, prefix.Namespace));
}

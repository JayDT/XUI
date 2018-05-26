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

#include "Interfaces/IXamlCPP.h"
#include "InstructionParser.h"
#include "InstructionInject.h"
#include "Core/Parser/MarkupExtensionParser.h"
#include "ReflEx/Member.h"

#include <boost/algorithm/string/predicate.hpp>
#include "cpplinq/linq.hpp"

using namespace XamlCPP::Core;
using namespace XamlCPP::Interfaces;

InstructionParser::InstructionParser(IRuntimeTypeSource *typeSource) : typeSource(typeSource)
{
}

bool InstructionParser::getEndOfStream() const
{
    return privateEndOfStream;
}

void InstructionParser::setEndOfStream(const bool &value)
{
    privateEndOfStream = value;
}

bool InstructionParser::getCurrentNodeIsElement() const
{
    return getCurrentNodeType() == NodeType::Element || getCurrentNodeType() == NodeType::EmptyElement;
}

XamlCPP::Core::NodeType InstructionParser::getCurrentNodeType() const
{
    return instructionStreamIterator != instructionStream.end() ? (*instructionStreamIterator)->NodeType : NodeType::None;
}

bool InstructionParser::getIsNestedPropertyImplicit() const
{
    return getCurrentNodeType() != NodeType::PropertyElement && getCurrentNodeType() != NodeType::EmptyPropertyElement && getCurrentNodeType() != NodeType::EndTag;
}

bool InstructionParser::getCurrentNodeIsText() const
{
    return getCurrentNodeType() == NodeType::Text;
}

ProtoInstruction *InstructionParser::getCurrent() const
{
    return (*instructionStreamIterator).get();
}

std::string InstructionParser::getCurrentText() const
{
    return getCurrent()->Text;
}

std::string InstructionParser::getCurrentPropertyText() const
{
    return getCurrent()->PropertyAttributeText;
}

ReflEx::MemberBase *InstructionParser::getCurrentMember() const
{
    return getCurrent()->PropertyAttribute;
}

std::shared_ptr<std::vector<std::shared_ptr<XamlCPP::Core::Instruction>>> InstructionParser::Parse(const std::vector<std::shared_ptr<XamlCPP::Core::ProtoInstruction>>& protoNodes)
{
    instructionResult = std::make_shared<std::vector<std::shared_ptr<XamlCPP::Core::Instruction>>>();
    instructionResult->reserve(protoNodes.size() * 10);
    instructionStream = protoNodes;

    //std::ostringstream stream;
    //for (auto const& i : protoNodes)
    //{
    //    stream << i->ToString() << std::endl;
    //}
    //
    //std::string debug = stream.str();

    instructionStreamIterator = instructionStream.begin();
    setEndOfStream(false);
    ParsePrefixDefinitions();
    ParseElements();

    return instructionResult;
}

void XamlCPP::Core::InstructionParser::AddInstruction(std::shared_ptr<XamlCPP::Core::Instruction> const& inst)
{
    instructionResult->push_back(inst);
}

void InstructionParser::ParseElements()
{
    if (getCurrentNodeIsText())
    {
        AddInstruction(Inject::Value(getCurrentText()));
    }

    while (!getEndOfStream() && (getCurrent()->NodeType == NodeType::Element || getCurrent()->NodeType == NodeType::EmptyElement))
    {
        switch (getCurrent()->NodeType)
        {
            case NodeType::Element:
                ParseNonEmptyElement();
                break;
            case NodeType::EmptyElement:
                ParseEmptyElement();
                break;
        }

        // There may be text nodes after each element. Skip all of them.
        SkipTextNodes();
    }
}

void InstructionParser::SkipTextNodes()
{
    while (getCurrentNodeType() == NodeType::Text)
    {
        SetNextInstruction();
    }
}

void InstructionParser::ParseEmptyElement()
{
    AddInstruction(Inject::StartOfObject(getCurrent()->XamlType));

    SetNextInstruction();
    ParseMembersOfObject();

    AddInstruction(Inject::EndOfObject());

    if (getCurrentNodeType() == NodeType::Text)
        SetNextInstruction();
}

void InstructionParser::ParseNonEmptyElement()
{
    AddInstruction(Inject::StartOfObject(getCurrent()->XamlType));
    auto parentType = getCurrent()->XamlType;

    if (parentType->NeedsConstructionParameters())
    {
        InjectNodesForTypeThatRequiresInitialization();
    }
    else
    {
        SetNextInstruction();
        ParseMembersOfObject();

        if (parentType->IsCollection())
        {
            ParseItemsOfCollection();
        }
        else
        {
            ParseContentPropertyIfAny(parentType);
        }

        SkipTextNodes();
        ParseNestedProperties(parentType);
    }

    ReadEndTag();
}

void InstructionParser::ParseItemsOfCollection()
{
    AddInstruction(Inject::Items());
    ParseElements();
    AddInstruction(Inject::EndOfMember());
}

void InstructionParser::InjectNodesForTypeThatRequiresInitialization()
{
    SetNextInstruction();

    ParseMembersOfObject();

    AddInstruction(Inject::Initialization());
    AddInstruction(Inject::Value(getCurrentText()));
    AddInstruction(Inject::EndOfMember());
}

void InstructionParser::ReadEndTag()
{
    SkipTextNodes();

    if (getCurrentNodeType() != NodeType::EndTag)
    {
        throw ParseException("Expected End Tag");
    }

    AddInstruction(Inject::EndOfObject());
    SetNextInstruction();
}

void InstructionParser::ParseNestedProperties(Proto::XamlType *parentType)
{
    while (getCurrentNodeType() == NodeType::PropertyElement || getCurrentNodeType() == NodeType::EmptyPropertyElement)
    {
        auto member = getCurrent()->PropertyElement;
        if (member->XamlType->IsCollection())
        {
            SetNextInstruction();
            ParseCollectionInsideThisProperty(member);
        }
        else
        {
            ParseNestedProperty(member);
        }

        SkipTextNodes();

        // After and EndTag, there could be a ContentProperty! so we consider parsing it.
        if (getCurrentNodeType() == NodeType::EndTag)
        {
            SetNextInstruction();
            ParseContentPropertyIfAny(parentType);
        }
    }
}

void InstructionParser::ParseContentPropertyIfAny(Proto::XamlType *parentType)
{
    if (getIsNestedPropertyImplicit())
    {
        auto contentProperty = parentType->getContentProperty();

        if (contentProperty == nullptr)
        {
            if (getCurrent()->NodeType != NodeType::Text)
                throw ParseException("Forgotten implement ContentAttribute!");
            AddInstruction(Inject::Initialization());
            AddInstruction(Inject::Value(getCurrentText()));
            AddInstruction(Inject::EndOfMember());
        }
        else if (contentProperty->XamlType->IsCollection())
        {
            ParseCollectionInsideThisProperty(contentProperty);
        }
        else
        {
            AddInstruction(Inject::StartOfMember(contentProperty));
            ParseElements();
            AddInstruction(Inject::EndOfMember());
        }
    }
}

void InstructionParser::SetNextInstruction()
{
    if (getEndOfStream())
    {
        throw ParseException("The end of the stream has already been reached!");
    }

    if (instructionStreamIterator != instructionStream.end())
    {
        ++instructionStreamIterator;
        instructionIndex = instructionStreamIterator - instructionStream.begin();
    }
    setEndOfStream(instructionStreamIterator == instructionStream.end());
}

void InstructionParser::ParseCollectionInsideThisProperty(ReflEx::MemberBase *member)
{
    AddInstruction(Inject::StartOfMember(member));

    if (getIsBeginingOfImplicitCollection())
    {
        AddInstruction(Inject::GetObject());
        AddInstruction(Inject::Items());

        ParseElements();

        AddInstruction(Inject::EndOfMember());
        AddInstruction(Inject::EndOfObject());
    }
    else
    {
        ParseNonEmptyElement();
    }

    AddInstruction(Inject::EndOfMember());
}

bool InstructionParser::getIsBeginingOfImplicitCollection() const
{
    return getCurrent()->XamlType == nullptr || !getCurrent()->XamlType->IsCollection();
}

void InstructionParser::ParseNestedProperty(ReflEx::MemberBase *member)
{
    AddInstruction(Inject::StartOfMember(member));
    SetNextInstruction();
    ParseInnerContentOfNestedProperty();
    SkipTextNodes();
    AddInstruction(Inject::EndOfMember());
}

void InstructionParser::ParseInnerContentOfNestedProperty()
{
    if (getCurrentNodeType() == NodeType::Text)
    {
        AddInstruction(Inject::Value(getCurrent()->Text));
    }
    else
    {
        ParseElements();
    }
}

void InstructionParser::ParseMembersOfObject()
{
    while (getCurrentNodeType() == NodeType::Attribute && !getEndOfStream())
    {
        auto valueOfMember = getCurrentPropertyText();

        AddInstruction(Inject::StartOfMember(getCurrentMember()));

        if (IsMarkupExtension(valueOfMember))
        {
            ParseMarkupExtension(valueOfMember);
        }
        else
        {
            AddInstruction(Inject::Value(valueOfMember));
        }

        AddInstruction(Inject::EndOfMember());
        SetNextInstruction();
    }
}

void InstructionParser::ParseMarkupExtension(const std::string &valueOfMember)
{
    auto parser = MarkupExtensionParser(this);
    parser.Parse(valueOfMember);
}

void InstructionParser::ParsePrefixDefinitions()
{
    while (getCurrentNodeType() == NodeType::PrefixDefinition && !getEndOfStream())
    {
        auto protoXamlNode = getCurrent();
        AddInstruction(Inject::PrefixDefinitionOfNamespace(protoXamlNode));
        SetNextInstruction();
    }
}

bool InstructionParser::IsMarkupExtension(const std::string &text)
{
    return text.length() > 3 && boost::starts_with(text, "{") && boost::ends_with(text, "}");
}

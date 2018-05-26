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

#include "MarkupExtensionParser.h"
#include "InstructionInject.h"
#include "InstructionParser.h"
#include "Interfaces/IRuntimeTypeSource.h"
#include "Core/Proto/XamlType.h"
#include "Core/ReflEx/Member.h"

void XamlCPP::Core::MarkupExtensionParser::ParseMarkupExtensionNode(MarkupExtensionNode *tree)
{
    auto& identifierNode = tree->Identifier;
    auto xamlType = m_InstructionParser->GetTypeSource()->GetByPrefix(identifierNode.Prefix, identifierNode.TypeName);

    m_InstructionParser->AddInstruction(Inject::StartOfObject(xamlType));

    if (!tree->Options.empty())
    {
        std::vector<PositionalOption*> positionals;
		std::vector<PropertyOption*> positionalProperties;
		std::vector<PropertyOption*> properties;
        positionals.reserve(tree->Options.size());
        for (auto& option : tree->Options)
        {
            auto positional = option->shared_from_base_dynamic<PositionalOption>();
            if (positional)
                positionals.push_back(positional.get());
            else
            {
                auto prop = option->shared_from_base_dynamic<PropertyOption>();
				if (prop)
				{
					if (prop->Property.empty())
						positionalProperties.push_back(prop.get());
					else
						properties.push_back(prop.get());
				}
            }
        }

        ParseArguments(positionals, positionalProperties);
        ParseProperties(properties, xamlType);
    }
    m_InstructionParser->AddInstruction(Inject::EndOfObject());
}

void XamlCPP::Core::MarkupExtensionParser::ParseArguments(std::vector<PositionalOption*> &options, std::vector<PropertyOption*> &assigns)
{
    if (!options.empty() || !assigns.empty())
    {
        m_InstructionParser->AddInstruction(Inject::MarkupExtensionArguments());
        for (auto option : options)
        {
            m_InstructionParser->AddInstruction(Inject::Value(option->Identifier));
        }
		for (auto option : assigns)
		{
			if (option->Property.empty())
			{
				auto stringNode = option->Value->ToStringNode();
				if (stringNode != nullptr)
					m_InstructionParser->AddInstruction(Inject::Value(stringNode->Value));

				auto markupExtensionNode = option->Value->ToMarkupExtensionNode();
				if (markupExtensionNode != nullptr)
				{
					ParseMarkupExtensionNode(markupExtensionNode);
				}
			}
		}
		m_InstructionParser->AddInstruction(Inject::EndOfMember());
    }
}

void XamlCPP::Core::MarkupExtensionParser::ParseProperties(std::vector<PropertyOption*> &options, Proto::XamlType *xamlType)
{
    for (auto option : options)
    {
        auto member = xamlType->GetMember(option->Property);
        m_InstructionParser->AddInstruction(Inject::StartOfMember(member));

        if (option->Value)
        {
            auto stringNode = option->Value->ToStringNode();
            if (stringNode != nullptr)
            {
                m_InstructionParser->AddInstruction(Inject::Value(stringNode->Value));
            }

            auto markupExtensionNode = option->Value->ToMarkupExtensionNode();
            if (markupExtensionNode != nullptr)
            {
                ParseMarkupExtensionNode(markupExtensionNode);
            }
        }

        m_InstructionParser->AddInstruction(Inject::EndOfMember());
    }
}

void XamlCPP::Core::MarkupExtensionParser::Parse(std::string const & expression)
{
    MarkupExpressionParser parser(false);
    parser.Parse(expression);
    System::ThrowIfFailed<System::InvalidOperationException>(parser.pos == parser.length, "[Markup Parser]: internal error!");
    if (parser.RootNodes && parser.RootNodes->ToMarkupExtensionNode())
        ParseMarkupExtensionNode(parser.RootNodes->ToMarkupExtensionNode());
}

bool XamlCPP::Core::MarkupExpressionParser::Parse(std::string const & expression, State state /*= State::MarkupExtensionStart*/)
{
    pos = 0;
    expr = expression.c_str();
    length = expression.size();
    int markupOpened = 0;

    while (expr[pos] != '\0' && state != State::End && state != State::EndWithFailed)
    {
        switch (state)
        {
            case State::MarkupExtensionStart:
                state = ParseStart();
                if (state != State::EndWithFailed)
                    ++markupOpened;
                break;

            case State::XamlIdentifier:
                state = ParseXamlIdentifier();
                break;

            case State::Options:
                state = ParseOptions();
                break;

            case State::Assignment:
                state = ParseAssignment();
                break;

            case State::Positional:
                state = ParsePositional();
                break;

            case State::StringValue:
                state = ParseStringValue();
                break;
        }
    }

    while (state != State::EndWithFailed && markupOpened-- > 0)
    {
        SkipWhitespace();
        if (!ParseChar(MarkupExpressionParser::CloseCurly))
            System::ThrowIfFailed<System::InvalidOperationException>(pos == length, "[Markup Parser]: internal error!");
    }

    return state == State::End;
}

XamlCPP::Core::MarkupExpressionParser::State XamlCPP::Core::MarkupExpressionParser::ParseStart()
{
    if (ParseChar(OpenCurly))
    {
        RootNodes = std::make_shared<MarkupExtensionNode>();
        return State::XamlIdentifier;
    }
    return State::EndWithFailed;
}

XamlCPP::Core::MarkupExpressionParser::State XamlCPP::Core::MarkupExpressionParser::ParseXamlIdentifier()
{
    if (!RootNodes || !RootNodes->ToMarkupExtensionNode())
        return State::EndWithFailed;

    SkipWhitespace();
    std::string first = IdentifierParse();
    std::string second;
    if (ParseChar(Colon))
        second = IdentifierParse();

    RootNodes->ToMarkupExtensionNode()->Identifier.Prefix = second.empty() ? "" : first;
    RootNodes->ToMarkupExtensionNode()->Identifier.TypeName = (second.empty() ? first : second) + "Extension";
    return State::Options;
}

XamlCPP::Core::MarkupExpressionParser::State XamlCPP::Core::MarkupExpressionParser::ParseOptions()
{
    if (!RootNodes || !RootNodes->ToMarkupExtensionNode())
        return State::EndWithFailed;

    SkipWhitespace();
    if (ParseChar(CloseCurly))
        return State::End;

    MarkupExpressionParser optionsParser(false);
    if (optionsParser.Parse(GetRemainingString(), State::Assignment))
    {
        System::ThrowIfFailed<System::InvalidOperationException>(optionsParser.CurrentAssignment != nullptr);
        pos += optionsParser.pos;
        EXCEPT(pos < length);
        auto PropertyOptions = std::make_shared<PropertyOption>(optionsParser.CurrentAssignment->Property, optionsParser.CurrentAssignment->NodeValue);
        RootNodes->ToMarkupExtensionNode()->Options.push_back(PropertyOptions);
    }
    else if (optionsParser.Parse(GetRemainingString(), State::Positional))
    {
        System::ThrowIfFailed<System::InvalidOperationException>(optionsParser.CurrentOption != nullptr);
        pos += optionsParser.pos;
        EXCEPT(pos < length);
        RootNodes->ToMarkupExtensionNode()->Options.push_back(optionsParser.CurrentOption);
    }
    else
        return State::EndWithFailed;

    SkipWhitespace();
    if (ParseChar(Comma))
    {
        ParseOptions();
    }

    return State::End;
}

XamlCPP::Core::MarkupExpressionParser::State XamlCPP::Core::MarkupExpressionParser::ParseAssignment()
{
	SkipWhitespace();
	std::string firstId = IdentifierParse();
	SkipWhitespace();
	if (ParseChar(EqualSign))
	{
		SkipWhitespace();
		CurrentAssignment = std::make_shared<AssignmentNode>(firstId, nullptr);
		MarkupExpressionParser AssignmentSource(false);

		// MarkupExtensionWithOptions Or SimpleMarkupExtension
		if (AssignmentSource.Parse(GetRemainingString(), State::MarkupExtensionStart) && (AssignmentSource.RootNodes && AssignmentSource.RootNodes->ToMarkupExtensionNode()))
		{
			pos += AssignmentSource.pos;
			EXCEPT(pos < length);
			CurrentAssignment->NodeValue = AssignmentSource.RootNodes;
			return State::End;
		}
		else // StringValueNode
		{
			AssignmentSource.RootNodes = nullptr;
			if (AssignmentSource.Parse(GetRemainingString(), State::StringValue))
			{
				pos += AssignmentSource.pos;
				EXCEPT(pos < length);
				CurrentAssignment->NodeValue = AssignmentSource.RootNodes;
				return State::End;
			}
		}
	}
	else if (expr[pos] == OpenCurly)
	{
		CurrentAssignment = std::make_shared<AssignmentNode>("", nullptr);
		MarkupExpressionParser AssignmentSource(false);

		if (AssignmentSource.Parse(GetRemainingString(), State::MarkupExtensionStart) && (AssignmentSource.RootNodes && AssignmentSource.RootNodes->ToMarkupExtensionNode()))
		{
			pos += AssignmentSource.pos;
			EXCEPT(pos < length);
			CurrentAssignment->NodeValue = AssignmentSource.RootNodes;
			return State::End;
		}
	}

    return State::EndWithFailed;
}

XamlCPP::Core::MarkupExpressionParser::State XamlCPP::Core::MarkupExpressionParser::ParsePositional()
{
    SkipWhitespace();

    std::string ret;
    while (expr[pos] != '\0' && (
        IsLetter(expr[pos]) ||
        isdigit(expr[pos]) ||
        IsCharInSeqvence(expr[pos], ':', '.', '[', ']', '(', ')', '!', '$', '#', '^', '_')))
        ret += expr[pos++];

    if (ret.empty())
        return State::EndWithFailed;

    CurrentOption = std::make_shared<PositionalOption>(ret);
    return State::End;
}

XamlCPP::Core::MarkupExpressionParser::State XamlCPP::Core::MarkupExpressionParser::ParseStringValue()
{
    SkipWhitespace();

    std::string ret;
    if (ParseChar(Quote))
    {
        while (expr[pos] != '\0' && !IsCharInSeqvence(expr[pos], Quote, OpenCurly, CloseCurly))
            ret += expr[pos++];

        if (!ParseChar(Quote))
            return State::EndWithFailed;
    }
    else
    {
        while (expr[pos] != '\0' && (
            IsLetter(expr[pos]) ||
            isdigit(expr[pos]) ||
            IsCharInSeqvence(expr[pos], ':', '.', '[', ']', '(', ')', '!', '$', '#', '^')))
            ret += expr[pos++];

        if (ret.empty())
            return State::EndWithFailed;
    }

    RootNodes = std::make_shared<StringNode>(ret);

    return State::End;
}

bool XamlCPP::Core::MarkupExpressionParser::IsLetter(char ch) const
{
    return ((ch <= 'z' && ch >= 'a') || (ch <= 'Z' && ch >= 'A'));
}

bool XamlCPP::Core::MarkupExpressionParser::IsValidIdentiferChar(char ch) const
{
    return expr[pos] == '_' || IsLetter(expr[pos]);
}

std::string XamlCPP::Core::MarkupExpressionParser::IdentifierParse()
{
    std::string ret;
    while (expr[pos] != '\0' && IsValidIdentiferChar(expr[pos]))
        ret += expr[pos++];
    return ret;
}

void XamlCPP::Core::MarkupExpressionParser::SkipWhitespace()
{
    while (expr[pos] != '\0' && isspace(expr[pos]))
    {
        ++pos;
    }
}

bool XamlCPP::Core::MarkupExpressionParser::TakeIf(char c)
{
    if (expr[pos] != '\0' && expr[pos] == c)
    {
        ++pos;
        return true;
    }
    else
    {
        return false;
    }
}

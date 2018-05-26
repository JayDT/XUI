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

#include "MarkupExpressionAssembler.h"
#include "DataBind/SourceObserver.h"

XamlCPP::Core::ExpressionAssembler::ExpressionAssembler(bool enableValidation)
    : EnableValidation(enableValidation)
{ }

void XamlCPP::Core::ExpressionAssembler::Parse(std::string const & expression)
{
    pos = 0;
    expr = expression.c_str();
    length = expression.size();
    State state = State::Start;

    while (expr[pos] != '\0' && state != State::End)
    {
        switch (state)
        {
            case State::Start:
                state = ParseStart();
                break;

            case State::AfterMember:
                state = ParseAfterMember();
                break;

            case State::BeforeMember:
                state = ParseBeforeMember();
                break;

            case State::AttachedProperty:
                state = ParseAttachedProperty();
                break;
        }
    }

    if (state == State::BeforeMember || Nodes.empty())
    {
        throw System::InvalidOperationException("Unexpected end of expression.");
    }

    for (int n = 0; n < Nodes.size() - 1; ++n)
    {
        Nodes[n]->Next = Nodes[n + 1];
    }
}

XamlCPP::Core::ExpressionAssembler::State XamlCPP::Core::ExpressionAssembler::ParseStart()
{
    if (ParseNot())
    {
        Nodes.push_back(std::make_shared<XamlCPP::DataBind::LogicalNotNode>());
        return State::Start;
    }
    else if (ParseOpenBrace())
    {
        return State::AttachedProperty;
    }
    else
    {
        auto identifier = IdentifierParse();

        if (!identifier.empty())
        {
            Nodes.push_back(std::make_shared<XamlCPP::DataBind::PropertyAccessorNode>(identifier, EnableValidation));
            return State::AfterMember;
        }
    }

    return State::End;
}

XamlCPP::Core::ExpressionAssembler::State XamlCPP::Core::ExpressionAssembler::ParseAfterMember()
{
    if (ParseMemberAccessor())
    {
        return State::BeforeMember;
    }
    else if (ParseStreamOperator())
    {
        Nodes.push_back(std::make_shared<XamlCPP::DataBind::StreamNode>());
        return State::AfterMember;
    }
    else
    {
        auto args = ArgumentParse();

        if (!args.empty())
        {
            Nodes.push_back(std::make_shared<XamlCPP::DataBind::IndexerNode>(args));
            return State::AfterMember;
        }
    }

    return State::End;
}

XamlCPP::Core::ExpressionAssembler::State XamlCPP::Core::ExpressionAssembler::ParseBeforeMember()
{
    if (ParseOpenBrace())
    {
        return State::AttachedProperty;
    }
    else
    {
        auto identifier = IdentifierParse();

        if (!identifier.empty())
        {
            Nodes.push_back(std::make_shared<XamlCPP::DataBind::PropertyAccessorNode>(identifier, EnableValidation));
            return State::AfterMember;
        }

        return State::End;
    }
}

XamlCPP::Core::ExpressionAssembler::State XamlCPP::Core::ExpressionAssembler::ParseAttachedProperty()
{
    auto owner = IdentifierParse();

    if (!TakeIf('.'))
    {
        throw System::ArgumentException("Invalid attached property name.");
    }

    auto name = IdentifierParse();

    if (!TakeIf(')'))
    {
        throw System::ArgumentException("Expected ')'.");
    }

    Nodes.push_back(std::make_shared<XamlCPP::DataBind::PropertyAccessorNode>(owner + '.' + name, EnableValidation));
    return State::AfterMember;
}

std::vector<std::string> XamlCPP::Core::ExpressionAssembler::ArgumentParse()
{
    std::vector<std::string> ret;
    if (expr[pos] != '[')
        return ret;

    while (expr[pos] != '\0')
    {
        std::string arg;
        while (expr[pos] != '\0' && expr[pos] != ',' && expr[pos] != ']')
        {
            if (!isspace(expr[pos]))
                arg += IsValidIdentiferChar(expr[pos++]);
            else
                ++pos;
        }

        if (expr[pos] == '\0')
            throw System::ArgumentException("Invalid Binding Argument indexer");

        if (arg.empty())
            return ret;

        SkipWhitespace();

        ret.push_back(arg);

        if (expr[pos] != ']')
        {
            ++pos;
            break;
        }

        if (expr[pos] != ',')
            throw System::ArgumentException("Invalid Binding Argument indexer");

        ++pos;
        SkipWhitespace();
    }

    return ret;
}

std::string XamlCPP::Core::ExpressionAssembler::IdentifierParse()
{
    std::string ret;
    while (expr[pos] != '\0' && IsValidIdentiferChar(expr[pos]))
        ret += expr[pos++];
    return ret;
}

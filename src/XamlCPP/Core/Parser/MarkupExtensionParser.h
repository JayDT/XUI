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

#ifndef __XAMLCPP_XML_CORE_MARKUP_PARSER_H__
#define __XAMLCPP_XML_CORE_MARKUP_PARSER_H__

#include "Interfaces/IParser.h"
#include "Reflection/Runtime/CXXReflection.h"
#include "Core/Parser/MarkupExtensionContext.h"

namespace XamlCPP::Core
{
    class InstructionParser;

    namespace Proto
    {
        class XamlType;
    };

    class MarkupExtensionParser
    {
        InstructionParser* m_InstructionParser;

        void ParseMarkupExtensionNode(MarkupExtensionNode *tree);
        void ParseArguments(std::vector<PositionalOption*> &options, std::vector<PropertyOption*> &assigns);
        void ParseProperties(std::vector<PropertyOption*> &options, Proto::XamlType *xamlType);

    public:

        MarkupExtensionParser(InstructionParser* instParser)
            : m_InstructionParser(instParser)
        {

        }


        void Parse(std::string const& expression);
    };

    struct MarkupExpressionParser
    {
        enum State
        {
            MarkupExtensionStart,
            XamlIdentifier,
            StringValue,
            Assignment,
            Positional,
            Attribute,
            Options,
            End,
            EndWithFailed,
        };

        bool EnableValidation = false;
        std::shared_ptr<XamlCPP::Core::TreeNode> RootNodes;
        std::shared_ptr<XamlCPP::Core::Option> CurrentOption;
        std::shared_ptr<XamlCPP::Core::AssignmentNode> CurrentAssignment;
        size_t pos = 0;
        size_t length = 0;
        const char* expr = nullptr;

        static constexpr const char Quote = '\'';
        static constexpr const char OpenCurly = '{';
        static constexpr const char CloseCurly = '}';
        static constexpr const char Comma = ',';
        static constexpr const char Colon = ':';
        static constexpr const char EqualSign = '=';

        MarkupExpressionParser(bool enableValidation)
            : EnableValidation(enableValidation)
        { }

        bool Parse(std::string const& expression, State state = State::MarkupExtensionStart);

        std::string GetRemainingString()
        {
            return std::string(&expr[pos]);
        }

    private:

        State ParseStart();

        State ParseXamlIdentifier();

        State ParseOptions();

        State ParseAssignment();

        State ParsePositional();

        State ParseStringValue();

        void SkipWhitespace();

        bool IsLetter(char ch) const;

        bool IsValidIdentiferChar(char ch) const;

        bool IsCharInSeqvence(char t, char c)
        {
            return t == c;
        }

        template<typename... ARGS>
        bool IsCharInSeqvence(char t, char c, ARGS... args)
        {
            return IsCharInSeqvence(t, c) || IsCharInSeqvence(t, args...);
        }

        std::string IdentifierParse();

        bool ParseChar(char c)
        {
            return expr[pos] != '\0' && TakeIf(c);
        }

        template<typename... ARGS>
        bool ParseChar(char c, ARGS... args)
        {
            return ParseChar(c) || ParseChar(args...);
        }

        bool TakeIf(char c);
    };
};

#endif // !__XAMLCPP_XML_CORE_MARKUP_PARSER_H__
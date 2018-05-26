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

#ifndef __XAMLCPP_XML_CORE_MARKUP_EXPRESSION_PARSER_H__
#define __XAMLCPP_XML_CORE_MARKUP_EXPRESSION_PARSER_H__

#include "Interfaces/IParser.h"
#include "Reflection/Runtime/CXXReflection.h"
#include "Core/Parser/MarkupExtensionContext.h"

namespace XamlCPP::DataBind
{
    class SourceObserverNode;
}

namespace XamlCPP::Core
{
    class InstructionParser;

    namespace Proto
    {
        class XamlType;
    };

    struct ExpressionAssembler
    {
        enum State
        {
            Start,
            AfterMember,
            BeforeMember,
            AttachedProperty,
            End,
        };

        bool EnableValidation = false;
        std::vector<std::shared_ptr<XamlCPP::DataBind::SourceObserverNode>> Nodes;
        size_t pos = 0;
        size_t length = 0;
        const char* expr = nullptr;

        ExpressionAssembler(bool enableValidation);

        void Parse(std::string const& expression);

    private:

        State ParseStart();

        State ParseAfterMember();

        State ParseBeforeMember();

        State ParseAttachedProperty();

        std::vector<std::string> ArgumentParse();

        bool IsLetter(char ch) const
        {
            return ((ch <= 'z' && ch >= 'a') || (ch <= 'Z' && ch >= 'A'));
        }

        bool IsValidIdentiferChar(char ch) const
        {
            return expr[pos] == '_' || IsLetter(expr[pos]);
        }

        std::string IdentifierParse();

        bool ParseNot()
        {
            return expr[pos] != '\0' && TakeIf('!');
        }

        bool ParseMemberAccessor()
        {
            return expr[pos] != '\0' && TakeIf('.');
        }

        bool ParseOpenBrace()
        {
            return expr[pos] != '\0' && TakeIf('(');
        }

        bool ParseStreamOperator()
        {
            return expr[pos] != '\0' && TakeIf('^');
        }

        void SkipWhitespace()
        {
            while (expr[pos] != '\0' && isspace(expr[pos]))
            {
                ++pos;
            }
        }

        bool TakeIf(char c)
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
    };
};

#endif // !__XAMLCPP_XML_CORE_MARKUP_EXPRESSION_PARSER_H__
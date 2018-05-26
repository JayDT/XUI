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

#ifndef __XAMLCPP_ORDERAWAREINSTRUCTIONPARSER_H__
#define __XAMLCPP_ORDERAWAREINSTRUCTIONPARSER_H__

#include "Interfaces/IParser.h"
#include "Reflection/Runtime/CXXReflection.h"
#include "standard/events.h"

namespace XamlCPP::Core
{
    class OrderAwareInstructionParser : public Interfaces::IInstructionParser
    {
    private:
        std::shared_ptr<IInstructionParser> parser;

    public:
        virtual ~OrderAwareInstructionParser()
        {
        }

        OrderAwareInstructionParser(std::shared_ptr<IInstructionParser> const& parser) : parser(parser)
        {
        }

        std::shared_ptr<std::vector<std::shared_ptr<XamlCPP::Core::Instruction>>> Parse(const std::vector<std::shared_ptr<XamlCPP::Core::ProtoInstruction>>& protoNodes) override;
    };
};

#endif // !__XAMLCPP_ORDERAWAREINSTRUCTIONPARSER_H__
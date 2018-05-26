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

#ifndef __XAMLCPP_XML_CORE_PHASE_PARSER_KIT_H__
#define __XAMLCPP_XML_CORE_PHASE_PARSER_KIT_H__

#include "Interfaces/IParser.h"
#include "Reflection/Runtime/CXXReflection.h"
#include "standard/events.h"

namespace XamlCPP::Core
{
    class PhaseParserKit
    {
        Interfaces::IProtoParser* protoParser;
        Interfaces::IInstructionParser* parser;
        Interfaces::IObjectAssembler* objectAssembler;

    public:

        PhaseParserKit(
            Interfaces::IProtoParser* protoParser,
            Interfaces::IInstructionParser* parser,
            Interfaces::IObjectAssembler* objectAssembler)
        {
            this->protoParser = protoParser;
            this->parser = parser;
            this->objectAssembler = objectAssembler;
        }

        virtual ~PhaseParserKit()
        {
            delete protoParser;
            delete parser;
            delete objectAssembler;
        }

        PROPERTY_TG(Interfaces::IProtoParser*, ProtoParser)
        virtual PROPERTY_T_GET(ProtoParser, ) const { return protoParser; }

        PROPERTY_TG(Interfaces::IInstructionParser*, Parser)
        virtual PROPERTY_T_GET(Parser, ) const { return parser; }

        PROPERTY_TG(Interfaces::IObjectAssembler*, ObjectAssembler)
        virtual PROPERTY_T_GET(ObjectAssembler, ) const { return objectAssembler; }
    };
};

#endif // !__XAMLCPP_XML_CORE_PHASE_PARSER_KIT_H__
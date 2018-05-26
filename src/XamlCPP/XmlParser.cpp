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
#include "XmlParser.h"
#include "standard/Errors.h"
#include "Instruction.h"

#include "Core/XML/DefaultXmlReader.h"
#include "Core/Parser/PhaseParserKit.h"

using namespace XamlCPP;

System::Reflection::RefObject XamlCPP::XmlParser::Parse(const std::vector<std::shared_ptr<Core::Instruction>>& xamlNodes)
{
    //std::string debugstr = objectAssembler->ToString(xamlNodes);
    for (int i = 0; i != xamlNodes.size(); ++i)
        objectAssembler->Process(xamlNodes[i].get());

    return objectAssembler->getResult();
}

XmlParser::XmlParser(const Core::PhaseParserKit* phaseParserKit)
    : phaseParserKit(phaseParserKit)
{
    System::ThrowIfFailed<System::ArgumentException>(phaseParserKit, nameof(phaseParserKit));
    objectAssembler = phaseParserKit->ObjectAssembler;
    protoParser = phaseParserKit->ProtoParser;
    parser = phaseParserKit->Parser;
}

System::Reflection::RefObject XamlCPP::XmlParser::Parse(Interfaces::IXmlReader* stream)
{
    std::vector<std::shared_ptr<XamlCPP::Core::ProtoInstruction>> xamlProtoNodes = protoParser->Parse(stream);
    std::shared_ptr<std::vector<std::shared_ptr<Core::Instruction>>> xamlNodes = parser->Parse(xamlProtoNodes);
    return Parse(*xamlNodes.get());
}

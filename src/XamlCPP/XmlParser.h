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

#ifndef __XAMLCPP_XML_PARSER_H__
#define __XAMLCPP_XML_PARSER_H__

#include <memory>
#include "standard/BasicPrimities.h"
#include "Reflection/Runtime/CXXReflection.h"
#include "Interfaces/IDeferredLoader.h"
#include "Interfaces/IParser.h"
#include "Interfaces/IAssemblers.h"
#include "Interfaces/IXmlReader.h"
#include "Core/Parser/PhaseParserKit.h"

#include <exception>

namespace XamlCPP
{
    struct LoadException : std::exception
    {
        int LineNumber = -1;
        int LinePosition = -1;

        LoadException() {}
        LoadException(const char* msg, int LineNumber, int LinePosition)
            : std::exception(msg)
            , LineNumber(LineNumber)
            , LinePosition(LinePosition) {}
    };

    class XmlParser : public Interfaces::IParser
    {
    private:
        const Core::PhaseParserKit* const phaseParserKit;
        Interfaces::IObjectAssembler* objectAssembler = nullptr;
        Interfaces::IProtoParser* protoParser = nullptr;
        Interfaces::IInstructionParser* parser = nullptr;

        virtual System::Reflection::RefObject Parse(const std::vector<std::shared_ptr<Core::Instruction>>& xamlNodes);

    public:
        XmlParser(const Core::PhaseParserKit* phaseParserKit);
        virtual ~XmlParser()
        {
            delete phaseParserKit;
        }

        System::Reflection::RefObject Parse(Interfaces::IXmlReader* stream) override;
    };
}

#endif // !__XAMLCPP_XML_PARSER_H__

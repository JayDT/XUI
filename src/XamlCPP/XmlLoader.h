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

#ifndef __XAMLCPP_XML_LOADER_H__
#define __XAMLCPP_XML_LOADER_H__

#include <memory>
#include "standard/BasicPrimities.h"
#include "Reflection/Runtime/CXXReflection.h"
#include "Interfaces/IDeferredLoader.h"
#include "Interfaces/ILoader.h"
#include "Interfaces/IParser.h"
#include "Interfaces/IAssemblers.h"
#include "Interfaces/IXmlReader.h"
#include "Core/Parser/PhaseParserKit.h"
#include "Runtime/ReflectionModule.h"

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

    class XmlLoader : public Interfaces::ILoader
    {
    private:
        Interfaces::IParserFactory* parserFactory = nullptr;
        Interfaces::IXmlReader* xmlReader = nullptr;
        Interfaces::DeferredLoaderMapping mapping;

        System::Reflection::RefObject Load(const std::ostringstream& stream, const Interfaces::IParser* parser);

    public:
        XmlLoader(const Interfaces::IParserFactory* parserFactory);

        System::Reflection::RefObject Load(const std::ostringstream& stream, const Core::Assembler::Settings* loadSettings) override;

        PROPERTY_TG(Interfaces::DeferredLoaderMapping, DeferredMapping)
            PROPERTY_T_GET(DeferredMapping, const&) const { return mapping; }
    };
}

#endif // !__XAMLCPP_XML_LOADER_H__

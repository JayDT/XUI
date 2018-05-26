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
#include "XmlLoader.h"
#include "standard/Errors.h"

#include "Core/XML/DefaultXmlReader.h"

using namespace XamlCPP;

System::Reflection::RefObject XamlCPP::XmlLoader::Load(const std::ostringstream & stream, const Interfaces::IParser * parser)
{
    try
    {
        xmlReader = new Core::XML::XmlDefaultReader(stream);
        return const_cast<Interfaces::IParser*>(parser)->Parse(xmlReader);
    }
    catch (std::exception e)
    {
        throw LoadException(System::String::format("Error loading XAML: %d %d %s", xmlReader->LineNumber, xmlReader->LinePosition, e.what()).c_str(), xmlReader->LineNumber, xmlReader->LinePosition);
    }
}

XamlCPP::XmlLoader::XmlLoader(const Interfaces::IParserFactory * parserFactory)
{
    this->parserFactory = (Interfaces::IParserFactory*)parserFactory;
}

System::Reflection::RefObject XamlCPP::XmlLoader::Load(const std::ostringstream & stream, const Core::Assembler::Settings * loadSettings)
{
    auto parser = std::unique_ptr<Interfaces::IParser>(parserFactory->Create(loadSettings));
    return Load(stream, parser.get());
}

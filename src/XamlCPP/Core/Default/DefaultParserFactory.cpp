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
#include "DefaultParserFactory.h"
#include "Core/ProtoInstructionParser.h"
#include "Core/Parser/PhaseParserKit.h"
#include "Core/Parser/OrderAwareInstructionParser.h"
#include "Core/Parser/InstructionParser.h"
#include "Core/Assembler/TopDownValueContext.h"
#include "Core/Assembler/ObjectAssembler.h"
#include "Core/ReflEx/ValueContext.h"
#include "XmlParser.h"

using namespace XamlCPP;

XamlCPP::Core::DefaultParserFactory::DefaultParserFactory(std::shared_ptr<Interfaces::IRuntimeTypeSource> const& runtimeTypeSource)
    : runtimeTypeSource(runtimeTypeSource)
{
}

Interfaces::IParser * XamlCPP::Core::DefaultParserFactory::CreateParser(Interfaces::IObjectAssembler * objectAssemblerForUndefinedRoot)
{
    auto xamlInstructionParser = new OrderAwareInstructionParser(std::make_shared<InstructionParser>(runtimeTypeSource.get()));
    auto phaseParserKit = new PhaseParserKit(new ProtoInstructionParser(runtimeTypeSource.get()), xamlInstructionParser, objectAssemblerForUndefinedRoot);
    
    return new XmlParser(phaseParserKit);
}

Interfaces::IParser * XamlCPP::Core::DefaultParserFactory::Create(const Core::Assembler::Settings * settings, Interfaces::DeferredLoaderMapping * mapping)
{
    Assembler::ObjectAssembler* objectAssemblerForUndefinedRoot = new Assembler::ObjectAssembler(runtimeTypeSource.get(), std::make_shared<ReflEx::ValueContext>(runtimeTypeSource, std::make_shared<Assembler::TopDownValueContext>()), (Core::Assembler::Settings *)settings);
    return CreateParser(objectAssemblerForUndefinedRoot);
}

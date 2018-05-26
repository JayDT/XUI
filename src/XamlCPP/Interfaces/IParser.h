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

#ifndef __XAMLCPP_XML_IPARSERS_H__
#define __XAMLCPP_XML_IPARSERS_H__

#include "Interfaces/IDeferredLoader.h"
#include <standard/collection.h>
#include <memory>
#include <vector>
#include <string>

namespace XamlCPP::Core
{
    struct ProtoInstruction;

    namespace Proto
    {
        class XamlType;
    }

    namespace Assembler
    {
        class Settings;
        class StoredInstance;
    }
}

namespace XamlCPP::Interfaces
{
    struct IXmlReader;
    typedef std::ostringstream StreamReader;

    struct IParser
    {
        virtual System::Reflection::RefObject Parse(IXmlReader* stream) = 0;
    };

    struct IParserFactory
    {
        virtual IParser* Create(const Core::Assembler::Settings* settings, Interfaces::DeferredLoaderMapping *mapping = nullptr) = 0;
    };

    struct IProtoParser
    {
        virtual std::vector<std::shared_ptr<XamlCPP::Core::ProtoInstruction>> Parse(IXmlReader* stream) = 0;
    };

    struct IInstructionParser
    {
        virtual std::shared_ptr<std::vector<std::shared_ptr<XamlCPP::Core::Instruction>>> Parse(const std::vector<std::shared_ptr<XamlCPP::Core::ProtoInstruction>>& in) = 0;
    };

    struct ITopDownValueContext
    {
        virtual void Add(System::Reflection::Object* instance, XamlCPP::Core::Proto::XamlType* xamlType) = 0;
        virtual System::Reflection::Object* GetLastInstance(XamlCPP::Core::Proto::XamlType* xamlType) = 0;
        virtual std::set<XamlCPP::Core::Assembler::StoredInstance> const& GetStoredInstances() const = 0;
    };
}

#endif // !__XAMLCPP_XML_PARSER_H__
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

#pragma once

#include <System/System.h>
#include <standard/collection.h>

namespace XamlCPP::Core::Assembler
{
    class StateCommuter;
}

namespace XamlCPP::Interfaces
{
    struct ITypeRepository;
    struct ITopDownValueContext;

    struct IValueContext
    {
        virtual ~IValueContext() {}
        virtual ITypeRepository* getTypeRepository() const = 0;
        virtual ITopDownValueContext* getTopDownValueContext() const = 0;
        virtual System::Collection::Map<std::string, System::Reflection::Variant> const&  getParsingDictionary() const = 0;

        //ToDo: Small hack for fast implement need rework
        XamlCPP::Core::Assembler::StateCommuter* m_StateCommuter = nullptr;
        std::string ClrClass;
        std::string ClrSubClass;
    };
}

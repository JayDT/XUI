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


//File: ITypeFactory.h

#pragma once
#include "System/System.h"
#include "Reflection/Runtime/Type.h"
#include "Reflection/Runtime/Variant.h"

namespace XamlCPP::Interfaces
{
    struct ITypeFactory
    {
        virtual ~ITypeFactory() {}
        virtual std::shared_ptr<System::Reflection::Object> Create(const System::Reflection::Type* type, std::vector<System::Reflection::Variant> const& args) = 0;
        virtual bool CanCreate(const System::Reflection::Type* type) = 0;
    };
}

//End of file


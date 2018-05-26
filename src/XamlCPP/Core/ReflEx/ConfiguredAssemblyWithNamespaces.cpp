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
#include "ConfiguredAssemblyWithNamespaces.h"
#include "cpplinq/linq.hpp"

const System::Reflection::Type * XamlCPP::Core::ReflEx::ConfiguredAssemblyWithNamespaces::Get(std::string const & typeName)
{
    for (std::string const& ns : Strings)
    {
        std::string fullName = ns + "::" + typeName;
        System::Reflection::Type const* type = cpplinq::from(Assembly->GetTypes()).first_or_default([&](System::Reflection::Type const* t) { return fullName == t->fullname; });
        if (type != nullptr)
            return type;
    }
    return nullptr;
}

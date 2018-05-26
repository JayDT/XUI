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
#include "ClrNamespace.h"
#include "Reflection/Runtime/ReflectionModule.h"

XamlCPP::Core::ReflEx::ClrNamespace::ClrNamespace(System::Reflection::meta::ReflectionModule * assembly, const std::string & ns)
    : assembly(assembly), ns(ns)
{
    this->Name = ns;
}

System::Reflection::meta::ReflectionModule * XamlCPP::Core::ReflEx::ClrNamespace::getAssembly() const
{
    return assembly;
}

std::string XamlCPP::Core::ReflEx::ClrNamespace::getNamespace() const
{
    return ns;
}

System::Reflection::Type const* XamlCPP::Core::ReflEx::ClrNamespace::Get(const std::string & typeName)
{
    std::string fullName = ns + "::" + typeName;
    return &assembly->GetType(fullName);
}

int XamlCPP::Core::ReflEx::ClrNamespace::GetHashCode()
{
    return 0; // (assembly->GetHashCode() * 397) ^ ns.GetHashCode();
}

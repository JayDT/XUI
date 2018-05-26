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
#include "TypeFactory.h"
#include "Runtime/TypeCreator.h"

std::shared_ptr<System::Reflection::Object> XamlCPP::Core::ReflEx::TypeFactory::Create(System::Reflection::Type const * type, std::vector<System::Reflection::Variant> const& args)
{
    return System::Reflection::meta::TypeCreator::CreateSharedEx(*type, args);
}

bool XamlCPP::Core::ReflEx::TypeFactory::CanCreate(System::Reflection::Type const * type)
{
    return true;
}

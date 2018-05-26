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

#ifndef _XAMLCPP_PCH_COMMON_H_
#define _XAMLCPP_PCH_COMMON_H_

// Required for Boost (1.64)
#define _HAS_AUTO_PTR_ETC	1

namespace System::Reflection::Assembly
{
    class AssemblyModulXamlCPP;
}

// Workaround 15.5 bug (Pending release will fix this)
#pragma float_control(except, off, push)
#include <numeric>
#pragma float_control(pop)

#include <standard/Platform/Common.h>
#include <standard/BasicPrimities.h>
#include <standard/Errors.h>
#include <standard/events.h>
#include <standard/callback.h>
#include <standard/misc.h>
#include <System/BuildInMetaAttributes.h>

#include <rxcpp/rx.hpp>

#include "Reflection/Runtime/CXXReflection.h"
#include "Reflection/Runtime/Variant.h"
#include "Reflection/Runtime/Object.h"

#include "XamlCPP/Interfaces/IXamlCPP.h"
#include "XamlCPP/Interfaces/ReflAttributes.h"

MetaAssemblyBegin
Meta(assembly: System::Reflection::AssemblyTitle("C++ Xaml parser"))
Meta(assembly: System::Reflection::AssemblyDescription(""))

Meta(assembly: XamlCPP::XmlnsDefinition("http://git.tauri.hu/Xaml", "XamlCPP::Markup"))
Meta(assembly: XamlCPP::XmlnsDefinition("http://git.tauri.hu/Xaml", "XamlCPP::DataBind"))

Meta(assembly: XamlCPP::XmlnsDefinition("http://git.tauri.hu/XUI", "XamlCPP::Markup"))
Meta(assembly: XamlCPP::XmlnsDefinition("http://git.tauri.hu/XUI", "XamlCPP::DataBind"))
MetaAssemblyEnd

#endif // _XAMLCPP_PCH_COMMON_H_

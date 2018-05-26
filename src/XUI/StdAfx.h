/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2014 Steven Kirk <The Avalonia Project>
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

#ifndef _XUI_PCH_COMMON_H_
#define _XUI_PCH_COMMON_H_

// Required for Boost (1.64)
#define _HAS_AUTO_PTR_ETC	1

namespace System::Reflection::Assembly
{
    class AssemblyModulXUI;
}

// Workaround 15.5 bug (Pending release will fix this) https://developercommunity.visualstudio.com/content/problem/162020/c3199-including-in-155.html
#pragma float_control(except, off, push)
#include <numeric>
#pragma float_control(pop)

#include <standard/Platform/Common.h>
#include <standard/BasicPrimities.h>
#include <standard/Errors.h>
#include <standard/events.h>
#include <standard/callback.h>
#include <standard/misc.h>
#include <standard/collection.h>
#include <System/BuildInMetaAttributes.h>
#include <System/rtString.h>
#include <System/System.h>

#include "Reflection/Runtime/CXXReflection.h"
#include "Reflection/Runtime/Variant.h"
#include "Reflection/Runtime/Object.h"

#include <rxcpp/rx.hpp>
#include <cpplinq/linq.hpp>

#include "XamlCPP/Interfaces/IXamlCPP.h"
#include "XamlCPP/Interfaces/ReflAttributes.h"
#include "XamlCPP/Interfaces/IStyle.h"
#include "XamlCPP/Interfaces/IStyleHost.h"
#include "XamlCPP/Interfaces/ISupportInitialize.h"
#include "XamlCPP/Interfaces/INamedScope.h"
#include "XamlCPP/Interfaces/IResourceMap.h"

#include <stack>
#include <memory>
#include <functional>
#include <future>
#include <atomic>
#include <boost/uuid/uuid.hpp>

#include <G3D/Matrix4.h>
#include <G3D/Vector4.h>
#include <G3D/ConvexPolyhedron.h>
#include <G3D/Rect2D.h>
#include <G3D/Color4uint8.h>

namespace System::Reflection::Assembly
{
    class AssemblyModulXUI;
}

struct __TemplateWorkAround__ : System::Reflection::Object
{
    System::Collection::Vector<System::String> __template0;
};

#define _ALLOW_KEYWORD_MACROS true
// open back door
#   define protected friend class System::Reflection::Assembly::AssemblyModulXUI; protected
#   define private friend class System::Reflection::Assembly::AssemblyModulXUI; private
#   include "UI/Controls/Control.h"
#	include "UI/LayoutDock/Controls/OverlayWindow.h"
#	include "UI/LayoutDock/Controls/LayoutItem.h"
#	include "UI/LayoutDock/Controls/LayoutAnchorControl.h"
#	include "UI/LayoutDock/Layouts/LayoutAnchorGroup.h"
#	include "UI/LayoutDock/DockingManager.h"
#   undef private
#   undef protected
#undef _ALLOW_KEYWORD_MACROS

MetaAssemblyBegin
Meta(assembly: System::Reflection::AssemblyTitle("eXtensionable User Interface"))
Meta(assembly: System::Reflection::AssemblyDescription(""))

Meta(assembly: XamlCPP::XmlnsDefinition("http://git.tauri.hu/XUI", "XUI"))
Meta(assembly: XamlCPP::XmlnsDefinition("http://git.tauri.hu/XUI", "XUI::UI"))
Meta(assembly: XamlCPP::XmlnsDefinition("http://git.tauri.hu/XUI", "XUI::UI::Controls"))
Meta(assembly: XamlCPP::XmlnsDefinition("http://git.tauri.hu/XUI", "XUI::UI::DataTemplates"))
Meta(assembly: XamlCPP::XmlnsDefinition("http://git.tauri.hu/XUI", "XUI::UI::Presenters"))
Meta(assembly: XamlCPP::XmlnsDefinition("http://git.tauri.hu/XUI", "XUI::UI::Shapes"))
Meta(assembly: XamlCPP::XmlnsDefinition("http://git.tauri.hu/XUI", "XUI::Core"))
Meta(assembly: XamlCPP::XmlnsDefinition("http://git.tauri.hu/XUI", "XUI::Core::Dependency"))
Meta(assembly: XamlCPP::XmlnsDefinition("http://git.tauri.hu/XUI", "XUI::Core::Media"))
Meta(assembly: XamlCPP::XmlnsDefinition("http://git.tauri.hu/XUI", "XUI::Core::Observer"))
Meta(assembly: XamlCPP::XmlnsDefinition("http://git.tauri.hu/XUI", "XUI::Core::TypeExtensions"))
Meta(assembly: XamlCPP::XmlnsDefinition("http://git.tauri.hu/XUI", "XUI::Styling"))
MetaAssemblyEnd
#endif // _XUI_PCH_COMMON_H_
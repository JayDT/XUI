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
#include "DynamicResourceExtension.h"
#include "DynamicResourceBinding.h"
#include "Core/Dependencies/IDependencyObject.h"
#include "Runtime/ReflectionModule.h"
#include "DataBind/InstancedBinding.h"
#include "Core/TypeExtensions/TypeConverter.h"
#include "XamlCPP.h"
#include "Interfaces/IStyle.h"
#include "Interfaces/IStyleHost.h"
#include "Interfaces/INamedScope.h"
#include "Interfaces/IValueContext.h"
#include "StateCommuter.h"

using namespace XamlCPP;

System::Reflection::Variant XamlCPP::Markup::DynamicResourceExtension::ProvideValue(Core::MarkupExtensionContext * markupExtensionContext)
{
    auto _anchor = markupExtensionContext->getTargetObject().IsObject() ? markupExtensionContext->getTargetObject().ToObject()->Dynamic_As<Interfaces::IResourceNode>() : nullptr;
    if (!_anchor)
    {
        _anchor = GetAnchor(markupExtensionContext);
    }
    return std::make_shared<DataBind::DynamicResourceBinding>(_anchor, GetResourceKey(), m_markup);
}

Interfaces::IResourceNode * XamlCPP::Markup::DynamicResourceExtension::GetAnchor(Core::MarkupExtensionContext * context)
{
    return nullptr;
}

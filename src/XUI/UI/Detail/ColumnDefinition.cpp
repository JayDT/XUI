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

#include "StdAfx.h"
#include "ColumnDefinition.h"
#include "DependencyPropertyMgr.h"

#include "Core/GlobalInitializerScheduler.h"

STATIC_CLASS_INITIALIZER(ColumnDefinition, XUI::UI::ColumnDefinition::StaticClassInitializer)

XUI_DEPENDENCY_PROPERTY_DECL(std::string)           XUI::UI::DefinitionBase::SharedSizeGroupProperty;

XUI_DEPENDENCY_PROPERTY_DECL(double)                XUI::UI::ColumnDefinition::MaxWidthProperty;
XUI_DEPENDENCY_PROPERTY_DECL(double)                XUI::UI::ColumnDefinition::MinWidthProperty;
XUI_DEPENDENCY_PROPERTY_DECL(XUI::UI::GridLength)   XUI::UI::ColumnDefinition::WidthProperty;

void XUI::UI::ColumnDefinition::StaticClassInitializer()
{
    XUI::UI::DefinitionBase::SharedSizeGroupProperty = XUI::Core::Dependency::BindProperty<std::string>::Register<XUI::UI::DefinitionBase>(nameof(SharedSizeGroup), "", true);

    XUI::UI::ColumnDefinition::MaxWidthProperty = XUI_DEPENDENCY_PROPERTY_REGISTER_WITH_ARGS(XUI::UI::ColumnDefinition, double)(nameof(MaxWidth), std::numeric_limits<double>::infinity());
    XUI::UI::ColumnDefinition::MinWidthProperty = XUI_DEPENDENCY_PROPERTY_REGISTER(XUI::UI::ColumnDefinition, double, nameof(MinWidth));
    XUI::UI::ColumnDefinition::WidthProperty    = XUI_DEPENDENCY_PROPERTY_REGISTER_WITH_ARGS(XUI::UI::ColumnDefinition, GridLength)(nameof(Width), GridLength(1, GridUnitType::Star));
}

XUI::UI::ColumnDefinition::ColumnDefinition()
{
}

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

#include "Line.h"
#include "Core/Media/Geometries/LineGeometry.h"
#include "Core/Media/Thickness_t.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;

STATIC_CLASS_POST_INITIALIZER(Line, XUI::UI::Shapes::Line::StaticConstructor)

Core::Dependency::RefDependencyProperty XUI::UI::Shapes::Line::StartPointProperty;
Core::Dependency::RefDependencyProperty XUI::UI::Shapes::Line::EndPointProperty;

void XUI::UI::Shapes::Line::StaticConstructor()
{
    StartPointProperty = (Core::Dependency::BindProperty<Core::Media::Point>::Register<XUI::UI::Shapes::Line>(nameof(StartPoint)));
    EndPointProperty = (Core::Dependency::BindProperty<Core::Media::Point>::Register<XUI::UI::Shapes::Line>(nameof(EndPoint)));

    StrokeThicknessProperty->OverrideDefaultValue<XUI::UI::Shapes::Line>(1.0);

    extern XUI::UI::Shapes::AffectGeometryFn AffectGeometryFunction;

    StartPointProperty->Changed.get_observable().subscribe(AffectGeometryFunction);
    EndPointProperty->Changed.get_observable().subscribe(AffectGeometryFunction);
}

std::shared_ptr<Core::Media::Geometry> XUI::UI::Shapes::Line::CreateDefiningGeometry()
{
    return std::make_shared<Core::Media::Geometries::LineGeometry>(StartPoint, EndPoint);
}

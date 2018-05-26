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

#include "Polyline.h"
#include "Core/Media/Geometries/PolylineGeometry.h"
#include "Core/Media/Thickness_t.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;

STATIC_CLASS_POST_INITIALIZER(Polyline, XUI::UI::Shapes::Polyline::StaticConstructor)

Core::Dependency::RefDependencyProperty XUI::UI::Shapes::Polyline::PointsProperty;
Core::Dependency::RefDependencyProperty XUI::UI::Shapes::Polyline::FillRuleProperty;

void XUI::UI::Shapes::Polyline::StaticConstructor()
{
    PointsProperty = (Core::Dependency::DirectProperty<XUI::UI::Shapes::Polyline, System::Collection::Vector<Core::Media::Point>>::RegisterDirect(nameof(Points),
        [](Polyline* x) { return x->Points; },
        [](Polyline* x, System::Collection::Vector<Core::Media::Point> const& v) { x->Points = v; })
        );
    FillRuleProperty = (Core::Dependency::BindProperty<XUI::Interfaces::FillRule>::Register<XUI::UI::Shapes::Polyline>(nameof(FillRule)));

    StrokeThicknessProperty->OverrideDefaultValue<XUI::UI::Shapes::Polyline>(1.0);

    extern XUI::UI::Shapes::AffectGeometryFn AffectGeometryFunction;

    PointsProperty->Changed.get_observable().subscribe(AffectGeometryFunction);
    FillRuleProperty->Changed.get_observable().subscribe(AffectGeometryFunction);
}

std::shared_ptr<Core::Media::Geometry> XUI::UI::Shapes::Polyline::CreateDefiningGeometry()
{
    return std::make_shared<Core::Media::Geometries::PolylineGeometry>((System::Collection::Vector<Core::Media::Point>&)Points, false);
}

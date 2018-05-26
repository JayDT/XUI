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

#include "EllipseGeometry.h"
#include "Core/Dependencies/DependencyPropertyMgr.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI::Core;

STATIC_CLASS_INITIALIZER(EllipseGeometry, XUI::Core::Media::Geometries::EllipseGeometry::StaticConstructor)

Dependency::RefDependencyProperty Media::Geometries::EllipseGeometry::RectProperty;

XUI::Core::Media::Rect XUI::Core::Media::Geometries::EllipseGeometry::GetRect() const
{
    return GetValue<Core::Media::Rect>(RectProperty);
}

void XUI::Core::Media::Geometries::EllipseGeometry::SetRect(Core::Media::Rect const & value)
{
    SetValue(RectProperty, value);
}

void Media::Geometries::EllipseGeometry::StaticConstructor()
{
    RectProperty = (Core::Dependency::BindProperty<Media::Rect>::Register<Media::Geometries::EllipseGeometry>(nameof(Rect)));

    RectProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        static_cast<Media::Geometries::EllipseGeometry*>(e.Sender)->RectChanged(e);
    });
}

Media::Geometries::EllipseGeometry::EllipseGeometry()
{
}

Media::Geometries::EllipseGeometry::EllipseGeometry(Media::Rect const& rect)
    : EllipseGeometry()
{
    Rect = rect;
}

std::shared_ptr<Media::Geometry> Media::Geometries::EllipseGeometry::RefClone()
{
    return std::make_shared<EllipseGeometry>(Rect);
}

void Media::Geometries::EllipseGeometry::RectChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e)
{
    auto rect = e.NewValue.GetValue<Media::Rect const&>();
    {
        double controlPointRatio = (std::sqrt(2.0) - 1.0) * 4.0 / 3.0;
        auto center = rect.center();
        auto radius = Vector(rect.width() / 2.0, rect.height() / 2.0);
    
        auto x0 = center.x - radius.x;
        auto x1 = center.x - (radius.x * controlPointRatio);
        auto x2 = center.x;
        auto x3 = center.x + (radius.x * controlPointRatio);
        auto x4 = center.x + radius.x;
    
        auto y0 = center.y - radius.y;
        auto y1 = center.y - (radius.y * controlPointRatio);
        auto y2 = center.y;
        auto y3 = center.y + (radius.y * controlPointRatio);
        auto y4 = center.y + radius.y;
    
        auto context = GetStreamGeometryContext()->Open();
        context->BeginFigure(Point(x2, y0), true);
        context->CubicBezierTo(Point(x3, y0), Point(x4, y1), Point(x4, y2));
        context->CubicBezierTo(Point(x4, y3), Point(x3, y4), Point(x2, y4));
        context->CubicBezierTo(Point(x1, y4), Point(x0, y3), Point(x0, y2));
        context->CubicBezierTo(Point(x0, y1), Point(x1, y0), Point(x2, y0));
        context->EndFigure(true);
    }
}

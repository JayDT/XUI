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
#include "LineGeometry.h"

#include "Core/Dependencies/DependencyPropertyMgr.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;

Core::Dependency::RefDependencyProperty Core::Media::Geometries::LineGeometry::StartPointProperty;
Core::Dependency::RefDependencyProperty Core::Media::Geometries::LineGeometry::EndPointProperty;

void Static_LineGeometry()
{
    Core::Media::Geometries::LineGeometry::StartPointProperty = (Core::Dependency::BindProperty<Core::Media::Point>::Register<Core::Media::Geometries::LineGeometry>(nameof(StartPoint)));
    Core::Media::Geometries::LineGeometry::EndPointProperty = (Core::Dependency::BindProperty<Core::Media::Point>::Register<Core::Media::Geometries::LineGeometry>(nameof(EndPoint)));

    Core::Media::Geometries::LineGeometry::StartPointProperty->Changed.get_observable().subscribe(&Core::Media::Geometries::LineGeometry::PointsChanged);
    Core::Media::Geometries::LineGeometry::EndPointProperty->Changed.get_observable().subscribe(&Core::Media::Geometries::LineGeometry::PointsChanged);
}

STATIC_INITIALIZER(Static_LineGeometry)

XUI::Core::Media::Geometries::LineGeometry::LineGeometry()
{
}


/// <summary>
/// Initializes a new instance of the <see cref="LineGeometry"/> class.
/// </summary>
/// <param name="startPoint">The start point.</param>
/// <param name="endPoint">The end point.</param>

XUI::Core::Media::Geometries::LineGeometry::LineGeometry(Point const & startPoint, Point const & endPoint) : LineGeometry()
{
    StartPoint = startPoint;
    EndPoint = endPoint;
    PrepareIfNeeded();
}

void XUI::Core::Media::Geometries::LineGeometry::PrepareIfNeeded()
{
    if (_isDirty)
    {
        _isDirty = false;

        {
            auto context = GetStreamGeometryContext()->Open();
            context->BeginFigure(StartPoint, false);
            context->LineTo(EndPoint);
            context->EndFigure(false);
        }
    }
}

void XUI::Core::Media::Geometries::LineGeometry::PointsChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
{
    XUI::Core::Media::Geometries::LineGeometry* geometry = e.Sender->As<XUI::Core::Media::Geometries::LineGeometry>();
    if (geometry)
        geometry->_isDirty = true;
}

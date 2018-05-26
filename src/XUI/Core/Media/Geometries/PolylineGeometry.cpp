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
#include "PolylineGeometry.h"
#include "DirectProperty.h"

#include "Core/Dependencies/DependencyPropertyMgr.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;

Core::Dependency::RefDependencyProperty Core::Media::Geometries::PolylineGeometry::PointsProperty;
Core::Dependency::RefDependencyProperty Core::Media::Geometries::PolylineGeometry::IsFilledProperty;

void Static_PolylineGeometry()
{
    Core::Media::Geometries::PolylineGeometry::PointsProperty = (Core::Dependency::DirectProperty<Core::Media::Geometries::PolylineGeometry, System::Collection::Vector<Core::Media::Point>>::RegisterDirect(nameof(Points), [](Core::Media::Geometries::PolylineGeometry* x)
    {
        return x->Points;
    }, nullptr));
    Core::Media::Geometries::PolylineGeometry::IsFilledProperty = (Core::Dependency::BindProperty<bool>::Register<Core::Media::Geometries::PolylineGeometry>(nameof(IsFilled)));

    Core::Media::Geometries::PolylineGeometry::PointsProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        XUI::Core::Media::Geometries::PolylineGeometry* geometry = e.Sender->Static_As<XUI::Core::Media::Geometries::PolylineGeometry>();
        if (geometry)
            geometry->OnPointsChanged(e.OldValue.GetValue<System::Collection::Vector<Core::Media::Point>>(), e.NewValue.GetValue<System::Collection::Vector<Core::Media::Point>>());
    });

    Core::Media::Geometries::PolylineGeometry::IsFilledProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        XUI::Core::Media::Geometries::PolylineGeometry* geometry = e.Sender->Static_As<XUI::Core::Media::Geometries::PolylineGeometry>();
        if (geometry)
            geometry->NotifyChanged();
    });
}

STATIC_INITIALIZER(Static_PolylineGeometry)


XUI::Core::Media::Geometries::PolylineGeometry::PolylineGeometry()
{
}


/// <summary>
/// Initializes a new instance of the <see cref="PolylineGeometry"/> class.
/// </summary>

XUI::Core::Media::Geometries::PolylineGeometry::PolylineGeometry(System::Collection::Vector<Core::Media::Point>& points, bool isFilled) : PolylineGeometry()
{
    for (auto const& p : points)
        _points.Add(p);
    IsFilled = isFilled;
}

void XUI::Core::Media::Geometries::PolylineGeometry::PrepareIfNeeded()
{
    if (_isDirty)
    {
        _isDirty = false;
    
        {
            auto context = GetStreamGeometryContext()->Open();
            auto points = Points;
            auto isFilled = IsFilled;
            if (points.size() > 0)
            {
                context->BeginFigure(points[0], isFilled);
                for (int i = 1; i < points.size(); i++)
                {
                    context->LineTo(points[i]);
                }
                context->EndFigure(isFilled);
            }
        }
    }
}

void XUI::Core::Media::Geometries::PolylineGeometry::SetPoints(System::Collection::Vector<Core::Media::Point> const & value)
{
    SetAndRaise<System::Collection::Vector<Core::Media::Point>>(PointsProperty.get(), _points, value);
}


/// <inheritdoc/>

std::shared_ptr<Core::Media::Geometry> XUI::Core::Media::Geometries::PolylineGeometry::RefClone()
{
    PrepareIfNeeded();
    return std::make_shared<PolylineGeometry>((System::Collection::Vector<Core::Media::Point>&)Points, IsFilled);
}

void XUI::Core::Media::Geometries::PolylineGeometry::OnPointsChanged(System::Collection::Vector<Core::Media::Point> const & oldValue, System::Collection::Vector<Core::Media::Point> const & newValue)
{
    if (_pointsObserver.is_subscribed())
        _pointsObserver.unsubscribe();

    _points.Clear();
    for (auto& p : newValue.GetNativeEnumerator())
        _points.Add(p);

    NotifyChanged();
}

void XUI::Core::Media::Geometries::PolylineGeometry::PointsCollectionChanged(void * sender, System::Collection::NotifyCollectionChangedEventArgs & e)
{
    System::Collection::Vector<Core::Media::Point>& list = *(System::Collection::Vector<Core::Media::Point>*)sender;
    switch (e.Action)
    {
        case System::Collection::NotifyCollectionChangedAction::Add:
            break;

        case System::Collection::NotifyCollectionChangedAction::Remove:
            break;

        case System::Collection::NotifyCollectionChangedAction::Replace:
        case System::Collection::NotifyCollectionChangedAction::Reset:
            break;
    }
}

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

#ifndef __XUI_POLYLINEGEOMETRY_H__
#define __XUI_POLYLINEGEOMETRY_H__

#include "Core/Dependencies/Models/DataProperty.h"
#include "Core/Media/MediaTypes.h"
#include "Core/Media/Geometries/PathGeometry.h"
#include "Interfaces/IGeometry.h"

namespace XUI::Core::Media::Geometries
{
    /// <summary>
    /// Represents the geometry of an polyline or polygon.
    /// </summary>
    class PolylineGeometry : public Geometry
    {
    private:
        System::Collection::Vector<Core::Media::Point> _points;
        bool _isDirty = true;
        rxcpp::subscription _pointsObserver;

    public:
        /// <summary>
        /// Defines the <see cref="Points"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty PointsProperty;

        /// <summary>
        /// Defines the <see cref="IsFilled"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty IsFilledProperty;

        virtual ~PolylineGeometry()
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="PolylineGeometry"/> class.
        /// </summary>
    public:
        PolylineGeometry();

        /// <summary>
        /// Initializes a new instance of the <see cref="PolylineGeometry"/> class.
        /// </summary>
        PolylineGeometry(System::Collection::Vector<Core::Media::Point> &points, bool isFilled);

        void PrepareIfNeeded();

        /// <summary>
        /// Gets or sets the figures.
        /// </summary>
        /// <value>
        /// The points.
        /// </value>
        PROPERTY_(System::Collection::Vector<Core::Media::Point> const&, Points);
        System::Collection::Vector<Core::Media::Point> const& GetPoints() const
        {
            return _points;
        }
        void SetPoints(System::Collection::Vector<Core::Media::Point> const& value);

        PROPERTY_(bool, IsFilled);
        bool GetIsFilled() const
        {
            return GetValue<bool>(IsFilledProperty);
        }
        void SetIsFilled(const bool &value)
        {
            SetValue(IsFilledProperty, value);
        }

        /// <inheritdoc/>
        std::shared_ptr<Core::Media::Geometry> RefClone() override;

        void OnPointsChanged(System::Collection::Vector<Core::Media::Point> const& oldValue, System::Collection::Vector<Core::Media::Point> const& newValue);
        void PointsCollectionChanged(void * sender, System::Collection::NotifyCollectionChangedEventArgs & e);

    public:
        void NotifyChanged()
        {
            _isDirty = true;
        }
    };
}


#endif	//#ifndef __XUI_POLYLINEGEOMETRY_H__

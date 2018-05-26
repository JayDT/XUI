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

#ifndef __XUI_LINEGEOMETRY_H__
#define __XUI_LINEGEOMETRY_H__

#include "Core/Dependencies/Models/DataProperty.h"
#include "Core/Media/MediaTypes.h"
#include "Core/Media/Geometries/PathGeometry.h"
#include "Interfaces/IGeometry.h"

namespace XUI::Core::Media::Geometries
{
    /// <summary>
    /// Represents the geometry of a line.
    /// </summary>
    class LineGeometry : public Geometry
    {
    private:
        bool _isDirty = false;

    public:
        /// <summary>
        /// Defines the <see cref="StartPoint"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty StartPointProperty;

        /// <summary>
        /// Defines the <see cref="EndPoint"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty EndPointProperty;

        PROPERTY_(Core::Media::Point, StartPoint);
        Core::Media::Point GetStartPoint() const
        {
            return GetValue<Core::Media::Point>(StartPointProperty);
        }
        void SetStartPoint(Point const& value)
        {
            SetValue(StartPointProperty, value);
        }

        PROPERTY_(Core::Media::Point, EndPoint);
        Core::Media::Point GetEndPoint() const
        {
            return GetValue<Core::Media::Point>(EndPointProperty);
        }
        void SetEndPoint(Core::Media::Point const& value)
        {
            SetValue(EndPointProperty, value);
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="LineGeometry"/> class.
        /// </summary>
        LineGeometry();

        /// <summary>
        /// Initializes a new instance of the <see cref="LineGeometry"/> class.
        /// </summary>
        /// <param name="startPoint">The start point.</param>
        /// <param name="endPoint">The end point.</param>
        LineGeometry(Point const& startPoint, Point const& endPoint);

    protected:

        void PrepareIfNeeded();
        
        /// <inheritdoc/>
        std::shared_ptr<Core::Media::Geometry> RefClone() override
        {
            PrepareIfNeeded();
            return std::make_shared<LineGeometry>(StartPoint, EndPoint);
        }

    public:
        static void PointsChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e);
    };
}


#endif	//#ifndef __XUI_LINEGEOMETRY_H__

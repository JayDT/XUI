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

#ifndef __XUI_ELLIPSEGEOMETRY_H__
#define __XUI_ELLIPSEGEOMETRY_H__

#include "Core/Dependencies/Models/DataProperty.h"
#include "Core/Media/MediaTypes.h"
#include "Core/Media/Geometry.h"

namespace XUI::Core::Media::Geometries
{
    /// <summary>
    /// Represents the geometry of an ellipse or circle.
    /// </summary>
    class TC_XUI_API EllipseGeometry : public Geometry
    {
    public:
        /// <summary>
        /// Defines the <see cref="Rect"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty RectProperty;

        PROPERTY_(Core::Media::Rect, Rect);
        Core::Media::Rect GetRect() const;
        void SetRect(Core::Media::Rect const& value);

        static void StaticConstructor();

        /// <summary>
        /// Initializes a new instance of the <see cref="EllipseGeometry"/> class.
        /// </summary>
        EllipseGeometry();

        /// <summary>
        /// Initializes a new instance of the <see cref="EllipseGeometry"/> class.
        /// </summary>
        /// <param name="rect">The rectangle that the ellipse should fill.</param>
        EllipseGeometry(Core::Media::Rect const& rect);

        /// <inheritdoc/>
        std::shared_ptr<Core::Media::Geometry> RefClone() override;

    private:
        void RectChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e);
    };
}


#endif	//#ifndef __XUI_ELLIPSEGEOMETRY_H__

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

#ifndef __XUI_RADIALGRADIENTBRUSH_H__
#define __XUI_RADIALGRADIENTBRUSH_H__

#include "Core/Media/GradientBrush.h"
#include "Core/Media/RelativePoint_t.h"

namespace XUI::Core::Media
{
    /// <summary>
    /// Paints an area with a radial gradient.
    /// </summary>
    class TC_XUI_API RadialGradientBrush final : public GradientBrush
    {
    public:
        /// <summary>
        /// Defines the <see cref="Center"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty CenterProperty;
        //static StyledProperty<RelativePoint*> *const CenterProperty = AvaloniaProperty::Register<RadialGradientBrush*, RelativePoint*>("Center", RelativePoint::Center);

        /// <summary>
        /// Defines the <see cref="GradientOrigin"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty GradientOriginProperty;
        //static StyledProperty<RelativePoint*> *const GradientOriginProperty = AvaloniaProperty::Register<RadialGradientBrush*, RelativePoint*>("GradientOrigin", RelativePoint::Center);

        /// <summary>
        /// Defines the <see cref="Radius"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty RadiusProperty;
        //static StyledProperty<double> *const RadiusProperty = AvaloniaProperty::Register<RadialGradientBrush*, double>("Radius", 0.5);

        /// <summary>
        /// Gets or sets the start point for the gradient.
        /// </summary>
        PROPERTY_(RelativePoint, Center);
        RelativePoint GetCenter() const
        {
            return GetValue<RelativePoint>(CenterProperty);
        }
        void SetCenter(RelativePoint const& value)
        {
            SetValue(CenterProperty, value);
        }

        /// <summary>
        /// Gets or sets the location of the two-dimensional focal point that defines the beginning
        /// of the gradient.
        /// </summary>
        PROPERTY_(RelativePoint, GradientOrigin);
        RelativePoint GetGradientOrigin() const
        {
            return GetValue<RelativePoint>(GradientOriginProperty);
        }
        void SetGradientOrigin(RelativePoint const& value)
        {
            SetValue(GradientOriginProperty, value);
        }

        /// <summary>
        /// Gets or sets the horizontal and vertical radius of the outermost circle of the radial
        /// gradient.
        /// </summary>
        // TODO: This appears to always be relative so should use a RelativeSize struct or something.
        PROPERTY_(double, Radius);
        double GetRadius() const
        {
            return GetValue<double>(RadiusProperty);
        }
        void SetRadius(const double &value)
        {
            SetValue(RadiusProperty, value);
        }

        virtual Interfaces::BurshTypes GetTypeId() const override { return Interfaces::BurshTypes::RADIAL_GRADIENT_BRUSH; }
    };
}


#endif	//#ifndef __XUI_RADIALGRADIENTBRUSH_H__

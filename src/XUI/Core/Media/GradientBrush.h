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

#ifndef __XUI_GRADIENTBRUSH_H__
#define __XUI_GRADIENTBRUSH_H__

#include "Core/Media/Brush.h"

namespace XUI::Core::Media
{
    enum class GradientSpreadMethod
    {
        Pad,
        Reflect,
        Repeat
    };

    struct GradientStop : public System::Reflection::Object
    {
        GradientStop() {}
        GradientStop(XUI::Core::Media::Color color, double offset)
        : m_Offset(offset), m_Color(color) {}
        PROPERTY_AUTO_QT(double, Offset)
        PROPERTY_AUTO_QT(XUI::Core::Media::Color, Color)
    };

    struct GradientStopCollection : public System::Collection::Vector<std::shared_ptr<XUI::Core::Media::GradientStop>>
    {
        GradientStopCollection()
        { }

        GradientStopCollection(GradientStopCollection const& from)
            : System::Collection::Vector<std::shared_ptr<XUI::Core::Media::GradientStop>>(from)
        { }

        GradientStopCollection(GradientStopCollection const&& from)
            : System::Collection::Vector<std::shared_ptr<XUI::Core::Media::GradientStop>>(std::move(from))
        { }

        void operator=(GradientStopCollection const& from)
        {
            Clear();
            for (auto const& v : from.GetNativeEnumerator())
            {
                Add(v);
            }
        }
    };

    /// <summary>
    /// Base class for brushes that draw with a gradient.
    /// </summary>
    class TC_XUI_API GradientBrush : public Brush
    {
    public:
        /// <summary>
        /// Defines the <see cref="SpreadMethod"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty SpreadMethodProperty;

        /// <summary>
        /// Defines the <see cref="GradientStops"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty GradientStopsProperty;

        /// <summary>
        /// Initializes a new instance of the <see cref="GradientBrush"/> class.
        /// </summary>
        GradientBrush()
        { }

        /// <summary>
        /// Gets or sets the brush's spread method that defines how to draw a gradient that
        /// doesn't fill the bounds of the destination control.
        /// </summary>
        PROPERTY_(GradientSpreadMethod, SpreadMethod);
        GradientSpreadMethod GetSpreadMethod() const
        {
            return GetValue<GradientSpreadMethod>(SpreadMethodProperty);
        }
        void SetSpreadMethod(GradientSpreadMethod value)
        {
            SetValue(SpreadMethodProperty, value);
        }

        /// <summary>
        /// Gets or sets the brush's gradient stops.
        /// </summary>
        PROPERTY_(std::shared_ptr<GradientStopCollection>, GradientStops);
        std::shared_ptr<GradientStopCollection> GetGradientStops() const
        {
            return GetValue<std::shared_ptr<GradientStopCollection>>(GradientStopsProperty);
        }
        void SetGradientStops(const std::shared_ptr<GradientStopCollection> &value)
        {
            SetValue(GradientStopsProperty, value);
        }

        virtual Interfaces::BurshTypes GetTypeId() const override { return Interfaces::BurshTypes::GRADIENT_BRUSH; }
    };
}


#endif	//#ifndef __XUI_GRADIENTBRUSH_H__

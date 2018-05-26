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

#ifndef _XUI_PEN_H__
#define _XUI_PEN_H__

#include "Interfaces/UIControls/IAnimate.h"

#include "Interfaces/IBrush.h"

#include <initializer_list>

namespace XUI::Core::Media
{
    enum class PenLineCap
    {
        Flat,
        Round,
        Square,
        Triangle
    };

    enum class PenLineJoin
    {
        Bevel,
        Miter,
        Round
    };

    class DashStyle : public Interfaces::AnimHandler
    {
    private:
        std::vector<double> _dashes;
        double _offset;
    public:
        static const DashStyle Dash;
        static const DashStyle Dot;
        static const DashStyle DashDot;
        static const DashStyle DashDotDot;

        DashStyle() {}
        DashStyle(const std::vector<double>& dashes, double offset = 0.0);
        virtual ~DashStyle() {}

        /// <summary>
        /// Gets and sets the length of alternating dashes and gaps.
        /// </summary>
        PROPERTY_(std::vector<double>, Dashes);
        std::vector<double> const& GetDashes() const { return _dashes; }
        void SetDashes(std::vector<double> const& value) { _dashes = value; }
        PROPERTY_G(_offset, Offset);
    };

    /// <summary>
    /// Describes how a stroke is drawn.
    /// </summary>
    class Pen
    {
        Interfaces::IBrush* _brush = nullptr;
        double _thickness = 1.0;
        DashStyle* _dashStyle = nullptr;
        PenLineCap _dashCap;
        PenLineCap _startLineCap = PenLineCap::Flat;
        PenLineCap _endLineCap = PenLineCap::Flat;
        PenLineJoin _lineJoin = PenLineJoin::Miter;
        double _miterLimit = 10.0;

    public:
        /// <summary>
        /// Initializes a new instance of the <see cref="Pen"/> class.
        /// </summary>
        /// <param name="brush">The brush used to draw.</param>
        /// <param name="thickness">The stroke thickness.</param>
        /// <param name="dashStyle">The dash style.</param>
        /// <param name="dashCap">The dash cap.</param>
        /// <param name="startLineCap">The start line cap.</param>
        /// <param name="endLineCap">The end line cap.</param>
        /// <param name="lineJoin">The line join.</param>
        /// <param name="miterLimit">The miter limit.</param>
        Pen(
            Interfaces::IBrush* brush, 
            double thickness = 1.0,
            DashStyle* dashStyle = nullptr, 
            PenLineCap dashCap = PenLineCap::Flat, 
            PenLineCap startLineCap = PenLineCap::Flat,
            PenLineCap endLineCap = PenLineCap::Flat,
            PenLineJoin lineJoin = PenLineJoin::Miter,
            double miterLimit = 10.0);

        /// <summary>
        /// Initializes a new instance of the <see cref="Pen"/> class.
        /// </summary>
        /// <param name="color">The stroke color.</param>
        /// <param name="thickness">The stroke thickness.</param>
        /// <param name="dashStyle">The dash style.</param>
        /// <param name="dashCap">The dash cap.</param>
        /// <param name="startLineCap">The start line cap.</param>
        /// <param name="endLineCap">The end line cap.</param>
        /// <param name="lineJoin">The line join.</param>
        /// <param name="miterLimit">The miter limit.</param>
        Pen(
            uint color, 
            double thickness = 1.0,
            DashStyle* dashStyle = nullptr, 
            PenLineCap dashCap = PenLineCap::Flat, 
            PenLineCap startLineCap = PenLineCap::Flat,
            PenLineCap endLineCap = PenLineCap::Flat,
            PenLineJoin lineJoin = PenLineJoin::Miter,
            double miterLimit = 10.0);

        /// <summary>
        /// Gets the brush used to draw the stroke.
        /// </summary>
        PROPERTY_T_G(Interfaces::IBrush*, _brush, Brush);

        /// <summary>
        /// Gets the stroke thickness.
        /// </summary>
        PROPERTY_G(_thickness, Thickness);

        PROPERTY_G(_dashStyle, Dashstyle);

        PROPERTY_G(_dashCap, DashCap);
        PROPERTY_G(_startLineCap, StartLineCap);
        PROPERTY_G(_endLineCap, EndLineCap);
        PROPERTY_G(_lineJoin, LineJoin);
        PROPERTY_G(_miterLimit, MiterLimit);
    };
}

#endif // !_XUI_PEN_H__
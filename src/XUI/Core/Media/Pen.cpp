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

#include "Pen.h"

using namespace XUI::Core::Media;

const DashStyle DashStyle::Dash({ 2.0, 2.0 }, 0.0);
const DashStyle DashStyle::Dot({ 0.0, 2.0 }, 0.0);
const DashStyle DashStyle::DashDot({ 2.0, 2.0, 0.0, 2.0 }, 0.0);
const DashStyle DashStyle::DashDotDot({ 2.0, 2.0, 0.0, 2.0, 0.0, 2.0 }, 0.0);

DashStyle::DashStyle(const std::vector<double>& dashes, double offset)
    : AnimHandler()
{
    _dashes = dashes;
    _offset = offset;
}

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

XUI::Core::Media::Pen::Pen(Interfaces::IBrush * brush, double thickness, DashStyle * dashStyle, PenLineCap dashCap, PenLineCap startLineCap, PenLineCap endLineCap, PenLineJoin lineJoin, double miterLimit)
{
    _brush = brush;
    _thickness = thickness;
    _dashCap = dashCap;
    _startLineCap = startLineCap;
    _endLineCap = endLineCap;
    _lineJoin = lineJoin;
    _miterLimit = miterLimit;
    _dashStyle = dashStyle;
}

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

XUI::Core::Media::Pen::Pen(uint color, double thickness, DashStyle * dashStyle, PenLineCap dashCap, PenLineCap startLineCap, PenLineCap endLineCap, PenLineJoin lineJoin, double miterLimit)
{
    _brush = nullptr; // new SolidColorBrush(color);
    _thickness = thickness;
    _startLineCap = startLineCap;
    _endLineCap = endLineCap;
    _lineJoin = lineJoin;
    _miterLimit = miterLimit;
    _dashStyle = dashStyle;
    _dashCap = dashCap;
}

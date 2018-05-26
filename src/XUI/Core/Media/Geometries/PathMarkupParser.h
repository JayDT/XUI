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

#ifndef __XUI_PATHMARKUPPARSER_H__
#define __XUI_PATHMARKUPPARSER_H__

#include "Interfaces/IGeometry.h"

namespace XUI::Core::Media::Geometries
{
    /// <summary>
    /// Parses a path markup string.
    /// </summary>
    class PathMarkupParser
    {
    public:
        /// <summary>
        /// Defines the command currently being processed.
        /// </summary>
        enum class Command
        {
            None,
            FillRule,
            Move,
            Line,
            HorizontalLine,
            VerticalLine,
            CubicBezierCurve,
            QuadraticBezierCurve,
            SmoothCubicBezierCurve,
            SmoothQuadraticBezierCurve,
            Arc,
            Close,
        }; 
    
    private:

        Interfaces::IStreamGeometryContext *const _context = nullptr;
        size_t pos = 0;
        size_t length = 0;
        const char* expr = nullptr;

    public:
        /// <summary>
        /// Initializes a new instance of the <see cref="PathMarkupParser"/> class.
        /// </summary>
        /// <param name="context">The context for the geometry.</param>
        virtual ~PathMarkupParser()
        {
        }

        PathMarkupParser(Interfaces::IStreamGeometryContext *context);

        /// <summary>
        /// Parses the specified markup string.
        /// </summary>
        /// <param name="s">The markup string.</param>
        void Parse(const std::string &s);

    private:
        Point MirrorControlPoint(Point const& controlPoint, Point const& center);

        bool ReadCommand(Command &command, bool &relative);

        Interfaces::FillRule ReadFillRule();

        double ReadDouble();

        Point ReadPoint(Point const& current, bool relative);

        Size ReadSize();

        bool ReadBool();

        Point ReadRelativePoint(Point const& lastPoint);

        void ReadSeparator();

        void ReadWhitespace();
    };
}


#endif	//#ifndef __XUI_PATHMARKUPPARSER_H__

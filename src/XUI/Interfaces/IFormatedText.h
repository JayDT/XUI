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

#ifndef _XUI_IFORMATED_TEXT_H__
#define _XUI_IFORMATED_TEXT_H__

#include "Core/Media/MediaTypes.h"
#include "Core/Media/Size_t.h"
#include "standard/BasicPrimities.h"

namespace XUI::Core::Media
{
    class Pen;
}

namespace XUI::Interfaces
{
    /// <summary>
    /// Holds a hit test result from a <see cref="FormattedText"/>.
    /// </summary>
    struct TextHitTestResult
    {
        /// <summary>
        /// Gets or sets a value indicating whether the point is inside the bounds of the text.
        /// </summary>
        bool IsInside;

        /// <summary>
        /// Gets the index of the hit character in the text.
        /// </summary>
        int TextPosition;

        /// <summary>
        /// Gets a value indicating whether the hit is on the trailing edge of the character.
        /// </summary>
        bool IsTrailing;
    };

    /// <summary>
    /// Stores information about a line of <see cref="FormattedText"/>.
    /// </summary>
    struct FormattedTextLine
    {
        FormattedTextLine() {}

        /// <summary>
        /// Initializes a new instance of the <see cref="FormattedTextLine"/> class.
        /// </summary>
        /// <param name="length">The length of the line, in characters.</param>
        /// <param name="height">The height of the line, in pixels.</param>
        FormattedTextLine(int length, float height)
        {
            Length = length;
            Height = height;
        }

        /// <summary>
        /// Gets the length of the line, in characters.
        /// </summary>
        int Length = 0;

        /// <summary>
        /// Gets the height of the line, in pixels.
        /// </summary>
        float Height = 0;

        /// <summary>
        /// Gets the start of the original text, in characters.
        /// </summary>
        int Start = 0;

        /// <summary>
        /// Gets the length of the original text, in characters.
        /// </summary>
        int TextLength = 0;

        /// <summary>
        /// Gets the vertical position of the line, in pixel.
        /// </summary>
        float Top = 0;

        /// <summary>
        /// Gets the width of the line, in pixels.
        /// </summary>
        float Width = 0;
    };

    /// <summary>
    /// Defines the platform-specific interface for <see cref="FormattedText"/>.
    /// </summary>
    struct IFormattedText
    {
        /// <summary>
        /// Gets the constraint of the text.
        /// </summary>
        PURE_VIRTUAL_R_PROPERTY(Core::Media::Size, Constraint);

        /// <summary>
        /// The measured size of the text.
        /// </summary>
        PURE_VIRTUAL_R_PROPERTY(Core::Media::Size, Size);

        /// <summary>
        /// Gets the text.
        /// </summary>
        PURE_VIRTUAL_R_PROPERTY(std::wstring const&, Text);

        /// <summary>
        /// Gets the lines in the text.
        /// </summary>
        /// <returns>
        /// A collection of <see cref="FormattedTextLine"/> objects.
        /// </returns>
        virtual std::vector<FormattedTextLine> const& GetLines() = 0;

        /// <summary>
        /// Hit tests a point in the text.
        /// </summary>
        /// <param name="point">The point.</param>
        /// <returns>
        /// A <see cref="TextHitTestResult"/> describing the result of the hit test.
        /// </returns>
        virtual TextHitTestResult HitTestPoint(Core::Media::Point const& point) = 0;

        /// <summary>
        /// Gets the bounds rectangle that the specified character occupies.
        /// </summary>
        /// <param name="index">The index of the character.</param>
        /// <returns>The character bounds.</returns>
        virtual Core::Media::Rect HitTestTextPosition(int index) = 0;

        /// <summary>
        /// Gets the bounds rectangles that the specified text range occupies.
        /// </summary>
        /// <param name="index">The index of the first character.</param>
        /// <param name="length">The number of characters in the text range.</param>
        /// <returns>The character bounds.</returns>
        virtual std::vector<Core::Media::Rect> HitTestTextRange(int index, int length) = 0;
    };

    /// <summary>
    /// Describes the formatting for a span of text in a <see cref="FormattedText"/> object.
    /// </summary>
    struct FormattedTextStyleSpan
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="FormattedTextStyleSpan"/> class.
        /// </summary>
        /// <param name="startIndex">The index of the first character in the span.</param>
        /// <param name="length">The length of the span.</param>
        /// <param name="foregroundBrush">The span's foreground brush.</param>
        FormattedTextStyleSpan(
            int startIndex,
            int length,
            struct IBrush* foregroundBrush = nullptr)
        {
            StartIndex = startIndex;
            Length = length;
            ForegroundBrush = foregroundBrush;
        }

        /// <summary>
        /// Gets the index of the first character in the span.
        /// </summary>
        int StartIndex;

        /// <summary>
        /// Gets the length of the span.
        /// </summary>
        int Length;

        /// <summary>
        /// Gets the span's foreground brush.
        /// </summary>
        struct IBrush* ForegroundBrush;
    };
}

#endif //!
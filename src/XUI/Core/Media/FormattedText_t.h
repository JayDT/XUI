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

#ifndef _XUI_MEDIA_FORMATTED_TEXT_TYPE_H__
#define _XUI_MEDIA_FORMATTED_TEXT_TYPE_H__

#include "Core/Media/MediaTypes.h"
#include "Interfaces/IPlatformRenderInterface.h"
#include "Interfaces/IFormatedText.h"

namespace XUI::Core::Media
{
    /// <summary>
    /// Represents a piece of text with formatting.
    /// </summary>
    class TC_XUI_API FormattedTextWrapper
    {
        Interfaces::IPlatformRenderInterface* _platform = nullptr;
        Size _constraint = Size::Infinity;
        Interfaces::IFormattedText* _platformImpl = nullptr;
        std::vector<Interfaces::FormattedTextStyleSpan> _spans;
        Typeface _typeface;
        TextAlignment _textAlignment = TextAlignment::Left;
        TextWrapping _wrapping = TextWrapping::NoWrap;
    
    public:
        std::wstring _text;

        /// <summary>
        /// Initializes a new instance of the <see cref="FormattedText"/> class.
        /// </summary>
        FormattedTextWrapper();
        ~FormattedTextWrapper();
    
        /// <summary>
        /// Initializes a new instance of the <see cref="FormattedText"/> class.
        /// </summary>
        /// <param name="platform">The platform render interface.</param>
        FormattedTextWrapper(Interfaces::IPlatformRenderInterface* platform);
    
        /// <summary>
        /// Gets or sets the constraint of the text.
        /// </summary>
        PROPERTY_(Size const&, Constraint);
        Size const& GetConstraint() const { return _constraint; }
        void SetConstraint(Size const& value)
        {
            _constraint = value;
            NotifyChanged();
        }

        /// <summary>
        /// Gets or sets the base typeface.
        /// </summary>
        PROPERTY_(XUI::Core::Media::Typeface const&, Typeface);
        XUI::Core::Media::Typeface const& GetTypeface() const { return _typeface; }
        void SetTypeface(XUI::Core::Media::Typeface const& value)
        {
            _typeface = value;
            NotifyChanged();
        }
    
        /// <summary>
        /// Gets or sets a collection of spans that describe the formatting of subsections of the
        /// text.
        /// </summary>
        PROPERTY_(std::vector<Interfaces::FormattedTextStyleSpan> const&, Spans);
        std::vector<Interfaces::FormattedTextStyleSpan> const& GetSpans() const { return _spans; }
        void SetSpans(std::vector<Interfaces::FormattedTextStyleSpan> const& value)
        {
            _spans = value;
            NotifyChanged();
        }
    
        /// <summary>
        /// Gets or sets the text.
        /// </summary>
        PROPERTY_(std::wstring const&, Text);
        std::wstring const& GetText() const { return _text; }
        void SetText(std::wstring const& value)
        {
            _text = value;
            NotifyChanged();
        }
    
        /// <summary>
        /// Gets or sets the aligment of the text.
        /// </summary>
        PROPERTY_(XUI::Core::Media::TextAlignment const&, TextAlignment);
        XUI::Core::Media::TextAlignment const& GetTextAlignment() const { return _textAlignment; }
        void SetTextAlignment(XUI::Core::Media::TextAlignment const& value)
        {
            _textAlignment = value;
            NotifyChanged();
        }

        /// <summary>
        /// Gets or sets the text wrapping.
        /// </summary>
        PROPERTY_(XUI::Core::Media::TextWrapping const&, TextWrapping);
        XUI::Core::Media::TextWrapping const& GetTextWrapping() const { return _wrapping; }
        void SetTextWrapping(XUI::Core::Media::TextWrapping const& value)
        {
            _wrapping = value;
            NotifyChanged();
        }
    
        /// <summary>
        /// Gets platform-specific platform implementation.
        /// </summary>
        Interfaces::IFormattedText* GetPlatformImpl();
    
        /// <summary>
        /// Gets the lines in the text.
        /// </summary>
        /// <returns>
        /// A collection of <see cref="FormattedTextLine"/> objects.
        /// </returns>
        std::vector<Interfaces::FormattedTextLine> const& GetLines()
        {
            return GetPlatformImpl()->GetLines();
        }
    
        /// <summary>
        /// Hit tests a point in the text.
        /// </summary>
        /// <param name="point">The point.</param>
        /// <returns>
        /// A <see cref="TextHitTestResult"/> describing the result of the hit test.
        /// </returns>
        Interfaces::TextHitTestResult HitTestPoint(Point const& point)
        {
            return GetPlatformImpl()->HitTestPoint(point);
        }
    
        /// <summary>
        /// Gets the bounds rectangle that the specified character occupies.
        /// </summary>
        /// <param name="index">The index of the character.</param>
        /// <returns>The character bounds.</returns>
        Rect HitTestTextPosition(int index)
        {
            return GetPlatformImpl()->HitTestTextPosition(index);
        }
    
        /// <summary>
        /// Gets the bounds rectangles that the specified text range occupies.
        /// </summary>
        /// <param name="index">The index of the first character.</param>
        /// <param name="length">The number of characters in the text range.</param>
        /// <returns>The character bounds.</returns>
        std::vector<Core::Media::Rect> HitTestTextRange(int index, int length)
        {
            return GetPlatformImpl()->HitTestTextRange(index, length);
        }
    
        /// <summary>
        /// Gets the size of the text, taking <see cref="Constraint"/> into account.
        /// </summary>
        /// <returns>The bounds box of the text.</returns>
        Size Measure()
        {
            return GetPlatformImpl()->Size;
        }

        void Invalidate()
        {
            NotifyChanged();
        }
    
    private:
        void NotifyChanged()
        {
            _platformImpl = nullptr;
        }
    };
}

#endif //!_XUI_MEDIA_FORMATTED_TEXT_TYPE_H__
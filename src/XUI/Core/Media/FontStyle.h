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

#ifndef _XUI_FONT_STYLE_H__
#define _XUI_FONT_STYLE_H__

#include <string>

namespace XUI::Core::Media
{
    /// <summary>
    /// Defines the available font styles.
    /// </summary>
    enum class FontStyle
    {
        /// <summary>
        /// A normal font.
        /// </summary>
        Normal,

        /// <summary>
        /// An oblique font.
        /// </summary>
        Oblique,

        /// <summary>
        /// An italic font.
        /// </summary>
        Italic,
    };

    /// <summary>
    /// Defines a set of predefined font weights.
    /// </summary>
    /// <remarks>
    /// As well as the values defined by this enumeration you can also pass any integer value by
    /// casting it to <see cref="FontWeight"/>, e.g. <code>(FontWeight)550</code>.
    /// </remarks>
    enum class FontWeight
    {
        /// <summary>
        /// Specifies a "thin" font weight.
        /// </summary>
        Thin = 100,

        /// <summary>
        /// Specifies an "extra light" font weight.
        /// </summary>
        ExtraLight = 200,

        /// <summary>
        /// Specifies an "ultra light" font weight.
        /// </summary>
        UltraLight = 200,

        /// <summary>
        /// Specifies a "light" font weight.
        /// </summary>
        Light = 300,

        /// <summary>
        /// Specifies a "normal" font weight.
        /// </summary>
        Normal = 400,

        /// <summary>
        /// Specifies a "regular" font weight.
        /// </summary>
        Regular = 400,

        /// <summary>
        /// Specifies a "medium" font weight.
        /// </summary>
        Medium = 500,

        /// <summary>
        /// Specifies a "demi-bold" font weight.
        /// </summary>
        DemiBold = 600,

        /// <summary>
        /// Specifies a "semi-bold" font weight.
        /// </summary>
        SemiBold = 600,

        /// <summary>
        /// Specifies a "bold" font weight.
        /// </summary>
        Bold = 700,

        /// <summary>
        /// Specifies an "extra bold" font weight.
        /// </summary>
        ExtraBold = 800,

        /// <summary>
        /// Specifies an "ultra bold" font weight.
        /// </summary>
        UltraBold = 800,

        /// <summary>
        /// Specifies a "black" font weight.
        /// </summary>
        Black = 900,

        /// <summary>
        /// Specifies a "heavy" font weight.
        /// </summary>
        Heavy = 900,

        /// <summary>
        /// Specifies an "extra black" font weight.
        /// </summary>
        ExtraBlack = 950,

        /// <summary>
        /// Specifies an "ultra black" font weight.
        /// </summary>
        UltraBlack = 950
    };

    /// <summary>
    /// Defines how text is aligned.
    /// </summary>
    enum class TextAlignment
    {
        /// <summary>
        /// The text is left-aligned.
        /// </summary>
        Left,

        /// <summary>
        /// The text is centered.
        /// </summary>
        Center,

        /// <summary>
        /// The text is right-aligned.
        /// </summary>
        Right,
    };

    /// <summary>
    /// Controls the wrapping mode of text.
    /// </summary>
    enum class TextWrapping
    {
        /// <summary>
        /// Text should not wrap.
        /// </summary>
        NoWrap,

        /// <summary>
        /// Text can wrap.
        /// </summary>
        Wrap
    };

    /// <summary>
    /// Represents a typeface.
    /// </summary>
    struct Typeface
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="Typeface"/> class.
        /// </summary>
        /// <param name="fontFamilyName">The name of the font family.</param>
        /// <param name="fontSize">The font size, in DIPs.</param>
        /// <param name="style">The font style.</param>
        /// <param name="weight">The font weight.</param>
        Typeface(
            std::string const& fontFamilyName,
            double fontSize,
            FontStyle style = FontStyle::Normal,
            FontWeight weight = FontWeight::Normal);

        /// <summary>
        /// Gets the name of the font family.
        /// </summary>
        std::string FontFamilyName;

        /// <summary>
        /// Gets the size of the font in DIPs.
        /// </summary>
        double FontSize;

        /// <summary>
        /// Gets the font style.
        /// </summary>
        FontStyle Style;

        /// <summary>
        /// Gets the font weight.
        /// </summary>
        FontWeight Weight;
    };
}

#endif //!_XUI_FONT_STYLE_H__
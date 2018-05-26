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

#include "FontStyle.h"

/// <summary>
/// Initializes a new instance of the <see cref="Typeface"/> class.
/// </summary>
/// <param name="fontFamilyName">The name of the font family.</param>
/// <param name="fontSize">The font size, in DIPs.</param>
/// <param name="style">The font style.</param>
/// <param name="weight">The font weight.</param>

XUI::Core::Media::Typeface::Typeface(std::string const & fontFamilyName, double fontSize, FontStyle style, FontWeight weight)
{
    if (fontSize <= 0)
    {
        throw System::ArgumentException("Font size must be > 0.");
    }

    if (int(weight) <= 0)
    {
        throw System::ArgumentException("Font weight must be > 0.");
    }

    FontFamilyName = fontFamilyName;
    FontSize = fontSize;
    Style = style;
    Weight = weight;
}

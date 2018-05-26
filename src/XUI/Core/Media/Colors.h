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

#ifndef _XUI_MEDIA_COLORS_H__
#define _XUI_MEDIA_COLORS_H__

#include "Core/Media/Color_t.h"

namespace XUI::Core::Media
{
    /// <summary>
    /// Defines a set of predefined colors.
    /// </summary>
    struct Meta(Enable) Meta(AbstractType) Colors final
    {
        /// <summary>
        /// Gets a color with an ARGB value of #fff0f8ff.
        /// </summary>
        PROPERTY_GET_ONLY(Color, AliceBlue);
        static Color GetAliceBlue()
        {
            return Color::FromUInt32(0xfff0f8ff);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #fffaebd7.
        /// </summary>
        PROPERTY_GET_ONLY(Color, AntiqueWhite);
        static Color GetAntiqueWhite()
        {
            return Color::FromUInt32(0xfffaebd7);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff00ffff.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Aqua);
        static Color GetAqua()
        {
            return Color::FromUInt32(0xff00ffff);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff7fffd4.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Aquamarine);
        static Color GetAquamarine()
        {
            return Color::FromUInt32(0xff7fffd4);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #fff0ffff.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Azure);
        static Color GetAzure()
        {
            return Color::FromUInt32(0xfff0ffff);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #fff5f5dc.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Beige);
        static Color GetBeige()
        {
            return Color::FromUInt32(0xfff5f5dc);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffffe4c4.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Bisque);
        static Color GetBisque()
        {
            return Color::FromUInt32(0xffffe4c4);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff000000.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Black);
        static Color GetBlack()
        {
            return Color::FromUInt32(0xff000000);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffffebcd.
        /// </summary>
        PROPERTY_GET_ONLY(Color, BlanchedAlmond);
        static Color GetBlanchedAlmond()
        {
            return Color::FromUInt32(0xffffebcd);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff0000ff.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Blue);
        static Color GetBlue()
        {
            return Color::FromUInt32(0xff0000ff);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff8a2be2.
        /// </summary>
        PROPERTY_GET_ONLY(Color, BlueViolet);
        static Color GetBlueViolet()
        {
            return Color::FromUInt32(0xff8a2be2);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffa52a2a.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Brown);
        static Color GetBrown()
        {
            return Color::FromUInt32(0xffa52a2a);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffdeb887.
        /// </summary>
        PROPERTY_GET_ONLY(Color, BurlyWood);
        static Color GetBurlyWood()
        {
            return Color::FromUInt32(0xffdeb887);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff5f9ea0.
        /// </summary>
        PROPERTY_GET_ONLY(Color, CadetBlue);
        static Color GetCadetBlue()
        {
            return Color::FromUInt32(0xff5f9ea0);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff7fff00.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Chartreuse);
        static Color GetChartreuse()
        {
            return Color::FromUInt32(0xff7fff00);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffd2691e.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Chocolate);
        static Color GetChocolate()
        {
            return Color::FromUInt32(0xffd2691e);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffff7f50.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Coral);
        static Color GetCoral()
        {
            return Color::FromUInt32(0xffff7f50);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff6495ed.
        /// </summary>
        PROPERTY_GET_ONLY(Color, CornflowerBlue);
        static Color GetCornflowerBlue()
        {
            return Color::FromUInt32(0xff6495ed);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #fffff8dc.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Cornsilk);
        static Color GetCornsilk()
        {
            return Color::FromUInt32(0xfffff8dc);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffdc143c.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Crimson);
        static Color GetCrimson()
        {
            return Color::FromUInt32(0xffdc143c);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff00ffff.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Cyan);
        static Color GetCyan()
        {
            return Color::FromUInt32(0xff00ffff);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff00008b.
        /// </summary>
        PROPERTY_GET_ONLY(Color, DarkBlue);
        static Color GetDarkBlue()
        {
            return Color::FromUInt32(0xff00008b);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff008b8b.
        /// </summary>
        PROPERTY_GET_ONLY(Color, DarkCyan);
        static Color GetDarkCyan()
        {
            return Color::FromUInt32(0xff008b8b);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffb8860b.
        /// </summary>
        PROPERTY_GET_ONLY(Color, DarkGoldenrod);
        static Color GetDarkGoldenrod()
        {
            return Color::FromUInt32(0xffb8860b);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffa9a9a9.
        /// </summary>
        PROPERTY_GET_ONLY(Color, DarkGray);
        static Color GetDarkGray()
        {
            return Color::FromUInt32(0xffa9a9a9);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff006400.
        /// </summary>
        PROPERTY_GET_ONLY(Color, DarkGreen);
        static Color GetDarkGreen()
        {
            return Color::FromUInt32(0xff006400);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffbdb76b.
        /// </summary>
        PROPERTY_GET_ONLY(Color, DarkKhaki);
        static Color GetDarkKhaki()
        {
            return Color::FromUInt32(0xffbdb76b);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff8b008b.
        /// </summary>
        PROPERTY_GET_ONLY(Color, DarkMagenta);
        static Color GetDarkMagenta()
        {
            return Color::FromUInt32(0xff8b008b);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff556b2f.
        /// </summary>
        PROPERTY_GET_ONLY(Color, DarkOliveGreen);
        static Color GetDarkOliveGreen()
        {
            return Color::FromUInt32(0xff556b2f);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffff8c00.
        /// </summary>
        PROPERTY_GET_ONLY(Color, DarkOrange);
        static Color GetDarkOrange()
        {
            return Color::FromUInt32(0xffff8c00);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff9932cc.
        /// </summary>
        PROPERTY_GET_ONLY(Color, DarkOrchid);
        static Color GetDarkOrchid()
        {
            return Color::FromUInt32(0xff9932cc);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff8b0000.
        /// </summary>
        PROPERTY_GET_ONLY(Color, DarkRed);
        static Color GetDarkRed()
        {
            return Color::FromUInt32(0xff8b0000);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffe9967a.
        /// </summary>
        PROPERTY_GET_ONLY(Color, DarkSalmon);
        static Color GetDarkSalmon()
        {
            return Color::FromUInt32(0xffe9967a);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff8fbc8f.
        /// </summary>
        PROPERTY_GET_ONLY(Color, DarkSeaGreen);
        static Color GetDarkSeaGreen()
        {
            return Color::FromUInt32(0xff8fbc8f);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff483d8b.
        /// </summary>
        PROPERTY_GET_ONLY(Color, DarkSlateBlue);
        static Color GetDarkSlateBlue()
        {
            return Color::FromUInt32(0xff483d8b);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff2f4f4f.
        /// </summary>
        PROPERTY_GET_ONLY(Color, DarkSlateGray);
        static Color GetDarkSlateGray()
        {
            return Color::FromUInt32(0xff2f4f4f);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff00ced1.
        /// </summary>
        PROPERTY_GET_ONLY(Color, DarkTurquoise);
        static Color GetDarkTurquoise()
        {
            return Color::FromUInt32(0xff00ced1);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff9400d3.
        /// </summary>
        PROPERTY_GET_ONLY(Color, DarkViolet);
        static Color GetDarkViolet()
        {
            return Color::FromUInt32(0xff9400d3);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffff1493.
        /// </summary>
        PROPERTY_GET_ONLY(Color, DeepPink);
        static Color GetDeepPink()
        {
            return Color::FromUInt32(0xffff1493);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff00bfff.
        /// </summary>
        PROPERTY_GET_ONLY(Color, DeepSkyBlue);
        static Color GetDeepSkyBlue()
        {
            return Color::FromUInt32(0xff00bfff);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff696969.
        /// </summary>
        PROPERTY_GET_ONLY(Color, DimGray);
        static Color GetDimGray()
        {
            return Color::FromUInt32(0xff696969);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff1e90ff.
        /// </summary>
        PROPERTY_GET_ONLY(Color, DodgerBlue);
        static Color GetDodgerBlue()
        {
            return Color::FromUInt32(0xff1e90ff);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffb22222.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Firebrick);
        static Color GetFirebrick()
        {
            return Color::FromUInt32(0xffb22222);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #fffffaf0.
        /// </summary>
        PROPERTY_GET_ONLY(Color, FloralWhite);
        static Color GetFloralWhite()
        {
            return Color::FromUInt32(0xfffffaf0);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff228b22.
        /// </summary>
        PROPERTY_GET_ONLY(Color, ForestGreen);
        static Color GetForestGreen()
        {
            return Color::FromUInt32(0xff228b22);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffff00ff.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Fuchsia);
        static Color GetFuchsia()
        {
            return Color::FromUInt32(0xffff00ff);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffdcdcdc.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Gainsboro);
        static Color GetGainsboro()
        {
            return Color::FromUInt32(0xffdcdcdc);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #fff8f8ff.
        /// </summary>
        PROPERTY_GET_ONLY(Color, GhostWhite);
        static Color GetGhostWhite()
        {
            return Color::FromUInt32(0xfff8f8ff);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffffd700.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Gold);
        static Color GetGold()
        {
            return Color::FromUInt32(0xffffd700);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffdaa520.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Goldenrod);
        static Color GetGoldenrod()
        {
            return Color::FromUInt32(0xffdaa520);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff808080.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Gray);
        static Color GetGray()
        {
            return Color::FromUInt32(0xff808080);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff008000.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Green);
        static Color GetGreen()
        {
            return Color::FromUInt32(0xff008000);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffadff2f.
        /// </summary>
        PROPERTY_GET_ONLY(Color, GreenYellow);
        static Color GetGreenYellow()
        {
            return Color::FromUInt32(0xffadff2f);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #fff0fff0.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Honeydew);
        static Color GetHoneydew()
        {
            return Color::FromUInt32(0xfff0fff0);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffff69b4.
        /// </summary>
        PROPERTY_GET_ONLY(Color, HotPink);
        static Color GetHotPink()
        {
            return Color::FromUInt32(0xffff69b4);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffcd5c5c.
        /// </summary>
        PROPERTY_GET_ONLY(Color, IndianRed);
        static Color GetIndianRed()
        {
            return Color::FromUInt32(0xffcd5c5c);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff4b0082.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Indigo);
        static Color GetIndigo()
        {
            return Color::FromUInt32(0xff4b0082);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #fffffff0.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Ivory);
        static Color GetIvory()
        {
            return Color::FromUInt32(0xfffffff0);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #fff0e68c.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Khaki);
        static Color GetKhaki()
        {
            return Color::FromUInt32(0xfff0e68c);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffe6e6fa.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Lavender);
        static Color GetLavender()
        {
            return Color::FromUInt32(0xffe6e6fa);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #fffff0f5.
        /// </summary>
        PROPERTY_GET_ONLY(Color, LavenderBlush);
        static Color GetLavenderBlush()
        {
            return Color::FromUInt32(0xfffff0f5);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff7cfc00.
        /// </summary>
        PROPERTY_GET_ONLY(Color, LawnGreen);
        static Color GetLawnGreen()
        {
            return Color::FromUInt32(0xff7cfc00);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #fffffacd.
        /// </summary>
        PROPERTY_GET_ONLY(Color, LemonChiffon);
        static Color GetLemonChiffon()
        {
            return Color::FromUInt32(0xfffffacd);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffadd8e6.
        /// </summary>
        PROPERTY_GET_ONLY(Color, LightBlue);
        static Color GetLightBlue()
        {
            return Color::FromUInt32(0xffadd8e6);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #fff08080.
        /// </summary>
        PROPERTY_GET_ONLY(Color, LightCoral);
        static Color GetLightCoral()
        {
            return Color::FromUInt32(0xfff08080);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffe0ffff.
        /// </summary>
        PROPERTY_GET_ONLY(Color, LightCyan);
        static Color GetLightCyan()
        {
            return Color::FromUInt32(0xffe0ffff);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #fffafad2.
        /// </summary>
        PROPERTY_GET_ONLY(Color, LightGoldenrodYellow);
        static Color GetLightGoldenrodYellow()
        {
            return Color::FromUInt32(0xfffafad2);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffd3d3d3.
        /// </summary>
        PROPERTY_GET_ONLY(Color, LightGray);
        static Color GetLightGray()
        {
            return Color::FromUInt32(0xffd3d3d3);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff90ee90.
        /// </summary>
        PROPERTY_GET_ONLY(Color, LightGreen);
        static Color GetLightGreen()
        {
            return Color::FromUInt32(0xff90ee90);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffffb6c1.
        /// </summary>
        PROPERTY_GET_ONLY(Color, LightPink);
        static Color GetLightPink()
        {
            return Color::FromUInt32(0xffffb6c1);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffffa07a.
        /// </summary>
        PROPERTY_GET_ONLY(Color, LightSalmon);
        static Color GetLightSalmon()
        {
            return Color::FromUInt32(0xffffa07a);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff20b2aa.
        /// </summary>
        PROPERTY_GET_ONLY(Color, LightSeaGreen);
        static Color GetLightSeaGreen()
        {
            return Color::FromUInt32(0xff20b2aa);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff87cefa.
        /// </summary>
        PROPERTY_GET_ONLY(Color, LightSkyBlue);
        static Color GetLightSkyBlue()
        {
            return Color::FromUInt32(0xff87cefa);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff778899.
        /// </summary>
        PROPERTY_GET_ONLY(Color, LightSlateGray);
        static Color GetLightSlateGray()
        {
            return Color::FromUInt32(0xff778899);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffb0c4de.
        /// </summary>
        PROPERTY_GET_ONLY(Color, LightSteelBlue);
        static Color GetLightSteelBlue()
        {
            return Color::FromUInt32(0xffb0c4de);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffffffe0.
        /// </summary>
        PROPERTY_GET_ONLY(Color, LightYellow);
        static Color GetLightYellow()
        {
            return Color::FromUInt32(0xffffffe0);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff00ff00.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Lime);
        static Color GetLime()
        {
            return Color::FromUInt32(0xff00ff00);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff32cd32.
        /// </summary>
        PROPERTY_GET_ONLY(Color, LimeGreen);
        static Color GetLimeGreen()
        {
            return Color::FromUInt32(0xff32cd32);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #fffaf0e6.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Linen);
        static Color GetLinen()
        {
            return Color::FromUInt32(0xfffaf0e6);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffff00ff.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Magenta);
        static Color GetMagenta()
        {
            return Color::FromUInt32(0xffff00ff);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff800000.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Maroon);
        static Color GetMaroon()
        {
            return Color::FromUInt32(0xff800000);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff66cdaa.
        /// </summary>
        PROPERTY_GET_ONLY(Color, MediumAquamarine);
        static Color GetMediumAquamarine()
        {
            return Color::FromUInt32(0xff66cdaa);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff0000cd.
        /// </summary>
        PROPERTY_GET_ONLY(Color, MediumBlue);
        static Color GetMediumBlue()
        {
            return Color::FromUInt32(0xff0000cd);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffba55d3.
        /// </summary>
        PROPERTY_GET_ONLY(Color, MediumOrchid);
        static Color GetMediumOrchid()
        {
            return Color::FromUInt32(0xffba55d3);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff9370db.
        /// </summary>
        PROPERTY_GET_ONLY(Color, MediumPurple);
        static Color GetMediumPurple()
        {
            return Color::FromUInt32(0xff9370db);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff3cb371.
        /// </summary>
        PROPERTY_GET_ONLY(Color, MediumSeaGreen);
        static Color GetMediumSeaGreen()
        {
            return Color::FromUInt32(0xff3cb371);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff7b68ee.
        /// </summary>
        PROPERTY_GET_ONLY(Color, MediumSlateBlue);
        static Color GetMediumSlateBlue()
        {
            return Color::FromUInt32(0xff7b68ee);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff00fa9a.
        /// </summary>
        PROPERTY_GET_ONLY(Color, MediumSpringGreen);
        static Color GetMediumSpringGreen()
        {
            return Color::FromUInt32(0xff00fa9a);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff48d1cc.
        /// </summary>
        PROPERTY_GET_ONLY(Color, MediumTurquoise);
        static Color GetMediumTurquoise()
        {
            return Color::FromUInt32(0xff48d1cc);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffc71585.
        /// </summary>
        PROPERTY_GET_ONLY(Color, MediumVioletRed);
        static Color GetMediumVioletRed()
        {
            return Color::FromUInt32(0xffc71585);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff191970.
        /// </summary>
        PROPERTY_GET_ONLY(Color, MidnightBlue);
        static Color GetMidnightBlue()
        {
            return Color::FromUInt32(0xff191970);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #fff5fffa.
        /// </summary>
        PROPERTY_GET_ONLY(Color, MintCream);
        static Color GetMintCream()
        {
            return Color::FromUInt32(0xfff5fffa);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffffe4e1.
        /// </summary>
        PROPERTY_GET_ONLY(Color, MistyRose);
        static Color GetMistyRose()
        {
            return Color::FromUInt32(0xffffe4e1);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffffe4b5.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Moccasin);
        static Color GetMoccasin()
        {
            return Color::FromUInt32(0xffffe4b5);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffffdead.
        /// </summary>
        PROPERTY_GET_ONLY(Color, NavajoWhite);
        static Color GetNavajoWhite()
        {
            return Color::FromUInt32(0xffffdead);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff000080.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Navy);
        static Color GetNavy()
        {
            return Color::FromUInt32(0xff000080);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #fffdf5e6.
        /// </summary>
        PROPERTY_GET_ONLY(Color, OldLace);
        static Color GetOldLace()
        {
            return Color::FromUInt32(0xfffdf5e6);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff808000.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Olive);
        static Color GetOlive()
        {
            return Color::FromUInt32(0xff808000);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff6b8e23.
        /// </summary>
        PROPERTY_GET_ONLY(Color, OliveDrab);
        static Color GetOliveDrab()
        {
            return Color::FromUInt32(0xff6b8e23);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffffa500.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Orange);
        static Color GetOrange()
        {
            return Color::FromUInt32(0xffffa500);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffff4500.
        /// </summary>
        PROPERTY_GET_ONLY(Color, OrangeRed);
        static Color GetOrangeRed()
        {
            return Color::FromUInt32(0xffff4500);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffda70d6.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Orchid);
        static Color GetOrchid()
        {
            return Color::FromUInt32(0xffda70d6);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffeee8aa.
        /// </summary>
        PROPERTY_GET_ONLY(Color, PaleGoldenrod);
        static Color GetPaleGoldenrod()
        {
            return Color::FromUInt32(0xffeee8aa);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff98fb98.
        /// </summary>
        PROPERTY_GET_ONLY(Color, PaleGreen);
        static Color GetPaleGreen()
        {
            return Color::FromUInt32(0xff98fb98);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffafeeee.
        /// </summary>
        PROPERTY_GET_ONLY(Color, PaleTurquoise);
        static Color GetPaleTurquoise()
        {
            return Color::FromUInt32(0xffafeeee);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffdb7093.
        /// </summary>
        PROPERTY_GET_ONLY(Color, PaleVioletRed);
        static Color GetPaleVioletRed()
        {
            return Color::FromUInt32(0xffdb7093);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffffefd5.
        /// </summary>
        PROPERTY_GET_ONLY(Color, PapayaWhip);
        static Color GetPapayaWhip()
        {
            return Color::FromUInt32(0xffffefd5);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffffdab9.
        /// </summary>
        PROPERTY_GET_ONLY(Color, PeachPuff);
        static Color GetPeachPuff()
        {
            return Color::FromUInt32(0xffffdab9);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffcd853f.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Peru);
        static Color GetPeru()
        {
            return Color::FromUInt32(0xffcd853f);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffffc0cb.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Pink);
        static Color GetPink()
        {
            return Color::FromUInt32(0xffffc0cb);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffdda0dd.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Plum);
        static Color GetPlum()
        {
            return Color::FromUInt32(0xffdda0dd);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffb0e0e6.
        /// </summary>
        PROPERTY_GET_ONLY(Color, PowderBlue);
        static Color GetPowderBlue()
        {
            return Color::FromUInt32(0xffb0e0e6);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff800080.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Purple);
        static Color GetPurple()
        {
            return Color::FromUInt32(0xff800080);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffff0000.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Red);
        static Color GetRed()
        {
            return Color::FromUInt32(0xffff0000);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffbc8f8f.
        /// </summary>
        PROPERTY_GET_ONLY(Color, RosyBrown);
        static Color GetRosyBrown()
        {
            return Color::FromUInt32(0xffbc8f8f);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff4169e1.
        /// </summary>
        PROPERTY_GET_ONLY(Color, RoyalBlue);
        static Color GetRoyalBlue()
        {
            return Color::FromUInt32(0xff4169e1);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff8b4513.
        /// </summary>
        PROPERTY_GET_ONLY(Color, SaddleBrown);
        static Color GetSaddleBrown()
        {
            return Color::FromUInt32(0xff8b4513);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #fffa8072.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Salmon);
        static Color GetSalmon()
        {
            return Color::FromUInt32(0xfffa8072);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #fff4a460.
        /// </summary>
        PROPERTY_GET_ONLY(Color, SandyBrown);
        static Color GetSandyBrown()
        {
            return Color::FromUInt32(0xfff4a460);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff2e8b57.
        /// </summary>
        PROPERTY_GET_ONLY(Color, SeaGreen);
        static Color GetSeaGreen()
        {
            return Color::FromUInt32(0xff2e8b57);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #fffff5ee.
        /// </summary>
        PROPERTY_GET_ONLY(Color, SeaShell);
        static Color GetSeaShell()
        {
            return Color::FromUInt32(0xfffff5ee);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffa0522d.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Sienna);
        static Color GetSienna()
        {
            return Color::FromUInt32(0xffa0522d);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffc0c0c0.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Silver);
        static Color GetSilver()
        {
            return Color::FromUInt32(0xffc0c0c0);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff87ceeb.
        /// </summary>
        PROPERTY_GET_ONLY(Color, SkyBlue);
        static Color GetSkyBlue()
        {
            return Color::FromUInt32(0xff87ceeb);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff6a5acd.
        /// </summary>
        PROPERTY_GET_ONLY(Color, SlateBlue);
        static Color GetSlateBlue()
        {
            return Color::FromUInt32(0xff6a5acd);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff708090.
        /// </summary>
        PROPERTY_GET_ONLY(Color, SlateGray);
        static Color GetSlateGray()
        {
            return Color::FromUInt32(0xff708090);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #fffffafa.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Snow);
        static Color GetSnow()
        {
            return Color::FromUInt32(0xfffffafa);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff00ff7f.
        /// </summary>
        PROPERTY_GET_ONLY(Color, SpringGreen);
        static Color GetSpringGreen()
        {
            return Color::FromUInt32(0xff00ff7f);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff4682b4.
        /// </summary>
        PROPERTY_GET_ONLY(Color, SteelBlue);
        static Color GetSteelBlue()
        {
            return Color::FromUInt32(0xff4682b4);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffd2b48c.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Tan);
        static Color GetTan()
        {
            return Color::FromUInt32(0xffd2b48c);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff008080.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Teal);
        static Color GetTeal()
        {
            return Color::FromUInt32(0xff008080);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffd8bfd8.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Thistle);
        static Color GetThistle()
        {
            return Color::FromUInt32(0xffd8bfd8);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffff6347.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Tomato);
        static Color GetTomato()
        {
            return Color::FromUInt32(0xffff6347);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #00ffffff.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Transparent);
        static Color GetTransparent()
        {
            return Color::FromUInt32(0x00ffffff);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff40e0d0.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Turquoise);
        static Color GetTurquoise()
        {
            return Color::FromUInt32(0xff40e0d0);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffee82ee.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Violet);
        static Color GetViolet()
        {
            return Color::FromUInt32(0xffee82ee);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #fff5deb3.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Wheat);
        static Color GetWheat()
        {
            return Color::FromUInt32(0xfff5deb3);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffffffff.
        /// </summary>
        PROPERTY_GET_ONLY(Color, White);
        static Color GetWhite()
        {
            return Color::FromUInt32(0xffffffff);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #fff5f5f5.
        /// </summary>
        PROPERTY_GET_ONLY(Color, WhiteSmoke);
        static Color GetWhiteSmoke()
        {
            return Color::FromUInt32(0xfff5f5f5);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ffffff00.
        /// </summary>
        PROPERTY_GET_ONLY(Color, Yellow);
        static Color GetYellow()
        {
            return Color::FromUInt32(0xffffff00);
        }

        /// <summary>
        /// Gets a color with an ARGB value of #ff9acd32.
        /// </summary>
        PROPERTY_GET_ONLY(Color, YellowGreen);
        static Color GetYellowGreen()
        {
            return Color::FromUInt32(0xff9acd32);
        }
    };
}


#endif	//#ifndef _XUI_MEDIA_COLORS_H__

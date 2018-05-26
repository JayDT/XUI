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

#ifndef __XUI_CURSORS_H__
#define __XUI_CURSORS_H__

#include "standard/BasicPrimities.h"

#include "Interfaces/ICursor.h"
#include "Interfaces/IStandardCursorFactory.h"


namespace XUI::UI::Controls
{
    class Visual;
}

namespace XUI::Core::Input
{
    enum class StandardCursorType : uint8
    {
        Arrow = 0,
        Ibeam,
        Wait,
        Cross,
        UpArrow,
        SizeWestEast,
        SizeNorthSouth,
        SizeAll,
        No,
        Hand,
        AppStarting,
        Help,
        TopSide,
        BottomSize,
        LeftSide,
        RightSide,
        TopLeftCorner,
        TopRightCorner,
        BottomLeftCorner,
        BottomRightCorner,


        SizeNS = SizeNorthSouth,
        SizeWE = SizeWestEast,

        // Not available in GTK directly, see http://www.pixelbeat.org/programming/x_cursors/ 
        // We might enable them later, preferably, by loading pixmax direclty from theme with fallback image
        // SizeNorthWestSouthEast,
        // SizeNorthEastSouthWest,
    };

    class TC_XUI_API Cursor : public Interfaces::ICursor
    {
    public:
        static Cursor Default;
        static void StaticClassInitializer();

        virtual ~Cursor() {}

        Cursor() {}
        Cursor(XUI::Interfaces::PlatformHandle const& platformCursor);

        Cursor(StandardCursorType const& cursorType);

        XUI::Interfaces::PlatformHandle PlatformCursor;

        void SetCursorType(Core::Input::StandardCursorType type) override
        {
            PlatformCursor = GetCursor(type);
        }

        XUI::Interfaces::PlatformHandle& GetPlatformCursor() override { return PlatformCursor; }

        /// <summary>
        /// Parses a Cursor string.
        /// </summary>
        /// <param name="s">The brush string.</param>
        /// <returns>The <see cref="Color"/>.</returns>
        static std::shared_ptr<Cursor> Parse(std::string const& s);

        std::string ToString() const override;
    private:
        static XUI::Interfaces::PlatformHandle GetCursor(StandardCursorType type);
    };
}


#endif	//#ifndef __XUI_CURSORS_H__

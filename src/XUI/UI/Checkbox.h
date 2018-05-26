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

#ifndef __XUI_UI_CHECKBOX_H__
#define __XUI_UI_CHECKBOX_H__

#include "UI/Detail/ToggleButton.h"

namespace XUI::UI
{
    class TC_XUI_API CheckBox : public ToggleButton
    {
    public:
        CheckBox()
        {
        }

        virtual ~CheckBox()
        {
        }
    };
}

#endif // !__XUI_UI_CHECKBOX_H__
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

#include "ListBoxItem.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;

STATIC_CLASS_INITIALIZER(ListBoxItem, XUI::UI::ListBoxItem::StaticClassInitializer)

XUI::UI::ListBoxItem::~ListBoxItem()
{
}

void XUI::UI::ListBoxItem::StaticClassInitializer()
{
    FocusableProperty->OverrideDefaultValue<ListBoxItem>(true);
}

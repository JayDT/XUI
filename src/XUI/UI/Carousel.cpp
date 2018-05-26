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

#include "Carousel.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::Interfaces;
using namespace XUI::UI;

STATIC_CLASS_POST_INITIALIZER(Carousel, Carousel::StaticClassInitializer)

XUI::Core::Dependency::RefDependencyProperty Carousel::IsVirtualizedProperty;
XUI::Core::Dependency::RefDependencyProperty Carousel::TransitionProperty;

void Carousel::StaticClassInitializer()
{
    IsVirtualizedProperty = (Core::Dependency::BindProperty<bool>::Register<Carousel>(nameof(IsVirtualized)));
    SelectionModeProperty->OverrideDefaultValue<Carousel>(SelectionMode::AlwaysSelected);
}

bool Carousel::GetIsVirtualized() const
{
    return GetValue<bool>(IsVirtualizedProperty);
}

void Carousel::SetIsVirtualized(const bool &value)
{
    SetValue(IsVirtualizedProperty, value);
}

//IPageTransition *Carousel::GetTransition() const
//{
//    return GetValue(TransitionProperty);
//}
//
//void Carousel::SetTransition(IPageTransition *value)
//{
//    SetValue(TransitionProperty, value);
//}

void Carousel::Next()
{
    if (SelectedIndex < Items->size() - 1)
    {
        ++SelectedIndex;
    }
}

void Carousel::Previous()
{
    if (SelectedIndex > 0)
    {
        --SelectedIndex;
    }
}

void Carousel::OnKeyDown(KeyEventArgs& e)
{
    // Ignore key presses.
}

void Carousel::OnPointerPressed(PointerPressedEventArgs& e)
{
    // Ignore pointer presses.
}

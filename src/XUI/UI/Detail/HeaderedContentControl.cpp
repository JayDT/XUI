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

#include "HeaderedContentControl.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(HeaderedContentControl, XUI::UI::HeaderedContentControl::StaticClassInitializer)

XUI::Core::Dependency::RefDependencyProperty HeaderedContentControl::HeaderProperty;
XUI::Core::Dependency::RefDependencyProperty HeaderedContentControl::IsSelectedProperty;

void XUI::UI::HeaderedContentControl::StaticClassInitializer()
{
    HeaderProperty = (Core::Dependency::BindProperty<System::Reflection::Variant>::Register<HeaderedContentControl>(nameof(Header)));
    IsSelectedProperty = (Core::Dependency::DirectProperty<HeaderedContentControl, bool>::RegisterDirect(nameof(IsSelected),
        [](HeaderedContentControl* x) { return x->IsSelected; },
        [](HeaderedContentControl* x, bool v) { x->IsSelected = v; })
        );

    //ToDo: This hack is temporary
    ClipToBoundsProperty->OverrideDefaultValue<HeaderedContentControl>(false);
    FocusableProperty->OverrideDefaultValue<HeaderedContentControl>(true);
}

XUI::UI::HeaderedContentControl::HeaderedContentControl()
{
}

XUI::UI::HeaderedContentControl::~HeaderedContentControl()
{
}

System::Reflection::Variant HeaderedContentControl::GetHeader()
{
    return std::move(GetValue(HeaderProperty.get()));
}

void HeaderedContentControl::SetHeader(System::Reflection::Variant const& value)
{
    SetValue(HeaderProperty, value);
}

bool HeaderedContentControl::GetIsSelected() const
{
    return _isSelected;
}

void HeaderedContentControl::SetIsSelected(bool value)
{
    SetAndRaise(IsSelectedProperty, _isSelected, value);
}

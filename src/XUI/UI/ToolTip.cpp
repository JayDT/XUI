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

#include "ToolTip.h"
#include "ToolTipService.h"
#include "UI/Detail/Popup.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(ToolTip, XUI::UI::ToolTip::StaticClassInitializer);

XUI::Core::Dependency::RefDependencyProperty ToolTip::IsOpenProperty;
XUI::Core::Dependency::RefDependencyProperty ToolTip::PlacementProperty;
XUI::Core::Dependency::RefDependencyProperty ToolTip::HorizontalOffsetProperty;
XUI::Core::Dependency::RefDependencyProperty ToolTip::VerticalOffsetProperty;
XUI::Core::Dependency::RefDependencyProperty ToolTip::ShowDelayProperty;

void XUI::UI::ToolTip::StaticClassInitializer()
{
    IsOpenProperty = (Core::Dependency::BindProperty<bool>::Register<ToolTip>(nameof(IsOpen)));
    PlacementProperty = (Core::Dependency::BindProperty<UI::PlacementMode>::Register<ToolTip>(nameof(Placement)));
    HorizontalOffsetProperty = (Core::Dependency::BindProperty<double>::Register<ToolTip>(nameof(HorizontalOffset)));
    VerticalOffsetProperty = (Core::Dependency::BindProperty<double>::Register<ToolTip>(nameof(VerticalOffset)));
    ShowDelayProperty = (Core::Dependency::BindProperty<int>::Register<ToolTip>(nameof(ShowDelay), 400));

    IsOpenProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        ToolTip* control = e.Sender->Static_As<ToolTip>();
        if (control)
            control->IsOpenChanged((XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e);
    });
}

ToolTip::~ToolTip()
{
    Close();
}

bool ToolTip::GetIsOpen()
{
    return GetValue<bool>(IsOpenProperty);
}

void ToolTip::SetIsOpen(bool value)
{
    SetValue(IsOpenProperty, value);
}

UI::PlacementMode ToolTip::GetPlacement()
{
    return GetValue<UI::PlacementMode>(PlacementProperty);
}

void ToolTip::SetPlacement(UI::PlacementMode value)
{
    SetValue(PlacementProperty, value);
}

double ToolTip::GetHorizontalOffset()
{
    return GetValue<double>(HorizontalOffsetProperty);
}

void ToolTip::SetHorizontalOffset(double value)
{
    SetValue(HorizontalOffsetProperty, value);
}

double ToolTip::GetVerticalOffset()
{
    return GetValue<double>(VerticalOffsetProperty);
}

void ToolTip::SetVerticalOffset(double value)
{
    SetValue(VerticalOffsetProperty, value);
}

int ToolTip::GetShowDelay()
{
    return GetValue<int>(ShowDelayProperty);
}

void ToolTip::SetShowDelay(int value)
{
    SetValue(ShowDelayProperty, value);
}

void XUI::UI::ToolTip::OnPointerLeave(Interfaces::PointerEventArgs & e)
{
    Controls::ContentControl::OnPointerLeave(e);

    Controls::Control* parent = Parent;
    if (parent && parent->IsMouseOver)
        return;

    if (IsMouseOver)
        return;

    Close();
}

void ToolTip::IsOpenChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    if (e.NewValue.GetValue<bool>())
    {
        Controls::Control* parent = Parent;

        Open(parent);
    }
    else
    {
        Close();
    }
}

void ToolTip::Open(Control *control)
{
    if (!_popup)
    {
        _popup = std::make_shared<PopupRoot>();
        _popup->Content = shared_from_base_static<ToolTip>();
        _popup->SetParent(control);
        _popup->Position = Popup::GetPosition(control, GetPlacement(), _popup.get(), GetHorizontalOffset(), GetVerticalOffset());
        InvalidateArrange();
    }

    _popup->Show();
}

void ToolTip::Close()
{
    if (_popup)
    {
        _popup->Content = nullptr;
        _popup->Hide();
        if (_popup->Parent)
            _popup->Parent->RemoveLogicalChild(_popup.get());
        _popup = nullptr;
    }
}

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

#include "ToolTipService.h"
#include "ToolTip.h"

using namespace XUI;
using namespace XUI::UI;

ToolTipService *ToolTipService::Instance()
{
    static ToolTipService _instance;
    return &_instance;
}

ToolTipService::ToolTipService()
{
}

void ToolTipService::TipChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    auto control = e.Sender->Static_As<Controls::Control>();

    if (e.OldValue.GetBase())
    {
        control->PointerEnter -= System::make_eventhandler(&ToolTipService::ControlPointerEnter, this);
        control->PointerLeave -= System::make_eventhandler(&ToolTipService::ControlPointerLeave, this);
    }

    if (e.NewValue.GetBase())
    {
        control->PointerEnter += System::make_eventhandler(&ToolTipService::ControlPointerEnter, this);
        control->PointerLeave += System::make_eventhandler(&ToolTipService::ControlPointerLeave, this);
    }
}

void ToolTipService::ControlPointerEnter(void* sender, Interfaces::PointerEventArgs& e)
{
    StopTimer();

    auto control = ((System::Reflection::Object*)sender)->Static_As<Controls::Control>();
    if (control && control->ToolTip)
    {
        ToolTip* tip = control->ToolTip->Static_As<ToolTip>();
        if (tip)
        {
            auto showDelay = tip->ShowDelay;
            if (showDelay == 0)
                Open(control);
            else
                StartShowTimer(showDelay, control);
        }
    }
}

void ToolTipService::ControlPointerLeave(void* sender, Interfaces::PointerEventArgs& e)
{
    auto control = ((System::Reflection::Object*)sender)->Static_As<Controls::Control>();
    if (control)
        Close(control);
}

void ToolTipService::StartShowTimer(int showDelay, Controls::Control *control)
{
    _timer.Interval = showDelay;
    _timer.Tick += [this, weakControl = control->weak_from_base_dynamic<Controls::Control>()](void*, System::Events::EventArg& e)
    {
        if (!weakControl.expired())
            Open(weakControl.lock().get());
    };
    _timer.Start();
}

void ToolTipService::Open(Controls::Control *control)
{
    StopTimer();

    if (control->ToolTip)
    {
        ToolTip* tip = control->ToolTip->Static_As<ToolTip>();
        if (tip)
        {
            tip->IsOpen = true;
        }
    }
}

void ToolTipService::Close(Controls::Control *control)
{
    StopTimer();

    if (control->ToolTip)
    {
        ToolTip* tip = control->ToolTip->Static_As<ToolTip>();
        if (tip && !tip->IsMouseOver)
        {
            tip->IsOpen = false;
        }
    }
}

void ToolTipService::StopTimer()
{
    if (_timer.IsEnabled)
        _timer.Stop();
}

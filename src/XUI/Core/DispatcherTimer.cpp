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

#include "DispatcherTimer.h"
#include "Interfaces/IWindowImpl.h"
#include "Platform/UIRenderer.h"
#include "Platform/UIService.h"

using namespace XUI::Core;

uint32 DispatcherTimer::m_nextID = 0;

DispatcherTimer::DispatcherTimer()
    : m_timer(++m_nextID)
{
}

DispatcherTimer::DispatcherTimer(uint32 interval, std::function<void()> const& callback)
    : m_timer(++m_nextID)
{
    Interval = interval;
    Tick += [callback](void*, System::Events::EventArg& e)
    {
        callback();
    };
}

void DispatcherTimer::Finalize()
{
    if (m_timer)
    {
        Stop();
    }
}

void DispatcherTimer::SetInterval(const uint32 value)
{
    bool enabled = IsEnabled;
    Stop();
    m_interval = value;
    IsEnabled = enabled;
}

void DispatcherTimer::SetIsEnabled(const bool value)
{
    if (IsEnabled != value)
    {
        if (value)
        {
            Start();
        }
        else
        {
            Stop();
        }
    }
}

rxcpp::subscription DispatcherTimer::Run(std::function<void()> action, uint32 interval)
{
    auto timer = std::make_shared<DispatcherTimer>(interval, action);

    timer->Start();

    return rxcpp::make_subscription([timer]()
    {
        timer->Stop();
    });
}

rxcpp::subscription DispatcherTimer::RunOnce(std::function<void()> action, uint32 interval)
{
    auto timer = std::make_shared<DispatcherTimer>();

    timer->Interval = interval;

    std::weak_ptr<DispatcherTimer> weak_timer = timer;
    timer->Tick += [action, weak_timer](void*, System::Events::EventArg& e)
    {
        action();
        if (!weak_timer.expired())
            weak_timer.lock()->Stop();
    };

    timer->Start();

    return rxcpp::make_subscription([timer]()
    {
        timer->Stop();
    });
}

void DispatcherTimer::Start()
{
    if (!IsEnabled)
    {
        auto service = Platform::UIService::Instance()->Get<Platform::UIRender>();
        service->GetFirstActiveWindowImpl()->SetTimer(m_timer, m_interval, [this]()
        {
            if (IsEnabled && Tick)
            {
                System::Events::EventArg e;
                Tick(this, e);
            }
        });

        m_isEnabled = true;
    }
}

void DispatcherTimer::Stop()
{
    if (IsEnabled)
    {
        auto service = Platform::UIService::Instance()->Get<Platform::UIRender>();
        service->GetFirstActiveWindowImpl()->UnsetTimer(m_timer);
        m_isEnabled = false;
    }
}

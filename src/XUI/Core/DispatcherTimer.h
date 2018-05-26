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

#ifndef __XUI_DISPATCHERTIMER_H__
#define __XUI_DISPATCHERTIMER_H__

#include <standard/events.h>
#include <rxcpp/rx.hpp>
#include <chrono>

namespace XUI::Core
{
    /// <summary>
    /// A timer that uses a <see cref="Dispatcher"/> to fire at a specified interval.
    /// </summary>
    class TC_XUI_API DispatcherTimer : public System::Reflection::Object
    {
    private:
        static uint32 m_nextID;
        uint32 m_timer = 0;
        uint32 m_interval = 0;
        bool m_isEnabled = false;

        /// <summary>
        /// Initializes a new instance of the <see cref="DispatcherTimer"/> class.
        /// </summary>
    public:
        virtual ~DispatcherTimer()
        {
            Stop();
        }

        DispatcherTimer();

        /// <summary>
        /// Initializes a new instance of the <see cref="DispatcherTimer"/> class.
        /// </summary>
        /// <param name="interval">The interval at which to tick.</param>
        /// <param name="priority">The priority to use.</param>
        /// <param name="callback">The event to call when the timer ticks.</param>
        DispatcherTimer(uint32 interval, std::function<void()> const& callback);

    private:
        /// <summary>
        /// Finalizes an instance of the <see cref="DispatcherTimer"/> class.
        /// </summary>
        void Finalize();

    public:
        /// <summary>
        /// Raised when the timer ticks.
        /// </summary>
        System::Events::EventHandler<> Tick;

        /// <summary>
        /// Gets or sets the interval at which the timer ticks.
        /// </summary>
        PROPERTY_(uint32, Interval);
        uint32 GetInterval() const
        {
            return m_interval;
        }
        void SetInterval(const uint32 value);

        /// <summary>
        /// Gets or sets a value indicating whether the timer is running.
        /// </summary>
        PROPERTY_(bool, IsEnabled);
        bool GetIsEnabled() const
        {
            return m_isEnabled;
        }
        void SetIsEnabled(const bool value);

        /// <summary>
        /// Gets or sets user-defined data associated with the timer.
        /// </summary>
        PROPERTY_AUTO_QT(System::Reflection::Variant, Tag)

        /// <summary>
        /// Starts a new timer.
        /// </summary>
        /// <param name="action">
        /// The method to call on timer tick. If the method returns false, the timer will stop.
        /// </param>
        /// <param name="interval">The interval at which to tick.</param>
        /// <param name="priority">The priority to use.</param>
        /// <returns>An <see cref="IDisposable"/> used to cancel the timer.</returns>
        static rxcpp::subscription Run(std::function<void()> action, uint32 interval);

        /// <summary>
        /// Runs a method once, after the specified interval.
        /// </summary>
        /// <param name="action">
        /// The method to call after the interval has elapsed.
        /// </param>
        /// <param name="interval">The interval after which to call the method.</param>
        /// <param name="priority">The priority to use.</param>
        /// <returns>An <see cref="IDisposable"/> used to cancel the timer.</returns>
        static rxcpp::subscription RunOnce(std::function<void()> action, uint32 interval);

        /// <summary>
        /// Starts the timer.
        /// </summary>
        void Start();

        /// <summary>
        /// Stops the timer.
        /// </summary>
        void Stop();
    };
}


#endif	//#ifndef __XUI_DISPATCHERTIMER_H__

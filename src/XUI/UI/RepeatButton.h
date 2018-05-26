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

#ifndef __XUI_REPEATBUTTON_H__
#define __XUI_REPEATBUTTON_H__

#include "UI/Button.h"

namespace XUI::UI
{
    class RepeatButton : public Button
    {
    public:
        /// <summary>
        /// Defines the <see cref="Delay"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty DelayProperty;

    private:
        //DispatcherTimer *_repeatTimer;

        /// <summary>
        /// Gets or sets the amount of time, in milliseconds, of repeating clicks.
        /// </summary>
    public:
        virtual ~RepeatButton()
        {
        }

        static void StaticClassInitializer();

        PROPERTY_(int, Delay);
        int GetDelay() const;
        void SetDelay(const int &value);

    private:
        void StartTimer();

        void StopTimer();

    protected:
        void OnKeyDown(Interfaces::KeyEventArgs& e) override;

        void OnKeyUp(Interfaces::KeyEventArgs& e) override;

        void OnPointerPressed(Interfaces::PointerPressedEventArgs& e) override;

        void OnPointerReleased(Interfaces::PointerReleasedEventArgs& e) override;
    };
}


#endif	//#ifndef __XUI_REPEATBUTTON_H__

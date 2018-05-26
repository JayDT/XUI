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

#ifndef __XUI_TOGGLEBUTTON_H__
#define __XUI_TOGGLEBUTTON_H__

#include "UI/Button.h"

namespace XUI::UI
{
    enum ToggleButtonState
    {
        TBS_NONE = 0,
        TBS_CHECKED = 1,
        TBS_INTERMEDIATE = 2
    };

    class TC_XUI_API ToggleButton : public Button
    {
    public:
        static Core::Dependency::RefDependencyProperty IsCheckedProperty;

        static Core::Dependency::RefDependencyProperty IsThreeStateProperty;

    private:
        ToggleButtonState _isChecked = ToggleButtonState::TBS_NONE;

    public:
        static void StaticClassInitializer();

        PROPERTY_(ToggleButtonState, IsChecked);
        ToggleButtonState GetIsChecked() const;
        void SetIsChecked(ToggleButtonState value);

        PROPERTY_(bool, IsThreeState);
        bool GetIsThreeState() const;
        void SetIsThreeState(bool value);

    protected:
        void OnClick(Core::Observer::RoutedEventArgs & e) override;

        virtual void Toggle();
    };
}


#endif	//#ifndef __XUI_TOGGLEBUTTON_H__

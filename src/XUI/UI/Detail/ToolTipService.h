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

#ifndef __XUI_TOOLTIPSERVICE_H__
#define __XUI_TOOLTIPSERVICE_H__

#include "Core/DispatcherTimer.h"

namespace XUI::UI
{
    namespace Controls
    {
        class Control;
    }

    /// <summary>
    /// Handeles <see cref="ToolTip"/> interaction with controls.
    /// </summary>
    class ToolTipService final
    {
    private:
        Core::DispatcherTimer _timer;

        ToolTipService();

    public:
        /// <summary>
        /// Called when the pointer enters a control with an attached tooltip.
        /// </summary>
        /// <param name="sender">The event sender.</param>
        /// <param name="e">The event args.</param>
        void ControlPointerEnter(void* sender, Interfaces::PointerEventArgs& e);

        /// <summary>
        /// Called when the pointer leaves a control with an attached tooltip.
        /// </summary>
        /// <param name="sender">The event sender.</param>
        /// <param name="e">The event args.</param>
        void ControlPointerLeave(void* sender, Interfaces::PointerEventArgs& e);

        void StartShowTimer(int showDelay, Controls::Control *control);

        void Open(Controls::Control *control);

        void Close(Controls::Control *control);

        void StopTimer();

        static ToolTipService *Instance();

        /// <summary>
        /// called when the <see cref="ToolTip.TipProperty"/> property changes on a control.
        /// </summary>
        /// <param name="e">The event args.</param>
        void TipChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e);
    };
}


#endif	//#ifndef __XUI_TOOLTIPSERVICE_H__

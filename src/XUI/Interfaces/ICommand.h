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

#ifndef __XUI_ICOMMAND_H__
#define __XUI_ICOMMAND_H__

#include "standard/events.h"
#include "Reflection/Runtime/Variant.h"

namespace XUI::Interfaces
{
    struct ICommand : System::Reflection::Object
    {
        virtual ~ICommand() {}

        //
        // Summary:
        //     Occurs when changes occur that affect whether or not the command should execute.
        System::Events::EventHandler<System::Events::EventArg, System::Events::DummyMutex> CanExecuteChanged;

        //
        // Summary:
        //     Defines the method that determines whether the command can execute in its current
        //     state.
        //
        // Parameters:
        //   parameter:
        //     Data used by the command. If the command does not require data to be passed,
        //     this object can be set to null.
        //
        // Returns:
        //     true if this command can be executed; otherwise, false.
        virtual bool CanExecute(System::Reflection::meta::Variant parameter, UI::Controls::Control* target) = 0;
        //
        // Summary:
        //     Defines the method to be called when the command is invoked.
        //
        // Parameters:
        //   parameter:
        //     Data used by the command. If the command does not require data to be passed,
        //     this object can be set to null.
        virtual void Execute(System::Reflection::meta::Variant parameter, UI::Controls::Control* target) = 0;
    };
}

#endif //!__XUI_ICOMMAND_H__
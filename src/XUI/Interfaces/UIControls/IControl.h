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

#ifndef __XUI_ICONTROL_H__
#define __XUI_ICONTROL_H__

#include "standard/events.h"
#include "UI/DataTemplates/DataTemplate.h"

#include <boost/uuid/uuid.hpp>

namespace System::Reflection::Assembly
{
    class AssemblyModulXUI;
};

namespace XUI::UI
{
    namespace Controls
    {
        class Control;
        class UIElement;
    }
}

namespace XUI::Interfaces::UIControls
{
    /// <summary>
    /// Defines the application-global data templates.
    /// </summary>
    struct
        IGlobalDataTemplates
    {
        //// {DD2721A4-293F-42D2-A087-7A1AA7C5F1BF}
        static constexpr boost::uuids::uuid __uuid = { 0xdd, 0x27, 0x21, 0xa4, 0x29, 0x3f, 0x42, 0xd2, 0xa0, 0x87, 0x7a, 0x1a, 0xa7, 0xc5, 0xf1, 0xbf };

        /// <summary>
        /// Gets the application-global data templates.
        /// </summary>
        virtual void AddTemplates(std::shared_ptr<UI::DataTemplates::FrameworkTemplate> const&) = 0;
        virtual void RemoveTemplates(std::shared_ptr<UI::DataTemplates::FrameworkTemplate> const&) = 0;
        virtual System::Collection::List<std::shared_ptr<UI::DataTemplates::FrameworkTemplate>>& GlobalTemplates() = 0;
    };

    struct
        IFocusScope
    {
        virtual ~IFocusScope() {}
    };

    /// <summary>
    /// Interface for styleable elements.
    /// </summary>
    struct Meta(Enable) ISelectable
    {
        virtual bool GetIsSelected() const = 0;
        virtual void SetIsSelected(const bool value) = 0;
    };
}

#endif // !__XUI_ICONTROL_H__

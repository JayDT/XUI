/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2016 OmniGUI Platform
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

#ifndef __XAMLCPP_ITRIGGER_BASE_H__
#define __XAMLCPP_ITRIGGER_BASE_H__

#include <System/System.h>
#include <standard/collection.h>

#undef FindResource

namespace XUI::UI::Controls
{
    class UIElement;
}

namespace XamlCPP::Interfaces
{
    struct IStyle;
    struct IStyleable;
    struct IStyleHost;

    struct Meta(AbstractType) ITriggerAction : public System::Reflection::Object
    {
        virtual void Invoke(XamlCPP::Interfaces::IStyleable *control) = 0;
    };

    /// <summary>
    /// Defines the interface for styles.
    /// </summary>
    struct Meta(AbstractType) ITriggerBase : public System::Reflection::Object
    {
        virtual System::Collection::Vector<std::shared_ptr<ITriggerAction>>& GetEnterActions() = 0;
        virtual System::Collection::Vector<std::shared_ptr<ITriggerAction>>& GetExitActions() = 0;
        virtual rxcpp::subscription Apply(XUI::UI::Controls::UIElement *control) = 0;
    };
}


#endif	//#ifndef __XAMLCPP_ITRIGGER_BASE_H__

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

#ifndef __XUI_HEADEREDCONTENTCONTROL_H__
#define __XUI_HEADEREDCONTENTCONTROL_H__

#include "UI/Controls/ContentControl.h"

namespace XUI::UI
{
    /// <summary>
    /// A <see cref="ContentControl"/> with a header.
    /// </summary>
    class TC_XUI_API HeaderedContentControl : public Controls::ContentControl, public Interfaces::UIControls::ISelectable
    {
    private:
        bool _isSelected = false;

    public:
        /// <summary>
        /// Defines the <see cref="Header"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty HeaderProperty;
        static Core::Dependency::RefDependencyProperty IsSelectedProperty;

        static void StaticClassInitializer();

        HeaderedContentControl();
        virtual ~HeaderedContentControl();

        /// <summary>
        /// Gets or sets the header content.
        /// </summary>
        PROPERTY_(System::Reflection::Variant, Header);
        System::Reflection::Variant GetHeader();
        void SetHeader(System::Reflection::Variant const& value);

        /// <summary>
        /// Gets or sets the selected state of the object.
        /// </summary>
        PROPERTY_(bool, IsSelected);
        virtual bool GetIsSelected() const override;
        virtual void SetIsSelected(const bool value) override;
    };
}


#endif	//#ifndef __XUI_HEADEREDCONTENTCONTROL_H__

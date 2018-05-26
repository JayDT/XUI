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

#ifndef __XUI_EXPANDER_H__
#define __XUI_EXPANDER_H__

#include "Detail/HeaderedContentControl.h"

namespace XUI::UI
{
    enum class ExpandDirection
    {
        Down,
        Up,
        Left,
        Right
    };

    class TC_XUI_API Expander : public HeaderedContentControl
    {
    public:
        static XUI::Core::Dependency::RefDependencyProperty ContentTransitionProperty;

        static XUI::Core::Dependency::RefDependencyProperty ExpandDirectionProperty;

        static XUI::Core::Dependency::RefDependencyProperty IsExpandedProperty;

        static void StaticClassInitializer();

    public:
        //PROPERTY_(IPageTransition * , ContentTransition);
        //IPageTransition *GetContentTransition() const;
        //void SetContentTransition(IPageTransition *value);

        PROPERTY_(XUI::UI::ExpandDirection , ExpandDirection);
        XUI::UI::ExpandDirection GetExpandDirection() const;
        void SetExpandDirection(const XUI::UI::ExpandDirection &value);

        PROPERTY_(bool , IsExpanded);
        bool GetIsExpanded() const;
        void SetIsExpanded(const bool &value);

    protected:
        virtual void OnIsExpandedChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e);

    private:
        //IPageTransition *_contentTransition;
        XUI::UI::ExpandDirection _expandDirection = XUI::UI::ExpandDirection::Down;
        bool _isExpanded = false;
    };
}


#endif	//#ifndef __XUI_EXPANDER_H__

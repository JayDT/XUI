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

#ifndef __XUI_POPUPROOT_H__
#define __XUI_POPUPROOT_H__

#include "UI/TopLevel.h"

namespace XUI::UI
{
    /// <summary>
    /// The root window of a <see cref="Popup"/>.
    /// </summary>
    class TC_XUI_API PopupRoot
        : public TopLevel
        , public Controls::NameScope
    {
    private:
        rxcpp::subscription _presenterSubscription;
        XUI::Interfaces::IWindowImpl* _parentWnd = nullptr;
        bool _platformImplInitialized = false;
        bool _embedWnd = false;

    public:
        /// <summary>
        /// Initializes static members of the <see cref="PopupRoot"/> class.
        /// </summary>
        static void StaticClassInitializer();
        virtual void InstanceInitializer() override;

        /// <summary>
        /// Initializes a new instance of the <see cref="PopupRoot"/> class.
        /// </summary>
        PopupRoot();

        /// <summary>
        /// Gets the parent control in the event route.
        /// </summary>
        /// <remarks>
        /// Popup events are passed to their parent window. This facilitates this.
        /// </remarks>
        PROPERTY_GET_ONLY(Controls::InputElement* , InteractiveParent);
        Controls::InputElement* GetInteractiveParent() const;

        /// <summary>
        /// Gets the control that is hosting the popup root.
        /// </summary>
        PROPERTY_GET_ONLY(Controls::Visual * , Host);
        Controls::Visual *GetHost() const;

        /// <inheritdoc/>
        virtual ~PopupRoot();

        void EndInit();

        /// <summary>
        /// Closes the window.
        /// </summary>
        void Close() override;

        /// <summary>
        /// Hides the window but does not close it.
        /// </summary>
        void Hide() override;

        /// <summary>
        /// Shows the window.
        /// </summary>
        void Show() override;

        XUI::Interfaces::IWindowImpl* GetParentWnd() const { return _parentWnd; }
        
        bool IsEmbed() const { return _embedWnd; }
        void SetEmbed(bool on) { _embedWnd = on; }

        void OnGotFocus(Interfaces::GotFocusEventArgs & e) override;
        void OnLostFocus(Core::Observer::RoutedEventArgs& e) override;

    protected:
        /// <inheritdoc/>
        void OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e) override;

        virtual void InitializePlatformWindow();

    private:
        void SetTemplatedParentAndApplyChildTemplates(Controls::Control *control);
    };
}


#endif	//#ifndef __XUI_POPUPROOT_H__

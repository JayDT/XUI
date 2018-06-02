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

#ifndef __XUI_UI_CONTROLS_BUTTON_H__
#define __XUI_UI_CONTROLS_BUTTON_H__

#include "UI/Controls/ContentControl.h"
#include "Interfaces/ICommand.h"
#include "Core/Input/KeyGesture.h"

namespace XUI::UI
{
    /// <summary>
    /// Defines how a <see cref="Button"/> reacts to clicks.
    /// </summary>
    enum class ClickMode
    {
        /// <summary>
        /// The <see cref="Button.Click"/> event is raised when the pointer is released.
        /// </summary>
        Release,

        /// <summary>
        /// The <see cref="Button.Click"/> event is raised when the pointer is pressed.
        /// </summary>
        Press,

        //// Summary:
        ////     Specifies that the Button event
        ////     should be raised when the mouse hovers over a control.
        Hover,
    };

    class TC_XUI_API Button : public Controls::ContentControl
    {
    public:

        /// <summary>
        /// Defines the <see cref="ClickMode"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty ClickModeProperty;

        /// <summary>
        /// Defines the <see cref="Command"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty CommandProperty;

        /// <summary>
        /// Defines the <see cref="HotKey"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty HotKeyProperty;

        /// <summary>
        /// Defines the <see cref="CommandParameter"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty CommandParameterProperty;

        /// <summary>
        /// Defines the <see cref="IsDefaultProperty"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty IsDefaultProperty;

        static Core::Dependency::RefDependencyProperty IsCancelProperty;
        static Core::Dependency::RefDependencyProperty IsDefaultedProperty;
        static Core::Dependency::RefDependencyProperty IsPressedProperty;

        /// <summary>
        /// Defines the <see cref="Click"/> event.
        /// </summary>
        static std::shared_ptr<Core::Observer::RoutedEvent> ClickEvent;

        static void StaticClassInitializer();

        Button();
        virtual ~Button();

        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::PropertyRoutedEventHandler Click;

        /// <summary>
        /// Gets or sets a value indicating how the <see cref="Button"/> should react to clicks.
        /// </summary>
        PROPERTY_(XUI::UI::ClickMode, ClickMode);
        XUI::UI::ClickMode GetClickMode() const;
        void SetClickMode(XUI::UI::ClickMode value);

        /// <summary>
        /// Gets or sets an <see cref="ICommand"/> to be invoked when the button is clicked.
        /// </summary>
        PROPERTY_(std::shared_ptr<Interfaces::ICommand>, Command);
        std::shared_ptr<Interfaces::ICommand> GetCommand() const;
        void SetCommand(std::shared_ptr<Interfaces::ICommand> const& value);

        /// <summary>
        /// Gets or sets an <see cref="KeyGesture"/> associated with this control
        /// </summary>
        PROPERTY_(Core::Input::KeyGesture, HotKey);
        Core::Input::KeyGesture GetHotKey() const;
        void SetHotKey(Core::Input::KeyGesture value);

        /// <summary>
        /// Gets or sets a parameter to be passed to the <see cref="Command"/>.
        /// </summary>
        PROPERTY_(System::Reflection::meta::Variant, CommandParameter);
        System::Reflection::meta::Variant GetCommandParameter() const;
        void SetCommandParameter(System::Reflection::meta::Variant value);

        /// <summary>
        /// Gets or sets a value indicating whether the button is the default button for the
        /// window.
        /// </summary>
        PROPERTY_(bool, IsDefault);
        bool GetIsDefault() const;
        void SetIsDefault(bool value);

        PROPERTY_(bool, IsCancel);
        bool GetIsCancel() const;
        void SetIsCancel(bool value);

        PROPERTY_(bool, IsDefaulted);
        bool GetIsDefaulted() const;
        void SetIsDefaulted(bool value);

        PROPERTY_(bool, IsPressed);
        bool GetIsPressed() const;
        void SetIsPressed(bool value);
    protected:

        /// <inheritdoc/>
        void OnAttachedToVisualTree(Controls::VisualTreeAttachmentEventArgs const& e)  override;

        /// <inheritdoc/>
        void OnKeyDown(Interfaces::KeyEventArgs& e) override;

        /// <inheritdoc/>
        void OnKeyUp(Interfaces::KeyEventArgs& e) override;

        /// <inheritdoc/>
        void OnDetachedFromVisualTree(Controls::VisualTreeAttachmentEventArgs const& e) override;

        /// <summary>
        /// Invokes the <see cref="Click"/> event.
        /// </summary>
        /// <param name="e">The event args.</param>
        virtual void OnClick(Core::Observer::RoutedEventArgs& e);

        /// <inheritdoc/>
        void OnPointerPressed(Interfaces::PointerPressedEventArgs& e) override;

        /// <inheritdoc/>
        void OnPointerReleased(Interfaces::PointerReleasedEventArgs& e) override;

        /// <summary>
        /// Called when the <see cref="Command"/> property changes.
        /// </summary>
        /// <param name="e">The event args.</param>
        static void CommandChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e);

        /// <summary>
        /// Called when the <see cref="IsDefault"/> property changes.
        /// </summary>
        /// <param name="e">The event args.</param>
        static void IsDefaultChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e);

        /// <summary>
        /// Called when the <see cref="ICommand.CanExecuteChanged"/> event fires.
        /// </summary>
        /// <param name="sender">The event sender.</param>
        /// <param name="e">The event args.</param>
        void CanExecuteChanged(void* sender, System::Events::EventArg& e);

        /// <summary>
        /// Starts listening for the Enter key when the button <see cref="IsDefault"/>.
        /// </summary>
        /// <param name="root">The input root.</param>
        void ListenForDefault(Controls::InputElement* root);

        /// <summary>
        /// Stops listening for the Enter key when the button is no longer <see cref="IsDefault"/>.
        /// </summary>
        /// <param name="root">The input root.</param>
        void StopListeningForDefault(Controls::InputElement* root);

        /// <summary>
        /// Raises the <see cref="Click"/> event.
        /// </summary>
        void RaiseClickEvent();

        /// <summary>
        /// Called when a key is pressed on the input root and the button <see cref="IsDefault"/>.
        /// </summary>
        /// <param name="sender">The event sender.</param>
        /// <param name="e">The event args.</param>
        void RootKeyDown(void* sender, Interfaces::KeyEventArgs& e);
    };
}

#endif // !__XUI_UI_CONTROLS_BUTTON_H__
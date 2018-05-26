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

#include "Button.h"

#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::Core::Observer;

STATIC_CLASS_INITIALIZER(Button, XUI::UI::Button::StaticClassInitializer)

Core::Dependency::RefDependencyProperty         XUI::UI::Button::ClickModeProperty;
Core::Dependency::RefDependencyProperty         XUI::UI::Button::CommandProperty;
Core::Dependency::RefDependencyProperty         XUI::UI::Button::HotKeyProperty;
Core::Dependency::RefDependencyProperty         XUI::UI::Button::CommandParameterProperty;
Core::Dependency::RefDependencyProperty         XUI::UI::Button::IsDefaultProperty;
Core::Dependency::RefDependencyProperty         XUI::UI::Button::IsCancelProperty;
Core::Dependency::RefDependencyProperty         XUI::UI::Button::IsDefaultedProperty;
Core::Dependency::RefDependencyProperty         XUI::UI::Button::IsPressedProperty;
std::shared_ptr<Core::Observer::RoutedEvent>    XUI::UI::Button::ClickEvent;

/// <summary>
/// Initializes static members of the <see cref="Button"/> class.
/// </summary>
void XUI::UI::Button::StaticClassInitializer()
{
    ClickModeProperty = (Core::Dependency::BindProperty<XUI::UI::ClickMode>::Register<XUI::UI::Button>(nameof(ClickMode)));
    CommandProperty = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::ICommand>>::Register<XUI::UI::Button>(nameof(Command)));
    HotKeyProperty = (Core::Dependency::BindProperty<Core::Input::KeyGesture>::Register<XUI::UI::Button>(nameof(HotKey)));
    CommandParameterProperty = (Core::Dependency::BindProperty<System::Reflection::meta::Variant>::Register<XUI::UI::Button>(nameof(CommandParameter)));
    IsDefaultProperty = (Core::Dependency::BindProperty<bool>::Register<XUI::UI::Button>(nameof(IsDefault)));
    IsCancelProperty = (Core::Dependency::BindProperty<bool>::Register<XUI::UI::Button>(nameof(IsCancel)));
    IsDefaultedProperty = (Core::Dependency::BindProperty<bool>::Register<XUI::UI::Button>(nameof(IsDefaulted)));
    IsPressedProperty = (Core::Dependency::BindProperty<bool>::Register<XUI::UI::Button>(nameof(IsPressed)));

    ClickEvent = (Core::Observer::RoutedEvent::Register<XUI::UI::Button, Core::Observer::RoutedEventArgs>(nameof(Click), RoutingStrategies(RoutingStrategies::Bubble)));

    FocusableProperty->OverrideDefaultValue<XUI::UI::Button>(true);
    ClickEvent->AddClassSpecHandler<Core::Observer::RoutedEventArgs, Button>(
		System::make_eventhandler([](void* x, Core::Observer::RoutedEventArgs& e)
		{
		    GENERIC_SAFE_CAST(Button*, x)->OnClick(e);
		})
	);
    CommandProperty->Changed.get_observable().subscribe(CommandChanged);
    IsDefaultProperty->Changed.get_observable().subscribe(IsDefaultChanged);

    InputElement::IsPointerOverProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        Button* control = e.Sender->Static_As<Button>();
        if (control && control->ClickMode == ClickMode::Hover)
        {
            control->RaiseClickEvent();
        }
    });
}

XUI::UI::Button::Button()
    : Click(ClickEvent.get(), this)
{
}

XUI::UI::Button::~Button()
{
}

XUI::UI::ClickMode XUI::UI::Button::GetClickMode() const
{
    return GetValue<XUI::UI::ClickMode>(ClickModeProperty.get());
}

void XUI::UI::Button::SetClickMode(XUI::UI::ClickMode value)
{
    SetValue(ClickModeProperty.get(), value);
}

std::shared_ptr<Interfaces::ICommand> XUI::UI::Button::GetCommand() const
{
    return GetValue<std::shared_ptr<Interfaces::ICommand>>(CommandProperty.get());
}

void XUI::UI::Button::SetCommand(std::shared_ptr<Interfaces::ICommand> const& value)
{
    SetValue(CommandProperty.get(), value);
}

Core::Input::KeyGesture XUI::UI::Button::GetHotKey() const
{
    return GetValue<Core::Input::KeyGesture>(HotKeyProperty.get());
}

void XUI::UI::Button::SetHotKey(Core::Input::KeyGesture value)
{
    SetValue(HotKeyProperty.get(), value);
}

System::Reflection::meta::Variant XUI::UI::Button::GetCommandParameter() const
{
    return GetValue(CommandParameterProperty.get());
}

void XUI::UI::Button::SetCommandParameter(System::Reflection::meta::Variant value)
{
    SetValue(CommandParameterProperty.get(), value);
}

bool XUI::UI::Button::GetIsDefault() const
{
    return GetValue<bool>(IsDefaultProperty.get());
}

void XUI::UI::Button::SetIsDefault(bool value)
{
    SetValue(IsDefaultProperty.get(), value);
}

bool XUI::UI::Button::GetIsCancel() const
{
    return GetValue<bool>(IsCancelProperty.get());
}

void XUI::UI::Button::SetIsCancel(bool value)
{
    SetValue(IsCancelProperty.get(), value);
}

bool XUI::UI::Button::GetIsDefaulted() const
{
    return GetValue<bool>(IsDefaultedProperty.get());
}

void XUI::UI::Button::SetIsDefaulted(bool value)
{
    SetValue(IsDefaultedProperty.get(), value);
}

bool XUI::UI::Button::GetIsPressed() const
{
    return GetValue<bool>(IsPressedProperty.get());
}

void XUI::UI::Button::SetIsPressed(bool value)
{
    SetValue(IsPressedProperty.get(), value);
}
/// <inheritdoc/>

void XUI::UI::Button::OnAttachedToVisualTree(Controls::VisualTreeAttachmentEventArgs const & e)
{
    Controls::ContentControl::OnAttachedToVisualTree(e);

    if (IsDefault)
    {
        auto inputElement = dynamic_cast<Controls::InputElement*>(e.Root);

        if (inputElement != nullptr)
        {
            ListenForDefault(inputElement);
        }
    }
}

/// <inheritdoc/>

void XUI::UI::Button::OnKeyDown(Interfaces::KeyEventArgs & e)
{
    if (e.Key == Interfaces::Key::Enter)
    {
        RaiseClickEvent();
        e.Handled = true;
    }
    else if (e.Key == Interfaces::Key::Space)
    {
        if (ClickMode == ClickMode::Press)
        {
            RaiseClickEvent();
        }

        e.Handled = true;
    }

    Controls::ContentControl::OnKeyDown(e);
}

/// <inheritdoc/>

void XUI::UI::Button::OnKeyUp(Interfaces::KeyEventArgs & e)
{
    if (e.Key == Interfaces::Key::Space)
    {
        if (ClickMode == ClickMode::Release)
        {
            RaiseClickEvent();
        }

        e.Handled = true;
    }
}

/// <inheritdoc/>

void XUI::UI::Button::OnDetachedFromVisualTree(Controls::VisualTreeAttachmentEventArgs const & e)
{
    Controls::ContentControl::OnDetachedFromVisualTree(e);

    if (IsDefault)
    {
        auto inputElement = dynamic_cast<Controls::InputElement*>(e.Root);

        if (inputElement != nullptr)
        {
            StopListeningForDefault(inputElement);
        }
    }
}

/// <summary>
/// Invokes the <see cref="Click"/> event.
/// </summary>
/// <param name="e">The event args.</param>

void XUI::UI::Button::OnClick(Core::Observer::RoutedEventArgs & e)
{
    if (Command && Command->CanExecute(CommandParameter, this))
        Command->Execute(CommandParameter, this);
}

/// <inheritdoc/>

void XUI::UI::Button::OnPointerPressed(Interfaces::PointerPressedEventArgs & e)
{
    Controls::ContentControl::OnPointerPressed(e);

    e.Handled = true;

    //PseudoClasses.insert(":pressed");

    if (ClickMode == ClickMode::Press)
    {
        RaiseClickEvent();
    }
    else
    {
        e.Device->Capture(this);
    }
}

/// <inheritdoc/>

void XUI::UI::Button::OnPointerReleased(Interfaces::PointerReleasedEventArgs & e)
{
    Controls::ContentControl::OnPointerReleased(e);

    e.Device->Capture(nullptr);
    //PseudoClasses.erase(":pressed");
    e.Handled = true;

    if (ClickMode == ClickMode::Release && (IsPointerOver || IsMouseOver))
    {
        RaiseClickEvent();
    }
}

/// <summary>
/// Called when the <see cref="Command"/> property changes.
/// </summary>
/// <param name="e">The event args.</param>

void XUI::UI::Button::CommandChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    auto button = dynamic_cast<Button*>(e.Sender);

    if (button != nullptr)
    {
        auto oldCommand = e.OldValue.GetValue<Interfaces::ICommand*>();
        auto newCommand = e.NewValue.GetValue<Interfaces::ICommand*>();

        if (oldCommand != nullptr)
        {
            oldCommand->CanExecuteChanged -= System::make_eventhandler(&Button::CanExecuteChanged, button);
        }

        if (newCommand != nullptr)
        {
            newCommand->CanExecuteChanged += System::make_eventhandler(&Button::CanExecuteChanged, button);
        }

        button->CanExecuteChanged(button, System::Events::EventArg());
    }
}

/// <summary>
/// Called when the <see cref="IsDefault"/> property changes.
/// </summary>
/// <param name="e">The event args.</param>

void XUI::UI::Button::IsDefaultChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    auto button = dynamic_cast<Button*>(e.Sender);
    auto isDefault = e.NewValue.GetValue<bool>();
    auto inputRoot = dynamic_cast<Controls::InputElement*>(button->VisualRoot);

    if (inputRoot != nullptr)
    {
        if (isDefault)
        {
            button->ListenForDefault(inputRoot);
        }
        else
        {
            button->StopListeningForDefault(inputRoot);
        }
    }
}

/// <summary>
/// Called when the <see cref="ICommand.CanExecuteChanged"/> event fires.
/// </summary>
/// <param name="sender">The event sender.</param>
/// <param name="e">The event args.</param>

void XUI::UI::Button::CanExecuteChanged(void * sender, System::Events::EventArg & e)
{
    // HACK: Just set the IsEnabled property for the moment. This needs to be changed to
    // use IsEnabledCore etc. but it will do for now.
    IsEnabled = Command == nullptr || Command->CanExecute(CommandParameter, this);
}

/// <summary>
/// Starts listening for the Enter key when the button <see cref="IsDefault"/>.
/// </summary>
/// <param name="root">The input root.</param>

void XUI::UI::Button::ListenForDefault(Controls::InputElement * root)
{
    root->AddHandler<Interfaces::KeyEventArgs>(KeyDownEvent.get(), System::make_eventhandler(&Button::RootKeyDown, this));
}

/// <summary>
/// Stops listening for the Enter key when the button is no longer <see cref="IsDefault"/>.
/// </summary>
/// <param name="root">The input root.</param>

void XUI::UI::Button::StopListeningForDefault(Controls::InputElement * root)
{
    root->RemoveHandler<Interfaces::KeyEventArgs>(KeyDownEvent.get(), System::make_eventhandler(&Button::RootKeyDown, this));
}

/// <summary>
/// Raises the <see cref="Click"/> event.
/// </summary>

void XUI::UI::Button::RaiseClickEvent()
{
    Core::Observer::RoutedEventArgs click(ClickEvent.get());
    RaiseEvent(click);
}

/// <summary>
/// Called when a key is pressed on the input root and the button <see cref="IsDefault"/>.
/// </summary>
/// <param name="sender">The event sender.</param>
/// <param name="e">The event args.</param>

void XUI::UI::Button::RootKeyDown(void * sender, Interfaces::KeyEventArgs & e)
{
    if (e.Key == Interfaces::Key::Enter && IsVisible && IsEnabled)
    {
        RaiseClickEvent();
    }
}


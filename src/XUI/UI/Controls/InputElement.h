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

#ifndef __XUI_UI_CONTROLS_INPUTELEMENT_H__
#define __XUI_UI_CONTROLS_INPUTELEMENT_H__

#include "UI/Controls/Visual.h"

#include "Interfaces/ICursor.h"
#include "Interfaces/InputEventArgs.h"
#include "Core/Observer/RoutedEvent.h"

namespace XUI
{
    namespace Core
    {
        namespace Media
        {
            struct Transform;
        }
    }
}

namespace XUI::UI::Controls
{
    class TC_XUI_API InputElement : public Visual
    {
    public:

        static void StaticClassInitializer();

        /// <summary>
        /// Defines the <see cref="Focusable"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty FocusableProperty;

        /// <summary>
        /// Defines the <see cref="IsEnabled"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty IsEnabledProperty;

        /// <summary>
        /// Defines the <see cref="IsEnabledCore"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty IsEnabledCoreProperty;

        /// <summary>
        /// Gets or sets associated mouse cursor.
        /// </summary>
        static Core::Dependency::RefDependencyProperty CursorProperty;

        /// <summary>
        /// Defines the <see cref="IsFocused"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty IsFocusedProperty;

        /// <summary>
        /// Defines the <see cref="IsHitTestVisible"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty IsHitTestVisibleProperty;

        /// <summary>
        /// Defines the <see cref="IsPointerOver"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty IsPointerOverProperty;

        static Core::Dependency::RefDependencyProperty IsKeyboardFocusedProperty;
        static Core::Dependency::RefDependencyProperty IsMouseOverProperty;

        /// <summary>
        /// Defines the <see cref="GotFocus"/> event.
        /// </summary>
        static std::shared_ptr<Core::Observer::RoutedEvent> GotFocusEvent;

        /// <summary>
        /// Defines the <see cref="LostFocus"/> event.
        /// </summary>
        static std::shared_ptr<Core::Observer::RoutedEvent> LostFocusEvent;

        /// <summary>
        /// Defines the <see cref="KeyDown"/> event.
        /// </summary>
        static std::shared_ptr<Core::Observer::RoutedEvent> KeyDownEvent;

        /// <summary>
        /// Defines the <see cref="KeyUp"/> event.
        /// </summary>
        static std::shared_ptr<Core::Observer::RoutedEvent> KeyUpEvent;

        /// <summary>
        /// Defines the <see cref="TextInput"/> event.
        /// </summary>
        static std::shared_ptr<Core::Observer::RoutedEvent> TextInputEvent;

        /// <summary>
        /// Defines the <see cref="PointerEnter"/> event.
        /// </summary>
        static std::shared_ptr<Core::Observer::RoutedEvent> PointerEnterEvent;

        /// <summary>
        /// Defines the <see cref="PointerLeave"/> event.
        /// </summary>
        static std::shared_ptr<Core::Observer::RoutedEvent> PointerLeaveEvent;

        /// <summary>
        /// Defines the <see cref="PointerMoved"/> event.
        /// </summary>
        static std::shared_ptr<Core::Observer::RoutedEvent> PointerMovedEvent;

        /// <summary>
        /// Defines the <see cref="PointerPressed"/> event.
        /// </summary>
        static std::shared_ptr<Core::Observer::RoutedEvent> PointerPressedEvent;

        /// <summary>
        /// Defines the <see cref="PointerReleased"/> event.
        /// </summary>
        static std::shared_ptr<Core::Observer::RoutedEvent> PointerReleasedEvent;

        /// <summary>
        /// Defines the <see cref="PointerWheelChanged"/> event.
        /// </summary>
        static std::shared_ptr<Core::Observer::RoutedEvent> PointerWheelChangedEvent;

        /// <summary>
        /// Defines the <see cref="Tapped"/> event.
        /// </summary>
        static std::shared_ptr<Core::Observer::RoutedEvent> TappedEvent;

        /// <summary>
        /// Defines the <see cref="DoubleTapped"/> event.
        /// </summary>
        static std::shared_ptr<Core::Observer::RoutedEvent> DoubleTappedEvent;


    private:
        bool _isFocused;
        bool _isPointerOver;

        std::map<Core::Observer::RoutedEvent*, std::list<std::shared_ptr<Core::Observer::RoutedEventHandler>>> _eventHandlers;

    public:

        template<typename TEventArg>
        struct EventHandler
        {
            InputElement* control;
            Core::Observer::RoutedEvent* routedEvent;
            Core::Observer::RoutingStrategies routes = Core::Observer::RoutingStrategies(Core::Observer::RoutingStrategies::Direct | Core::Observer::RoutingStrategies::Bubble);
            bool handledEventsToo = false;

            EventHandler(
                InputElement* control, 
                Core::Observer::RoutedEvent* routedEvent,
                Core::Observer::RoutingStrategies routes = Core::Observer::RoutingStrategies(Core::Observer::RoutingStrategies::Direct | Core::Observer::RoutingStrategies::Bubble),
                bool handledEventsToo = false)
                : control(control)
                , routedEvent(routedEvent)
                , routes(routes)
                , handledEventsToo(handledEventsToo)
            { }

            void AddHandler(System::Events::EventHook<TEventArg>* handler)
            {
                control->AddHandler<TEventArg>(routedEvent, handler, routes, handledEventsToo);
            }

            void RemoveHandler(System::Events::EventHook<TEventArg>* handler)
            {
                control->RemoveHandler<TEventArg>(routedEvent, handler);
            }

            void operator+=(System::Events::EventHook<TEventArg>* handler)
            {
                control->AddHandler<TEventArg>(routedEvent, handler, routes, handledEventsToo);
            }

            void operator-=(System::Events::EventHook<TEventArg>* handler)
            {
                control->RemoveHandler<TEventArg>(routedEvent, handler);
            }
        };

        InputElement();
        virtual ~InputElement();

        PROPERTY_(bool, IsKeyboardFocused);
        bool GetIsKeyboardFocused() const;
        void SetIsKeyboardFocused(bool value);

        PROPERTY_(bool, IsMouseOver);
        bool GetIsMouseOver() const;
        void SetIsMouseOver(bool value);

        PROPERTY_(bool, Focusable);
        bool GetFocusable() const;
        void SetFocusable(bool value);

        PROPERTY_(bool, IsFocused);
        bool GetIsFocused() const;
        void SetIsFocused(bool value);

        PROPERTY_(bool, IsPointerOver);
        bool GetIsPointerOver() const;
        void SetIsPointerOver(bool value);

        /// <summary>
        /// Gets a value indicating whether the control is effectively enabled for user interaction.
        /// </summary>
        /// <remarks>
        /// The <see cref="IsEnabled"/> property is used to toggle the enabled state for individual
        /// controls. The <see cref="IsEnabledCore"/> property takes into account the
        /// <see cref="IsEnabled"/> value of this control and its parent controls.
        /// </remarks>
        PROPERTY_(bool, IsEnabledCore);
        bool GetIsEnabledCore() const;
        void SetIsEnabledCore(bool value);

        /// <summary>
        /// Gets or sets a value indicating whether the control is enabled for user interaction.
        /// </summary>
        PROPERTY_(bool, IsEnabled);
        bool GetIsEnabled() const;
        void SetIsEnabled(bool value);

        /// <summary>
        /// Gets or sets associated mouse cursor.
        /// </summary>
        PROPERTY_(std::shared_ptr<Interfaces::ICursor>, Cursor);
        std::shared_ptr<Interfaces::ICursor> GetCursor() const;
        void SetCursor(std::shared_ptr<Interfaces::ICursor> const& value);

        PROPERTY_(bool, IsHitTestVisible);
        bool GetIsHitTestVisible() const;
        void SetIsHitTestVisible(bool value);

        /// <summary>
        /// Focuses the control.
        /// </summary>
        void Focus();

        static void IsEnabledChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e);

        /// <summary>
        /// Updates the <see cref="IsEnabledCore"/> property value.
        /// </summary>
        void UpdateIsEnabledCore();

        /// <summary>
        /// Updates the <see cref="IsEnabledCore"/> property based on the parent's
        /// <see cref="IsEnabledCore"/>.
        /// </summary>
        /// <param name="parent">The parent control.</param>
        void UpdateIsEnabledCore(InputElement* parent);

#pragma region Events

        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&GotFocusEvent>              GotFocus;                       //<Core::Observer::RoutedEventArgs>     
        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&LostFocusEvent>             LostFocus;                      //<Core::Observer::RoutedEventArgs>     
        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&KeyDownEvent>               KeyDown;                        //<Interfaces::KeyEventArgs>            
        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&KeyUpEvent>                 KeyUp;                          //<Interfaces::KeyEventArgs>            
        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&TextInputEvent>             TextInput;                      //<Interfaces::TextInputEventArgs>      
        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&PointerEnterEvent>          PointerEnter;                   //<Interfaces::PointerEventArgs>        
        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&PointerLeaveEvent>          PointerLeave;                   //<Interfaces::PointerEventArgs>        
        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&PointerMovedEvent>          PointerMoved;                   //<Interfaces::PointerEventArgs>        
        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&PointerPressedEvent>        PointerPressed;                 //<Interfaces::PointerPressedEventArgs> 
        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&PointerReleasedEvent>       PointerReleased;                //<Interfaces::PointerReleasedEventArgs>
        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&PointerWheelChangedEvent>   PointerWheelChanged;            //<Interfaces::PointerWheelEventArgs>   
        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&TappedEvent>                Tapped;
        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&DoubleTappedEvent>          DoubleTapped;

        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&PointerEnterEvent, Core::Observer::RoutingStrategies::Tunnel>          PreviewPointerEnter;                   //<Interfaces::PointerEventArgs>        
        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&PointerLeaveEvent, Core::Observer::RoutingStrategies::Tunnel>          PreviewPointerLeave;                   //<Interfaces::PointerEventArgs>        
        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&PointerMovedEvent, Core::Observer::RoutingStrategies::Tunnel>          PreviewPointerMoved;                   //<Interfaces::PointerEventArgs>        
        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&PointerPressedEvent, Core::Observer::RoutingStrategies::Tunnel>        PreviewPointerPressed;                 //<Interfaces::PointerPressedEventArgs> 
        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&PointerReleasedEvent, Core::Observer::RoutingStrategies::Tunnel>       PreviewPointerReleased;                //<Interfaces::PointerReleasedEventArgs>
        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&PointerWheelChangedEvent, Core::Observer::RoutingStrategies::Tunnel>   PreviewPointerWheelChanged;            //<Interfaces::PointerWheelEventArgs>   

#pragma endregion


#pragma region Event Implementation

        /// <summary>
        /// Adds a handler for the specified routed event.
        /// </summary>
        /// <param name="routedEvent">The routed event.</param>
        /// <param name="handler">The handler.</param>
        /// <param name="routes">The routing strategies to listen to.</param>
        /// <param name="handledEventsToo">Whether handled events should also be listened for.</param>
        /// <returns>A disposable that terminates the event subscription.</returns>
        rxcpp::subscription AddHandler(
            Core::Observer::RoutedEvent* routedEvent,
            System::Events::IEventHook* handler,
            Core::Observer::RoutingStrategies routes = Core::Observer::RoutingStrategies(Core::Observer::RoutingStrategies::Direct | Core::Observer::RoutingStrategies::Bubble),
            bool handledEventsToo = false);

        rxcpp::subscription AddHandler(
            Core::Observer::RoutedEvent* routedEvent,
            std::shared_ptr<System::Events::IEventHook> const& handler,
            Core::Observer::RoutingStrategies routes = Core::Observer::RoutingStrategies(Core::Observer::RoutingStrategies::Direct | Core::Observer::RoutingStrategies::Bubble),
            bool handledEventsToo = false);

        template<typename TEventArg>
        rxcpp::subscription AddHandler(
            Core::Observer::RoutedEvent* routedEvent,
            System::Events::EventHook<TEventArg>* handler,
            Core::Observer::RoutingStrategies routes = Core::Observer::RoutingStrategies(Core::Observer::RoutingStrategies::Direct | Core::Observer::RoutingStrategies::Bubble),
            bool handledEventsToo = false)
        {
            return AddHandler(routedEvent, static_cast<System::Events::IEventHook*>(handler), routes, handledEventsToo);
        }

        /// <summary>
        /// Removes a handler for the specified routed event.
        /// </summary>
        /// <param name="routedEvent">The routed event.</param>
        /// <param name="handler">The handler.</param>
        void RemoveHandler(Core::Observer::RoutedEvent* routedEvent, System::Events::IEventHook* handler);

        /// <summary>
        /// Removes a handler for the specified routed event.
        /// </summary>
        /// <typeparam name="TEventArgs">The type of the event's args.</typeparam>
        /// <param name="routedEvent">The routed event.</param>
        /// <param name="handler">The handler.</param>
        template<typename TEventArgs>
        void RemoveHandler(Core::Observer::RoutedEvent* routedEvent, System::Events::EventHook<TEventArgs>* handler)
        {
            RemoveHandler(routedEvent, static_cast<System::Events::IEventHook*>(handler));
        }

        /// <summary>
        /// Raises a routed event.
        /// </summary>
        /// <param name="e">The event args.</param>
        void RaiseEvent(Core::Observer::RoutedEventArgs const& e);

        /// <summary>
        /// Bubbles an event.
        /// </summary>
        /// <param name="e">The event args.</param>
        void BubbleEvent(Core::Observer::RoutedEventArgs const& e);

        /// <summary>
        /// Tunnels an event.
        /// </summary>
        /// <param name="e">The event args.</param>
        void TunnelEvent(Core::Observer::RoutedEventArgs const& e);

        /// <summary>
        /// Carries out the actual invocation of an event on this object.
        /// </summary>
        /// <param name="e">The event args.</param>
        void RaiseEventImpl(Core::Observer::RoutedEventArgs const& e);
#pragma endregion

        bool CanHitTest() override { return IsHitTestVisible; }

        protected:

            virtual void UpdateCursor();

            /// <inheritdoc/>
            void OnDetachedFromVisualTreeCore(VisualTreeAttachmentEventArgs const& e) override;
            
            /// <inheritdoc/>
            void OnAttachedToVisualTreeCore(VisualTreeAttachmentEventArgs const& e) override;

            /// <summary>
            /// Called before the <see cref="GotFocus"/> event occurs.
            /// </summary>
            /// <param name="e">The event args.</param>
            virtual void OnGotFocus(Interfaces::GotFocusEventArgs& e);

            /// <summary>
            /// Called before the <see cref="LostFocus"/> event occurs.
            /// </summary>
            /// <param name="e">The event args.</param>
            virtual void OnLostFocus(Core::Observer::RoutedEventArgs& e);

            /// <summary>
            /// Called before the <see cref="KeyDown"/> event occurs.
            /// </summary>
            /// <param name="e">The event args.</param>
            virtual void OnKeyDown(Interfaces::KeyEventArgs& e);

            /// <summary>
            /// Called before the <see cref="KeyUp"/> event occurs.
            /// </summary>
            /// <param name="e">The event args.</param>
            virtual void OnKeyUp(Interfaces::KeyEventArgs& e);

			/// <summary>
			/// Called before the <see cref="KeyDown"/> event occurs.
			/// </summary>
			/// <param name="e">The event args.</param>
			virtual void OnPreviewKeyDown(Interfaces::KeyEventArgs& e)
			{
			}

			/// <summary>
			/// Called before the <see cref="KeyUp"/> event occurs.
			/// </summary>
			/// <param name="e">The event args.</param>
			virtual void OnPreviewKeyUp(Interfaces::KeyEventArgs& e)
			{
			}

            /// <summary>
            /// Called before the <see cref="TextInput"/> event occurs.
            /// </summary>
            /// <param name="e">The event args.</param>
            virtual void OnTextInput(Interfaces::TextInputEventArgs& e);

            /// <summary>
            /// Called before the <see cref="PointerEnter"/> event occurs.
            /// </summary>
            /// <param name="e">The event args.</param>
            virtual void OnPointerEnter(Interfaces::PointerEventArgs& e);

            /// <summary>
            /// Called before the <see cref="PointerLeave"/> event occurs.
            /// </summary>
            /// <param name="e">The event args.</param>
            virtual void OnPointerLeave(Interfaces::PointerEventArgs& e);

            /// <summary>
            /// Called before the <see cref="PointerMoved"/> event occurs.
            /// </summary>
            /// <param name="e">The event args.</param>
            virtual void OnPointerMoved(Interfaces::PointerEventArgs& e);

            /// <summary>
            /// Called before the <see cref="PointerPressed"/> event occurs.
            /// </summary>
            /// <param name="e">The event args.</param>
            virtual void OnPointerPressed(Interfaces::PointerPressedEventArgs& e);

            /// <summary>
            /// Called before the <see cref="PointerReleased"/> event occurs.
            /// </summary>
            /// <param name="e">The event args.</param>
            virtual void OnPointerReleased(Interfaces::PointerReleasedEventArgs& e);

            /// <summary>
            /// Called before the <see cref="PointerWheelChanged"/> event occurs.
            /// </summary>
            /// <param name="e">The event args.</param>
            virtual void OnPointerWheelChanged(Interfaces::PointerWheelEventArgs& e);

            /// <summary>
            /// Called before the <see cref="PointerEnter"/> event occurs.
            /// </summary>
            /// <param name="e">The event args.</param>
            virtual void OnPreviewPointerEnter(Interfaces::PointerEventArgs& e) {
            }

            /// <summary>
            /// Called before the <see cref="PointerLeave"/> event occurs.
            /// </summary>
            /// <param name="e">The event args.</param>
            virtual void OnPreviewPointerLeave(Interfaces::PointerEventArgs& e) {
            }

            /// <summary>
            /// Called before the <see cref="PointerMoved"/> event occurs.
            /// </summary>
            /// <param name="e">The event args.</param>
            virtual void OnPreviewPointerMoved(Interfaces::PointerEventArgs& e) {
            }

            /// <summary>
            /// Called before the <see cref="PointerPressed"/> event occurs.
            /// </summary>
            /// <param name="e">The event args.</param>
            virtual void OnPreviewPointerPressed(Interfaces::PointerPressedEventArgs& e) {
            }

            /// <summary>
            /// Called before the <see cref="PointerReleased"/> event occurs.
            /// </summary>
            /// <param name="e">The event args.</param>
            virtual void OnPreviewPointerReleased(Interfaces::PointerReleasedEventArgs& e) {
            }

            /// <summary>
            /// Called before the <see cref="PointerWheelChanged"/> event occurs.
            /// </summary>
            /// <param name="e">The event args.</param>
            virtual void OnPreviewPointerWheelChanged(Interfaces::PointerWheelEventArgs& e) {
            }
    };
}

#endif // !__XUI_UI_CONTROLS_INPUTELEMENT_H__
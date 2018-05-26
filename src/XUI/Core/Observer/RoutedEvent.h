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

#ifndef __XUI_ROUTED_EVENT_H__
#define __XUI_ROUTED_EVENT_H__

#include "standard/BasicPrimities.h"
#include "standard/events.h"
#include "RxExtension/Subject.h"

//#include <rxcpp/rx.hpp>

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/Controls/InputElement.h"))
MetaAssemblyEnd

namespace System::Reflection::Assembly
{
    class AssemblyModulXUI;
}

namespace XUI::UI::Controls
{
    class InputElement;
}

namespace XUI::Core::Observer
{
    enum RoutingStrategies
    {
        Direct = 0x01,
        Tunnel = 0x02,
        Bubble = 0x04,
    };

    class RoutedEvent;

    class RoutedEventArgs : public System::Events::EventArg
    {
    public:
        RoutedEventArgs()
        {
        }

        RoutedEventArgs(RoutedEvent* routedEvent)
        {
            RoutedEvent = routedEvent;
        }

        RoutedEventArgs(RoutedEvent* routedEvent, UI::Controls::InputElement* source)
        {
            RoutedEvent = routedEvent;
            Source = source;
        }

        bool Handled = false;
        RoutedEvent* RoutedEvent = nullptr;
        RoutingStrategies Route = RoutingStrategies::Direct;
        UI::Controls::InputElement* Source = nullptr;
        System::Reflection::meta::Object* Sender = nullptr;
    };

    template<typename T>
    struct RoutedPropertyChangedEventArgs : Core::Observer::RoutedEventArgs
    {
        RoutedPropertyChangedEventArgs() {}

        RoutedPropertyChangedEventArgs(Core::Observer::RoutedEvent* routedEvent)
            : RoutedEventArgs(routedEvent)
        { }
        virtual ~RoutedPropertyChangedEventArgs() {}

        T NewValue;
        T OldValue;
    };

    class 
        TC_XUI_API
        Meta(AbstractType)
        RoutedEvent : public System::Reflection::meta::Object
    {
    public:


    private:
        typedef System::RX::Subject<RoutedEventArgs> subject_routed_event_t;

        subject_routed_event_t _raised;
        subject_routed_event_t _routeFinished;

        std::map<std::shared_ptr<System::Events::IEventHook>, rxcpp::subscription> _subscribedEvents;

    public:
        //RoutedEvent() {}

        RoutedEvent(
            std::string const& name,
            RoutingStrategies routingStrategies,
            System::Reflection::meta::Type const* eventArgsType,
            System::Reflection::meta::Type const* ownerType);

        System::Reflection::meta::Type const* EventArgsType = nullptr;
        System::Reflection::meta::Type const* OwnerType = nullptr;
        std::string Name;
        RoutingStrategies RoutingStrategies;

        PROPERTY_TG(subject_routed_event_t, Raised)
        PROPERTY_T_GET(Raised, &) { return _raised; }

        PROPERTY_TG(subject_routed_event_t, RouteFinished)
        PROPERTY_T_GET(RouteFinished, &) { return _routeFinished; }

        template<typename TOwner, typename TEventArgs>
        static std::shared_ptr<RoutedEvent> Register(
            std::string name,
            XUI::Core::Observer::RoutingStrategies routingStrategy)
        {
            System::ThrowIfFailed<System::ArgumentNullException>(!name.empty());

            return std::make_shared<RoutedEvent>(name, routingStrategy, &typeof(TEventArgs), &typeof(TOwner));
        }

        template<typename TEventArgs>
        static std::shared_ptr<RoutedEvent> Register(
            std::string name,
            XUI::Core::Observer::RoutingStrategies routingStrategy,
            System::Reflection::meta::Type const* ownerType)
        {
            System::ThrowIfFailed<System::ArgumentNullException>(!name.empty());

            return std::make_shared<RoutedEvent>(name, routingStrategy, &typeof(TEventArgs), ownerType);
        }

        rxcpp::subscription AddClassHandler(
            System::Reflection::meta::Type const* targetType,
            System::Events::EventHook<RoutedEventArgs>* handler,
            XUI::Core::Observer::RoutingStrategies routes = XUI::Core::Observer::RoutingStrategies(RoutingStrategies::Direct | RoutingStrategies::Bubble),
            bool handledEventsToo = false);

        template<typename TEventArg, typename TTarget>
        rxcpp::subscription AddClassSpecHandler(
            System::Events::EventHook<TEventArg>* handler,
            XUI::Core::Observer::RoutingStrategies routes = XUI::Core::Observer::RoutingStrategies(RoutingStrategies::Direct | RoutingStrategies::Bubble),
            bool handledEventsToo = false)
        {
            rxcpp::composite_subscription unregisters;

            System::Events::EventHook<RoutedEventArgs>* interface_handler = System::make_eventhandler([handler](void* sender, RoutedEventArgs& e)
            {
                System::Reflection::meta::Object* _sender = (System::Reflection::meta::Object*)sender;
                TEventArg& event_arg = static_cast<TEventArg&>(e);

                if (_sender != nullptr)
                {
                    (*handler)((void*)_sender, event_arg);
                }
            });

            unregisters.add(AddClassHandler(&typeof(TTarget), interface_handler, routes, handledEventsToo));
            unregisters.add(rxcpp::make_subscription([interface_handler]()
            {
                if (interface_handler)
                    delete interface_handler;
            }));

            return unregisters;
        }

        template<typename TEventArg>
        rxcpp::subscription AddClassSpecHandler(
            System::Reflection::meta::Type const* targetType,
            System::Events::EventHook<TEventArg>* handler,
            XUI::Core::Observer::RoutingStrategies routes = XUI::Core::Observer::RoutingStrategies(RoutingStrategies::Direct | RoutingStrategies::Bubble),
            bool handledEventsToo = false)
        {
            rxcpp::composite_subscription unregisters;

            System::Events::EventHook<RoutedEventArgs>* interface_handler = System::make_eventhandler([handler](void* sender, RoutedEventArgs& e)
            {
                System::Reflection::meta::Object* _sender = (System::Reflection::meta::Object*)sender;
                TEventArg& event_arg = static_cast<TEventArg&>(e);

                if (_sender != nullptr)
                {
                    (*handler)((void*)_sender, event_arg);
                }
            });

            unregisters.add(AddClassHandler(targetType, interface_handler, routes, handledEventsToo));
            unregisters.add(rxcpp::make_subscription([interface_handler]()
            {
                if (interface_handler)
                    delete interface_handler;
            }));

            return unregisters;
        }

        void AddHandler(std::shared_ptr<System::Events::IEventHook> const& handler)
        {
            rxcpp::subscription& _detacher = _subscribedEvents[handler];
            if (_detacher.is_subscribed())
                _detacher.unsubscribe();

            _detacher = AddClassHandler(nullptr, dynamic_cast<System::Events::EventHook<RoutedEventArgs>*>(handler.get()));
        }

        void RemoveHandler(std::shared_ptr<System::Events::IEventHook> const& handler)
        {
            for (auto itr = _subscribedEvents.begin(); itr != _subscribedEvents.end();)
            {
                if (itr->first->IsValid() && itr->first->IsEqual(handler.get()))
                {
                    if (itr->second.is_subscribed())
                        itr->second.unsubscribe();

                    _subscribedEvents.erase(itr++);
                }
                else
                    ++itr;
            }
        }

        void InvokeRaised(System::Reflection::meta::Object* sender, RoutedEventArgs& e)
        {
            e.Sender = sender;
            if (_raised.has_observers())
                _raised.get_observer().on_next(e); // .get_subscriber().on_next(e); // Tuple.Create(sender, e));
        }

        void InvokeRouteFinished(RoutedEventArgs& e)
        {
            if (_routeFinished.has_observers())
                _routeFinished.get_observer().on_next(e); // get_subscriber().on_next(e);
        }
    };

    typedef std::shared_ptr<RoutedEvent> RefRoutedEvent;

    struct RoutedEventHandler
    {
        RoutedEventHandler(System::Events::IEventHook* _delegate,
            RoutingStrategies routes,
            bool alsoifhandled)
            : Handler(_delegate)
            , Routes(routes)
            , AlsoIfHandled(alsoifhandled)
        {}

        RoutedEventHandler(std::shared_ptr<System::Events::IEventHook> const& _delegate,
            RoutingStrategies routes,
            bool alsoifhandled)
            : RefHandle(_delegate)
            , Routes(routes)
            , AlsoIfHandled(alsoifhandled)
        {}

        ~RoutedEventHandler()
        {
            if (Handler)
                delete Handler;
            Handler = nullptr;
        }

        System::Events::IEventHook* GetHandler() const
        {
            if (Handler)
                return Handler;
            return RefHandle.get();
        }

        System::Events::IEventHook* Handler = nullptr; // non managed ptr
        std::shared_ptr<System::Events::IEventHook> RefHandle;
        RoutingStrategies Routes;
        bool AlsoIfHandled;
    };

    struct TC_XUI_API PropertyRoutedEventHandler : System::Events::IPropertyEvent
    {
        UI::Controls::InputElement* m_owner = nullptr;
        Core::Observer::RoutedEvent* m_routedEvent = nullptr;
        Core::Observer::RoutingStrategies m_routes = Core::Observer::RoutingStrategies::Direct;

        PropertyRoutedEventHandler(Core::Observer::RoutedEvent* routedEvent, UI::Controls::InputElement* Owner, Core::Observer::RoutingStrategies routes = Core::Observer::RoutingStrategies(Core::Observer::RoutingStrategies::Bubble | Core::Observer::RoutingStrategies::Direct))
            : m_routedEvent(routedEvent)
            , m_owner(Owner)
            , m_routes(routes)
        {}

        virtual ~PropertyRoutedEventHandler() {}

        rxcpp::subscription operator+=(std::shared_ptr<System::Events::IEventHook> const& handler)
        {
            return AddHandler(handler);
        }

        rxcpp::subscription operator+=(System::Events::IEventHook* handler)
        {
            return AddHandler(handler);
        }

        void operator-=(std::shared_ptr<System::Events::IEventHook> const& handler)
        {
            RemoveHandler(handler);
        }

        void operator-=(System::Events::IEventHook const* handler)
        {
            RemoveHandler(handler);
        }

        rxcpp::subscription AddHandler(std::shared_ptr<System::Events::IEventHook> const& handler);
        rxcpp::subscription AddHandler(System::Events::IEventHook* handler);

        void RemoveHandler(std::shared_ptr<System::Events::IEventHook> const& handler);
        void RemoveHandler(System::Events::IEventHook const* handler);
    };

    template<std::shared_ptr<Core::Observer::RoutedEvent>* Event, int routes = int(Core::Observer::RoutingStrategies::Bubble | Core::Observer::RoutingStrategies::Direct)>
    struct SpecPropertyRoutedEventHandler : PropertyRoutedEventHandler
    {
        SpecPropertyRoutedEventHandler(UI::Controls::InputElement* Owner)
            : PropertyRoutedEventHandler(Event->get(), Owner, Core::Observer::RoutingStrategies(routes))
        {}

        virtual ~SpecPropertyRoutedEventHandler() {}

    };
}
#endif //!__XUI_ROUTED_EVENT_H__

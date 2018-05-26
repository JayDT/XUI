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

#include "RoutedEvent.h"
#include "UI/Controls/InputElement.h"

using namespace XUI::Core::Observer;

XUI::Core::Observer::RoutedEvent::RoutedEvent(
    std::string const & name, 
    XUI::Core::Observer::RoutingStrategies routingStrategies, 
    System::Reflection::meta::Type const * eventArgsType, 
    System::Reflection::meta::Type const * ownerType)
{
    System::ThrowIfFailed<System::ArgumentNullException>(!name.empty());
    System::ThrowIfFailed<System::ArgumentNullException>(eventArgsType != nullptr);
    System::ThrowIfFailed<System::ArgumentNullException>(ownerType != nullptr);
    System::ThrowIfFailed<System::InvalidCastException>(typeof(XUI::Core::Observer::RoutedEventArgs).IsAssignableFrom(*eventArgsType));

    EventArgsType = eventArgsType;
    Name = name;
    OwnerType = ownerType;
    RoutingStrategies = routingStrategies;
}

rxcpp::subscription XUI::Core::Observer::RoutedEvent::AddClassHandler(System::Reflection::meta::Type const * targetType, System::Events::EventHook<RoutedEventArgs>* handler, XUI::Core::Observer::RoutingStrategies routes, bool handledEventsToo)
{
    return _raised.get_observable().subscribe([targetType, routes, handledEventsToo, handler](RoutedEventArgs const& args)
    {
        System::Reflection::meta::Object* sender = args.Sender;
        auto const& e = args;

        if ((!targetType || targetType->IsAssignableFrom(*sender->GetType())) &&
            ((e.Route == RoutingStrategies::Direct) || (e.Route & routes) != 0) &&
            (!e.Handled || handledEventsToo))
        {
            try
            {
                (*handler)((void*)sender, (RoutedEventArgs&)e);
            }
            catch (System::TargetInvocationException ex)
            {
                EXCEPT(false);
            }
        }
    });
}

rxcpp::subscription PropertyRoutedEventHandler::AddHandler(System::Events::IEventHook* handler)
{
    return m_owner->AddHandler(m_routedEvent, handler, m_routes);
}

rxcpp::subscription PropertyRoutedEventHandler::AddHandler(std::shared_ptr<System::Events::IEventHook> const & handler)
{
    return m_owner->AddHandler(m_routedEvent, handler, m_routes);
}

void PropertyRoutedEventHandler::RemoveHandler(std::shared_ptr<System::Events::IEventHook> const & handler)
{
    m_owner->RemoveHandler(m_routedEvent, handler.get());
}

void PropertyRoutedEventHandler::RemoveHandler(System::Events::IEventHook const* handler)
{
    m_owner->RemoveHandler(m_routedEvent, (System::Events::IEventHook*)handler);
}

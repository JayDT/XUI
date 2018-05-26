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

#ifndef __IROUTED_COMMAND_H__
#define __IROUTED_COMMAND_H__

#include "Interfaces/ICommand.h"
#include "Interfaces/InputEventArgs.h"

namespace XUI::UI::Controls
{
    class Control;
}

namespace XUI::Core::Observer
{
    class CommandManager;

    struct RoutedCommandEventArgs : Core::Observer::RoutedEventArgs
    {
        RoutedCommandEventArgs() {}
        RoutedCommandEventArgs(Core::Observer::RoutedEvent* routedEvent)
            : Core::Observer::RoutedEventArgs(routedEvent)
        { }
        virtual ~RoutedCommandEventArgs() {}

        Interfaces::ICommand* Command = nullptr;
    };

    struct ExecutedRoutedEventArgs : RoutedCommandEventArgs
    {
        ExecutedRoutedEventArgs() {}
        ExecutedRoutedEventArgs(Core::Observer::RoutedEvent* routedEvent)
            : RoutedCommandEventArgs(routedEvent)
        { }
        virtual ~ExecutedRoutedEventArgs() {}

        System::Reflection::Variant Parameter;
    };

    struct CanExecuteRoutedEventArgs : RoutedCommandEventArgs
    {
        CanExecuteRoutedEventArgs() {}

        CanExecuteRoutedEventArgs(Core::Observer::RoutedEvent* routedEvent)
            : RoutedCommandEventArgs(routedEvent)
        { }
        virtual ~CanExecuteRoutedEventArgs() {}

        System::Reflection::Variant Parameter;
        bool CanExecute = true;
        bool ContinueRouting = true;
    };

    typedef std::shared_ptr<System::Events::EventHook<ExecutedRoutedEventArgs>> ExecutedRoutedEventHandler;
    typedef std::shared_ptr<System::Events::EventHook<CanExecuteRoutedEventArgs>> CanExecuteRoutedEventHandler;

    struct EventHandleCommand : public Interfaces::ICommand
    {
        EventHandleCommand(ExecutedRoutedEventHandler executed = nullptr, CanExecuteRoutedEventHandler canExecute = nullptr)
        {
            _CanExecute = canExecute;
            _Executed = executed;
        }
        virtual ~EventHandleCommand()
        {
            if (Unregisters.is_subscribed())
                Unregisters.unsubscribe();
        }

        // Inherited via ICommand
        bool CanExecute(System::Reflection::meta::Variant parameter, UI::Controls::Control* target) override;

        void Execute(System::Reflection::meta::Variant parameter, UI::Controls::Control* target) override;

        ExecutedRoutedEventHandler _Executed;
        CanExecuteRoutedEventHandler _CanExecute;

        rxcpp::composite_subscription Unregisters;
    };

    struct CommandBinding : System::Reflection::Object
    {
        CommandBinding() {}
        CommandBinding(Interfaces::ICommand* command, ExecutedRoutedEventHandler executed = nullptr, CanExecuteRoutedEventHandler canExecute = nullptr) : Command(command)
        {
            CanExecute = canExecute;
            Executed = executed;
        }
        virtual ~CommandBinding()
        {
            if (Unregisters.is_subscribed())
                Unregisters.unsubscribe();
        }

        Interfaces::ICommand* Command = nullptr;

        ExecutedRoutedEventHandler PreviewExecuted;
        ExecutedRoutedEventHandler Executed;
        CanExecuteRoutedEventHandler PreviewCanExecute;
        CanExecuteRoutedEventHandler CanExecute;

        rxcpp::composite_subscription Unregisters;
    };

    // This only fast implementation need review
    class RoutedCommand : public Interfaces::ICommand
    {
        friend class CommandManager;

    private:
        std::string _name;
        const System::Reflection::Type* _ownerType;
        System::Collection::List<std::shared_ptr<Interfaces::InputGesture>> _inputGestures;
        System::Collection::List<std::shared_ptr<CommandBinding>> _commands;

        template<typename TEventArg>
        rxcpp::subscription AddClassSpecHandler(
            Core::Observer::RoutedEvent* event,
            System::Reflection::meta::Type const* targetType,
            RoutedCommand* targetCommand,
            System::Events::EventHook<TEventArg>* handler,
            XUI::Core::Observer::RoutingStrategies routes = XUI::Core::Observer::RoutingStrategies(RoutingStrategies::Direct | RoutingStrategies::Bubble),
            bool handledEventsToo = false)
        {
            rxcpp::composite_subscription unregisters;

            System::Events::EventHook<RoutedEventArgs>* interface_handler = System::make_eventhandler([handler, targetCommand](void* sender, RoutedEventArgs& e)
            {
                System::Reflection::meta::Object* _sender = (System::Reflection::meta::Object*)sender;
                TEventArg& event_arg = static_cast<TEventArg&>(e);

                if (_sender != nullptr && targetCommand == event_arg.Command)
                {
                    (*handler)((void*)_sender, event_arg);
                }
            });

            unregisters.add(event->AddClassHandler(targetType, interface_handler, routes, handledEventsToo));
            unregisters.add(rxcpp::make_subscription([interface_handler]()
            {
                if (interface_handler)
                    delete interface_handler;
            }));

            return unregisters;
        }

    public:

        static Core::Observer::RefRoutedEvent PreviewExecutedEvent;
        static Core::Observer::RefRoutedEvent ExecutedEvent;
        static Core::Observer::RefRoutedEvent PreviewCanExecuteEvent;
        static Core::Observer::RefRoutedEvent CanExecuteEvent;

        Meta(Disable) static System::Events::EventHandler<System::Events::EventArg> RequerySuggested;

        static void InvalidateRequerySuggested();

        static void StaticClassInitializer();

        RoutedCommand();
        RoutedCommand(std::string const& name, const System::Reflection::Type* ownerType);
        RoutedCommand(std::string const& name, const System::Reflection::Type* ownerType, System::Collection::List<std::shared_ptr<Interfaces::InputGesture>> const& inputGestures);

        std::string const& Name() const { return _name; }
        const System::Reflection::Type* OwnerType() const { return _ownerType; }
        System::Collection::List<std::shared_ptr<Interfaces::InputGesture>>  const& InputGestures() const { return _inputGestures; }

        // Inherited via ICommand
        virtual bool CanExecute(System::Reflection::meta::Variant parameter, UI::Controls::Control* target) override;
        virtual void Execute(System::Reflection::meta::Variant parameter, UI::Controls::Control* target) override;

        rxcpp::subscription AddClassHandler(
            System::Reflection::meta::Type const* targetType,
            std::shared_ptr<CommandBinding> command);

        rxcpp::subscription AddClassInputBinding(
            System::Reflection::meta::Type const* targetType,
            std::shared_ptr<Interfaces::InputGesture> const& inputBind);

        template<typename TOwner>
        static std::shared_ptr<RoutedCommand> Register(std::string name)
        {
            System::ThrowIfFailed<System::ArgumentNullException>(!name.empty());

            return std::make_shared<RoutedCommand>(name, &typeof(TOwner));
        }

    };

    typedef std::shared_ptr<RoutedCommand> RefRoutedCommand;
}

#endif // !__IOBSERVER_H__

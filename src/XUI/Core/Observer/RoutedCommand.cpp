#include "RoutedCommand.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XamlCPP::Core::Dependency;
using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_PRE_INITIALIZER(RoutedCommand, XUI::Core::Observer::RoutedCommand::StaticClassInitializer)

Core::Observer::RefRoutedEvent Core::Observer::RoutedCommand::PreviewExecutedEvent;
Core::Observer::RefRoutedEvent Core::Observer::RoutedCommand::ExecutedEvent;
Core::Observer::RefRoutedEvent Core::Observer::RoutedCommand::PreviewCanExecuteEvent;
Core::Observer::RefRoutedEvent Core::Observer::RoutedCommand::CanExecuteEvent;
System::Events::EventHandler<> Core::Observer::RoutedCommand::RequerySuggested;

/// <summary>
/// Initializes static members of the <see cref="DataGrid"/> class.
/// </summary>
void XUI::Core::Observer::RoutedCommand::StaticClassInitializer()
{
    PreviewExecutedEvent = (Core::Observer::RoutedEvent::Register<XUI::Core::Observer::RoutedCommand, ExecutedRoutedEventArgs>(nameof(PreviewExecuted), RoutingStrategies(RoutingStrategies::Tunnel | RoutingStrategies::Bubble)));
    ExecutedEvent = (Core::Observer::RoutedEvent::Register<XUI::Core::Observer::RoutedCommand, ExecutedRoutedEventArgs>(nameof(Executed), RoutingStrategies(RoutingStrategies::Tunnel | RoutingStrategies::Bubble)));
    PreviewCanExecuteEvent = (Core::Observer::RoutedEvent::Register<XUI::Core::Observer::RoutedCommand, CanExecuteRoutedEventArgs>(nameof(PreviewCanExecute), RoutingStrategies(RoutingStrategies::Tunnel | RoutingStrategies::Bubble)));
    CanExecuteEvent = (Core::Observer::RoutedEvent::Register<XUI::Core::Observer::RoutedCommand, CanExecuteRoutedEventArgs>(nameof(CanExecute), RoutingStrategies(RoutingStrategies::Tunnel | RoutingStrategies::Bubble)));
}

void XUI::Core::Observer::RoutedCommand::InvalidateRequerySuggested()
{
}

XUI::Core::Observer::RoutedCommand::RoutedCommand()
{
}

XUI::Core::Observer::RoutedCommand::RoutedCommand(std::string const & name, const System::Reflection::Type * ownerType)
    : _name(name)
    , _ownerType(ownerType)
{
}

XUI::Core::Observer::RoutedCommand::RoutedCommand(std::string const & name, const System::Reflection::Type * ownerType, System::Collection::List<std::shared_ptr<Interfaces::InputGesture>> const & inputGestures)
    : _name(name)
    , _ownerType(ownerType)
{
}

bool XUI::Core::Observer::RoutedCommand::CanExecute(System::Reflection::meta::Variant parameter, UI::Controls::Control * target)
{
    CanExecuteRoutedEventArgs preview_e(RoutedCommand::PreviewCanExecuteEvent.get());
    preview_e.Source = target;
    preview_e.Command = this;
    preview_e.Parameter = parameter;
    target->RaiseEvent(preview_e);

    if (preview_e.CanExecute)
    {
        CanExecuteRoutedEventArgs e(RoutedCommand::CanExecuteEvent.get());
        e.Source = target;
        e.Command = this;
        e.Parameter = parameter;
        target->RaiseEvent(e);

        return e.CanExecute;
    }

    return false;
}

void XUI::Core::Observer::RoutedCommand::Execute(System::Reflection::meta::Variant parameter, UI::Controls::Control * target)
{
    {
        ExecutedRoutedEventArgs e(RoutedCommand::PreviewExecutedEvent.get());
        e.Source = target;
        e.Command = this;
        e.Parameter = parameter;
        target->RaiseEvent(e);
    }

    ExecutedRoutedEventArgs e(RoutedCommand::ExecutedEvent.get());
    e.Source = target;
    e.Command = this;
    e.Parameter = parameter;
    target->RaiseEvent(e);
}

rxcpp::subscription XUI::Core::Observer::RoutedCommand::AddClassHandler(System::Reflection::meta::Type const * targetType, std::shared_ptr<CommandBinding> command)
{
    if (command->CanExecute)
        command->Unregisters.add(AddClassSpecHandler<CanExecuteRoutedEventArgs>(CanExecuteEvent.get(), targetType, this, command->CanExecute.get(), XUI::Core::Observer::RoutingStrategies(XUI::Core::Observer::RoutingStrategies::Bubble | XUI::Core::Observer::RoutingStrategies::Direct), false));
    if (command->PreviewCanExecute)
        command->Unregisters.add(AddClassSpecHandler<CanExecuteRoutedEventArgs>(PreviewCanExecuteEvent.get(), targetType, this, command->PreviewCanExecute.get(), XUI::Core::Observer::RoutingStrategies::Tunnel, false));
    if (command->Executed)
        command->Unregisters.add(AddClassSpecHandler<ExecutedRoutedEventArgs>(ExecutedEvent.get(), targetType, this, command->Executed.get(), XUI::Core::Observer::RoutingStrategies(XUI::Core::Observer::RoutingStrategies::Bubble | XUI::Core::Observer::RoutingStrategies::Direct), false));
    if (command->PreviewExecuted)
        command->Unregisters.add(AddClassSpecHandler<ExecutedRoutedEventArgs>(PreviewExecutedEvent.get(), targetType, this, command->PreviewExecuted.get(), XUI::Core::Observer::RoutingStrategies::Tunnel, false));

    _commands.Add(command);

    return rxcpp::make_subscription([this, weak_command = std::weak_ptr<CommandBinding>(command)]()
    {
        if (!weak_command.expired())
        {
            auto cmd = weak_command.lock();
            _commands.Remove(cmd);
            if (cmd->Unregisters.is_subscribed())
                cmd->Unregisters.unsubscribe();
        }
    });
}

rxcpp::subscription XUI::Core::Observer::RoutedCommand::AddClassInputBinding(System::Reflection::meta::Type const * targetType, std::shared_ptr<Interfaces::InputGesture> const & inputBind)
{
    auto inputEvent = System::make_eventhandler([this, input = inputBind.get()](void* sender, Core::Observer::RoutedEventArgs& e)
    {
        System::Reflection::meta::Object* _sender = (System::Reflection::meta::Object*)sender;
        if (_sender != nullptr && input->Matches(_sender, e))
        {
            if (CanExecute(e, static_cast<UI::Controls::Control*>(_sender)))
                Execute(e, static_cast<UI::Controls::Control*>(_sender));
        }
    });

    inputBind->Unregisters.add(UI::Controls::InputElement::KeyDownEvent->AddClassHandler(targetType, inputEvent, XUI::Core::Observer::RoutingStrategies(XUI::Core::Observer::RoutingStrategies::Bubble | XUI::Core::Observer::RoutingStrategies::Direct), false));

    _inputGestures.Add(inputBind);

    return rxcpp::make_subscription([this, weak_command = std::weak_ptr<Interfaces::InputGesture>(inputBind), inputEvent]()
    {
        if (!weak_command.expired())
        {
            auto cmd = weak_command.lock();
            _inputGestures.Remove(cmd);
            if (cmd->Unregisters.is_subscribed())
                cmd->Unregisters.unsubscribe();
        }

        delete inputEvent;
    });
}

// Inherited via ICommand

bool XUI::Core::Observer::EventHandleCommand::CanExecute(System::Reflection::meta::Variant parameter, UI::Controls::Control * target)
{
    if (_CanExecute)
    {
        CanExecuteRoutedEventArgs e;
        (*_CanExecute)(this, e);
        return e.CanExecute;
    }

    return true;
}

void XUI::Core::Observer::EventHandleCommand::Execute(System::Reflection::meta::Variant parameter, UI::Controls::Control * target)
{
    ExecutedRoutedEventArgs e;
    (*_Executed)(this, e);
}

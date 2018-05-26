#include "StdAfx.h"
#include "LayoutItem.h"
#include "LayoutElement.h"
#include "LayoutContainer.h"
#include "LayoutContent.h"
#include "LayoutRoot.h"
#include "LayoutDocumentPaneGroup.h"
#include "LayoutDocumentFloatingWindow.h"
#include "DockingManager.h"
#include "Presenters/ContentPresenter.h"
#include "XamlCPP/DataBind/Binding.h"
#include "Core/Observer/RoutedCommand.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XamlCPP::Core::Dependency;
using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(LayoutItem, XUI::UI::LayoutItem::StaticClassInitializer);

Core::Dependency::RefDependencyProperty XUI::UI::LayoutItem::TitleProperty;
Core::Dependency::RefDependencyProperty XUI::UI::LayoutItem::IconSourceProperty;
Core::Dependency::RefDependencyProperty XUI::UI::LayoutItem::ContentIdProperty;
Core::Dependency::RefDependencyProperty XUI::UI::LayoutItem::IsSelectedProperty;
Core::Dependency::RefDependencyProperty XUI::UI::LayoutItem::IsActiveProperty;
Core::Dependency::RefDependencyProperty XUI::UI::LayoutItem::CanCloseProperty;
Core::Dependency::RefDependencyProperty XUI::UI::LayoutItem::CanFloatProperty;
Core::Dependency::RefDependencyProperty XUI::UI::LayoutItem::CloseCommandProperty;
Core::Dependency::RefDependencyProperty XUI::UI::LayoutItem::FloatCommandProperty;
Core::Dependency::RefDependencyProperty XUI::UI::LayoutItem::DockAsDocumentCommandProperty;
Core::Dependency::RefDependencyProperty XUI::UI::LayoutItem::CloseAllButThisCommandProperty;
Core::Dependency::RefDependencyProperty XUI::UI::LayoutItem::CloseAllCommandProperty;
Core::Dependency::RefDependencyProperty XUI::UI::LayoutItem::ActivateCommandProperty;
Core::Dependency::RefDependencyProperty XUI::UI::LayoutItem::NewVerticalTabGroupCommandProperty;
Core::Dependency::RefDependencyProperty XUI::UI::LayoutItem::NewHorizontalTabGroupCommandProperty;
Core::Dependency::RefDependencyProperty XUI::UI::LayoutItem::MoveToNextTabGroupCommandProperty;
Core::Dependency::RefDependencyProperty XUI::UI::LayoutItem::MoveToPreviousTabGroupCommandProperty;

void XUI::UI::LayoutItem::StaticClassInitializer()
{
    TitleProperty = (Core::Dependency::BindProperty<std::wstring>::Register<LayoutItem>(nameof(Title), [](IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        auto control = static_cast<LayoutItem*>(d);
        if (control->Element)
            control->Element->Title = e.NewValue.GetValue<std::wstring>();
    }, nullptr));
    IconSourceProperty  = (Core::Dependency::BindProperty<bool>::Register<LayoutItem>(nameof(IconSource)));
    ContentIdProperty  = (Core::Dependency::BindProperty<int>::Register<LayoutItem>(nameof(ContentId)));
    IsSelectedProperty  = (Core::Dependency::BindProperty<bool>::Register<LayoutItem>(nameof(IsSelected)));
    IsActiveProperty  = (Core::Dependency::BindProperty<bool>::Register<LayoutItem>(nameof(IsActive)));
    CanCloseProperty  = (Core::Dependency::BindProperty<bool>::Register<LayoutItem>(nameof(CanClose)));
    CanFloatProperty  = (Core::Dependency::BindProperty<bool>::Register<LayoutItem>(nameof(CanFloat)));
    CloseCommandProperty  = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::ICommand>>::Register<LayoutItem>(nameof(CloseCommand)));
    FloatCommandProperty  = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::ICommand>>::Register<LayoutItem>(nameof(FloatCommand)));
    DockAsDocumentCommandProperty  = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::ICommand>>::Register<LayoutItem>(nameof(DockAsDocumentCommand)));
    CloseAllButThisCommandProperty  = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::ICommand>>::Register<LayoutItem>(nameof(CloseAllButThisCommand)));
    CloseAllCommandProperty  = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::ICommand>>::Register<LayoutItem>(nameof(CloseAllCommand)));
    ActivateCommandProperty  = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::ICommand>>::Register<LayoutItem>(nameof(ActivateCommand)));
    NewVerticalTabGroupCommandProperty  = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::ICommand>>::Register<LayoutItem>(nameof(NewVerticalTabGroupCommand)));
    NewHorizontalTabGroupCommandProperty  = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::ICommand>>::Register<LayoutItem>(nameof(NewHorizontalTabGroupCommand)));
    MoveToNextTabGroupCommandProperty  = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::ICommand>>::Register<LayoutItem>(nameof(MoveToNextTabGroupCommand)));
    MoveToPreviousTabGroupCommandProperty  = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::ICommand>>::Register<LayoutItem>(nameof(MoveToPreviousTabGroupCommand)));

    IsVisibleProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        auto control = e.Sender->Static_As<LayoutItem>();
        if (control)
            control->OnVisibilityChanged(control, (XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e);
    });

    ToolTipProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        auto control = e.Sender->Static_As<LayoutItem>();
        if (control)
            control->OnToolTipChanged(control, (XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e);
    });
}

XUI::UI::LayoutItem::LayoutItem()
{
}

XUI::UI::LayoutItem::~LayoutItem()
{
}

void XUI::UI::LayoutItem::SetElement(LayoutContent * value)
{
    _LayoutElement = (value ? value->shared_from_base_static<LayoutContent>() : nullptr);
}


void XUI::UI::LayoutItem::Attach(LayoutContent *model)
{
    Element = model;
    Model = model->Content;

    InitDefaultCommands();

    Element->IsSelectedChanged += System::make_eventhandler(&LayoutItem::LayoutElement_IsSelectedChanged, this);
    Element->IsActiveChanged += System::make_eventhandler(&LayoutItem::LayoutElement_IsActiveChanged, this);

    DataContext = this;
}

void XUI::UI::LayoutItem::LayoutElement_IsActiveChanged(void* sender, System::Events::EventArg& e)
{
    if (_isActiveReentrantFlag.CanEnter())
    {
        {
            _isActiveReentrantFlag.Enter();
            //auto bnd = BindingOperations::GetBinding(this, IsActiveProperty);
            IsActive = Element->IsActive;
            //auto bnd2 = BindingOperations::GetBinding(this, IsActiveProperty);
        }
    }
}

void XUI::UI::LayoutItem::LayoutElement_IsSelectedChanged(void* sender, System::Events::EventArg& e)
{
    if (_isSelectedReentrantFlag.CanEnter())
    {
        {
            _isSelectedReentrantFlag.Enter();
            IsSelected = Element->IsSelected;
        }
    }
}

void XUI::UI::LayoutItem::Detach()
{
    Element->IsSelectedChanged -= System::make_eventhandler(&LayoutItem::LayoutElement_IsSelectedChanged, this);
    Element->IsActiveChanged -= System::make_eventhandler(&LayoutItem::LayoutElement_IsActiveChanged, this);
    Element = nullptr;
    Model = nullptr;
}


void XUI::UI::LayoutItem::InitDefaultCommands()
{
    _defaultCloseCommand = std::make_shared<Core::Observer::EventHandleCommand>(System::make_shared_eventhandler([this](void*, Core::Observer::ExecutedRoutedEventArgs& e) { ExecuteCloseCommand(e.Parameter); }), System::make_shared_eventhandler([this](void*, Core::Observer::CanExecuteRoutedEventArgs& e) { e.CanExecute = CanExecuteCloseCommand(e.Parameter); }));
    _defaultFloatCommand = std::make_shared<Core::Observer::EventHandleCommand>(System::make_shared_eventhandler([this](void*, Core::Observer::ExecutedRoutedEventArgs& e) { ExecuteFloatCommand(e.Parameter); }), System::make_shared_eventhandler([this](void*, Core::Observer::CanExecuteRoutedEventArgs& e) { e.CanExecute = CanExecuteFloatCommand(e.Parameter); }));
    _defaultDockAsDocumentCommand = std::make_shared<Core::Observer::EventHandleCommand>(System::make_shared_eventhandler([this](void*, Core::Observer::ExecutedRoutedEventArgs& e) { ExecuteDockAsDocumentCommand(e.Parameter); }), System::make_shared_eventhandler([this](void*, Core::Observer::CanExecuteRoutedEventArgs& e) { e.CanExecute = CanExecuteDockAsDocumentCommand(e.Parameter); }));
    _defaultCloseAllButThisCommand = std::make_shared<Core::Observer::EventHandleCommand>(System::make_shared_eventhandler([this](void*, Core::Observer::ExecutedRoutedEventArgs& e) { ExecuteCloseAllButThisCommand(e.Parameter); }), System::make_shared_eventhandler([this](void*, Core::Observer::CanExecuteRoutedEventArgs& e) { e.CanExecute = CanExecuteCloseAllButThisCommand(e.Parameter); }));
    _defaultCloseAllCommand = std::make_shared<Core::Observer::EventHandleCommand>(System::make_shared_eventhandler([this](void*, Core::Observer::ExecutedRoutedEventArgs& e) { ExecuteCloseAllCommand(e.Parameter); }), System::make_shared_eventhandler([this](void*, Core::Observer::CanExecuteRoutedEventArgs& e) { e.CanExecute = CanExecuteCloseAllCommand(e.Parameter); }));
    _defaultActivateCommand = std::make_shared<Core::Observer::EventHandleCommand>(System::make_shared_eventhandler([this](void*, Core::Observer::ExecutedRoutedEventArgs& e) { ExecuteActivateCommand(e.Parameter); }), System::make_shared_eventhandler([this](void*, Core::Observer::CanExecuteRoutedEventArgs& e) { e.CanExecute = CanExecuteActivateCommand(e.Parameter); }));
    _defaultNewVerticalTabGroupCommand = std::make_shared<Core::Observer::EventHandleCommand>(System::make_shared_eventhandler([this](void*, Core::Observer::ExecutedRoutedEventArgs& e) { ExecuteNewVerticalTabGroupCommand(e.Parameter); }), System::make_shared_eventhandler([this](void*, Core::Observer::CanExecuteRoutedEventArgs& e) { e.CanExecute = CanExecuteNewVerticalTabGroupCommand(e.Parameter); }));
    _defaultNewHorizontalTabGroupCommand = std::make_shared<Core::Observer::EventHandleCommand>(System::make_shared_eventhandler([this](void*, Core::Observer::ExecutedRoutedEventArgs& e) { ExecuteNewHorizontalTabGroupCommand(e.Parameter); }), System::make_shared_eventhandler([this](void*, Core::Observer::CanExecuteRoutedEventArgs& e) { e.CanExecute = CanExecuteNewHorizontalTabGroupCommand(e.Parameter); }));
    _defaultMoveToNextTabGroupCommand = std::make_shared<Core::Observer::EventHandleCommand>(System::make_shared_eventhandler([this](void*, Core::Observer::ExecutedRoutedEventArgs& e) { ExecuteMoveToNextTabGroupCommand(e.Parameter); }), System::make_shared_eventhandler([this](void*, Core::Observer::CanExecuteRoutedEventArgs& e) { e.CanExecute = CanExecuteMoveToNextTabGroupCommand(e.Parameter); }));
    _defaultMoveToPreviousTabGroupCommand = std::make_shared<Core::Observer::EventHandleCommand>(System::make_shared_eventhandler([this](void*, Core::Observer::ExecutedRoutedEventArgs& e) { ExecuteMoveToPreviousTabGroupCommand(e.Parameter); }), System::make_shared_eventhandler([this](void*, Core::Observer::CanExecuteRoutedEventArgs& e) { e.CanExecute = CanExecuteMoveToPreviousTabGroupCommand(e.Parameter); }));

}

void XUI::UI::LayoutItem::ClearDefaultBindings()
{
    if (CloseCommand == _defaultCloseCommand)
        ClearValue(CloseCommandProperty.get());
    if (FloatCommand == _defaultFloatCommand)
        ClearValue(FloatCommandProperty.get());
    if (DockAsDocumentCommand == _defaultDockAsDocumentCommand)
        ClearValue(DockAsDocumentCommandProperty.get());
    if (CloseAllButThisCommand == _defaultCloseAllButThisCommand)
        ClearValue(CloseAllButThisCommandProperty.get());
    if (CloseAllCommand == _defaultCloseAllCommand)
        ClearValue(CloseAllCommandProperty.get());
    if (ActivateCommand == _defaultActivateCommand)
        ClearValue(ActivateCommandProperty.get());
    if (NewVerticalTabGroupCommand == _defaultNewVerticalTabGroupCommand)
        ClearValue(NewVerticalTabGroupCommandProperty.get());
    if (NewHorizontalTabGroupCommand == _defaultNewHorizontalTabGroupCommand)
        ClearValue(NewHorizontalTabGroupCommandProperty.get());
    if (MoveToNextTabGroupCommand == _defaultMoveToNextTabGroupCommand)
        ClearValue(MoveToNextTabGroupCommandProperty.get());
    if (MoveToPreviousTabGroupCommand == _defaultMoveToPreviousTabGroupCommand)
        ClearValue(MoveToPreviousTabGroupCommandProperty.get());
}

void XUI::UI::LayoutItem::SetDefaultBindings()
{
    if (CloseCommand == nullptr)
        CloseCommand = _defaultCloseCommand;
    if (FloatCommand == nullptr)
        FloatCommand = _defaultFloatCommand;
    if (DockAsDocumentCommand == nullptr)
        DockAsDocumentCommand = _defaultDockAsDocumentCommand;
    if (CloseAllButThisCommand == nullptr)
        CloseAllButThisCommand = _defaultCloseAllButThisCommand;
    if (CloseAllCommand == nullptr)
        CloseAllCommand = _defaultCloseAllCommand;
    if (ActivateCommand == nullptr)
        ActivateCommand = _defaultActivateCommand;
    if (NewVerticalTabGroupCommand == nullptr)
        NewVerticalTabGroupCommand = _defaultNewVerticalTabGroupCommand;
    if (NewHorizontalTabGroupCommand == nullptr)
        NewHorizontalTabGroupCommand = _defaultNewHorizontalTabGroupCommand;
    if (MoveToNextTabGroupCommand == nullptr)
        MoveToNextTabGroupCommand = _defaultMoveToNextTabGroupCommand;
    if (MoveToPreviousTabGroupCommand == nullptr)
        MoveToPreviousTabGroupCommand = _defaultMoveToPreviousTabGroupCommand;

    IsSelected = Element->IsSelected;
    IsActive = Element->IsActive;
}

void XUI::UI::LayoutItem::_ClearDefaultBindings()
{
    ClearDefaultBindings();
}

void XUI::UI::LayoutItem::_SetDefaultBindings()
{
    SetDefaultBindings();
}

bool XUI::UI::LayoutItem::IsViewExists()
{
    return (_view != nullptr);
}

Presenters::ContentPresenter *LayoutItem::GetView() const
{
    if (_view == nullptr)
    {
        _view = std::make_shared<Presenters::ContentPresenter>();

        std::shared_ptr<XamlCPP::DataBind::Binding> bindDataContext = std::make_shared<XamlCPP::DataBind::Binding>("Content", XUI::Core::Dependency::BindingMode::OneWay);
        bindDataContext->Source = _LayoutElement;
        _viewBindings.add(XUI::Core::Dependency::DependencyObject::Bind(_view.get(), XUI::UI::Presenters::ContentPresenter::ContentProperty.get(), bindDataContext.get(), nullptr));

        if ((Element != nullptr) && (Element->Root != nullptr))
        {
            bindDataContext = std::make_shared<XamlCPP::DataBind::Binding>("LayoutItemTemplate", XUI::Core::Dependency::BindingMode::OneWay);
            bindDataContext->Source = Element->Root->GetManager()->shared_from_base_static<DockingManager>();
            _viewBindings.add(XUI::Core::Dependency::DependencyObject::Bind(_view.get(), XUI::UI::Presenters::ContentPresenter::ContentTemplateProperty.get(), bindDataContext.get(), nullptr));

            //bindDataContext = std::make_shared<XamlCPP::DataBind::Binding>("LayoutItemTemplateSelector", XUI::Core::Dependency::BindingMode::OneWay);
            //bindDataContext->Source = Element->Root->GetManager()->shared_from_base_static<DockingManager>();
            //XUI::Core::Dependency::DependencyObject::Bind(_view.get(), XUI::UI::Presenters::ContentPresenter::ContentTemplateSelectorProperty.get(), bindDataContext.get(), nullptr);

            if (Element->Root->GetManager())
            {
                Element->Root->GetManager()->AddLogicalChild(_view.get());
            }
        }
    }

    return _view.get();
}

void XUI::UI::LayoutItem::OnTitleChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    (static_cast<LayoutItem*>(d))->OnTitleChanged(e);
}

void XUI::UI::LayoutItem::OnTitleChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    if (Element)
    {
        Element->Title = e.NewValue.GetValue<std::wstring>();
    }
}

void XUI::UI::LayoutItem::OnToolTipChanged(DependencyObject *s, Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    (static_cast<LayoutItem*>(s))->OnToolTipChanged();
}

void XUI::UI::LayoutItem::OnToolTipChanged()
{
    if (Element)
    {
        Element->ToolTip = ToolTip;
    }
}

void XUI::UI::LayoutItem::OnIconSourceChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    (static_cast<LayoutItem*>(d))->OnIconSourceChanged(e);
}

void XUI::UI::LayoutItem::OnIconSourceChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    //if (Element)
    //{
    //    Element->IconSource = IconSource;
    //}
}

void XUI::UI::LayoutItem::OnVisibilityChanged(DependencyObject *s, Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    (static_cast<LayoutItem*>(s))->OnVisibilityChanged();
}

void XUI::UI::LayoutItem::OnVisibilityChanged()
{
    if (Element && IsVisible/*Visibility == System::Windows::Visibility::Collapsed*/)
    {
        Element->Close();
    }
}

void XUI::UI::LayoutItem::OnContentIdChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    (static_cast<LayoutItem*>(d))->OnContentIdChanged(e);
}

void XUI::UI::LayoutItem::OnContentIdChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    if (Element)
    {
        //Element->ContentId = e.NewValue.;
    }
}

void XUI::UI::LayoutItem::OnIsSelectedChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    (static_cast<LayoutItem*>(d))->OnIsSelectedChanged(e);
}

void XUI::UI::LayoutItem::OnIsSelectedChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    if (_isSelectedReentrantFlag.CanEnter())
    {
        {
            _isSelectedReentrantFlag.Enter();
            if (Element)
            {
                Element->IsSelected = e.NewValue.GetValue<bool>();
            }
        }
    }
}

void XUI::UI::LayoutItem::OnIsActiveChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    (static_cast<LayoutItem*>(d))->OnIsActiveChanged(e);
}

void XUI::UI::LayoutItem::OnIsActiveChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    if (_isActiveReentrantFlag.CanEnter())
    {
        {
            _isActiveReentrantFlag.Enter();
            if (Element)
            {
                Element->IsActive = e.NewValue.GetValue<bool>();
            }
        }
    }
}

void XUI::UI::LayoutItem::OnCanCloseChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    (static_cast<LayoutItem*>(d))->OnCanCloseChanged(e);
}

void XUI::UI::LayoutItem::OnCanCloseChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    if (Element)
    {
        Element->CanClose = e.NewValue.GetValue<bool>();
    }
}

void XUI::UI::LayoutItem::OnCanFloatChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    (static_cast<LayoutItem*>(d))->OnCanFloatChanged(e);
}

void XUI::UI::LayoutItem::OnCanFloatChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    if (Element)
    {
        Element->CanFloat = e.NewValue.GetValue<bool>();
    }
}

void XUI::UI::LayoutItem::OnCloseCommandChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    (static_cast<LayoutItem*>(d))->OnCloseCommandChanged(e);
}

void XUI::UI::LayoutItem::OnCloseCommandChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
}

System::Reflection::Variant XUI::UI::LayoutItem::CoerceCloseCommandValue(DependencyObject *d, System::Reflection::Variant value)
{
    return value;
}

bool XUI::UI::LayoutItem::CanExecuteCloseCommand(System::Reflection::Variant parameter)
{
    return Element && Element->CanClose;
}

void XUI::UI::LayoutItem::ExecuteCloseCommand(System::Reflection::Variant parameter)
{
    Close();
}

void XUI::UI::LayoutItem::OnFloatCommandChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    (static_cast<LayoutItem*>(d))->OnFloatCommandChanged(e);
}

void XUI::UI::LayoutItem::OnFloatCommandChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
}

System::Reflection::Variant XUI::UI::LayoutItem::CoerceFloatCommandValue(DependencyObject *d, System::Reflection::Variant value)
{
    return value;
}

bool XUI::UI::LayoutItem::CanExecuteFloatCommand(System::Reflection::Variant anchorable)
{
    return Element && Element->CanFloat && LayoutElement::FindParent<LayoutFloatingWindow>(Element) == nullptr;
}

void XUI::UI::LayoutItem::ExecuteFloatCommand(System::Reflection::Variant parameter)
{
    Element->Root->GetManager()->_ExecuteFloatCommand(Element);
}

void XUI::UI::LayoutItem::Float()
{

}

void XUI::UI::LayoutItem::OnDockAsDocumentCommandChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    (static_cast<LayoutItem*>(d))->OnDockAsDocumentCommandChanged(e);
}

void XUI::UI::LayoutItem::OnDockAsDocumentCommandChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
}

System::Reflection::Variant XUI::UI::LayoutItem::CoerceDockAsDocumentCommandValue(DependencyObject *d, System::Reflection::Variant value)
{
    return value;
}

bool XUI::UI::LayoutItem::CanExecuteDockAsDocumentCommand(System::Reflection::Variant parameter)
{
    return Element && LayoutElement::FindParent<LayoutDocumentPane>(Element) == nullptr;
}

void XUI::UI::LayoutItem::ExecuteDockAsDocumentCommand(System::Reflection::Variant parameter)
{
    Element->Root->GetManager()->_ExecuteDockAsDocumentCommand(Element);
}

void XUI::UI::LayoutItem::OnCloseAllButThisCommandChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    (static_cast<LayoutItem*>(d))->OnCloseAllButThisCommandChanged(e);
}

void XUI::UI::LayoutItem::OnCloseAllButThisCommandChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
}

System::Reflection::Variant XUI::UI::LayoutItem::CoerceCloseAllButThisCommandValue(DependencyObject *d, System::Reflection::Variant value)
{
    return value;
}

bool XUI::UI::LayoutItem::CanExecuteCloseAllButThisCommand(System::Reflection::Variant parameter)
{
    if (Element == nullptr)
        return false;

    auto root = Element->Root;
    if (root == nullptr)
        return false;

    return LayoutContainer::ContainsChild(Element->Root->GetManager()->Layout.get(), [&](LayoutElement* d) -> bool
    {
        return d != Element && (d->Parent->Is<LayoutDocumentPane>() || d->Parent->Is<LayoutDocumentFloatingWindow>());
    });
}

void XUI::UI::LayoutItem::ExecuteCloseAllButThisCommand(System::Reflection::Variant parameter)
{
    Element->Root->GetManager()->_ExecuteCloseAllButThisCommand(Element);
}

void XUI::UI::LayoutItem::OnCloseAllCommandChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    (static_cast<LayoutItem*>(d))->OnCloseAllCommandChanged(e);
}

void XUI::UI::LayoutItem::OnCloseAllCommandChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
}

System::Reflection::Variant XUI::UI::LayoutItem::CoerceCloseAllCommandValue(DependencyObject *d, System::Reflection::Variant value)
{
    return value;
}

bool XUI::UI::LayoutItem::CanExecuteCloseAllCommand(System::Reflection::Variant parameter)
{
    if (Element == nullptr)
        return false;

    auto root = Element->Root;
    if (root == nullptr)
        return false;

    return LayoutContainer::ContainsChild(Element->Root->GetManager()->Layout.get(), [&](LayoutElement* d) -> bool
    {
        return (d->Parent->Is<LayoutDocumentPane>() || d->Parent->Is<LayoutDocumentFloatingWindow>());
    });
}

void XUI::UI::LayoutItem::ExecuteCloseAllCommand(System::Reflection::Variant parameter)
{
    Element->Root->GetManager()->_ExecuteCloseAllCommand(Element);
}

void XUI::UI::LayoutItem::OnActivateCommandChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    (static_cast<LayoutItem*>(d))->OnActivateCommandChanged(e);
}

void XUI::UI::LayoutItem::OnActivateCommandChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
}

System::Reflection::Variant XUI::UI::LayoutItem::CoerceActivateCommandValue(DependencyObject *d, System::Reflection::Variant value)
{
    return value;
}

bool XUI::UI::LayoutItem::CanExecuteActivateCommand(System::Reflection::Variant parameter)
{
    return Element;
}

void XUI::UI::LayoutItem::ExecuteActivateCommand(System::Reflection::Variant parameter)
{
    Element->Root->GetManager()->_ExecuteContentActivateCommand(Element);
}

void XUI::UI::LayoutItem::OnNewVerticalTabGroupCommandChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    (static_cast<LayoutItem*>(d))->OnNewVerticalTabGroupCommandChanged(e);
}

void XUI::UI::LayoutItem::OnNewVerticalTabGroupCommandChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
}

bool XUI::UI::LayoutItem::CanExecuteNewVerticalTabGroupCommand(System::Reflection::Variant parameter)
{
    if (Element == nullptr)
        return false;

    auto parentDocumentGroup = LayoutElement::FindParent<LayoutDocumentPaneGroup>(Element);
    auto parentDocumentPane = Element->Parent->Static_As<LayoutDocumentPane>();
    return ((parentDocumentGroup == nullptr || parentDocumentGroup->Children.size() == 1 || parentDocumentGroup->Root->GetManager()->AllowMixedOrientation || parentDocumentGroup->Orientation == UI::Orientation::Horizontal) && parentDocumentPane != nullptr && parentDocumentPane->Children.size() > 1);
}

void XUI::UI::LayoutItem::ExecuteNewVerticalTabGroupCommand(System::Reflection::Variant parameter)
{
    auto layoutElement = Element;
    auto parentDocumentGroup = LayoutElement::FindParent<LayoutDocumentPaneGroup>(Element);
    auto parentDocumentPane = Element->Parent->Static_As<LayoutDocumentPane>();

    if (parentDocumentGroup == nullptr)
    {
        auto grandParent = parentDocumentPane->Parent;
        parentDocumentGroup = new LayoutDocumentPaneGroup();
        parentDocumentGroup->Orientation = UI::Orientation::Horizontal;
        grandParent->ReplaceChild(parentDocumentPane, parentDocumentGroup);
        parentDocumentGroup->AddChild(parentDocumentPane);
    }
    parentDocumentGroup->Orientation = UI::Orientation::Horizontal;
    int indexOfParentPane = parentDocumentGroup->IndexOfChild(parentDocumentPane);

    auto newLayoutDocumentPane = std::make_shared<LayoutDocumentPane>();
    newLayoutDocumentPane->AddChild(layoutElement);
    parentDocumentGroup->InsertChildAt(indexOfParentPane + 1, newLayoutDocumentPane.get());
    Element->IsActive = true;
    Element->Root->CollectGarbage();
}

void XUI::UI::LayoutItem::OnNewHorizontalTabGroupCommandChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    (static_cast<LayoutItem*>(d))->OnNewHorizontalTabGroupCommandChanged(e);
}

void XUI::UI::LayoutItem::OnNewHorizontalTabGroupCommandChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
}

bool XUI::UI::LayoutItem::CanExecuteNewHorizontalTabGroupCommand(System::Reflection::Variant parameter)
{
    if (Element == nullptr)
        return false;

    auto parentDocumentGroup = LayoutElement::FindParent<LayoutDocumentPaneGroup>(Element);
    auto parentDocumentPane = Element->Parent->Static_As<LayoutDocumentPane>();
    return ((parentDocumentGroup == nullptr || parentDocumentGroup->Children.size() == 1 || parentDocumentGroup->Root->GetManager()->AllowMixedOrientation || parentDocumentGroup->Orientation == UI::Orientation::Vertical) && parentDocumentPane != nullptr && parentDocumentPane->Children.size() > 1);
}

void XUI::UI::LayoutItem::ExecuteNewHorizontalTabGroupCommand(System::Reflection::Variant parameter)
{
    auto layoutElement = Element;
    auto parentDocumentGroup = LayoutElement::FindParent<LayoutDocumentPaneGroup>(Element);
    auto parentDocumentPane = Element->Parent->Static_As<LayoutDocumentPane>();

    if (parentDocumentGroup == nullptr)
    {
        auto grandParent = parentDocumentPane->Parent;
        parentDocumentGroup = new LayoutDocumentPaneGroup();
        parentDocumentGroup->Orientation = UI::Orientation::Vertical;
        grandParent->ReplaceChild(parentDocumentPane, parentDocumentGroup);
        parentDocumentGroup->AddChild(parentDocumentPane);
    }
    parentDocumentGroup->Orientation = UI::Orientation::Vertical;
    int indexOfParentPane = parentDocumentGroup->IndexOfChild(parentDocumentPane);

    auto newLayoutDocumentPane = std::make_shared<LayoutDocumentPane>();
    newLayoutDocumentPane->AddChild(layoutElement);
    parentDocumentGroup->InsertChildAt(indexOfParentPane + 1, newLayoutDocumentPane.get());
    Element->IsActive = true;
    Element->Root->CollectGarbage();
}

void XUI::UI::LayoutItem::OnMoveToNextTabGroupCommandChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    (static_cast<LayoutItem*>(d))->OnMoveToNextTabGroupCommandChanged(e);
}

void XUI::UI::LayoutItem::OnMoveToNextTabGroupCommandChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
}

bool XUI::UI::LayoutItem::CanExecuteMoveToNextTabGroupCommand(System::Reflection::Variant parameter)
{
    if (Element == nullptr)
        return false;

    auto parentDocumentGroup = LayoutElement::FindParent<LayoutDocumentPaneGroup>(Element);
    auto parentDocumentPane = Element->Parent->Static_As<LayoutDocumentPane>();
    return (parentDocumentGroup != nullptr && parentDocumentPane != nullptr && parentDocumentGroup->Children.size() > 1 &&
        parentDocumentGroup->IndexOfChild(parentDocumentPane) < parentDocumentGroup->Children.size() - 1 &&
        (parentDocumentGroup->Children[parentDocumentGroup->IndexOfChild(parentDocumentPane) + 1])->Static_As<LayoutDocumentPane>() != nullptr);
}

void XUI::UI::LayoutItem::ExecuteMoveToNextTabGroupCommand(System::Reflection::Variant parameter)
{
    auto layoutElement = Element;
    auto parentDocumentGroup = LayoutElement::FindParent<LayoutDocumentPaneGroup>(Element);
    auto parentDocumentPane = Element->Parent->Static_As<LayoutDocumentPane>();
    int indexOfParentPane = parentDocumentGroup->IndexOfChild(parentDocumentPane);
    auto nextDocumentPane = parentDocumentGroup->GetChild(indexOfParentPane + 1)->Static_As<LayoutDocumentPane>();
    nextDocumentPane->InsertChildAt(0, layoutElement);
    Element->IsActive = true;
    Element->Root->CollectGarbage();
}

void XUI::UI::LayoutItem::OnMoveToPreviousTabGroupCommandChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    (static_cast<LayoutItem*>(d))->OnMoveToPreviousTabGroupCommandChanged(e);
}

void XUI::UI::LayoutItem::OnMoveToPreviousTabGroupCommandChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
}

bool XUI::UI::LayoutItem::CanExecuteMoveToPreviousTabGroupCommand(System::Reflection::Variant parameter)
{
    if (Element == nullptr)
    {
        return false;
    }
    auto parentDocumentGroup = LayoutElement::FindParent<LayoutDocumentPaneGroup>(Element);
    auto parentDocumentPane = Element->Parent->Static_As<LayoutDocumentPane>();
    return (parentDocumentGroup != nullptr && parentDocumentPane != nullptr && parentDocumentGroup->Children.size() > 1 && parentDocumentGroup->IndexOfChild(parentDocumentPane) > 0 && (parentDocumentGroup->Children[parentDocumentGroup->IndexOfChild(parentDocumentPane) - 1])->Static_As<LayoutDocumentPane>() != nullptr);
}

void XUI::UI::LayoutItem::ExecuteMoveToPreviousTabGroupCommand(System::Reflection::Variant parameter)
{
    auto layoutElement = Element;
    auto parentDocumentGroup = LayoutElement::FindParent<LayoutDocumentPaneGroup>(Element);
    auto parentDocumentPane = Element->Parent->Static_As<LayoutDocumentPane>();
    int indexOfParentPane = parentDocumentGroup->IndexOfChild(parentDocumentPane);
    auto nextDocumentPane = parentDocumentGroup->GetChild(indexOfParentPane - 1)->Static_As<LayoutDocumentPane>();
    nextDocumentPane->InsertChildAt(0, layoutElement);
    Element->IsActive = true;
    Element->Root->CollectGarbage();
}


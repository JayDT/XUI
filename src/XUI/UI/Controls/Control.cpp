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

#include "Control.h"

#include "UI/Grid.h"
#include "UI/Detail/ToolTipService.h"
#include "Core/Application.h"
#include "Core/GlobalInitializerScheduler.h"
#include "UI/ContextMenu.h"
#include "Platform/UIService.h"
#include "Interfaces/UIControls/IControl.h"
#include "XamlCPP/Core/LogInterface.h"
#include "XamlCPP/Interfaces/INamedScope.h"
#include "XamlCPP/Interfaces/IInstanceLifeCycleListener.h"

using namespace XUI;
using namespace XUI::UI::Controls;

STATIC_CLASS_INITIALIZER(Control, Control::StaticClassInitializer)

/// <summary>
/// Defines the <see cref="DataContext"/> property.
/// </summary>
Core::Dependency::RefDependencyProperty         Control::DataContextProperty;
Core::Dependency::RefDependencyProperty         Control::NameProperty;
Core::Dependency::RefDependencyProperty         Control::ParentProperty;
Core::Dependency::RefDependencyProperty         Control::TagProperty;
Core::Dependency::RefDependencyProperty         Control::ContextMenuProperty;
Core::Dependency::RefDependencyProperty         Control::ToolTipProperty;
Core::Dependency::RefDependencyProperty         Control::StyleProperty;
Core::Dependency::RefDependencyProperty         Control::FocusVisualStyleProperty;
Core::Dependency::RefDependencyProperty         Control::TemplateProperty;

std::shared_ptr<Core::Observer::RoutedEvent>    Control::RequestBringIntoViewEvent;

/// <summary>
/// Initializes static members of the <see cref="Control"/> class.
/// </summary>
void XUI::UI::Controls::Control::StaticClassInitializer()
{
    DataContextProperty = (Core::Dependency::BindProperty<System::Reflection::Variant>::Register<Control>(nameof(DataContext), nullptr, true, Core::Dependency::BindingMode::OneWay, nullptr, &Control::DataContextNotifying));
    NameProperty = (Core::Dependency::DirectProperty<Control, std::string>::RegisterDirect(nameof(Name), [](XUI::UI::Controls::Control* x) {return x->Name; }, [](XUI::UI::Controls::Control* x, std::string const& value) { x->Name = value; }));
    ParentProperty = (Core::Dependency::DirectProperty<Control, Control*>::RegisterDirect(nameof(Parent), [](XUI::UI::Controls::Control* x) { return x->Parent; }, nullptr));
    TagProperty = (Core::Dependency::BindProperty<System::Reflection::meta::Variant>::Register<Control>(nameof(Tag)));
    ContextMenuProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::UI::ContextMenu>>::Register<Control>(nameof(ContextMenu)));
    TemplateProperty = (Core::Dependency::BindProperty<std::shared_ptr<UI::DataTemplates::ControlTemplate>>::Register<XUI::UI::Controls::Control>(nameof(Template)));
    ToolTipProperty = (Core::Dependency::BindProperty<System::Reflection::RefObject>::Register<XUI::UI::Controls::Control>(nameof(ToolTip)));

    StyleProperty = (Core::Dependency::DirectProperty<Control, std::shared_ptr<XamlCPP::Interfaces::IStyle>>::RegisterDirect(nameof(Style), [](XUI::UI::Controls::Control* x) {return x->Style; }, [](XUI::UI::Controls::Control* x, std::shared_ptr<XamlCPP::Interfaces::IStyle> const& value) { x->Style = value; }));
    FocusVisualStyleProperty = (Core::Dependency::DirectProperty<Control, std::shared_ptr<XamlCPP::Interfaces::IStyle>>::RegisterDirect(nameof(FocusVisualStyle), [](XUI::UI::Controls::Control* x) {return x->FocusVisualStyle; }, [](XUI::UI::Controls::Control* x, std::shared_ptr<XamlCPP::Interfaces::IStyle> const& value) { x->FocusVisualStyle = value; }));

    RequestBringIntoViewEvent = (Core::Observer::RoutedEvent::Register<XUI::UI::Controls::Control, Core::Observer::RoutedEventArgs>(nameof(RequestBringIntoView), Core::Observer::RoutingStrategies::Bubble));

    TemplateProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        static_cast<UI::Controls::Control*>(e.Sender)->OnTemplateChanged(e);
    });

    DataContextProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        static_cast<UI::Controls::Control*>(e.Sender)->OnDataContextChanged();
    });

    //PseudoClass<bool>(IsEnabledCoreProperty.get(), [](bool x) -> bool { return !x; }, ":disabled");
    //PseudoClass<bool>(IsFocusedProperty.get(), [](bool x) -> bool { return x; }, ":focus");
    //PseudoClass<bool>(IsPointerOverProperty.get(), [](bool x) -> bool { return x; }, ":pointerover");
    //PseudoClass<bool>(IsMouseOverProperty.get(), [](bool x) -> bool { return x; }, ":pointerover");
}

/// <summary>
/// Initializes a new instance of the <see cref="TemplateAppliedEventArgs"/> class.
/// </summary>
/// <param name="nameScope">The applied template's name scope.</param>
XUI::UI::Controls::TemplateAppliedEventArgs::TemplateAppliedEventArgs(Control * scope)
    : RoutedEventArgs(UIElement::TemplateAppliedEvent.get())
{
    Scope = scope;
}

XUI::UI::Controls::LogicalTreeAttachmentEventArgs::LogicalTreeAttachmentEventArgs(Control * root)
{
    System::ThrowIfFailed<System::ArgumentNullException>(root != nullptr);

    Root = root->shared_from_base_static<XUI::UI::Controls::Control>();
}

XUI::UI::Controls::Control::Control()
    : TemplateApplied(this, TemplateAppliedEvent.get())
    , RequestBringIntoView(this)
{
    _logicalChildren.CollectionChanged += System::make_eventhandler(&Control::LogicalChildrenCollectionChanged, this);
}

Control::~Control()
{
    if (_styleDetach)
        delete _styleDetach;
    _styleDetach = nullptr;
}

System::Reflection::Variant XUI::UI::Controls::Control::GetDataContext() const
{
    return GetValue(DataContextProperty.get());
}

void XUI::UI::Controls::Control::SetDataContext(System::Reflection::Variant const& value)
{
    SetValue(DataContextProperty.get(), value);
}

std::shared_ptr<XUI::UI::ContextMenu> XUI::UI::Controls::Control::GetContextMenu() const
{
    return GetValue<std::shared_ptr<XUI::UI::ContextMenu>>(ContextMenuProperty);
}

void XUI::UI::Controls::Control::SetContextMenu(std::shared_ptr<XUI::UI::ContextMenu> const& value)
{
    SetValue(ContextMenuProperty, value);
}

System::Reflection::RefObject Control::GetToolTip() const
{
    return GetValue<System::Reflection::RefObject>(ToolTipProperty);
}

void Control::SetToolTip(System::Reflection::RefObject const & value)
{
    if (_toolTipSubsciption.is_subscribed())
        _toolTipSubsciption.unsubscribe();

    if (ToolTip)
    {
        Controls::Control* oldtip = ToolTip->Static_As<Controls::Control>();
        if (oldtip)
            RemoveLogicalChild(oldtip);
    }

    SetValue(ToolTipProperty, value);

    if (ToolTip)
    {
        rxcpp::composite_subscription _subscription;
        _subscription.add(PointerEnter += System::make_eventhandler(&UI::ToolTipService::ControlPointerEnter, UI::ToolTipService::Instance()));
        _subscription.add(PointerLeave += System::make_eventhandler(&UI::ToolTipService::ControlPointerLeave, UI::ToolTipService::Instance()));
        _toolTipSubsciption = _subscription;

        Controls::Control* newtip = ToolTip->Static_As<Controls::Control>();
        if (newtip)
            AddLogicalChild(newtip);
    }

}

void XUI::UI::Controls::Control::DataContextNotifying(XamlCPP::Core::Dependency::IDependencyObject * o, System::Reflection::meta::Variant const& notifying)
{
    auto control = o->Static_As<Control>();

    if (control != nullptr)
        control->OnDataContextChanging();
}

System::Reflection::meta::Variant XUI::UI::Controls::Control::GetTag() const
{
    return GetValue(TagProperty.get());
}

void XUI::UI::Controls::Control::SetTag(System::Reflection::meta::Variant const & value)
{
    SetValue(TagProperty.get(), value);
}

void XUI::UI::Controls::Control::OnDataContextChanged()
{
    if (DataContextChanged)
        DataContextChanged(this, System::Events::EventArg());
}

void XUI::UI::Controls::Control::OnGotFocus(Interfaces::GotFocusEventArgs & e)
{
    InputElement::OnGotFocus(e);

    if (IsFocused &&
        (e.NavigationMethod == Interfaces::NavigationMethod::Tab ||
            e.NavigationMethod == Interfaces::NavigationMethod::Directional))
    {
    }
}

void XUI::UI::Controls::Control::OnLostFocus(Core::Observer::RoutedEventArgs & e)
{
    InputElement::OnLostFocus(e);
}

Control * XUI::UI::Controls::Control::FindStyleRoot(Control * e)
{
    return e;
}

void XUI::UI::Controls::Control::OnAttachedToLogicalTreeCore(LogicalTreeAttachmentEventArgs & e)
{
    if (!_isAttachedToLogicalTree)
    {
        _isAttachedToLogicalTree = true;

        if (_initCount == 0)
        {
            RegisterWithNameScope();
            ApplyStyling();
			if (!_styled)
				m_Resources.CollectionChanged += System::make_eventhandler(&Control::ResourceCollectionChanged, this);
			_styled = true;
        }

        OnAttachedToLogicalTree(e);
    }

    if (ChangedLogicalTree)
        ChangedLogicalTree(this, System::Events::sEventArg);

    for (auto const& child : LogicalChildren.GetContainer())
    {
        child->OnAttachedToLogicalTreeCore(e);
    }
}

void XUI::UI::Controls::Control::OnDetachedFromLogicalTreeCore(LogicalTreeAttachmentEventArgs & e)
{
    if (_isAttachedToLogicalTree)
    {
        if (!Name.empty())
        {
            if (!_nameScope.expired())
            {
                _nameScope.lock()->Unregister(Name);
            }
            else
            {
                XamlCPP::Interfaces::INameScope* nameScope = XUI::Platform::UIService::Instance()->Get<XamlCPP::Interfaces::INameScope>();
                if (nameScope)
                    nameScope->Unregister(Name);
            }
        }

        // Never can release FrameworkTemplate Namescope until remove template
        if (_selfTemplateOwner.expired() || e.Root.lock().get() == this)
            _nameScope.reset();
        _isAttachedToLogicalTree = false;

        if (_styleDetach && _styleDetach->has_observers())
            _styleDetach->get_observer().on_next(static_cast<XamlCPP::Interfaces::IStyleable*>(this)); //.get_subscriber().on_next(static_cast<XamlCPP::Interfaces::IStyleable*>(this));

        for(auto const& child : LogicalChildren.GetContainer())
        {
            child->OnDetachedFromLogicalTreeCore(e);
        }

//#if DEBUG
//        if (((INotifyCollectionChangedDebug)_classes).GetCollectionChangedSubscribers() ? .Length > 0)
//        {
//            Logger.Warning(
//                LogArea.Control,
//                this,
//                "{Type} detached from logical tree but still has class listeners",
//                this.GetType());
//        }
//#endif
    }
}

void XUI::UI::Controls::Control::ApplyStyling()
{
    if (Style)
    {
        Style->Attach(static_cast<XamlCPP::Interfaces::IStyleable*>(this), static_cast<XamlCPP::Interfaces::IStyleHost*>(this));
    }
    else
    {
        auto style = FindResource("{Type: " + std::string(GetType()->GetFullName()) + "}");
        if (style.GetBase() && *style.GetType() == typeid(XUI::Styling::Style))
        {
            Style = style.GetValue<std::shared_ptr<XUI::Styling::Style>>();
            return;
        }
    }

    //ApplyTemplate();
}


void Control::InitializedIfPrepared()
{
    if (!IsInitialized && _isAttachedToLogicalTree && _initCount == 0)
    {
        IsInitialized = true;
        if (Initialized)
        {
            Initialized(static_cast<System::Reflection::Object*>(this), System::Events::EventArg());
            Initialized.clear();
        }
    }
}

/// <inheritdoc/>
void XUI::UI::Controls::Control::BeginInit()
{
    ++_initCount;
}

/// <inheritdoc/>
void XUI::UI::Controls::Control::EndInit()
{
    if (_initCount <= 0)
    {
        throw System::InvalidOperationException("BeginInit was not called.");
    }

    if (--_initCount == 0)
    {
        InitializedIfPrepared();

        if (!_styled)
        {
			bool firstInit = !_styled;
            _styled = true;
            RegisterWithNameScope();
            ApplyStyling();
			if (firstInit)
				m_Resources.CollectionChanged += System::make_eventhandler(&Control::ResourceCollectionChanged, this);
		}
    }
}

/// <summary>
/// Sets the control's logical parent.
/// </summary>
/// <param name="parent">The parent.</param>
void XUI::UI::Controls::Control::SetParent(Control * parent)
{
    auto old = Parent;

    if (parent != old)
    {
        if (old != nullptr && parent != nullptr)
        {
            throw System::InvalidOperationException("The Control already has a parent.");
        }

        if (_isAttachedToLogicalTree && !_isMovingInLogicalTemplate)
        {
            Interfaces::IRenderRoot* renderRootParent = Dynamic_As<Interfaces::IRenderRoot>();
            auto oldRoot = renderRootParent && renderRootParent->IsRootMode() ? this : FindRenderRoot(old);

#ifdef _DEBUG
            if (oldRoot == nullptr)
            {
                throw System::InvalidOperationException("Was attached to logical tree but cannot find root.");
            }
#endif

            if (oldRoot)
            {
                auto e = LogicalTreeAttachmentEventArgs(static_cast<Control*>(oldRoot));
                OnDetachedFromLogicalTreeCore(e);
            }
        }

        if (!_isMovingInLogicalTemplate && (_inheritanceParent == nullptr || parent == nullptr))
        {
            SetInheritanceParent(parent);
        }

        _parent = parent ? parent->shared_from_base_static<Control>() : nullptr;

        if (!_parent.expired() && parent->IsAttachedToLogicalTree && !_isMovingInLogicalTemplate)
        {
            Interfaces::IRenderRoot* renderRootParent = Dynamic_As<Interfaces::IRenderRoot>();
            Visual* newRoot = renderRootParent && renderRootParent->IsRootMode() ? this : FindRenderRoot(parent);
            if (!newRoot || !newRoot->Is<Control>())
                throw System::InvalidOperationException("Parent is atttached to logical tree but cannot find root.");

            auto e = LogicalTreeAttachmentEventArgs(static_cast<Control*>(newRoot));
            OnAttachedToLogicalTreeCore(e);
        }

        RaisePropertyChanged(ParentProperty.get(), old, _parent, Core::Dependency::BindingLevel::LocalValue);
    }
}

void XUI::UI::Controls::Control::OnAttachedToLogicalTree(LogicalTreeAttachmentEventArgs& e)
{
    InitializedIfPrepared();
    if (AttachedToLogicalTree)
        AttachedToLogicalTree(this, e);
}

void XUI::UI::Controls::Control::OnDetachedFromLogicalTree(LogicalTreeAttachmentEventArgs& e)
{
    if (DetachedFromLogicalTree)
        DetachedFromLogicalTree(this, e);
}

void XUI::UI::Controls::Control::OnAttachedToVisualTreeCore(VisualTreeAttachmentEventArgs const & e)
{
    Visual::OnAttachedToVisualTreeCore(e);

    if (!IsInitialized)
    {
        IsInitialized = true;
        if (Initialized)
        {
            Initialized(static_cast<System::Reflection::Object*>(this), System::Events::EventArg());
            Initialized.clear();
        }
    }
}

void XUI::UI::Controls::Control::OnDetachedFromVisualTreeCore(VisualTreeAttachmentEventArgs const & e)
{
    Visual::OnDetachedFromVisualTreeCore(e);
}

void XUI::UI::Controls::Control::OnDataContextChanging()
{
}

void XUI::UI::Controls::Control::AddLogicalChild(Control * child)
{
    if (child->Parent == this)
        return;

    System::ThrowIfFailed<System::ArgumentException>(child->Parent == this || child->Parent == nullptr);

    LogicalChildren.Add(child->shared_from_base_static<Control>());
}

void XUI::UI::Controls::Control::RemoveLogicalChild(Control * child)
{
    System::ThrowIfFailed<System::ArgumentException>(child->Parent == this || child->Parent == nullptr);

    for (auto iChild = LogicalChildren.GetContainer().begin(); iChild != LogicalChildren.GetContainer().end(); ++iChild)
        if (iChild->get() == child)
        {
            LogicalChildren.Remove(*iChild); // .GetContainer().erase(iChild);
            break;
        }
}

std::shared_ptr<XUI::UI::DataTemplates::DataTemplate> Control::FindInternalDataTemplate(std::string const & data)
{
    auto iResource = m_DataTemplates.GetNativeEnumerator().find(data);
    if (iResource != m_DataTemplates.GetNativeEnumerator().end())
        return iResource->second.TryGetValue<std::shared_ptr<XUI::UI::DataTemplates::DataTemplate>>();

    Core::Dependency::DependencyObject* object = GetInheritanceParent();
    while (object)
    {
        if (object->Is<Control>())
        {
            auto iResource = static_cast<Control*>(object)->DataTemplates.GetNativeEnumerator().find(data);
            if (iResource != static_cast<Control*>(object)->DataTemplates.GetNativeEnumerator().end())
                return iResource->second.TryGetValue<std::shared_ptr<XUI::UI::DataTemplates::DataTemplate>>();
        }
        object = object->GetInheritanceParent();
    }

    return nullptr;
}

std::shared_ptr<XUI::UI::DataTemplates::DataTemplate> XUI::UI::Controls::Control::FindDataTemplate(System::Reflection::meta::Variant const& data)
{
    auto contentType = data.GetValue<const System::Reflection::Type*>();

    std::string _typename = contentType->GetFullName();

    if (*contentType == typeid(std::wstring) || *contentType == typeid(std::string))
        _typename = "System::String";

    auto defaultName = "{Type: " + _typename + "}.DataTemplate";

    auto datatemplate = FindInternalDataTemplate(defaultName);
    if (datatemplate)
        return datatemplate;

    auto iGlobalResource = Core::Application::m_DataTemplates.GetNativeEnumerator().find(defaultName);
    if (iGlobalResource != Core::Application::m_DataTemplates.GetNativeEnumerator().end())
        if (iGlobalResource->second.IsObject() && iGlobalResource->second.GetBase()->IsSharedPointer())
            return iGlobalResource->second.TryGetValue<std::shared_ptr<XUI::UI::DataTemplates::DataTemplate>>();

    return {};
}

std::shared_ptr<XUI::UI::DataTemplates::ControlTemplate> XUI::UI::Controls::Control::FindControlTemplate(System::Reflection::meta::Variant const& data)
{
    if (!data.Valid() && Template)
        return Template;

    std::string _typename = data.GetBase() ? data.GetValue<const System::Reflection::Type*>()->GetFullName() : GetType()->GetFullName();

    auto defaultName = "{Type: " + _typename + "}.ControlTemplate";
    auto resource = FindResource(defaultName);
    if (resource.IsObject() && resource.GetBase()->IsSharedPointer())
        return std::move(resource.TryGetValue<std::shared_ptr<XUI::UI::DataTemplates::ControlTemplate>>());
    return {};
}

std::shared_ptr<UI::DataTemplates::ControlTemplate> XUI::UI::Controls::Control::GetTemplate() const
{
    return GetValue<std::shared_ptr<UI::DataTemplates::ControlTemplate>>(TemplateProperty.get());
}

void XUI::UI::Controls::Control::SetTemplate(std::shared_ptr<UI::DataTemplates::ControlTemplate> const& value)
{
    if (Template == value)
        return;

    if (m_controlTemplateRoot)
        RemoveTemplate();

    SetValue(TemplateProperty.get(), value);
}

System::Reflection::Variant const & Control::FindInternalResource(const std::string & name) const
{
    auto iResource = Resources.GetNativeEnumerator().find(name);
    if (iResource != Resources.GetNativeEnumerator().end())
        return iResource->second;

    Core::Dependency::DependencyObject* object = GetInheritanceParent();
    if (object)
    {
        auto const& value = object->FindInternalResource(name);
        if (value.GetBase())
            return value;
    }

    return System::Reflection::Variant::Empty;
}

System::Reflection::Variant const& XUI::UI::Controls::Control::FindResource(const std::string &name) const
{
    auto const& resource = FindInternalResource(name);
    if (resource.GetBase())
        return resource;

    auto iResource = Core::Application::m_Resources.GetNativeEnumerator().find(name);
    if (iResource != Core::Application::m_Resources.GetNativeEnumerator().end())
        return iResource->second;

    return System::Reflection::Variant::Empty;
}

void XUI::UI::Controls::Control::ApplyTemplate()
{
    if (!_styled)
        return;

    if (!_defaultControlTemplateInitialized && !Template)
    {
        _defaultControlTemplateInitialized = true;
        SetTemplate(FindControlTemplate(System::Reflection::meta::Variant::Empty));
    }

    if (_styled && Template && !m_controlTemplateRoot)
    {
        _defaultControlTemplateInitialized = true;

        Control* templateParent = this;
        //if (Template->TargetType && templateParent->GetType() != Template->TargetType)
        //    templateParent = VisitLogicalAncestors([&](Control* c) { return c && c->GetType() == Template->TargetType; });

        System::ThrowIfFailed<System::InvalidOperationException>(templateParent, "Invalid template parent");

        RemoveTemplate();
        XamlCPP::Platform::ILog::Verbose(XamlCPP::Platform::LogArea::Control, nullptr, "Creating control template");

        //std::list<std::shared_ptr<Controls::Control>> objects;
        //auto listener = std::make_shared<CollectorLifeCycleListener>(&objects);

        m_controlTemplateRoot = Template->Build(/*listener.get()*/nullptr)->Static_As<Core::Dependency::DependencyObject>()->shared_from_base_static<Core::Dependency::DependencyObject>();
        if (m_controlTemplateRoot)
        {
            Visual* visual = m_controlTemplateRoot->As<XUI::UI::Controls::Visual>();
            if (visual)
            {
                std::list<UI::Controls::Visual*> visuals;
                visual->GetVisualDescendents(visuals);
                //for (auto obj : objects)
                //    if (obj->_initCount)
                //        visuals.push_back(obj.get());

                XUI::UI::Controls::Control* control = visual->As<XUI::UI::Controls::Control>();
                XUI::UI::Controls::UIElement* element = visual->As<XUI::UI::Controls::UIElement>();
                if (element)
                    element->TemplatedParent = shared_from_base_static<Core::Dependency::DependencyObject>();

                //if (control)
                //    control->BeginInit();
                AddVisualChild(visual);
                if (control)
                    AddLogicalChild(control);
                //if (control)
                //    control->EndInit();

                Template->Attach(this, visuals);
                TemplateAppliedEventArgs e(this);
                OnTemplateApplied(e);
            }
        }
    }

    ApplyDataTemplate();
}

void XUI::UI::Controls::Control::RemoveTemplate()
{
    RemoveDataTemplate();

    Template->Detach(this);

    if (m_controlTemplateRoot)
    {
        Visual* visual = m_controlTemplateRoot->As<XUI::UI::Controls::Visual>();
        if (visual)
        {
            XUI::UI::Controls::UIElement* element = visual->As<XUI::UI::Controls::UIElement>();
            if (element)
                element->TemplatedParent = nullptr;

            RemoveVisualChild(visual);
        }

        m_controlTemplateRoot = nullptr;
    }
}

XamlCPP::Interfaces::IStyleHost * XUI::UI::Controls::Control::GetStylingParent()
{
    return _parent.lock()->As<XamlCPP::Interfaces::IStyleHost>();
}

void XUI::UI::Controls::Control::RegisterWithNameScope()
{
    auto namedScope = NamedScope;
    if (namedScope && !Name.empty())
        namedScope->Register(Name, shared_from_base_static<System::Reflection::Object>());
}

XamlCPP::Interfaces::INameScope* XUI::UI::Controls::Control::GetNamedScope() const
{
    if (!_nameScope.expired())
        return _nameScope.lock().get();
    XamlCPP::Interfaces::INameScope* namedScope = GetSharedNamedScope().get();
    if (namedScope)
        return namedScope;

    return Platform::UIService::Instance()->Get<XamlCPP::Interfaces::INameScope>();
}

std::shared_ptr<XamlCPP::Interfaces::INameScope> XUI::UI::Controls::Control::GetSharedNamedScope() const
{
    //if (const_cast<XUI::UI::Controls::Control*>(this)->Is<UI::Grid>())
    //    printf("");

    if (_nameScope.expired() && const_cast<XUI::UI::Controls::Control*>(this)->Is<XamlCPP::Interfaces::INameScope>())
    {
        _nameScope = const_cast<XUI::UI::Controls::Control*>(this)->shared_from_base_dynamic<XamlCPP::Interfaces::INameScope>();
    }
    else if (_nameScope.expired())
    {
        Control* parent = Parent;
        while (parent)
        {
            if (parent->Is<XamlCPP::Interfaces::INameScope>())
            {
                _nameScope = parent->shared_from_base_dynamic<XamlCPP::Interfaces::INameScope>();
                break;
            }

            if (!parent->_nameScope.expired())
            {
                _nameScope = parent->_nameScope;
                break;
            }

            parent = parent->Parent;
        }

        //if (_nameScope.expired())
        //{
        //    Visual* visualParent = Parent;
        //    while (visualParent)
        //    {
        //        if (visualParent->Is<XamlCPP::Interfaces::INameScope>())
        //        {
        //            _nameScope = visualParent->shared_from_base_dynamic<XamlCPP::Interfaces::INameScope>();
        //            break;
        //        }
        //
        //        visualParent = visualParent->VisualParent;
        //    }
        //}
    }

    return _nameScope.lock();
}

void XUI::UI::Controls::Control::LogicalChildrenCollectionChanged(void * sender, System::Collection::NotifyCollectionChangedEventArgs & e)
{
    LogicalChildrens& list = *(LogicalChildrens*)sender;
    switch (e.Action)
    {
        case System::Collection::NotifyCollectionChangedAction::Add:
            SetLogicalParent(list[e.NewStartingIndex].get());
            break;
    
        case System::Collection::NotifyCollectionChangedAction::Remove:
            ClearLogicalParent(list[e.OldStartingIndex].get());
            break;
    
        case System::Collection::NotifyCollectionChangedAction::Replace:
        case System::Collection::NotifyCollectionChangedAction::Reset:
            throw System::NotSupportedException("Reset should not be signalled on LogicalChildren collection");
    }
}

void Control::ResourceCollectionChanged(void * sender, System::Collection::NotifyCollectionChangedEventArgs & e)
{
	switch (e.Action)
	{
		case System::Collection::NotifyCollectionChangedAction::Add:
		{
			if (ResourcesChanged)
			{
				XamlCPP::Interfaces::ResourceChangedEventArgs res_e;
				res_e.ResourceKey = e.NewValue.GetValue<std::string>();
				ResourcesChanged(static_cast<System::Reflection::Object*>(this), res_e);
			}
			break;
		}

		case System::Collection::NotifyCollectionChangedAction::Reset:
		case System::Collection::NotifyCollectionChangedAction::Remove:
		{
			if (ResourcesChanged)
			{
				Core::UIDispatcher::OnRender += [self = weak_from_base_static<Controls::Control>(), resourceKey = e.NewValue.GetValue<std::string>()]()
				{
					auto control = self.lock();
					if (control)
					{
						XamlCPP::Interfaces::ResourceChangedEventArgs res_e;
						res_e.ResourceKey = resourceKey;
						res_e.Removed = true;
						control->ResourcesChanged(static_cast<System::Reflection::Object*>(control.get()), res_e);
					}
				};
			}
			break;
		}
		case System::Collection::NotifyCollectionChangedAction::Replace:
			break;
	}
}

void XUI::UI::Controls::Control::SetLogicalParent(Control * children)
{
    if (children && children->Parent == nullptr)
        children->SetParent(this);
}

void XUI::UI::Controls::Control::ClearLogicalParent(Control * children)
{
    if (children && children->Parent == this)
        children->SetParent(nullptr);
}

std::shared_ptr<XamlCPP::Interfaces::IStyle> const & XUI::UI::Controls::Control::GetStyle() const
{
    return _style;
}

void XUI::UI::Controls::Control::SetStyle(std::shared_ptr<XamlCPP::Interfaces::IStyle> const & value)
{
    if (_style)
        _style->Detach(static_cast<XamlCPP::Interfaces::IStyleable*>(this));

    //System::ThrowIfFailed<System::NotSupportedException>(value->Static_As<XUI::Styling::Style>()->TargetType != nullptr);
    SetAndRaise(StyleProperty, _style, value);

    if (_initCount == 0)
        ApplyStyling();
}

std::shared_ptr<XamlCPP::Interfaces::IStyle> const & XUI::UI::Controls::Control::GetFocusVisualStyle() const
{
    return _focusVisualStyle;
}

void XUI::UI::Controls::Control::SetFocusVisualStyle(std::shared_ptr<XamlCPP::Interfaces::IStyle> const & value)
{
    if (_focusVisualStyle)
        _focusVisualStyle->Detach(static_cast<XamlCPP::Interfaces::IStyleable*>(this));

    //System::ThrowIfFailed<System::NotSupportedException>(value->Static_As<XUI::Styling::Style>()->TargetType != nullptr);
    SetAndRaise(FocusVisualStyleProperty, _focusVisualStyle, value);
}

void XUI::UI::Controls::Control::BringIntoView(Core::Media::Rect const & rect)
{
    RequestBringIntoViewEventArgs e;
    e.RoutedEvent = UI::Controls::Control::RequestBringIntoViewEvent.get();
    e.TargetObject = this;
    e.TargetRect = rect;
    RaiseEvent(e);
}

/// <summary>
/// Called when the control's template is applied.
/// </summary>
/// <param name="e">The event args.</param>
void XUI::UI::Controls::Control::OnTemplateApplied(TemplateAppliedEventArgs const & e)
{
    RaiseEvent(e);
}

/// <summary>
/// Called when the <see cref="Template"/> property changes.
/// </summary>
/// <param name="e">The event args.</param>
void XUI::UI::Controls::Control::OnTemplateChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
{
    InvalidateMeasure();
}

void CollectorLifeCycleListener::OnEnd(System::Reflection::Object * instance)
{
    if (m_collection && instance)
    {
        auto control = instance->Static_As<Controls::Control>();
        if (control)
            m_collection->push_back(control->shared_from_base_static<Controls::Control>());
    }
}

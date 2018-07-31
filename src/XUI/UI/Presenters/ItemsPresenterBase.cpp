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

#include "ItemsPresenterBase.h"
#include "Core/Media/MediaTypes.h"
#include "Core/Media/Colors.h"
#include "Core/Media/Pen.h"
#include "UI/Controls/ItemsControl.h"
#include "UI/Panel.h"
#include "UI/StackPanel.h"
#include "UI/Controls/ItemsControl.h"

#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;
using namespace XUI::UI::Presenters;

STATIC_CLASS_INITIALIZER(ItemsPresenterBase, XUI::UI::Presenters::ItemsPresenterBase::StaticClassInitializer)

Core::Dependency::RefDependencyProperty Presenters::ItemsPresenterBase::ItemsProperty;
Core::Dependency::RefDependencyProperty Presenters::ItemsPresenterBase::ItemsPanelProperty;
Core::Dependency::RefDependencyProperty Presenters::ItemsPresenterBase::ItemTemplateProperty;
Core::Dependency::RefDependencyProperty Presenters::ItemsPresenterBase::MemberSelectorProperty;

std::shared_ptr<XamlCPP::Interfaces::ITemplate> DefaultPanel = std::make_shared<DataTemplates::FuncTemplate<std::shared_ptr<XUI::UI::Panel>>>([]()
{
    auto panel = std::make_shared<UI::StackPanel>();
    return panel;
});

void XUI::UI::Presenters::ItemsPresenterBase::StaticClassInitializer()
{
    ItemsProperty = (Core::Dependency::DirectProperty<Presenters::ItemsPresenterBase, System::Collection::Generic::IContainer*>::RegisterDirect(nameof(Items),
        [](XUI::UI::Presenters::ItemsPresenterBase* x) { return x->Items; },
        [](XUI::UI::Presenters::ItemsPresenterBase* x, System::Collection::Generic::IContainer * v) { x->Items = v; })
        );

    ItemsPanelProperty = (Core::Dependency::BindProperty<std::shared_ptr<XamlCPP::Interfaces::ITemplate>>::Register<Presenters::ItemsPresenterBase>(nameof(ItemsPanel), DefaultPanel));
    ItemTemplateProperty = (Core::Dependency::BindProperty<std::shared_ptr<DataTemplates::DataTemplate>>::Register<Presenters::ItemsPresenterBase>(nameof(ItemTemplate)));
    MemberSelectorProperty = (Core::Dependency::BindProperty<System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)>>::Register<Presenters::ItemsPresenterBase>(nameof(MemberSelector)));

    TemplatedParentProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e)
    {
        ItemsPresenterBase* control = e.Sender->Static_As<ItemsPresenterBase>();
        if (control)
            control->TemplatedParentChanged((XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e);
    });
}

ItemsPresenterBase::ItemsPresenterBase()
{

}

System::Collection::Generic::IContainer* ItemsPresenterBase::GetItems() const
{
    return _items;
}

void ItemsPresenterBase::SetItems(System::Collection::Generic::IContainer* value)
{
    if (_createdPanel)
    {
        if (_items != nullptr)
        {
            _items->CollectionChanged -= System::make_eventhandler(&ItemsPresenterBase::ItemsCollectionChanged, this);
        }
    }

    if (_createdPanel && value != nullptr)
    {
        value->CollectionChanged += System::make_eventhandler(&ItemsPresenterBase::ItemsCollectionChanged, shared_from_base_static<ItemsPresenterBase>());
    }

    SetAndRaise(ItemsProperty, _items, value);

    if (_createdPanel)
    {
        System::Collection::NotifyCollectionChangedEventArgs e;
        e.Action = System::Collection::NotifyCollectionChangedAction::Init;
        ItemsChanged(e);
    }
}

std::shared_ptr<Generators::IItemContainerGenerator> const& ItemsPresenterBase::GetItemContainerGenerator() const
{
    if (_generator == nullptr)
        _generator = CreateItemContainerGenerator();

    return _generator;
}

void ItemsPresenterBase::SetItemContainerGenerator(std::shared_ptr<Generators::IItemContainerGenerator> const & value)
{
    if (_generator != nullptr)
        throw System::InvalidOperationException("ItemContainerGenerator already created.");

    _generator = value;
}

std::shared_ptr<XamlCPP::Interfaces::ITemplate> ItemsPresenterBase::GetItemsPanel() const
{
    return GetValue<std::shared_ptr<XamlCPP::Interfaces::ITemplate>>(ItemsPanelProperty);
}

void ItemsPresenterBase::SetItemsPanel(std::shared_ptr<XamlCPP::Interfaces::ITemplate> const& value)
{
    SetValue(ItemsPanelProperty, value);
}

std::shared_ptr<DataTemplates::DataTemplate> ItemsPresenterBase::GetItemTemplate() const
{
    return GetValue<std::shared_ptr<DataTemplates::DataTemplate>>(ItemTemplateProperty);
}

void ItemsPresenterBase::SetItemTemplate(std::shared_ptr<DataTemplates::DataTemplate> const& value)
{
    SetValue(ItemTemplateProperty, value);
}

System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)> ItemsPresenterBase::GetMemberSelector() const
{
    return GetValue<System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)>>(MemberSelectorProperty);
}

void ItemsPresenterBase::SetMemberSelector(System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)> const& value)
{
    SetValue(MemberSelectorProperty, value);
}

std::shared_ptr<UI::Panel> ItemsPresenterBase::GetPanel() const
{
    return m_Panel;
}

void ItemsPresenterBase::SetPanel(std::shared_ptr<UI::Panel> const& value)
{
    m_Panel = value;
}

void ItemsPresenterBase::ApplyTemplate()
{
    Controls::Control::ApplyTemplate();

    if (!_createdPanel)
    {
        if (IsInitialized)
            CreatePanel();
        else
            Initialized += System::make_eventhandler([this](void*, System::Events::EventArg& e)
                            {
                                if (!_createdPanel)
                                    CreatePanel();
                            });
    }
}

void ItemsPresenterBase::ScrollIntoView(System::Reflection::Variant const& item)
{
}

std::shared_ptr<Generators::IItemContainerGenerator> ItemsPresenterBase::CreateItemContainerGenerator() const
{
    auto i = TemplatedParent->Dynamic_As<Controls::ItemsControl>();
    auto result = i ? i->GetItemContainerGenerator() : nullptr;

    if (result == nullptr)
    {
        result = std::make_shared<Generators::ItemContainerGeneratorGeneric>(const_cast<ItemsPresenterBase*>(this));
        result->ItemTemplate = ItemTemplate;
    }

    return result;
}

Core::Media::Size ItemsPresenterBase::MeasureOverride(Core::Media::Size const& availableSize)
{
    Panel->Measure(availableSize);
    return Panel->DesiredSize;
}

Core::Media::Size ItemsPresenterBase::ArrangeOverride(Core::Media::Size const& finalSize)
{
    Panel->Arrange(Core::Media::Rect::xywh(0, 0, finalSize.Width, finalSize.Height));
    return finalSize;
}

void ItemsPresenterBase::PanelCreated(UI::Panel* panel)
{
}

void XUI::UI::Presenters::ItemsPresenterBase::OnAttachedToLogicalTree(Controls::LogicalTreeAttachmentEventArgs & e)
{
    Controls::Control::OnAttachedToLogicalTree(e);
}

void XUI::UI::Presenters::ItemsPresenterBase::OnDetachedFromLogicalTree(Controls::LogicalTreeAttachmentEventArgs & e)
{
    Controls::Control::OnDetachedFromLogicalTree(e);
}

void ItemsPresenterBase::CreatePanel()
{
    auto control = ItemsPanel->Build(nullptr)->Static_As<Controls::Control>()->shared_from_base_static<Controls::Control>();
    Panel = control->shared_from_base_static<UI::Panel>();
    if (Panel == nullptr)
        throw System::AggregateException("");

    Panel->SetValue(TemplatedParentProperty, TemplatedParent);

    //for (auto const& child : LogicalChildren.GetNativeEnumerator())
    //{
    //    RemoveLogicalChild(child.get());
    //}
    //for (auto const& child : VisualChildren.GetNativeEnumerator())
    //{
    //    RemoveVisualChild(child.get());
    //}

    LogicalChildren.Add(Panel);
    VisualChildren.Add(Panel);

    _createdPanel = true;

    if (Items != nullptr)
        Items->CollectionChanged += System::make_eventhandler(&ItemsPresenterBase::ItemsCollectionChanged, shared_from_base_static<ItemsPresenterBase>());

    PanelCreated(Panel.get());

    System::Collection::NotifyCollectionChangedEventArgs e;
    e.Action = System::Collection::NotifyCollectionChangedAction::Init;
    ItemsChanged(e);
}

void ItemsPresenterBase::ItemsCollectionChanged(void *sender, System::Collection::NotifyCollectionChangedEventArgs& e)
{
    if (_createdPanel)
    {
        ItemsChanged(e);
    }
}

void ItemsPresenterBase::TemplatedParentChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    XUI::UI::Presenters::ItemsPresenterBase* contentPresenter = e.Sender->As<XUI::UI::Presenters::ItemsPresenterBase>();
    std::shared_ptr<Core::Dependency::DependencyObject> parent = e.NewValue.TryGetValue<std::shared_ptr<Core::Dependency::DependencyObject>>();
    if (contentPresenter && parent)
    {
        XUI::UI::Controls::ItemsControl* contentControl = parent->Dynamic_As<XUI::UI::Controls::ItemsControl>();
        if (contentControl)
            contentControl->RegisterItemsPresenter(contentPresenter);
    }
    else
    {
        std::shared_ptr<Core::Dependency::DependencyObject> parent = e.OldValue.TryGetValue<std::shared_ptr<Core::Dependency::DependencyObject>>();
        if (contentPresenter && parent)
        {
            XUI::UI::Controls::ItemsControl* contentControl = parent->As<XUI::UI::Controls::ItemsControl>();
            if (contentControl)
                contentControl->RegisterItemsPresenter(nullptr);
        }
    }

    //(dynamic_cast<IItemsPresenterHost*>(e.NewValue)) == nullptr ? nullptr : (dynamic_cast<IItemsPresenterHost*>(e.NewValue)).RegisterItemsPresenter(this);
}

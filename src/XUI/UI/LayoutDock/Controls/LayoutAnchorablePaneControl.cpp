#include "StdAfx.h"
#include "LayoutAnchorablePaneControl.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XamlCPP::Core::Dependency;
using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_POST_INITIALIZER(LayoutAnchorablePaneControl, XUI::UI::LayoutAnchorablePaneControl::StaticClassInitializer)

void XUI::UI::LayoutAnchorablePaneControl::StaticClassInitializer()
{
    SelectionModeProperty->OverrideDefaultValue<LayoutAnchorablePaneControl>(SelectionMode::AlwaysSelected);
    FocusableProperty->OverrideDefaultValue<LayoutAnchorablePaneControl>(false);

    SelectorContentProperty->OverrideDefaultValue<LayoutAnchorablePaneControl>(System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)>(
        [](System::Reflection::Variant const& layout) -> System::Reflection::Variant
    {
        if (layout.IsObject())
        {
            auto content = layout.ToObject()->Static_As<LayoutContent>();
            if (content)
                return content->Content;
        }
        return layout;
    }));

    SelectedIndexProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e)
    {
        LayoutAnchorablePaneControl* tabcontrol = e.Sender->As<LayoutAnchorablePaneControl>();
        if (tabcontrol)
        {
            if (tabcontrol->Model && tabcontrol->Model->SelectedContent())
                tabcontrol->Model->SelectedContent()->IsActive = true;
        }
    });
}

XUI::UI::LayoutAnchorablePaneControl::LayoutAnchorablePaneControl()
{
}

XUI::UI::LayoutAnchorablePaneControl::LayoutAnchorablePaneControl(std::shared_ptr<LayoutAnchorablePane> const & model)
{
    Model = model;
}

void XUI::UI::LayoutAnchorablePaneControl::Initialize()
{
    Items->Clear();
    for (auto itr = Model->Children.GetNativeEnumerator().begin(); itr != Model->Children.GetNativeEnumerator().end(); ++itr)
        Items->Add(*itr);

    Model->Children.CollectionChanged += System::make_eventhandler(&LayoutAnchorablePaneControl::OnModelItemsChanged, this);

    //SetBinding(ItemsSourceProperty, new Binding("Model.Children"){ Source = this });
    //SetBinding(FlowDirectionProperty, new Binding("Model.Root.Manager.FlowDirection"){ Source = this });

    LayoutUpdated += System::make_eventhandler(&LayoutAnchorablePaneControl::OnLayoutUpdated, this);
}

XUI::UI::LayoutAnchorablePaneControl::~LayoutAnchorablePaneControl()
{
}

void XUI::UI::LayoutAnchorablePaneControl::OnLayoutUpdated(void * sender, System::Events::EventArg & e)
{
    auto modelWithAtcualSize = Model->Static_As<LayoutAnchorablePane>();
    modelWithAtcualSize->ActualWidth = ActualWidth();
    modelWithAtcualSize->ActualHeight = ActualHeight();
}

void XUI::UI::LayoutAnchorablePaneControl::OnModelItemsChanged(void * sender, System::Collection::NotifyCollectionChangedEventArgs & e)
{
    switch (e.Action)
    {
        case System::Collection::NotifyCollectionChangedAction::Add:
        {
            auto item = Model->Children[e.NewStartingIndex];
            Items->Insert(e.NewStartingIndex, item);
            break;
        }
        case System::Collection::NotifyCollectionChangedAction::Remove:
        {
            auto item = Model->Children[e.OldStartingIndex];
            Items->Remove(item);
            break;
        }
        case System::Collection::NotifyCollectionChangedAction::Reset:
        {
            Items->Clear();
            break;
        }
        case System::Collection::NotifyCollectionChangedAction::AfterRemove:
            break;
        case System::Collection::NotifyCollectionChangedAction::Replace:
        {
            Items->Replace(e.OldStartingIndex, e.NewStartingIndex);
            //Presenter->Panel->InvalidateArrange();
            break;
        }
        default:
        {
            throw System::NotSupportedException("Invalid model event");
        }
    }
}

void XUI::UI::LayoutAnchorablePaneControl::OnTemplateApplied(Controls::TemplateAppliedEventArgs const & e)
{
    TabControl::OnTemplateApplied(e);

    if (!m_TabStrip.expired() && m_TabStrip.lock()->Is<AnchorablePaneTabPanel>())
        _tabItemHost = m_TabStrip.lock()->shared_from_base_static<AnchorablePaneTabPanel>();
    else
        _tabItemHost.reset();
}

void XUI::UI::LayoutAnchorablePaneControl::OnGotFocus(Interfaces::GotFocusEventArgs & e)
{
    if (Model && Model->SelectedContent())
        Model->SelectedContent()->IsActive = true;

    UI::TabControl::OnGotFocus(e);
}

void XUI::UI::LayoutAnchorablePaneControl::OnPointerPressed(Interfaces::PointerPressedEventArgs & e)
{
    UI::TabControl::OnPointerPressed(e);

    if (!e.Handled && Model && Model->SelectedContent())
        Model->SelectedContent()->IsActive = true;
}

void XUI::UI::LayoutAnchorablePaneControl::UpdateSelectedTab()
{
    UI::TabControl::UpdateSelectedTab();
}

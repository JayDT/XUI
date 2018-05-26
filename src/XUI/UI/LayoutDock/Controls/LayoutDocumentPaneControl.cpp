#include "StdAfx.h"
#include "LayoutDocumentPaneControl.h"
#include "DocumentPaneTabPanel.h"
#include "LayoutDocumentTabItem.h"
#include "LayoutContent.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XamlCPP::Core::Dependency;
using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_POST_INITIALIZER(LayoutDocumentPaneControl, XUI::UI::LayoutDocumentPaneControl::StaticClassInitializer)

void XUI::UI::LayoutDocumentPaneControl::StaticClassInitializer()
{
    SelectorContentProperty->OverrideDefaultValue<LayoutDocumentPaneControl>(System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)>(
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

    SelectionModeProperty->OverrideDefaultValue<LayoutDocumentPaneControl>(SelectionMode::AlwaysSelected);
    FocusableProperty->OverrideDefaultValue<LayoutDocumentPaneControl>(false);

    SelectedIndexProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e)
    {
        LayoutDocumentPaneControl* tabcontrol = e.Sender->As<LayoutDocumentPaneControl>();
        if (tabcontrol)
        {
            if (tabcontrol->Model && tabcontrol->Model->GetSelectedContent())
                tabcontrol->Model->GetSelectedContent()->IsActive = true;
        }
    });
}

XUI::UI::LayoutDocumentPaneControl::LayoutDocumentPaneControl()
{
}

XUI::UI::LayoutDocumentPaneControl::LayoutDocumentPaneControl(std::shared_ptr<LayoutDocumentPane> const & model)
{
    Model = model;
}

XUI::UI::LayoutDocumentPaneControl::~LayoutDocumentPaneControl()
{
}

void XUI::UI::LayoutDocumentPaneControl::Initialize()
{
    Items->Clear();
    for (auto itr = Model->Children.GetNativeEnumerator().begin(); itr != Model->Children.GetNativeEnumerator().end(); ++itr)
        Items->Add(*itr);

    Model->Children.CollectionChanged += System::make_eventhandler(&LayoutDocumentPaneControl::OnModelItemsChanged, this);

    //SetBinding(ItemsSourceProperty, new Binding("Model.Children"){ Source = this });
    //SetBinding(FlowDirectionProperty, new Binding("Model.Root.Manager.FlowDirection"){ Source = this });

    LayoutUpdated += System::make_eventhandler(&LayoutDocumentPaneControl::OnLayoutUpdated, this);
}

void XUI::UI::LayoutDocumentPaneControl::OnLayoutUpdated(void * sender, System::Events::EventArg & e)
{
    auto modelWithAtcualSize = Model->Static_As<LayoutDocumentPane>();
    modelWithAtcualSize->ActualWidth = ActualWidth();
    modelWithAtcualSize->ActualHeight = ActualHeight();
}

void LayoutDocumentPaneControl::OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e)
{
    TabControl::OnTemplateApplied(e);

    if (!m_TabStrip.expired() && m_TabStrip.lock()->Is<DocumentPaneTabPanel>())
        _tabItemHost = m_TabStrip.lock()->shared_from_base_static<DocumentPaneTabPanel>();
    else
        _tabItemHost.reset();
}

void XUI::UI::LayoutDocumentPaneControl::OnModelItemsChanged(void * sender, System::Collection::NotifyCollectionChangedEventArgs & e)
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
        default:
        {
            throw System::NotSupportedException("Invalid model event");
        }
    }
}

void XUI::UI::LayoutDocumentPaneControl::OnGotFocus(Interfaces::GotFocusEventArgs & e)
{
}

void XUI::UI::LayoutDocumentPaneControl::OnPointerPressed(Interfaces::PointerPressedEventArgs & e)
{
    UI::TabControl::OnPointerPressed(e);

    if (!e.Handled && Model && Model->GetSelectedContent())
        Model->GetSelectedContent()->IsActive = true;
}

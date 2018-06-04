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

#include "TabControl.h"
#include "UI/Detail/HeaderedContentControl.h"
#include "UI/Controls/ContentControl.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI::UI;
using namespace XUI::UI::Controls;

STATIC_CLASS_INITIALIZER(TabControl, XUI::UI::TabControl::StaticClassInitializer)

XUI::Core::Dependency::RefDependencyProperty TabControl::TransitionProperty;
XUI::Core::Dependency::RefDependencyProperty TabControl::TabStripPlacementProperty;
XUI::Core::Dependency::RefDependencyProperty TabControl::SelectedContentProperty;
XUI::Core::Dependency::RefDependencyProperty TabControl::SelectorContentProperty;

std::function<System::Reflection::Variant(System::Reflection::Variant const&)> TabControl::ContentSelector = TabControl::SelectContent;
std::function<System::Reflection::Variant(System::Reflection::Variant const&)> TabControl::HeaderSelector = TabControl::SelectHeader;

void TabControl::StaticClassInitializer()
{
    //TransitionProperty = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::IPageTransition>>::Register<TabControl>(nameof(Transition)));
    TabStripPlacementProperty = (Core::Dependency::BindProperty<UI::Dock>::Register<TabControl>(nameof(TabStripPlacement)));
    SelectedContentProperty = (Core::Dependency::DirectProperty<TabControl, System::Reflection::Variant>::RegisterDirect(nameof(SelectedContent),
        [](TabControl* x) { return x->SelectedContent; },
        [](TabControl* x, System::Reflection::Variant const& v) { x->SelectedContent = v; })
    );
    SelectorContentProperty = (Core::Dependency::BindProperty<System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)>>::Register<TabControl>(nameof(SelectorContent), System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)>(TabControl::SelectContent)));

    SelectionModeProperty->OverrideDefaultValue<TabControl>(SelectionMode::AlwaysSelected);
    FocusableProperty->OverrideDefaultValue<TabControl>(false);
    TabStripPlacementProperty->Changed.get_observable().subscribe(Visual::AffectsMeasureInvalidate);

    SelectedItemProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e)
    {
        XUI::UI::TabControl* tabcontrol = e.Sender->As<XUI::UI::TabControl>();
        if (tabcontrol)
            tabcontrol->UpdateSelectedTab();
    });
}

XUI::UI::TabControl::TabControl()
{
    Items = std::make_shared<System::Collection::List<System::Reflection::RefObject>>();
}

XUI::UI::TabControl::~TabControl()
{
}

Controls::Control *TabControl::GetPages()
{
    return m_Pages.lock().get();
}

void TabControl::SetPages(Controls::Control *value)
{
    m_Pages = value->weak_from_base_static<Controls::Control>();
}

Controls::Control *TabControl::GetTabStrip()
{
    return m_TabStrip.lock().get();
}

void TabControl::SetTabStrip(Controls::Control *value)
{
    m_TabStrip = value->weak_from_base_static<Controls::Control>();
}

System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)> XUI::UI::TabControl::GetSelectorContent()
{
    return GetValue<System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)>>(SelectorContentProperty);
}

void XUI::UI::TabControl::SetSelectorContent(System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)> const & value)
{
    SetValue(SelectorContentProperty, value);
}

//IPageTransition *TabControl::GetTransition()
//{
//    return GetValue(TransitionProperty);
//}
//
//void TabControl::SetTransition(IPageTransition *value)
//{
//    SetValue(TransitionProperty, value);
//}

Dock TabControl::GetTabStripPlacement()
{
    return GetValue<Dock>(TabStripPlacementProperty);
}

void TabControl::SetTabStripPlacement(Dock value)
{
    SetValue(TabStripPlacementProperty, value);
}

void XUI::UI::TabControl::EndInit()
{
    XUI::UI::SelectingItemsControl::EndInit();

    if (_initCount == 0)
        UpdateSelectedTab();
}

//IItemContainerGenerator *TabControl::CreateItemContainerGenerator()
//{
//    // TabControl doesn't actually create items - instead its TabStrip and Carousel
//    // children create the items. However we want it to be a SelectingItemsControl
//    // so that it has the Items/SelectedItem etc properties. In this case, we can
//    // return a null ItemContainerGenerator to disable the creation of item containers.
//    return nullptr;
//}

void TabControl::OnTemplateApplied(TemplateAppliedEventArgs const& e)
{
    SelectingItemsControl::OnTemplateApplied(e);

    if (Template)
    {
        auto _border = Template->FindName("tcBorder", this);
        auto _tabstrip = Template->FindName("PART_TabStrip", this);
        auto _pages = Template->FindName("PART_Content", this);

        if (_tabstrip)
            TabStrip = _tabstrip->Static_As<Controls::Control>();

        if (_pages)
            Pages = _pages->Static_As<Controls::Control>();

        UpdateSelectedTab();
    }
}

System::Reflection::Variant TabControl::SelectContent(System::Reflection::Variant const& o)
{
    if (o.GetType()->IsAssignableFrom<Controls::ContentControl>())
    {
        auto content = o.ToObject()->Static_As<Controls::ContentControl>();
        if (!content)
            return o;
        return content->Content;
    }
    else
    {
        return o;
    }
}

System::Reflection::Variant TabControl::SelectHeader(System::Reflection::Variant const& o)
{
    if (o.GetType()->IsAssignableFrom<UI::HeaderedContentControl>())
    {
        auto headered = o.TryGetValue<UI::HeaderedContentControl*>();
        if (!headered)
            return std::string("");
        return headered->Header;
    }
    else if (!o.GetType()->IsAssignableFrom<Controls::Control>())
    {
        // Non-headered control items should result in TabStripItems with empty content.
        // If a TabStrip is created with non IHeadered controls as its items, don't try to
        // display the control in the TabStripItem: the content portion will also try to 
        // display this control, resulting in dual-parentage breakage.
        return std::string("");
    }
    else
    {
        return o;
    }
}

void XUI::UI::TabControl::UpdateSelectedTab()
{
    if (!Items || Items->empty() || m_Pages.expired())
        return;

    int index = SelectedIndex;
    if (index == -1)
        return;

    System::Reflection::Variant item = Items->ElementAt(index);
    auto content = item;
    if (SelectorContent.GetDelegate())
        content = SelectorContent.GetDelegate()(item);

    System::Reflection::Object* obj = content.ToObject();
    if (obj)
    {
        auto oldControl = m_SelectedContentHost.lock();
        Controls::Control* control = obj->Static_As<Controls::Control>();
        if (control && oldControl.get() != control)
        {
            auto contentControl = m_Pages.lock();
            if (contentControl && contentControl->Is<Presenters::ContentPresenter>())
            {
                Presenters::ContentPresenter* presenter = static_cast<Presenters::ContentPresenter*>(contentControl.get());
                m_SelectedContentHost = control->shared_from_base_static<Controls::Control>();
                if (!m_SelectedContentHost.expired())
                    m_SelectedContentHost.lock()->IsEnabledCore = true;

                if (oldControl)
                    oldControl->_isMovingInLogicalTemplate = true;
                control->_isMovingInLogicalTemplate = true;
                SelectedContent = content;
                if (oldControl)
                    oldControl->_isMovingInLogicalTemplate = false;
                control->_isMovingInLogicalTemplate = false;

                //NotifyPropertyChanged::RaisePropertyChanged("SelectedContent");
                //presenter->Content = content;

                if (oldControl)
                {
                    oldControl->IsEnabledCore = false;
                    //if (oldControl->Parent)
                    //{
                    //    if (oldControl->Parent->GetType()->IsAssignableFrom<UI::HeaderedContentControl>())
                    //    {
                    //        auto headered = oldControl->Parent->Static_As<UI::HeaderedContentControl>();
                    //        headered->IsSelected = false;
                    //    }
                    //}
                }
                //if (item.GetType()->IsAssignableFrom<UI::HeaderedContentControl>())
                //{
                //    auto headered = item.TryGetValue<UI::HeaderedContentControl*>();
                //    headered->IsSelected = true;
                //}
            }
        }
    }

}

void XUI::UI::TabControl::LostSelection()
{
    SelectingItemsControl::LostSelection();
    if (TabStrip)
    {
        SelectingItemsControl* tabPanel = TabStrip->Static_As<SelectingItemsControl>();
        if (tabPanel)
        {
            tabPanel->SelectionMode = SelectionMode;
            tabPanel->LostSelection();
        }
    }
}

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

#include "HeaderedItemsControl.h"
#include "Platform/UIService.h"
#include "XamlCPP/Core/LogInterface.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::Interfaces;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(HeaderedItemsControl, HeaderedItemsControl::StaticClassInitializer)

XUI::Core::Dependency::RefDependencyProperty HeaderedItemsControl::HeaderProperty;

void HeaderedItemsControl::StaticClassInitializer()
{
    HeaderProperty = (Core::Dependency::BindProperty<System::Reflection::Variant>::Register<HeaderedItemsControl>(nameof(Header)));

    // Header Template
    ItemTemplateProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        HeaderedItemsControl* control = e.Sender->As<HeaderedItemsControl>();
        if (control)
        {
            control->RemoveDataTemplate();
            control->ApplyDataTemplate();
        }
    });
}

System::Reflection::Variant HeaderedItemsControl::GetHeader() const
{
    return GetValue(HeaderProperty.get());
}

void HeaderedItemsControl::SetHeader(System::Reflection::Variant value)
{
    SetValue(HeaderProperty, value);
}

Presenters::ContentPresenter *HeaderedItemsControl::GetHeaderPresenter() const
{
    return m_HeaderPresenter.get();
}

void HeaderedItemsControl::SetHeaderPresenter(Presenters::ContentPresenter *value)
{
    if (value)
        m_HeaderPresenter = value->shared_from_base_static<Presenters::ContentPresenter>();
}

//void HeaderedItemsControl::ApplyDataTemplate()
//{
//    if (ItemTemplate && !m_headerDataTemplateRoot && HeaderPresenter)
//    {
//        RemoveDataTemplate();
//        XamlCPP::Platform::ILog::Verbose(XamlCPP::Platform::LogArea::Control, nullptr, "Creating data template");
//        m_headerDataTemplateRoot = ItemTemplate->Build()->As<Core::Dependency::DependencyObject>()->shared_from_base_static<Core::Dependency::DependencyObject>();
//        if (m_headerDataTemplateRoot)
//        {
//            Control* dataControl = m_headerDataTemplateRoot->As<XUI::UI::Controls::Control>();
//            if (dataControl)
//            {
//                HeaderPresenter->_dataTemplate = ItemTemplate; // Set affected data template
//                HeaderPresenter->Content = dataControl->shared_from_base_static<XUI::UI::Controls::Control>();
//                dataControl->TemplatedParent = shared_from_base_static<Core::Dependency::DependencyObject>();
//
//                ItemTemplate->Attach(this, m_headerDataTemplateRoot.get());
//            }
//        }
//    }
//}
//
//void HeaderedItemsControl::RemoveDataTemplate()
//{
//    if (ItemTemplate)
//        ItemTemplate->Detach(this);
//
//    if (m_headerDataTemplateRoot)
//    {
//        Control* dataControl = m_headerDataTemplateRoot->As<XUI::UI::Controls::Control>();
//        if (dataControl)
//        {
//            dataControl->TemplatedParent = nullptr;
//
//            HeaderPresenter->RemoveLogicalChild(dataControl);
//            HeaderPresenter->RemoveVisualChild(dataControl);
//        }
//
//        m_headerDataTemplateRoot = nullptr;
//    }
//}
//
//std::shared_ptr<XUI::UI::DataTemplates::DataTemplate> XUI::UI::HeaderedItemsControl::FindDataTemplate(System::Reflection::meta::Variant const & data)
//{
//    //if (HeaderTemplate)
//    //    return HeaderTemplate;
//    if (ItemTemplate)
//        return ItemTemplate;
//    return Control::FindDataTemplate(data);
//}

void HeaderedItemsControl::OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e)
{
    auto presenter = Template->FindName("PART_HeaderPresenter", this);
    if (presenter)
        HeaderPresenter = presenter->Dynamic_As<Presenters::ContentPresenter>();

    ItemsControl::OnTemplateApplied(e);
}


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

#include "ContentControl.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI::Controls;

STATIC_CLASS_INITIALIZER(ContentControl, ContentControl::StaticClassInitializer)

Core::Dependency::RefDependencyProperty     ContentControl::ContentProperty;
Core::Dependency::RefDependencyProperty     ContentControl::ContentTemplateProperty;
Core::Dependency::RefDependencyProperty     ContentControl::HorizontalContentAlignmentProperty;
Core::Dependency::RefDependencyProperty     ContentControl::VerticalContentAlignmentProperty;

auto ContentChanged = [](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
{
    //ContentControl* control = e.Sender->As<ContentControl>();
    //if (control)
    //{
    //}
};

/// <summary>
/// Initializes static members of the <see cref="ContentControl"/> class.
/// </summary>
void XUI::UI::Controls::ContentControl::StaticClassInitializer()
{
    ContentProperty = (Core::Dependency::BindProperty<System::Reflection::meta::Variant>::Register<XUI::UI::Controls::ContentControl>(nameof(Content)));
    ContentTemplateProperty = (Core::Dependency::BindProperty<std::shared_ptr<UI::DataTemplates::DataTemplate>>::Register<XUI::UI::Controls::ContentControl>(nameof(ContentTemplate)));
    HorizontalContentAlignmentProperty = (Core::Dependency::BindProperty<XUI::UI::Controls::HorizontalAlignment>::Register<XUI::UI::Controls::ContentControl>(nameof(HorizontalContentAlignment)));
    VerticalContentAlignmentProperty = (Core::Dependency::BindProperty<XUI::UI::Controls::VerticalAlignment>::Register<XUI::UI::Controls::ContentControl>(nameof(VerticalContentAlignment)));

    //ContentProperty->Changed.get_observable().subscribe(ContentChanged);
    //ContentTemplateProperty->Changed.get_observable().subscribe(ContentChanged);
}

XUI::UI::Controls::ContentControl::~ContentControl()
{
}

System::Reflection::meta::Variant XUI::UI::Controls::ContentControl::GetContent() const
{
    return std::move(GetValue(ContentProperty.get()));
}

void XUI::UI::Controls::ContentControl::SetContent(System::Reflection::meta::Variant const& value)
{
    System::Reflection::meta::Variant pValue = GetContent();
    if (pValue == value)
        return;

    // Workaround for xaml loader because subcontentcontrols not apply same time template with parent templated control
    if (!m_contentPresenter)
    {
        Control* control = pValue.ToObject() ? pValue.ToObject()->Static_As<Control>() : nullptr;

        if (control)
        {
            if (control->Parent == this)
                RemoveLogicalChild(control);
            if (control->VisualParent == this)
                RemoveVisualChild(control);
        }
    }

    SetValue(ContentProperty.get(), value);

    if (!m_contentPresenter)
    {
        Control* control = value.ToObject() ? value.ToObject()->Static_As<Control>() : nullptr;
        if (control)
        {
            if (!control->Parent)
                AddLogicalChild(control);
            if (!control->VisualParent)
                AddVisualChild(control);
        }
    }
}

std::shared_ptr<UI::DataTemplates::DataTemplate> XUI::UI::Controls::ContentControl::GetContentTemplate() const
{
    return GetValue<std::shared_ptr<UI::DataTemplates::DataTemplate>>(ContentTemplateProperty.get());
}

void XUI::UI::Controls::ContentControl::SetContentTemplate(std::shared_ptr<UI::DataTemplates::DataTemplate> const& value)
{
    SetValue(ContentTemplateProperty.get(), value);
}

XUI::UI::Controls::HorizontalAlignment XUI::UI::Controls::ContentControl::GetHorizontalContentAlignment() const
{
    return GetValue<XUI::UI::Controls::HorizontalAlignment>(HorizontalContentAlignmentProperty.get());
}

void XUI::UI::Controls::ContentControl::SetHorizontalContentAlignment(XUI::UI::Controls::HorizontalAlignment value)
{
    SetValue(HorizontalContentAlignmentProperty.get(), value);
}

XUI::UI::Controls::VerticalAlignment XUI::UI::Controls::ContentControl::GetVerticalContentAlignment() const
{
    return GetValue<XUI::UI::Controls::VerticalAlignment>(VerticalContentAlignmentProperty.get());
}

void XUI::UI::Controls::ContentControl::SetVerticalContentAlignment(XUI::UI::Controls::VerticalAlignment value)
{
    SetValue(VerticalContentAlignmentProperty.get(), value);
}

void XUI::UI::Controls::ContentControl::SetPresenter(Presenters::ContentPresenter * value)
{
    if (m_contentPresenter.get() == value)
        return;

    System::Reflection::meta::Variant pValue = GetContent();
    if (pValue == value)
        return;

    // Workaround for xaml loader because subcontentcontrols not apply same time template with parent templated control
    //Control* control = pValue.ToObject() ? pValue.ToObject()->Static_As<Control>() : nullptr;
    //
    //if (control)
    //{
    //    if (control->Parent == this)
    //        RemoveLogicalChild(control);
    //    if (control->VisualParent == this)
    //        RemoveVisualChild(control);
    //}

    m_contentPresenter = value->shared_from_base_static<Presenters::ContentPresenter>();
}

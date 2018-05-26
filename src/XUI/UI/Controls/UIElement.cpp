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

#include "UIElement.h"
#include "Control.h"
#include "Core/Media/Colors.h"
#include "XamlCPP/Core/LogInterface.h"

#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::Interfaces;
using namespace XUI::Core::Observer;
using namespace XUI::UI::Controls;

STATIC_CLASS_INITIALIZER(UIElement, UIElement::StaticClassInitializer)

Core::Dependency::RefDependencyProperty         XUI::UI::Controls::UIElement::BackgroundProperty;
Core::Dependency::RefDependencyProperty         XUI::UI::Controls::UIElement::BorderBrushProperty;
Core::Dependency::RefDependencyProperty         XUI::UI::Controls::UIElement::BorderThicknessProperty;
Core::Dependency::RefDependencyProperty         XUI::UI::Controls::UIElement::FontFamilyProperty;
Core::Dependency::RefDependencyProperty         XUI::UI::Controls::UIElement::FontSizeProperty;
Core::Dependency::RefDependencyProperty         XUI::UI::Controls::UIElement::FontStyleProperty;
Core::Dependency::RefDependencyProperty         XUI::UI::Controls::UIElement::FontWeightProperty;
Core::Dependency::RefDependencyProperty         XUI::UI::Controls::UIElement::ForegroundProperty;
Core::Dependency::RefDependencyProperty         XUI::UI::Controls::UIElement::PaddingProperty;
Core::Dependency::RefDependencyProperty         XUI::UI::Controls::UIElement::SnapsToDevicePixelsProperty;
Core::Dependency::RefDependencyProperty         XUI::UI::Controls::UIElement::OverridesDefaultStyleProperty;
Core::Dependency::RefDependencyProperty         XUI::UI::Controls::UIElement::TemplatedParentProperty;
std::shared_ptr<Core::Observer::RoutedEvent>    XUI::UI::Controls::UIElement::TemplateAppliedEvent;

void UIElement::StaticClassInitializer()
{
    BackgroundProperty = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::IBrush>>::Register<XUI::UI::Controls::UIElement>(nameof(Background)));
    BorderBrushProperty = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::IBrush>>::Register<XUI::UI::Controls::UIElement>(nameof(BorderBrush)));
    BorderThicknessProperty = (Core::Dependency::BindProperty<double>::Register<XUI::UI::Controls::UIElement>(nameof(BorderThickness)));
    FontFamilyProperty = (Core::Dependency::BindProperty<std::string>::Register<XUI::UI::Controls::UIElement>(nameof(FontFamily), "mono_dejavu_sans_", true));
    FontSizeProperty = (Core::Dependency::BindProperty<double>::Register<XUI::UI::Controls::UIElement>(nameof(FontSize), 10, true));
    FontStyleProperty = (Core::Dependency::BindProperty<Core::Media::FontStyle>::Register<XUI::UI::Controls::UIElement>(nameof(FontStyle), Core::Media::FontStyle::Normal, true));
    FontWeightProperty = (Core::Dependency::BindProperty<Core::Media::FontWeight>::Register<XUI::UI::Controls::UIElement>(nameof(FontWeight), Core::Media::FontWeight::Normal, true));
    ForegroundProperty = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::IBrush>>::Register<XUI::UI::Controls::UIElement>(nameof(Foreground), std::make_shared<Core::Media::SolidColorBrush>(Core::Media::Colors::GetWhiteSmoke()), true));
    PaddingProperty = (Core::Dependency::BindProperty<Core::Media::Thickness>::Register<XUI::UI::Controls::UIElement>(nameof(Padding)));
    OverridesDefaultStyleProperty = (Core::Dependency::DirectProperty<UIElement, bool>::RegisterDirect(nameof(OverridesDefaultStyle), [](XUI::UI::Controls::UIElement* x) {return x->OverridesDefaultStyle; }, [](XUI::UI::Controls::UIElement* x, bool value) { x->OverridesDefaultStyle = value; }));
    SnapsToDevicePixelsProperty = (Core::Dependency::DirectProperty<UIElement, bool>::RegisterDirect(nameof(SnapsToDevicePixels), [](XUI::UI::Controls::UIElement* x) {return x->SnapsToDevicePixels; }, [](XUI::UI::Controls::UIElement* x, bool value) { x->SnapsToDevicePixels = value; }));
    TemplatedParentProperty = (
        Core::Dependency::DirectProperty<UIElement, std::shared_ptr<Core::Dependency::DependencyObject>>::RegisterDirect(
        nameof(TemplatedParent),
        [](XUI::UI::Controls::UIElement* x) {return x->TemplatedParent; },
        [](XUI::UI::Controls::UIElement* x, std::shared_ptr<Core::Dependency::DependencyObject> const& value)
    { x->TemplatedParent = value; }));

    TemplateAppliedEvent = (Core::Observer::RoutedEvent::Register<XUI::UI::Controls::UIElement, Core::Observer::RoutedEventArgs>(nameof(TemplateApplied), RoutingStrategies::Direct));

    BackgroundProperty->Changed.get_observable().subscribe(&Visual::AffectsVisualInvalidate);
    BorderBrushProperty->Changed.get_observable().subscribe(&Visual::AffectsVisualInvalidate);
    BorderThicknessProperty->Changed.get_observable().subscribe(&Visual::AffectsVisualInvalidate);

    //ClipToBoundsProperty.OverrideDefaultValue<TemplatedControl>(true);
}

UIElement::UIElement()
    : m_snapsToDevicePixels(false)
    , m_overridesDefaultStyle(false)
    , TemplateApplied(this)
{

}

UIElement::~UIElement()
{

}

void XUI::UI::Controls::UIElement::SetSnapsToDevicePixels(bool value)
{
    SetAndRaise(SnapsToDevicePixelsProperty, m_snapsToDevicePixels, value);
}

void XUI::UI::Controls::UIElement::SetOverridesDefaultStyle(bool value)
{
    SetAndRaise(OverridesDefaultStyleProperty, m_overridesDefaultStyle, value);
}

void XUI::UI::Controls::UIElement::OnInheritanceParentChanged()
{
    if (m_cachedTemplateParent)
        m_templatedParent = nullptr;
    m_cachedTemplateParent = false;

    InputElement::OnInheritanceParentChanged();
}

std::shared_ptr<XUI::Interfaces::IBrush> XUI::UI::Controls::UIElement::GetBackground() const
{
    return GetValue<std::shared_ptr<Interfaces::IBrush>>(BackgroundProperty.get());
}

void XUI::UI::Controls::UIElement::SetBackground(std::shared_ptr<XUI::Interfaces::IBrush> value)
{
    SetValue(BackgroundProperty.get(), value->shared_from_base_static<Interfaces::IBrush>());
}

std::shared_ptr<Core::Dependency::DependencyObject> const & XUI::UI::Controls::UIElement::GetTemplatedParent() const
{
    // Performance note: This special case for inherit dependency property because too frevently use
    if (!m_templatedParent && !m_cachedTemplateParent)
    {
        m_cachedTemplateParent = true;

        Core::Dependency::DependencyObject* inheritParent = GetInheritanceParent();
        while (inheritParent)
        {
            Core::Dependency::DependencyObject* templateParent = inheritParent->GetInheritTemplatedParent();
            if (templateParent)
                return m_templatedParent = static_cast<XUI::UI::Controls::UIElement*>(templateParent)->m_templatedParent;

            inheritParent = inheritParent->GetInheritanceParent();
        }
    }
    return m_templatedParent;
}

void XUI::UI::Controls::UIElement::SetTemplatedParent(std::shared_ptr<Core::Dependency::DependencyObject> const& value)
{
    //System::ThrowIfFailed<System::InvalidOperationException>(m_templatedParent == nullptr || m_templatedParent == value);

    SetAndRaise(TemplatedParentProperty, m_templatedParent, value);

    for (auto visual : VisualChildren.GetNativeEnumerator())
    {
        XUI::UI::Controls::UIElement* element = visual->As<XUI::UI::Controls::UIElement>();
        if (element)
        {
            element->m_cachedTemplateParent = true;
            element->TemplatedParent = value;
        }
    }
}

/// <inheritdoc/>
UIElement * XUI::UI::Controls::UIElement::GetTemplateFocusTarget()
{
    return this;
}

std::shared_ptr<XUI::Interfaces::IBrush> XUI::UI::Controls::UIElement::GetBorderBrush() const
{
    return GetValue<std::shared_ptr<Interfaces::IBrush>>(BorderBrushProperty.get());
}

void XUI::UI::Controls::UIElement::SetBorderBrush(std::shared_ptr<XUI::Interfaces::IBrush> value)
{
    SetValue(BorderBrushProperty.get(), value ? value->shared_from_base_static<Interfaces::IBrush>() : nullptr);
}

double XUI::UI::Controls::UIElement::GetBorderThickness() const
{
    return GetValue<double>(BorderThicknessProperty.get());
}

void XUI::UI::Controls::UIElement::SetBorderThickness(double value)
{
    SetValue(BorderThicknessProperty.get(), value);
}

std::string XUI::UI::Controls::UIElement::GetFontFamily() const
{
    return GetValue<std::string>(FontFamilyProperty.get());
}

void XUI::UI::Controls::UIElement::SetFontFamily(std::string value)
{
    SetValue(FontFamilyProperty.get(), value);
}

double XUI::UI::Controls::UIElement::GetFontSize() const
{
    return GetValue<double>(FontSizeProperty.get());
}

void XUI::UI::Controls::UIElement::SetFontSize(double value)
{
    SetValue(FontSizeProperty.get(), value);
}

Core::Media::FontStyle XUI::UI::Controls::UIElement::GetFontStyle() const
{
    return GetValue<Core::Media::FontStyle>(FontStyleProperty.get());
}

void XUI::UI::Controls::UIElement::SetFontStyle(Core::Media::FontStyle value)
{
    SetValue(FontStyleProperty.get(), value);
}

Core::Media::FontWeight XUI::UI::Controls::UIElement::GetFontWeight() const
{
    return GetValue<Core::Media::FontWeight>(FontWeightProperty.get());
}

void XUI::UI::Controls::UIElement::SetFontWeight(Core::Media::FontWeight value)
{
    SetValue(FontWeightProperty.get(), value);
}

std::shared_ptr<XUI::Interfaces::IBrush> XUI::UI::Controls::UIElement::GetForeground() const
{
    return GetValue<std::shared_ptr<Interfaces::IBrush>>(ForegroundProperty.get());
}

void XUI::UI::Controls::UIElement::SetForeground(std::shared_ptr<XUI::Interfaces::IBrush> value)
{
    SetValue(ForegroundProperty.get(), value->shared_from_base_static<Interfaces::IBrush>());
}

Core::Media::Thickness XUI::UI::Controls::UIElement::GetPadding() const
{
    return GetValue<Core::Media::Thickness>(PaddingProperty.get());
}

void XUI::UI::Controls::UIElement::SetPadding(Core::Media::Thickness value)
{
    SetValue(PaddingProperty.get(), value);
}

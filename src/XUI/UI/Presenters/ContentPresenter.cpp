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

#include "StdAfx.h"
#include "Core/GlobalInitializerScheduler.h"
#include "ContentPresenter.h"
#include "ContentControl.h"
#include "TextBlock.h"
#include "Core/Media/Pen.h"
#include "Platform/UIService.h"
#include "XamlCPP/Core/LogInterface.h"
#include "UI/DataTemplates/FuncDataTemplate.h"

using namespace XUI;
using namespace XUI::UI::Presenters;

STATIC_CLASS_INITIALIZER(ContentPresenter, XUI::UI::Presenters::ContentPresenter::StaticClassInitializer);

Core::Dependency::RefDependencyProperty ContentPresenter::ChildProperty;

Core::Dependency::RefDependencyProperty     ContentPresenter::RecognizesAccessKeyProperty;
Core::Dependency::RefDependencyProperty     ContentPresenter::ContentProperty;
Core::Dependency::RefDependencyProperty     ContentPresenter::ContentTemplateProperty;
Core::Dependency::RefDependencyProperty     ContentPresenter::HorizontalContentAlignmentProperty;
Core::Dependency::RefDependencyProperty     ContentPresenter::VerticalContentAlignmentProperty;
Core::Dependency::RefDependencyProperty     ContentPresenter::CornerRadiusProperty;

void XUI::UI::Presenters::ContentPresenter::StaticClassInitializer()
{
    ChildProperty = (Core::Dependency::DirectProperty<XUI::UI::Presenters::ContentPresenter, std::shared_ptr<XUI::UI::Controls::Control>>::RegisterDirect(nameof(Child),
        [](XUI::UI::Presenters::ContentPresenter* x) {
            return x->Child; 
        }, [](XUI::UI::Presenters::ContentPresenter* x, std::shared_ptr<XUI::UI::Controls::Control> const& value) {
            x->Child = value;
        }));

    RecognizesAccessKeyProperty = (Core::Dependency::BindProperty<bool>::Register<XUI::UI::Presenters::ContentPresenter>(nameof(RecognizesAccessKey)));
    ContentProperty = (Core::Dependency::BindProperty<System::Reflection::Variant>::Register<XUI::UI::Presenters::ContentPresenter>(nameof(Content)));
    ContentTemplateProperty = (Core::Dependency::BindProperty<std::shared_ptr<UI::DataTemplates::DataTemplate>>::Register<XUI::UI::Presenters::ContentPresenter>(nameof(ContentTemplate)));
    HorizontalContentAlignmentProperty = (Core::Dependency::BindProperty<XUI::UI::Controls::HorizontalAlignment>::Register<XUI::UI::Presenters::ContentPresenter>(nameof(HorizontalContentAlignment)));
    VerticalContentAlignmentProperty = (Core::Dependency::BindProperty<XUI::UI::Controls::VerticalAlignment>::Register<XUI::UI::Presenters::ContentPresenter>(nameof(VerticalContentAlignment)));
    CornerRadiusProperty = (Core::Dependency::BindProperty<float>::Register<XUI::UI::Presenters::ContentPresenter>(nameof(CornerRadius)));


    CornerRadiusProperty->Changed.get_observable().subscribe(&Controls::Visual::AffectsMeasureInvalidate);
    ContentProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e)
    {
        XUI::UI::Presenters::ContentPresenter* contentParent = e.Sender->As<XUI::UI::Presenters::ContentPresenter>();
        contentParent->ContentChanged(e);
    });
    TemplatedParentProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e)
    {
        XUI::UI::Presenters::ContentPresenter* contentPresenter = e.Sender->As<XUI::UI::Presenters::ContentPresenter>();
        std::shared_ptr<Core::Dependency::DependencyObject> parent = e.NewValue.TryGetValue<std::shared_ptr<Core::Dependency::DependencyObject>>();
        if (contentPresenter && parent)
        {
            XUI::UI::Controls::ContentControl* contentControl = parent->Static_As<XUI::UI::Controls::ContentControl>();
            if (contentControl)
                contentControl->Presenter = contentPresenter;
        }
        else
        {
            std::shared_ptr<Core::Dependency::DependencyObject> parent = e.OldValue.TryGetValue<std::shared_ptr<Core::Dependency::DependencyObject>>();
            if (contentPresenter && parent)
            {
                XUI::UI::Controls::ContentControl* contentControl = parent->Static_As<XUI::UI::Controls::ContentControl>();
                if (contentControl)
                    contentControl->Presenter = nullptr;
            }
        }
    });
}

XUI::UI::Presenters::ContentPresenter::ContentPresenter()
{
}

XUI::UI::Presenters::ContentPresenter::~ContentPresenter()
{
}

bool XUI::UI::Presenters::ContentPresenter::GetRecognizesAccessKey() const
{
    return GetValue<bool>(RecognizesAccessKeyProperty);
}

void XUI::UI::Presenters::ContentPresenter::SetRecognizesAccessKey(bool const & value)
{
    SetValue(RecognizesAccessKeyProperty, value);
}

System::Reflection::meta::Variant XUI::UI::Presenters::ContentPresenter::GetContent() const
{
    return std::move(GetValue(ContentProperty.get()));
}

void XUI::UI::Presenters::ContentPresenter::SetContent(System::Reflection::meta::Variant const & value)
{
    SetValue(ContentProperty.get(), value);
}

std::shared_ptr<UI::DataTemplates::DataTemplate> XUI::UI::Presenters::ContentPresenter::GetContentTemplate() const
{
    return GetValue<std::shared_ptr<UI::DataTemplates::DataTemplate>>(ContentTemplateProperty.get());
}

void XUI::UI::Presenters::ContentPresenter::SetContentTemplate(std::shared_ptr<UI::DataTemplates::DataTemplate> const& value)
{
    SetValue(ContentTemplateProperty.get(), value);
}

XUI::UI::Controls::HorizontalAlignment XUI::UI::Presenters::ContentPresenter::GetHorizontalContentAlignment() const
{
    return GetValue<XUI::UI::Controls::HorizontalAlignment>(HorizontalContentAlignmentProperty.get());
}

void XUI::UI::Presenters::ContentPresenter::SetHorizontalContentAlignment(XUI::UI::Controls::HorizontalAlignment value)
{
    SetValue(HorizontalContentAlignmentProperty.get(), value);
}

XUI::UI::Controls::VerticalAlignment XUI::UI::Presenters::ContentPresenter::GetVerticalContentAlignment() const
{
    return GetValue<XUI::UI::Controls::VerticalAlignment>(VerticalContentAlignmentProperty.get());
}

void XUI::UI::Presenters::ContentPresenter::SetVerticalContentAlignment(XUI::UI::Controls::VerticalAlignment value)
{
    SetValue(VerticalContentAlignmentProperty.get(), value);
}

float XUI::UI::Presenters::ContentPresenter::GetCornerRadius() const
{
    return GetValue<float>(CornerRadiusProperty.get());
}

void XUI::UI::Presenters::ContentPresenter::SetCornerRadius(float value)
{
    SetValue(CornerRadiusProperty.get(), value);
}

std::shared_ptr<UI::Controls::Control> const& XUI::UI::Presenters::ContentPresenter::GetChild() const
{
    return _child;
}

void XUI::UI::Presenters::ContentPresenter::SetChild(std::shared_ptr<UI::Controls::Control> const& value)
{
    SetAndRaise(ChildProperty, _child, value);
}

void XUI::UI::Presenters::ContentPresenter::Render(Interfaces::IDrawingContextImpl * context)
{
    if (BorderBrush == nullptr && Background == nullptr)
        return;

    auto background = Background;
    auto borderBrush = BorderBrush;
    auto borderThickness = BorderThickness;
    auto cornerRadius = CornerRadius;
    Core::Media::Rect rect = Core::Media::Rect::xywh(0, 0, Bounds.width(), Bounds.height());
    Core::Media::Thickness(borderThickness / 2.0).Deflate(rect);

    if (background != nullptr)
    {
        context->FillRectangle(background.get(), rect, cornerRadius);
    }

    if (borderBrush != nullptr && borderThickness > 0)
    {
        Core::Media::Pen pen(borderBrush.get(), borderThickness);
        context->DrawRectangle(pen, rect, cornerRadius);
    }

}

/// <summary>
/// Updates the <see cref="Child"/> control based on the control's <see cref="Content"/>.
/// </summary>
/// <remarks>
/// Usually the <see cref="Child"/> control is created automatically when 
/// <see cref="ApplyTemplate"/> is called; however for this to happen, the control needs to
/// be attached to a logical tree (if the control is not attached to the logical tree, it
/// is reasonable to expect that the DataTemplates needed for the child are not yet 
/// available). This method forces the <see cref="Child"/> control's creation at any point, 
/// and is particularly useful in unit tests.
/// </remarks>
void XUI::UI::Presenters::ContentPresenter::UpdateChild()
{
    auto content = Content;
    auto oldChild = Child.get();
    auto newChild = CreateChild();

    // Remove the old child if we're not recycling it.
    if (oldChild && newChild != oldChild)
    {
        if (oldChild && oldChild->Parent && (oldChild->Parent == this || oldChild->Parent == TemplatedParent.get()))
            oldChild->Parent->RemoveLogicalChild(oldChild);
        oldChild->GetVisualParent()->RemoveVisualChild(oldChild);
    }

    // Set the DataContext if the data isn't a control.
    if (!typeof(Control).IsAssignableFrom(*content.GetType()))
        DataContext = content; // .TryGetValue<System::Reflection::RefObject>();
    else
        ClearValue(DataContextProperty.get());

    // Update the Child.
    if (newChild == nullptr)
    {
        Child = nullptr;
    }
    else if (newChild != oldChild)
    {
        Child = newChild->shared_from_base_static<Controls::Control>();

        if (newChild && newChild->Parent && newChild->Parent != this && newChild->Parent != TemplatedParent.get())
            newChild->Parent->RemoveLogicalChild(newChild);

        if (newChild->Parent == nullptr)
            AddLogicalChild(newChild);

        // Workaround for window content presenter
        if (newChild->GetVisualParent())
            newChild->GetVisualParent()->RemoveVisualChild(newChild);

        if (newChild->GetVisualParent() == nullptr)
            AddVisualChild(newChild);
    }

    _createdChild = true;
}

/// <inheritdoc/>


/// <inheritdoc/>

void XUI::UI::Presenters::ContentPresenter::OnAttachedToVisualTree(Controls::VisualTreeAttachmentEventArgs const & e)
{
    Control::OnAttachedToVisualTree(e);
}

void XUI::UI::Presenters::ContentPresenter::OnAttachedToLogicalTree(Controls::LogicalTreeAttachmentEventArgs & e)
{
    Control::OnAttachedToLogicalTree(e);

    _createdChild = false;
    InvalidateMeasure();

    if (IsAttachedToLogicalTree)
        UpdateChild();
}

/// <summary>
/// Creates the child control.
/// </summary>
/// <returns>The child control or null.</returns>

XUI::UI::Controls::Control* XUI::UI::Presenters::ContentPresenter::CreateChild()
{
    auto content = Content;
    auto oldChild = Child.get();
    XUI::UI::Controls::Control* newChild = content.IsObject() ? content.ToObject()->Static_As<Controls::Control>() : nullptr;

    if (content.GetBase() && newChild == nullptr)
    {
        auto contentType = content.GetType();

        // Performance Note: Default content template apply from code (less xaml parse)
        if (contentType->IsValid() && (contentType->IsEnum() || *contentType == typeid(std::wstring) || *contentType == typeid(std::string) || *contentType == typeid(System::String)))
        {
            if (!_inBuildHost || !Child)
            {
                RemoveDataTemplate();

                _inBuildHost = true;

                std::shared_ptr<UI::TextBlock> textBlock = std::make_shared<UI::TextBlock>();
                textBlock->BeginInit();
                SyncProperties(textBlock.get());
                if (*contentType == typeid(System::String))
                    textBlock->Text = System::String::ToWString(content.ToString());
                else
                    textBlock->Text = System::String::ToWString(content.ToString());
                textBlock->EndInit();
                Child = textBlock;
            }
            else
            {
                SyncProperties(Child.get());
                static_cast<UI::TextBlock*>(Child.get())->Text = System::String::ToWString(content.ToString());
            }

            newChild = Child.get();
        }
        else
        {
            auto newDataTemplate = FindDataTemplate(contentType);

            if (newDataTemplate == m_dataTemplate)
            {
                newChild = oldChild;
            }
            else
            {
                RemoveDataTemplate();

                m_dataTemplate = newDataTemplate;

                ApplyDataTemplate();

                if (m_dataTemplateRoot)
                    newChild = m_dataTemplateRoot->Static_As<Controls::Control>();
            }
        }
    }
    else
    {
        RemoveDataTemplate();
        m_dataTemplate.reset();
    }

    return newChild;
}

void XUI::UI::Presenters::ContentPresenter::ApplyDataTemplate()
{
    //if (!ContentTemplate)
    //    SetContentTemplate(FindDataTemplate(GetContent().GetBase() ? GetContent().GetType() : &System::Reflection::Type::Invalid()));

    if (m_dataTemplate && !m_dataTemplateRoot)
    {
        RemoveDataTemplate();
        XamlCPP::Platform::ILog::Verbose(XamlCPP::Platform::LogArea::Control, nullptr, "Creating data template");
        m_dataTemplateRoot = m_dataTemplate->Build(nullptr)->As<Core::Dependency::DependencyObject>()->shared_from_base_static<Core::Dependency::DependencyObject>();
        if (m_dataTemplateRoot)
        {
            Control* dataControl = m_dataTemplateRoot->As<XUI::UI::Controls::Control>();
            if (dataControl)
            {
                dataControl->TemplatedParent = shared_from_base_static<Core::Dependency::DependencyObject>();

                m_dataTemplate->Attach(this, m_dataTemplateRoot.get());
            }
        }
    }
}

void XUI::UI::Presenters::ContentPresenter::RemoveDataTemplate()
{
    if (m_dataTemplate)
        m_dataTemplate->Detach(this);

    if (m_dataTemplateRoot)
    {
        Control* dataControl = m_dataTemplateRoot->As<XUI::UI::Controls::Control>();
        if (dataControl)
        {
            dataControl->TemplatedParent = nullptr;

            RemoveLogicalChild(dataControl);
            RemoveVisualChild(dataControl);
        }

        m_dataTemplateRoot = nullptr;
    }
}

std::shared_ptr<XUI::UI::DataTemplates::DataTemplate> XUI::UI::Presenters::ContentPresenter::FindDataTemplate(System::Reflection::meta::Variant const & data)
{
    auto contentType = data.GetValue<const System::Reflection::Type*>();

    if (ContentTemplate && (!ContentTemplate->DataType || !ContentTemplate->DataType->IsValid() ||  ContentTemplate->DataType->IsAssignableFrom(*contentType)))
        return ContentTemplate;
    std::shared_ptr<XUI::UI::DataTemplates::DataTemplate> dataTemplate = Control::FindDataTemplate(data);
    if (dataTemplate && (!dataTemplate->DataType || !dataTemplate->DataType->IsValid() || dataTemplate->DataType->IsAssignableFrom(*contentType) || (*dataTemplate->DataType == typeid(System::String) && (*contentType == typeid(std::string) || *contentType == typeid(std::wstring)))))
        return dataTemplate;
    return nullptr;
}

Core::Media::Size XUI::UI::Presenters::ContentPresenter::MeasureOverride(Core::Media::Size const & availableSize)
{
    auto child = Child;
    auto padding = Padding + Core::Media::Thickness(BorderThickness);

    if (child != nullptr)
    {
        child->Measure(availableSize.Deflate(padding));
        return child->DesiredSize.Inflate(padding);
    }
    else
    {
        return Core::Media::Size(padding.Left + padding.Right, padding.Bottom + padding.Top);
    }
}

Core::Media::Size XUI::UI::Presenters::ContentPresenter::ArrangeOverride(Core::Media::Size const & finalSize)
{
    auto child = Child;

    if (child != nullptr)
    {
        auto padding = Padding + Core::Media::Thickness(BorderThickness);
        auto sizeMinusPadding = finalSize.Deflate(padding);
        auto size = sizeMinusPadding;
        auto horizontalAlignment = HorizontalContentAlignment;
        auto verticalAlignment = VerticalContentAlignment;
        auto originX = padding.Left;
        auto originY = padding.Top;

        if (horizontalAlignment != Controls::HorizontalAlignment::Stretch)
        {
            size = size.WithWidth(child->DesiredSize.Width);
        }

        if (verticalAlignment != Controls::VerticalAlignment::Stretch)
        {
            size = size.WithHeight(child->DesiredSize.Height);
        }

        switch (horizontalAlignment)
        {
            case Controls::HorizontalAlignment::Stretch:
            case Controls::HorizontalAlignment::Center:
                originX += (sizeMinusPadding.Width - size.Width) / 2;
                break;
            case Controls::HorizontalAlignment::Right:
                originX = finalSize.Width - size.Width;
                break;
        }

        switch (verticalAlignment)
        {
            case Controls::VerticalAlignment::Stretch:
            case Controls::VerticalAlignment::Center:
                originY += (sizeMinusPadding.Height - size.Height) / 2;
                break;
            case Controls::VerticalAlignment::Bottom:
                originY = finalSize.Height - size.Height;
                break;
        }

        Core::Media::Rect rect = Core::Media::Rect::xywh(originX, originY, size.Width, size.Height);
        child->Arrange(rect);
    }

    return finalSize;
}

/// <summary>
/// Called when the <see cref="Content"/> property changes.
/// </summary>
/// <param name="e">The event args.</param>

void XUI::UI::Presenters::ContentPresenter::ContentChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e)
{
    _createdChild = false;
    if (IsAttachedToLogicalTree)
    {
        UpdateChild();
    }
    else if (Child.get())
    {
        if (Child && Child->Parent && (Child->Parent == this || Child->Parent == TemplatedParent.get()))
            Child->Parent->RemoveLogicalChild(Child.get());
        if (Child && Child->GetVisualParent() && (Child->GetVisualParent() == this || Child->GetVisualParent() == TemplatedParent.get()))
            Child->GetVisualParent()->RemoveVisualChild(Child.get());
        Child = nullptr;
    }

    InvalidateMeasure();
}

void XUI::UI::Presenters::ContentPresenter::TemplatedParentChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e)
{
    //(e.NewValue as IContentPresenterHost) ? .RegisterContentPresenter(this);
}

void XUI::UI::Presenters::ContentPresenter::SyncProperties(XUI::UI::Controls::Control * e)
{
    e->FontFamily = this->FontFamily;
    e->FontSize = this->FontSize;
    e->FontStyle = this->FontStyle;
    e->FontWeight = this->FontWeight;
    e->Foreground = this->Foreground;
    e->HorizontalAlignment = this->HorizontalContentAlignment;
    e->VerticalAlignment = this->VerticalContentAlignment;
}
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

#include "TextBlock.h"
#include "Core/GlobalInitializerScheduler.h"
#include "Interfaces/IFormatedText.h"

using namespace XUI;
using namespace XUI::Core::Observer;

STATIC_CLASS_INITIALIZER(TextBlock, XUI::UI::TextBlock::StaticClassInitializer);

Core::Dependency::RefDependencyProperty     XUI::UI::TextBlock::TextProperty;

Core::Dependency::RefDependencyProperty     XUI::UI::TextBlock::TextAlignmentProperty;
Core::Dependency::RefDependencyProperty     XUI::UI::TextBlock::TextWrappingProperty;

void XUI::UI::TextBlock::StaticClassInitializer()
{
    TextProperty = (Core::Dependency::DirectProperty<XUI::UI::TextBlock, std::wstring>::RegisterDirect(nameof(Text),
        [](XUI::UI::TextBlock* x) { return x->Text; },
        [](XUI::UI::TextBlock* x, std::wstring const& v) { x->Text = v; })
    );

    TextAlignmentProperty = (Core::Dependency::BindProperty<Core::Media::TextAlignment>::Register<TextBlock>(nameof(TextAlignment)));
    TextWrappingProperty = (Core::Dependency::BindProperty<Core::Media::TextWrapping>::Register<TextBlock>(nameof(TextWrapping)));

    ClipToBoundsProperty->OverrideDefaultValue<XUI::UI::TextBlock>(true);

    TextProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        TextBlock* control = e.Sender->Static_As<TextBlock>();
        if (control)
            control->InvalidateFormattedText();
    });

    TextAlignmentProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        TextBlock* control = e.Sender->Static_As<TextBlock>();
        if (control)
            control->InvalidateFormattedText();
    });

    FontSizeProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        TextBlock* control = e.Sender->Static_As<TextBlock>();
        if (control)
            control->InvalidateFormattedText();
    });

    FontStyleProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        TextBlock* control = e.Sender->Static_As<TextBlock>();
        if (control)
            control->InvalidateFormattedText();
    });

    FontWeightProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        TextBlock* control = e.Sender->Static_As<TextBlock>();
        if (control)
            control->InvalidateFormattedText();
    });
}

XUI::UI::TextBlock::TextBlock()
{
}

XUI::UI::TextBlock::~TextBlock()
{
}

void XUI::UI::TextBlock::SetText(std::wstring const & value)
{
    SetAndRaise(TextProperty.get(), _formattedText._text, value);
}

Core::Media::TextWrapping XUI::UI::TextBlock::GetTextWrapping() const
{
    return GetValue<Core::Media::TextWrapping>(TextWrappingProperty.get());
}

void XUI::UI::TextBlock::SetTextWrapping(Core::Media::TextWrapping value)
{
    SetValue(TextWrappingProperty.get(), value);
}

Core::Media::TextAlignment XUI::UI::TextBlock::GetTextAlignment() const
{
    return GetValue<Core::Media::TextAlignment>(TextAlignmentProperty.get());
}

void XUI::UI::TextBlock::SetTextAlignment(Core::Media::TextAlignment value)
{
    SetValue(TextAlignmentProperty.get(), value);
}

Core::Media::FormattedTextWrapper & XUI::UI::TextBlock::GetFormattedText() const
{
    if (!_isValidFormattedText)
    {
        InitializeFormattedText();
        _isValidFormattedText = true;
    }

    return _formattedText;
}

/// <summary>
/// Renders the <see cref="TextBlock"/> to a drawing context.
/// </summary>
/// <param name="context">The drawing context.</param>

void XUI::UI::TextBlock::Render(Interfaces::IDrawingContextImpl * context)
{
    auto background = Background;

    if (background != nullptr)
    {
        context->FillRectangle(background.get(), Bounds);
    }

    context->DrawText(Foreground.get(), Core::Media::Point::Zero(), FormattedText.GetPlatformImpl());

    Controls::Control::Render(context);
}

/// <summary>
/// Creates the <see cref="FormattedText"/> used to render the text.
/// </summary>
/// <param name="constraint">The constraint of the text.</param>
/// <returns>A <see cref="FormattedText"/> object.</returns>

void XUI::UI::TextBlock::InitializeFormattedText() const
{
    _formattedText.Typeface = Core::Media::Typeface(FontFamily, FontSize, FontStyle, FontWeight);
    _formattedText.TextAlignment = TextAlignment;
    _formattedText.TextWrapping = TextWrapping;
}

/// <summary>
/// Invalidates <see cref="FormattedText"/>.
/// </summary>

void XUI::UI::TextBlock::InvalidateFormattedText()
{
    _isValidFormattedText = false;
    _formattedText.Invalidate();
    InvalidateMeasure();
}

/// <summary>
/// Measures the control.
/// </summary>
/// <param name="availableSize">The available size for the control.</param>
/// <returns>The desired size.</returns>

Core::Media::Size XUI::UI::TextBlock::MeasureOverride(Core::Media::Size const & availableSize)
{
    if (!Text.empty())
    {
        if (TextWrapping == Core::Media::TextWrapping::Wrap)
            FormattedText.Constraint = Core::Media::Size(availableSize.Width, std::numeric_limits<double>::max());
        else
            FormattedText.Constraint = Core::Media::Size::Infinity;
    
        return FormattedText.Measure();
    }

    return Core::Media::Size::Empty;
}

void XUI::UI::TextBlock::OnAttachedToLogicalTree(Controls::LogicalTreeAttachmentEventArgs & e)
{
    Control::OnAttachedToLogicalTree(e);
    InvalidateFormattedText();
}

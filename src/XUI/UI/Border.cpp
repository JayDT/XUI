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

#include "Border.h"

#include "Core/Media/Pen.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;

STATIC_CLASS_INITIALIZER(Border, XUI::UI::Border::StaticClassInitializer)

Core::Dependency::RefDependencyProperty XUI::UI::Border::CornerRadiusProperty;

void XUI::UI::Border::StaticClassInitializer()
{
    CornerRadiusProperty = (Core::Dependency::BindProperty<float>::Register<XUI::UI::Border>(nameof(CornerRadius)));
    CornerRadiusProperty->Changed.get_observable().subscribe(&Controls::Visual::AffectsMeasureInvalidate);
}

XUI::UI::Border::Border()
{
}

XUI::UI::Border::~Border()
{
}

float XUI::UI::Border::GetCornerRadius() const
{
    return GetValue<float>(CornerRadiusProperty.get());
}

void XUI::UI::Border::SetCornerRadius(float value)
{
    SetValue(CornerRadiusProperty.get(), value);
}

void XUI::UI::Border::Render(Interfaces::IDrawingContextImpl * context)
{
    auto background = Background;
    auto borderBrush = BorderBrush;

    if (Background != nullptr || borderBrush != nullptr)
    {
        Core::Media::Rect rect = Core::Media::Rect::xywh(0, 0, Bounds.width(), Bounds.height());
        Core::Media::Thickness borderThickness(BorderThickness / 2.0);
        borderThickness.Deflate(rect);

        if (Background != nullptr)
            context->FillRectangle(background.get(), rect, CornerRadius);

        if (borderBrush != nullptr && BorderThickness > 0)
        {
            Core::Media::Pen pen(borderBrush.get(), BorderThickness);
            context->DrawRectangle(pen, rect, CornerRadius);
        }
    }

    Controls::Decorator::Render(context);
}

/// <summary>
/// Measures the control.
/// </summary>
/// <param name="availableSize">The available size.</param>
/// <returns>The desired size of the control.</returns>
Core::Media::Size XUI::UI::Border::MeasureOverride(Core::Media::Size const & availableSize)
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

/// <summary>
/// Arranges the control's child.
/// </summary>
/// <param name="finalSize">The size allocated to the control.</param>
/// <returns>The space taken.</returns>
Core::Media::Size XUI::UI::Border::ArrangeOverride(Core::Media::Size const & finalSize)
{
    auto child = Child;

    if (child != nullptr)
    {
        auto padding = Padding + Core::Media::Thickness(BorderThickness);
        Core::Media::Rect rect(finalSize.ToVector());
        padding.Deflate(rect);
        child->Arrange(rect);
    }

    return finalSize;
}

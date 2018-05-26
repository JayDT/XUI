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

#include "Image.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;

STATIC_CLASS_INITIALIZER(Image, XUI::UI::Image::StaticConstructor)

Core::Dependency::RefDependencyProperty XUI::UI::Image::SourceProperty;
Core::Dependency::RefDependencyProperty XUI::UI::Image::StretchProperty;

void XUI::UI::Image::StaticConstructor()
{
    SourceProperty = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::IBitmapImpl>>::Register<XUI::UI::Image>(nameof(Source)));
    StretchProperty = (Core::Dependency::BindProperty<Core::Media::Stretch>::Register<XUI::UI::Image>(nameof(Stretch), Core::Media::Stretch::Uniform));

    SourceProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        UI::Controls::Visual* visual = e.Sender->As<UI::Controls::Visual>();
        if (visual)
            visual->InvalidateVisual();
    });

    StretchProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        UI::Controls::Visual* visual = e.Sender->As<UI::Controls::Visual>();
        if (visual)
            visual->InvalidateVisual();
    });
}

std::shared_ptr<Interfaces::IBitmapImpl> XUI::UI::Image::GetSource() const
{
    return GetValue<std::shared_ptr<Interfaces::IBitmapImpl>>(SourceProperty);
}

void XUI::UI::Image::SetSource(std::shared_ptr<Interfaces::IBitmapImpl> const& value)
{
    SetValue(SourceProperty, value);
}

Core::Media::Stretch XUI::UI::Image::GetStretch() const
{
    return GetValue<Core::Media::Stretch>(StretchProperty);
}

void XUI::UI::Image::SetStretch(Core::Media::Stretch value)
{
    SetValue(StretchProperty, value);
}

void XUI::UI::Image::Render(Interfaces::IDrawingContextImpl *context)
{
    auto source = Source;

    if (source != nullptr)
    {
        Core::Media::Rect viewPort = Core::Media::Rect(Bounds.GetSize().ToVector());
        Core::Media::Size sourceSize(source->PixelWidth(), source->PixelHeight());
        Core::Media::Vector scale = Core::Media::Vector::CalculateScaling(Stretch, Bounds.GetSize(), sourceSize);
        Core::Media::Size scaledSize = sourceSize * scale;
        Core::Media::Rect destRect = viewPort.CenterRect(Core::Media::Rect(scaledSize.ToVector())).intersect(viewPort);
        Core::Media::Rect sourceRect = Core::Media::Rect(sourceSize.ToVector()).CenterRect(Core::Media::Rect((destRect.GetSize() / scale).ToVector()));

        context->DrawImage(source.get(), 1, sourceRect, destRect);
    }

    Controls::Control::Render(context);
}

Core::Media::Size XUI::UI::Image::MeasureOverride(Core::Media::Size const& availableSize)
{
    auto source = Source;

    if (source != nullptr)
    {
        Core::Media::Size sourceSize(source->PixelWidth(), source->PixelHeight());

        if (std::isinf(availableSize.Width) || std::isinf(availableSize.Height))
        {
            return sourceSize;
        }
        else
        {
            return Core::Media::Size::CalculateSize(Stretch, availableSize, sourceSize);
        }
    }
    else
    {
        return Core::Media::Size::Empty;
    }
}

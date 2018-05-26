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

#include "Decorator.h"

#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::Interfaces;
using namespace XUI::Core::Observer;
using namespace XUI::UI::Controls;

STATIC_CLASS_INITIALIZER(Decorator, Decorator::StaticClassInitializer)

Core::Dependency::RefDependencyProperty Decorator::ChildProperty;

/// <summary>
/// Initializes static members of the <see cref="Decorator"/> class.
/// </summary>
void XUI::UI::Controls::Decorator::StaticClassInitializer()
{
    ChildProperty = (Core::Dependency::DirectProperty<XUI::UI::Controls::Decorator, std::shared_ptr<XUI::UI::Controls::Control>>::RegisterDirect(nameof(Child),
        [](XUI::UI::Controls::Decorator* x) {
        return x->Child;
    }, [](XUI::UI::Controls::Decorator* x, std::shared_ptr<XUI::UI::Controls::Control> const& value) {
        x->Child = value;
    }));

    ChildProperty->Changed.get_observable().subscribe(&Visual::AffectsMeasureInvalidate);
    ChildProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        static_cast<UI::Controls::Decorator*>(e.Sender)->ChildChanged(e);
    });
}

/// <inheritdoc/>
Core::Media::Size Decorator::MeasureOverride(Core::Media::Size const& availableSize)
{
    auto content = Child;
    auto padding = Padding;
    
    if (content != nullptr)
    {
        content->Measure(availableSize.Deflate(padding));
        return content->DesiredSize.Inflate(padding);
    }
    else
    {
        return Core::Media::Size(padding.Left + padding.Right, padding.Bottom + padding.Top);
    }
    return Core::Media::Size();
}

/// <inheritdoc/>
Core::Media::Size Decorator::ArrangeOverride(Core::Media::Size const& finalSize)
{
    if (Child != nullptr)
    {
        Core::Media::Rect rect(finalSize.ToVector());
        Padding.Deflate(rect);
        Child->Arrange(rect);
    }
    return finalSize;
}

/// <summary>
/// Called when the <see cref="Child"/> property changes.
/// </summary>
/// <param name="e">The event args.</param>
void Decorator::ChildChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e)
{
    auto oldChild = e.OldValue.GetValue<Control*>();
    auto newChild = e.NewValue.GetValue<Control*>();
    
    if (oldChild != nullptr)
    {
        //oldChild->SetParent(nullptr);
        RemoveLogicalChild(oldChild);
        RemoveVisualChild(oldChild);
    }
    
    if (newChild != nullptr)
    {
        //newChild->SetParent(this);
        AddLogicalChild(newChild);
        AddVisualChild(newChild);
    }
}

std::shared_ptr<UI::Controls::Control> const& XUI::UI::Controls::Decorator::GetChild() const
{
    return _child;
}

void XUI::UI::Controls::Decorator::SetChild(std::shared_ptr<UI::Controls::Control> const& value)
{
    System::ThrowIfFailed<System::InvalidOperationException>(value->Static_As<Controls::Control>() != Static_As<Controls::Control>());
    SetAndRaise(ChildProperty.get(), _child, value);
}

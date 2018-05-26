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

#include "Panel.h"

#include "Core/GlobalInitializerScheduler.h"
#include "Core/Media/Brush.h"

using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(Panel, XUI::UI::Panel::StaticClassInitializer)

std::shared_ptr<XUI::Core::Dependency::DependencyProperty> XUI::UI::Panel::BackgroundProperty;

void XUI::UI::Panel::StaticClassInitializer()
{
    BackgroundProperty = XUI_DEPENDENCY_PROPERTY_REGISTER(XUI::UI::Panel, std::shared_ptr<XUI::Core::Media::Brush>, nameof(Background));
}

Panel::Panel()
{
    _children.CollectionChanged += System::make_eventhandler(&Panel::ChildrenChanged, this);
}

void Panel::SetChildren(Controls::Control::LogicalChildrens const& value)
{
    //System::ThrowIfFailed<std::invalid_argument>(!value.empty());

    VisualChildren.Clear();
    _children.Clear();
    for (auto itr = value.GetNativeEnumerator().begin(); itr != value.GetNativeEnumerator().end(); ++itr)
        _children.Add(*itr);
}

std::shared_ptr<XUI::Interfaces::IBrush> Panel::GetBackground() const
{
    return GetValue<std::shared_ptr<XUI::Interfaces::IBrush>>(BackgroundProperty.get());
}

void Panel::SetBackground(std::shared_ptr<XUI::Interfaces::IBrush> value)
{
    SetValue(BackgroundProperty.get(), std::shared_ptr<XUI::Interfaces::IBrush>(value));
}

void Panel::Render(XUI::Interfaces::IDrawingContextImpl* context)
{
    auto background = Background;
    if (background != nullptr)
    {
        Core::Media::Point renderSize = Core::Media::Point(Bounds.width(), Bounds.height());
        Core::Media::Rect rect(renderSize);
        context->FillRectangle(background.get(), rect);
    }

    Control::Render(context);
}

void XUI::UI::Panel::ChildrenChanged(void * sender, System::Collection::NotifyCollectionChangedEventArgs & e)
{
    System::Collection::IContainer<std::shared_ptr<Controls::Control>>* list = (System::Collection::IContainer<std::shared_ptr<Controls::Control>>*)sender;

    switch (e.Action)
    {
        case System::Collection::NotifyCollectionChangedAction::Add:
        {
            Control* control = static_cast<Control*>(e.NewValue.ToObject()); // (*list)[e.NewStartingIndex].get();
            if (control->GetParent() == nullptr)
                AddLogicalChild(control);
            AddVisualChild(control);
            break;
        }
        case System::Collection::NotifyCollectionChangedAction::Remove:
        {
            Control* control = (*list)[e.OldStartingIndex].get();
            if (control->GetParent() == this)
                RemoveLogicalChild(control);
            RemoveVisualChild(control);
            break;
        }
        case System::Collection::NotifyCollectionChangedAction::Replace:
        {
            break;
        }
        case System::Collection::NotifyCollectionChangedAction::Reset:
        {
            for (auto icontrol = list->GetEnumerator(); !icontrol->IsFinished(); icontrol->MoveNext())
            {
                RemoveLogicalChild((**icontrol).get());
                RemoveVisualChild((**icontrol).get());
            }
            break;
        }
    }

    InvalidateMeasure();
}

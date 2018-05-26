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

#include "VirtualizingStackPanel.h"
#include "UI/Presenters/ItemsPresenter.h"
#include "Core/GlobalInitializerScheduler.h"

STATIC_CLASS_INITIALIZER(VirtualizingStackPanel, XUI::UI::VirtualizingStackPanel::StaticClassInitializer);

using namespace XUI;
using namespace XUI::UI;

Core::Dependency::RefDependencyProperty VirtualizingStackPanel::IsVirtualizingProperty;

void XUI::UI::VirtualizingStackPanel::StaticClassInitializer()
{
    IsVirtualizingProperty = (Core::Dependency::BindProperty<System::Reflection::RefObject>::RegisterAttached<VirtualizingStackPanel, Controls::Control>(nameof(IsVirtualizing)));
}

XUI::UI::VirtualizingStackPanel::~VirtualizingStackPanel()
{
}

bool VirtualizingStackPanel::GetIsFull()
{
    return Orientation == Orientation::Horizontal ? _takenSpace >= _availableSpace.Width : _takenSpace >= _availableSpace.Height;
}

std::shared_ptr<Presenters::ItemVirtualizer> VirtualizingStackPanel::GetController()
{
    return _controller;
}

void VirtualizingStackPanel::SetController(std::shared_ptr<Presenters::ItemVirtualizer> const& value)
{
    _controller = value;
}

int VirtualizingStackPanel::GetOverflowCount()
{
    return _canBeRemoved;
}

UI::Orientation VirtualizingStackPanel::GetScrollDirection()
{
    return Orientation;
}

double VirtualizingStackPanel::GetAverageItemSize()
{
    return _averageItemSize;
}

double VirtualizingStackPanel::GetPixelOverflow()
{
    auto bounds = Orientation == Orientation::Horizontal ? _availableSpace.Width : _availableSpace.Height;
    return std::max(0.0, _takenSpace - bounds);
}

double VirtualizingStackPanel::GetPixelOffset()
{
    return _pixelOffset;
}

void VirtualizingStackPanel::SetPixelOffset(double value)
{
    if (_pixelOffset != value)
    {
        _pixelOffset = value;
        InvalidateArrange();
    }
}

double VirtualizingStackPanel::GetCrossAxisOffset()
{
    return _crossAxisOffset;
}

void VirtualizingStackPanel::SetCrossAxisOffset(double value)
{
    if (_crossAxisOffset != value)
    {
        _crossAxisOffset = value;
        InvalidateArrange();
    }
}

void VirtualizingStackPanel::ForceInvalidateMeasure()
{
    InvalidateMeasure();
    _forceRemeasure = true;
}

Core::Media::Size VirtualizingStackPanel::MeasureOverride(Core::Media::Size const& availableSize)
{
    if (_forceRemeasure || _previousMeasure != availableSize || _availableSpace != availableSize)
    {
        _forceRemeasure = false;
        _availableSpace = availableSize;
        if (Controller)
            Controller->UpdateControls();
    }

    return StackPanel::MeasureOverride(availableSize);
}

Core::Media::Size VirtualizingStackPanel::ArrangeOverride(Core::Media::Size const& finalSize)
{
    if (finalSize.Height != 0 && finalSize.Width != 0)
        _availableSpace = finalSize;

    _canBeRemoved = 0;
    _takenSpace = 0;
    _averageItemSize = 0;
    _averageCount = 0;

    auto result = StackPanel::ArrangeOverride(finalSize);
    _takenSpace += _pixelOffset;
    if (Controller)
        Controller->UpdateControls();
    return result;
}

void VirtualizingStackPanel::ChildrenChanged(void *sender, System::Collection::NotifyCollectionChangedEventArgs& e)
{
    StackPanel::ChildrenChanged(sender, e);

    System::Collection::IContainer<std::shared_ptr<Controls::Control>>* list = (System::Collection::IContainer<std::shared_ptr<Controls::Control>>*)sender;

    switch (e.Action)
    {
        case System::Collection::NotifyCollectionChangedAction::Add:
            UpdateAdd(static_cast<Controls::Control*>(e.NewValue.ToObject()));
            break;

        case System::Collection::NotifyCollectionChangedAction::Remove:
            UpdateRemove((*list)[e.OldStartingIndex].get());
            break;
    }
}

Controls::InputElement *VirtualizingStackPanel::GetControlInDirection(Presenters::NavigationDirection direction, Controls::Control *from)
{
    if (from == nullptr)
    {
        return nullptr;
    }

    auto logicalScrollable = Parent->Static_As<Presenters::ItemsPresenter>();

    if (logicalScrollable && logicalScrollable->IsLogicalScrollEnabled == true)
    {
        return logicalScrollable->GetControlInDirection(direction, from);
    }
    else
    {
        return StackPanel::GetControlInDirection(direction, from);
    }
}

void VirtualizingStackPanel::ArrangeChild(Controls::Control *child, Core::Media::Rect const& _rect, Core::Media::Size const& panelSize, UI::Orientation orientation)
{
    if (orientation == UI::Orientation::Vertical)
    {
        Core::Media::Rect rect = Core::Media::Rect::xywh(_rect.x0() - _crossAxisOffset, _rect.y0() - _pixelOffset, _rect.width(), _rect.height());
        child->Arrange(rect);

        if (rect.y0() >= _availableSpace.Height)
        {
            ++_canBeRemoved;
        }

        if (rect.Bottom >= _takenSpace)
        {
            _takenSpace = rect.Bottom;
        }

        AddToAverageItemSize(rect.height());
    }
    else
    {
        Core::Media::Rect rect = Core::Media::Rect::xywh(_rect.x0() - _pixelOffset, _rect.y0() - _crossAxisOffset, _rect.width(), _rect.height());
        child->Arrange(rect);

        if (rect.x0() >= _availableSpace.Width)
        {
            ++_canBeRemoved;
        }

        if (rect.Right >= _takenSpace)
        {
            _takenSpace = rect.Right;
        }

        AddToAverageItemSize(rect.width());
    }
}

void VirtualizingStackPanel::UpdateAdd(Controls::Control *child)
{
    auto bounds = Bounds;
    auto gap = Gap;

    child->Measure(_availableSpace);
    ++_averageCount;

    if (Orientation == UI::Orientation::Vertical)
    {
        auto height = child->DesiredSize.Height;
        _takenSpace += height + gap;
        AddToAverageItemSize(height);
    }
    else
    {
        auto width = child->DesiredSize.Width;
        _takenSpace += width + gap;
        AddToAverageItemSize(width);
    }
}

void VirtualizingStackPanel::UpdateRemove(Controls::Control *child)
{
    auto bounds = Bounds;
    auto gap = Gap;

    if (Orientation == UI::Orientation::Vertical)
    {
        auto height = child->DesiredSize.Height;
        _takenSpace -= height + gap;
        RemoveFromAverageItemSize(height);
    }
    else
    {
        auto width = child->DesiredSize.Width;
        _takenSpace -= width + gap;
        RemoveFromAverageItemSize(width);
    }

    if (_canBeRemoved > 0)
    {
        --_canBeRemoved;
    }
}

void VirtualizingStackPanel::AddToAverageItemSize(double value)
{
    ++_averageCount;
    _averageItemSize += (value - _averageItemSize) / _averageCount;
}

void VirtualizingStackPanel::RemoveFromAverageItemSize(double value)
{
    _averageItemSize = ((_averageItemSize * _averageCount) - value) / (_averageCount - 1);
    --_averageCount;
}

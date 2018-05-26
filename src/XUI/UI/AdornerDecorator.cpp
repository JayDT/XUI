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

#include "AdornerDecorator.h"
#include "AdornerLayer.h"
#include "Core/Media/MediaTypes.h"
#include "Core/Media/Size_t.h"

using namespace XUI;
using namespace XUI::UI;

XUI::UI::AdornerDecorator::AdornerDecorator()
    : Controls::Decorator()
    , AdornerLayer(std::make_shared<XUI::UI::AdornerLayer>())
{
}

void XUI::UI::AdornerDecorator::EndInit()
{
    Controls::Control::EndInit();

    if (_initCount == 0)
    {
        if (!AdornerLayer->GetInheritanceParent())
        {
            AdornerLayer->ZIndex = std::numeric_limits<int>::max();
            AdornerLayer->SetParent(this);
            AddVisualChild(AdornerLayer.get());
        }
    }
}

Core::Media::Size XUI::UI::AdornerDecorator::MeasureOverride(Core::Media::Size const& availableSize)
{
    AdornerLayer->Measure(availableSize);
    return Decorator::MeasureOverride(availableSize);
}

Core::Media::Size XUI::UI::AdornerDecorator::ArrangeOverride(Core::Media::Size const& arrangeSize)
{
    AdornerLayer->Arrange(Core::Media::Rect(arrangeSize.ToVector()));
    return Decorator::ArrangeOverride(arrangeSize);
}

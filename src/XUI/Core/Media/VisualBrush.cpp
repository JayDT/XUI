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
#include "VisualBrush.h"
#include "UI/Controls/Visual.h"
#include "Platform/UIRenderer.h"
#include "Platform/UIService.h"

using namespace XUI;

std::shared_ptr<UI::Controls::Visual> XUI::Core::Media::VisualBrush::GetVisual() const
{
    return GetValue<std::shared_ptr<UI::Controls::Visual>>(VisualProperty);
}

void XUI::Core::Media::VisualBrush::SetVisual(std::shared_ptr<UI::Controls::Visual> const & value)
{
    SetValue(VisualProperty, value);
}

void XUI::Core::Media::VisualBrush::Render(Interfaces::IDrawingContextImpl * context)
{
    if (Visual)
    {
        auto renderService = Platform::UIService::Instance()->Get<Platform::UIRender>();
        renderService->Render(context, Visual.get(), Visual->GetBounds(), Platform::UIRenderFlags::RELATIVE_RENDER | Platform::UIRenderFlags::RELATIVE_FIRST_IDENTIFY_MAT);
    }
}

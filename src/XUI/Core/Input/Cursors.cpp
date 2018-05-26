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

#include "Cursors.h"

#include "Core/GlobalInitializerScheduler.h"
#include "Platform/UIService.h"

using namespace XUI;
using namespace XUI::Core::Input;

STATIC_CLASS_AFTER_SERVICE_INITIALIZER(Cursor, XUI::Core::Input::Cursor::StaticClassInitializer)

Cursor Cursor::Default;

void Cursor::StaticClassInitializer()
{
    Default.SetCursorType(StandardCursorType::Arrow);
}

Cursor::Cursor(XUI::Interfaces::PlatformHandle const& platformCursor)
    : PlatformCursor(std::move(platformCursor))
{
}

Cursor::Cursor(StandardCursorType const& cursorType)
    : Cursor(GetCursor(cursorType))
{
}

std::shared_ptr<Cursor> XUI::Core::Input::Cursor::Parse(std::string const & s)
{
    auto value = typeof(StandardCursorType).GetEnum()->GetValue(s);
    if (value.GetBase())
        return std::make_shared<Core::Input::Cursor>(Core::Input::StandardCursorType(value.ToInt()));
    return std::shared_ptr<Cursor>();
}

std::string XUI::Core::Input::Cursor::ToString() const
{
    return "Arrow"; // ToDo
}

XUI::Interfaces::PlatformHandle Cursor::GetCursor(StandardCursorType type)
{
    auto platform = Platform::UIService::Instance()->Get<Interfaces::IStandardCursorFactory>();

    if (platform == nullptr)
        throw std::exception();

    return platform->GetCursor(type);
}

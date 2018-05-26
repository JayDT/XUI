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

#include "Path.h"
#include "Core/Media/Thickness_t.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;

STATIC_CLASS_INITIALIZER(Path, XUI::UI::Shapes::Path::StaticConstructor)

Core::Dependency::RefDependencyProperty XUI::UI::Shapes::Path::DataProperty;

void XUI::UI::Shapes::Path::StaticConstructor()
{
    DataProperty = (Core::Dependency::BindProperty<std::shared_ptr<Core::Media::Geometry>>::Register<XUI::UI::Shapes::Path>(nameof(Data)));

    extern XUI::UI::Shapes::AffectGeometryFn AffectGeometryFunction;

    DataProperty->Changed.get_observable().subscribe(AffectGeometryFunction);
}

std::shared_ptr<Core::Media::Geometry> XUI::UI::Shapes::Path::CreateDefiningGeometry()
{
    return Data;
}

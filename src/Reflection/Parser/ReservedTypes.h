/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2016 Austin Brunkhorst, All Rights Reserved.
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

#pragma once

#include "standard/Preprocessor/pp_basic.h"
#include "Reflection/Runtime/MetaProperty.h"

static_assert(std::is_class<System::Reflection::meta::Object>::value, "Reserved type is invalid");
static_assert(std::is_class<System::Reflection::meta::MetaProperty>::value, "Reserved type is invalid");

const auto kTypeObject = STRINGIFY(System::Reflection::meta::Object);
const auto kTypeMetaProperty = STRINGIFY(System::Reflection::meta::MetaProperty);

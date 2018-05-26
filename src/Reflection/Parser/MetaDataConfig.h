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

namespace native_attribute
{
    const auto Enable = "Enable";
    const auto Disable = "Disable";
    const auto WhiteListMethods = "WhiteListMethods";

    const auto Register = "Register";

    const auto DisplayName = "DisplayName";

    const auto EnablePtrType = "EnablePtrType";
    const auto EnableConstPtrType = "EnableConstPtrType";

    const auto ReflIncludeType = "ReflInclude";
    const auto AbstractType = "AbstractType";
    const auto EnableArrayType = "EnableArrayType";

    const auto DisableNonDynamicCtor = "DisableNonDynamic";
    const auto DynamicCtorWrap = "WrapObject";

    const auto PropertyType = "PropertyType";

    const auto PropertyGetter = "Getter";
    const auto PropertySetter = "Setter";

    const auto ExplicitGetter = "ExplicitGetter";
    const auto ExplicitSetter = "ExplicitSetter";

    const auto AssemblyAttributeNs = "assembly";
}

extern unsigned long internalID;
const auto kMetaExternalTypeDefName = "__META_EXTERNAL__";
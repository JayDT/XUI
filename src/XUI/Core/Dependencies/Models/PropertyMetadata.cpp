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
#include "Core/Dependencies/Models/PropertyMetadata.h"
#include "XamlCPP/Core/Dependencies/IPropertyObject.h"

#include "rxcpp/rx.hpp"
#include "rxcpp/rx-predef.hpp"

using namespace XUI::Core::Dependency;

/// <summary>
/// Initializes a new instance of the <see cref="PropertyMetadata"/> class.
/// </summary>
/// <param name="defaultBindingMode">The default binding mode.</param>

XUI::Core::Dependency::PropertyMetadata::PropertyMetadata(BindingMode defaultBindingMode, CoerceValueCallback onCoerceValue)
    : _defaultBindingMode(defaultBindingMode)
    , _onCoerceValueCallback(onCoerceValue)
{
}

/// <summary>
/// Gets the default binding mode for the property.
/// </summary>

BindingMode XUI::Core::Dependency::PropertyMetadata::DefaultBindingMode()
{
    return _defaultBindingMode == BindingMode::Default ?
        BindingMode::OneWay : _defaultBindingMode;
}

/// <summary>
/// Merges the metadata with the base metadata.
/// </summary>
/// <param name="baseMetadata">The base metadata to merge.</param>
/// <param name="property">The property to which the metadata is being applied.</param>

void XUI::Core::Dependency::PropertyMetadata::Merge(PropertyMetadata & baseMetadata, XamlCPP::Core::Dependency::IPropertyObject * property)
{
    if (_defaultBindingMode == BindingMode::Default)
    {
        _defaultBindingMode = baseMetadata.DefaultBindingMode();
    }
}

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
#include "Core/Dependencies/Models/PropertyPrototype.h"
#include "XamlCPP/Core/Dependencies/IPropertyObject.h"
#include "Core/Dependencies/Models/PropertyMetadata.h"
#include "Core/Dependencies/Models/PropertyMetadata.h"
#include "Core/Dependencies/DependencyPropertyMgr.h"
#include "Reflection/Runtime/Variant.h"
#include "Reflection/Runtime/Object.h"
#include "Reflection/Runtime/CXXReflection.h"
#include "Reflection/Runtime/TypeConfig.h"

#include <standard/collection.h>
#include <type_traits>

using namespace XUI::Core::Dependency;

XUI::Core::Dependency::DependencyProperty::DependencyProperty(
    std::string name, 
    System::Reflection::meta::Type const * OwnerType, 
    System::Reflection::meta::Type const * ValueType, 
    std::shared_ptr<PropertyMetadata> const & metadata, 
    NotifyFunction* Notifying)
{
    _metadataCache.set_empty_key(nullptr);

    Id = DependencyPropertyMgr::GeneratePropertyUID();
    _name = name;
    _propertyType = ValueType;

    _ownerType = OwnerType;
    if (Notifying)
        _notifying = *Notifying;

    _metadata[OwnerType] = metadata;
    _defaultMetadata = metadata;
}

XUI::Core::Dependency::DependencyProperty::DependencyProperty(
    DependencyProperty * source, 
    System::Reflection::meta::Type const * OwnerType, 
    std::shared_ptr<PropertyMetadata> metadata)
    : _initialized(source->_initialized)
    , _changed(source->_changed)
{
    System::ThrowIfFailed<System::ArgumentNullException>(source != nullptr);
    System::ThrowIfFailed<System::ArgumentNullException>(OwnerType != nullptr);

    _metadataCache.set_empty_key(nullptr);

    _name = source->Name;
    _propertyType = source->PropertyType;
    _ownerType = OwnerType;
    _notifying = source->Notifying;

    Id = source->Id;
    _defaultMetadata = source->_defaultMetadata;

    if (metadata != nullptr)
    {
        _metadata[OwnerType] = metadata;
    }
}

/// <summary>
/// Gets the property metadata for the specified type.
/// </summary>
/// <param name="type">The type.</param>
/// <returns>
/// The property metadata.
/// </returns>
///
PropertyMetadata* XUI::Core::Dependency::DependencyProperty::GetMetadata(System::Reflection::meta::Type const * type) const
{
    System::ThrowIfFailed<System::ArgumentNullException>(type != nullptr);

    MetaDataCacheMap::iterator cache_result;
    System::Reflection::meta::Type const* currentType = type;

    if (System::Collection::GetContains(_metadataCache, type, cache_result))
    {
        return cache_result->second;
    }

    // Build Cache
    MetaDataMap::iterator result;
    type->VisitInheritClasses([this, &result](System::Reflection::meta::Type const* type)
    {
        if (System::Collection::GetContains(_metadata, type, result))
        {
            _metadataCache[type] = result->second.get();
        }

        return true;
    });

    if (result != _metadata.end())
        return result->second.get();

    _metadataCache[type] = _defaultMetadata.get();

    return _defaultMetadata.get();
}

/// <summary>
/// Overrides the metadata for the property on the specified type.
/// </summary>
/// <param name="type">The type.</param>
/// <param name="metadata">The metadata.</param>

void XUI::Core::Dependency::DependencyProperty::OverrideMetadata(System::Reflection::meta::Type const * type, std::shared_ptr<PropertyMetadata> metadata)
{
    System::ThrowIfFailed<System::ArgumentNullException>(type != nullptr);
    System::ThrowIfFailed<System::ArgumentNullException>(metadata != nullptr);

    if (_metadata.find(type) != _metadata.end())
    {
        throw new System::InvalidOperationException("Metadata is already set for {Name} on {type}.");
    }

    auto baseMetadata = GetMetadata(type);
    metadata->Merge(*baseMetadata, this);
    _metadata[type] = metadata;
    _metadataCache.clear();
}

BindingMode XUI::Core::Dependency::DependencyProperty::GetDefaultBindingMode(System::Reflection::Type const * type)
{
    return GetMetadata(type)->DefaultBindingMode();
}

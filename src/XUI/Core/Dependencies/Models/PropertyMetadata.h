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

#ifndef __PROPERTY_METADATA_H__
#define __PROPERTY_METADATA_H__

#include "Core/Dependencies/DependencyDefines.h"
#include "XamlCPP/Core/Dependencies/IPropertyObject.h"
#include "standard/misc.h"

namespace XamlCPP::Core::Dependency
{
    struct IPropertyObject;
    struct IBindPropertyMetadata;
    struct IDirectPropertyMetadata;
}

namespace XUI::Core::Dependency
{
    typedef std::function<System::Reflection::Variant(DependencyObject*, System::Reflection::Variant const&)> CoerceValueCallback;

    /// <summary>
    /// Base class for dependency property metadata.
    /// </summary>
    class TC_XUI_API PropertyMetadata : public System::EnableSharedFromThisEx<PropertyMetadata>
    {
    private:
        BindingMode _defaultBindingMode;
        CoerceValueCallback _onCoerceValueCallback;

    public:

        /// <summary>
        /// Initializes a new instance of the <see cref="PropertyMetadata"/> class.
        /// </summary>
        /// <param name="defaultBindingMode">The default binding mode.</param>
        PropertyMetadata(BindingMode defaultBindingMode = BindingMode::Default, CoerceValueCallback onCoerceValue = nullptr);
        virtual ~PropertyMetadata() {}

        /// <summary>
        /// Gets the default binding mode for the property.
        /// </summary>
        BindingMode DefaultBindingMode();

        System::Reflection::Variant CoerceValue(DependencyObject* d, System::Reflection::Variant const& baseValue)
        {
            if (_onCoerceValueCallback)
                return _onCoerceValueCallback(d, baseValue);
            return baseValue;
        }

        /// <summary>
        /// Merges the metadata with the base metadata.
        /// </summary>
        /// <param name="baseMetadata">The base metadata to merge.</param>
        /// <param name="property">The property to which the metadata is being applied.</param>
        virtual void Merge(
            PropertyMetadata& baseMetadata,
            XamlCPP::Core::Dependency::IPropertyObject* property);

        virtual XamlCPP::Core::Dependency::IBindPropertyMetadata const* ToIBindPropertyMetadata() const { return nullptr; }
        virtual XamlCPP::Core::Dependency::IDirectPropertyMetadata const* ToIDirectPropertyMetadata() const { return nullptr; }
    };
}

#endif // !__PROPERTY_METADATA_H__
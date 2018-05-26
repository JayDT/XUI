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

#ifndef __MODEL_DATA_PROPERTY_DIRECT_DATA_H__
#define __MODEL_DATA_PROPERTY_DIRECT_DATA_H__

#include "Reflection/Runtime/Variant.h"
#include "standard/BasicPrimities.h"
#include <functional>

#include "Core/Dependencies/Models/PropertyMetadata.h"

namespace XUI::Core::Dependency
{
    /// <summary>
    /// Metadata for direct dependency properties.
    /// </summary>
    class TC_XUI_API DirectPropertyMetadata : public PropertyMetadata, public XamlCPP::Core::Dependency::IDirectPropertyMetadata
    {
        System::Reflection::Variant _unsetValue;
        bool _enableDataValidation;

    public:
        DirectPropertyMetadata()
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="StyledPropertyMetadata{TValue}"/> class.
        /// </summary>
        /// <param name="unsetValue">
        /// The value to use when the property is set to <see cref="dependencyProperty.UnsetValue"/>
        /// </param>
        /// <param name="defaultBindingMode">The default binding mode.</param>
        /// <param name="enableDataValidation">
        /// Whether the property is interested in data validation.
        /// </param>
        template<typename TValue>
        DirectPropertyMetadata(
            TValue unsetValue = TValue(),
            BindingMode defaultBindingMode = BindingMode::Default,
            bool enableDataValidation = false,
            CoerceValueCallback onCoerceValue = nullptr)
                : PropertyMetadata(defaultBindingMode, onCoerceValue)
        {
            _unsetValue = System::Reflection::Variant(unsetValue, (System::Reflection::variant_policy::Copy*)nullptr);
            _enableDataValidation = enableDataValidation;
        }

        DirectPropertyMetadata(
            System::Reflection::Variant const& unsetValue,
            BindingMode defaultBindingMode = BindingMode::Default,
            bool enableDataValidation = false,
            CoerceValueCallback onCoerceValue = nullptr)
            : PropertyMetadata(defaultBindingMode, onCoerceValue)
        {
            _unsetValue = unsetValue;
            _enableDataValidation = enableDataValidation;
        }

        virtual ~DirectPropertyMetadata()
        {}

        XamlCPP::Core::Dependency::IDirectPropertyMetadata const* ToIDirectPropertyMetadata() const override final { return this; }

        /// <summary>
        /// Gets the value to use when the property is set to <see cref="dependencyProperty.UnsetValue"/>.
        /// </summary>
        virtual System::Reflection::meta::Variant const& UnsetValue() const override final { return _unsetValue; }

        /// <summary>
        /// Gets a value indicating whether the property is interested in data validation.
        /// </summary>
        /// <remarks>
        /// Data validation is validation performed at the target of a binding, for example in a
        /// view model using the INotifyDataErrorInfo interface. Only certain properties on a
        /// control (such as a TextBox's Text property) will be interested in recieving data
        /// validation messages so this feature must be explicitly enabled by setting this flag.
        /// </remarks>
        PROPERTY_(bool, EnableDataValidation);
        bool GetEnableDataValidation() const  override { return _enableDataValidation; }
        void SetEnableDataValidation(bool value) { _enableDataValidation = value; }

        /// <inheritdoc/>
        void Merge(PropertyMetadata& baseMetadata, XamlCPP::Core::Dependency::IPropertyObject* _property) override
        {
            PropertyMetadata::Merge(baseMetadata, _property);

            XamlCPP::Core::Dependency::IDirectPropertyMetadata const* src = baseMetadata.ToIDirectPropertyMetadata();

            if (src != nullptr)
            {
                _unsetValue = src->UnsetValue();
            }
        }
    };
}

#endif // !__MODEL_DATA_PROPERTY_DIRECT_DATA_H__

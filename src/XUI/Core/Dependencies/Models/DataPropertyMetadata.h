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

#ifndef __MODEL_DATA_PROPERTY_META_DATA_H__
#define __MODEL_DATA_PROPERTY_META_DATA_H__

#include "Reflection/Runtime/Variant.h"
#include "standard/BasicPrimities.h"
#include <functional>

#include "Core/Dependencies/Models/PropertyMetadata.h"
#include "Core/Dependencies/Models/PropertyPrototype.h"

namespace XUI::Core::Dependency
{
    /// <summary>
    /// Metadata for styled dependency properties.
    /// </summary>
    class TC_XUI_API BindPropertyMetadata : public PropertyMetadata, public XamlCPP::Core::Dependency::IBindPropertyMetadata
    {
    private:
        System::Reflection::Variant _defaultValue;
        IBindPropertyMetadata::ValidateFunction _validateFunc;

    public:
        BindPropertyMetadata()
        {

        }

        /// <summary>
        /// Initializes a new instance of the <see cref="StyledPropertyMetadata{TValue}"/> class.
        /// </summary>
        /// <param name="defaultValue">The default value of the property.</param>
        /// <param name="validate">A validation function.</param>
        /// <param name="defaultBindingMode">The default binding mode.</param>
        template<typename TValue>
        BindPropertyMetadata(
            TValue defaultValue = TValue(),
            IBindPropertyMetadata::ValidateFunction validate = nullptr,
            BindingMode defaultBindingMode = BindingMode::Default,
            CoerceValueCallback onCoerceValue = nullptr)
                : PropertyMetadata(defaultBindingMode, onCoerceValue)
        {
            _defaultValue = System::Reflection::Variant(defaultValue, (System::Reflection::variant_policy::Copy*)nullptr);
            _validateFunc = validate;
        }

        BindPropertyMetadata(
            System::Reflection::Variant const& defaultValue,
            IBindPropertyMetadata::ValidateFunction validate = nullptr,
            BindingMode defaultBindingMode = BindingMode::Default,
            CoerceValueCallback onCoerceValue = nullptr)
            : PropertyMetadata(defaultBindingMode, onCoerceValue)
        {
            _defaultValue = defaultValue;
            _validateFunc = validate;
        }

        virtual ~BindPropertyMetadata() {}

        IBindPropertyMetadata const* ToIBindPropertyMetadata() const override { return this; }

        /// <summary>
        /// Gets the default value for the property.
        /// </summary>
        PROPERTY_(System::Reflection::Variant const&, DefaultValue);
        System::Reflection::Variant const& GetDefaultValue() const { return _defaultValue; }
        void SetDefaultValue(System::Reflection::Variant const& value) { _defaultValue = value; }

        /// <summary>
        /// Gets the validation callback.
        /// </summary>
        PROPERTY_T_GS(IBindPropertyMetadata::ValidateFunction const&, _validateFunc, Validate);

        /// <inheritdoc/>
        void Merge(PropertyMetadata& baseMetadata, XamlCPP::Core::Dependency::IPropertyObject* property) override
        {
            PropertyMetadata::Merge(baseMetadata, property);

            XamlCPP::Core::Dependency::IBindPropertyMetadata const* src = baseMetadata.ToIBindPropertyMetadata();

            if (src != nullptr)
            {
                if (!DefaultValue.GetBase())
                    DefaultValue = src->GetDefaultValue(_defaultValue.GetType());

                if (!Validate)
                    Validate = src->GetValidationFunc(_defaultValue.GetType());
            }
        }

        // Inherited via IDataPropertyMetadata
        System::Reflection::meta::Variant const& GetDefaultValue(System::Reflection::meta::Type const * type) const override final
        {
            //if (_defaultValue.GetNativeTypeHash() == typeid(System::Reflection::meta::Variant).hash_code())
            //    return _defaultValue;
            return _defaultValue;
        }

        IBindPropertyMetadata::ValidateFunction const& GetValidationFunc(System::Reflection::meta::Type const * type) const override final
        {
            return _validateFunc;
        }

        //static ValidateFunction Cast(ValidateFunction& f)
        //{
        //    if (f == null)
        //    {
        //        return null;
        //    }
        //    else
        //    {
        //        return (o, v) => f(o, (TValue)v);
        //    }
        //}
    };
}

#endif //!__MODEL_DATA_PROPERTY_META_DATA_H__
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

#ifndef __MODEL_DATA_PROPERTY_H__
#define __MODEL_DATA_PROPERTY_H__

#include "Core/Dependencies/Models/PropertyPrototype.h"
#include "Core/Dependencies/Models/DataPropertyMetadata.h"
#include "Core/Dependencies/DependencyObject.h"
#include "Reflection/Runtime/Variant.h"
#include <functional>

#define XUI_DEPENDENCY_PROPERTY_DECL(Type) XUI::Core::Dependency::RefDependencyProperty
#define XUI_DEPENDENCY_PROPERTY_REGISTER(OwnerType, Type, Name) XUI::Core::Dependency::BindProperty<Type>::Register<OwnerType>(Name)
#define XUI_DEPENDENCY_PROPERTY_REGISTER_WITH_ARGS(OwnerType, Type) XUI::Core::Dependency::BindProperty<Type>::Register<OwnerType>

namespace XUI::Core::Dependency
{
    template<typename TValue>
    class TC_XUI_API BindProperty : public DependencyProperty, public XamlCPP::Core::Dependency::IBindPropertyAccessor
    {
        bool _inherits = false;
        bool _isAttached = false;
    public:
        /// <summary>
        /// Gets a value indicating whether the property inherits its value.
        /// </summary>
        /// <value>
        /// A value indicating whether the property inherits its value.
        /// </value>
        bool GetInherits() override { return _inherits; }
        void SetIsAttached(bool value) { _isAttached = value; }
        bool GetIsAttached() override { return _isAttached; }

        BindProperty(
            std::string name,
            System::Reflection::meta::Type const* owner_type,
            TValue defaultValue,
            std::shared_ptr<PropertyMetadata> const& metadata, //DataPropertyMetadata<TValue>
            bool inherits = false,
            NotifyFunction* Notifying = nullptr)
            : DependencyProperty(name, owner_type, &typeof(TValue), metadata, Notifying)
        {
            //if (name.Contains("."))
            //{
            //    throw System::ArgumentException("'name' may not contain periods.");
            //}

            _inherits = inherits;
        }

        /// <param name="source">The property to add the owner to.</param>
        /// <param name="ownerType">The type of the class that registers the property.</param>
        BindProperty(DependencyProperty* source, System::Reflection::meta::Type const* ownerType)
            : DependencyProperty(source, ownerType, nullptr)
        {
            _inherits = source->Inherits;
        }

        virtual ~BindProperty()
        {}

        /// <summary>
        /// Gets the default value for the property on the specified type.
        /// </summary>
        /// <param name="type">The type.</param>
        /// <returns>The default value.</returns>
        System::Reflection::meta::Variant const& GetDefaultValue(System::Reflection::meta::Type const* type) const override
        {
            System::ThrowIfFailed<System::ArgumentNullException>(type != nullptr);
            return DependencyProperty::GetMetadata(type)->ToIBindPropertyMetadata()->GetDefaultValue(type);
            //auto& ret = static_cast<typename BindPropertyMetadata<TValue>*>(DependencyProperty::GetMetadata(type)->ToIDirectPropertyMetadata())->DefaultValue;
            //if (typeid(TValue) == typeid(System::Reflection::meta::Variant))
            //    return ret;
            //return System::Reflection::meta::Variant(ret, ReferenceVariantPolicy);
        }

        XamlCPP::Core::Dependency::IBindPropertyAccessor const* ToIBindPropertyAccessor() const override { return this; }

        // Inherited via DependencyProperty
        virtual System::Reflection::meta::Variant GetValue(XamlCPP::Core::Dependency::IDependencyObject * instance) const override
        {
            return static_cast<DependencyObject*>(instance)->GetPropertyData(const_cast<BindProperty*>(this));
        }

        virtual void SetValue(XamlCPP::Core::Dependency::IDependencyObject * instance, System::Reflection::meta::Variant const & value) override
        {
            instance->StorePropertyData(this, value);
        }

        virtual void OverrideDefaultValue(System::Reflection::meta::Type const * type, System::Reflection::meta::Variant const & defaultValue) override
        {
            OverrideMetadata(type, std::make_shared<BindPropertyMetadata>(defaultValue));
        }

        virtual XamlCPP::Core::Dependency::IBindPropertyMetadata::ValidateFunction const& GetValidationFunc(System::Reflection::meta::Type const * type) const override
        {
            System::ThrowIfFailed<System::ArgumentNullException>(type != nullptr);
            return DependencyProperty::GetMetadata(type)->ToIBindPropertyMetadata()->GetValidationFunc(type);
        }
        
        /// <summary>
        /// Overrides the metadata for the property on the specified type.
        /// </summary>
        /// <param name="type">The type.</param>
        /// <param name="metadata">The metadata.</param>
        void OverrideMetadata(System::Reflection::meta::Type const * type, std::shared_ptr<BindPropertyMetadata> const& metadata)
        {
            System::ThrowIfFailed<System::ArgumentException>(typeof(TValue).IsAssignableFrom(*metadata->GetDefaultValue().GetType()), "Incompatible default value");
            DependencyProperty::OverrideMetadata(type, metadata);
        }

        template<class TOwner>
        static std::shared_ptr<BindProperty<TValue>> Register(
            const char* name,
            NotifyFunction notifying,
            CoerceValueCallback onCoerceValue,
            TValue defaultValue = TValue())
        {
            return Register<TOwner>(name, defaultValue, false, BindingMode::OneWay, nullptr, notifying, onCoerceValue);
        }

        template<class TOwner>
        static std::shared_ptr<BindProperty<TValue>> Register(
            const char* name,
            TValue defaultValue = TValue(),
            bool inherits = false,
            BindingMode defaultBindingMode = BindingMode::OneWay,
            XamlCPP::Core::Dependency::IBindPropertyMetadata::ValidateFunction validate = nullptr,
            NotifyFunction notifying = nullptr,
            CoerceValueCallback onCoerceValue = nullptr)
        {
            System::ThrowIfFailed<System::ArgumentNullException>(name != nullptr);
        
            auto metadata = std::make_shared<BindPropertyMetadata>(
                defaultValue,
                validate,
                defaultBindingMode,
                onCoerceValue);
            
            auto result = std::make_shared<BindProperty<TValue>>(
                name,
                &typeof(TOwner),
                defaultValue,
                std::shared_ptr<PropertyMetadata>(metadata),
                inherits,
                notifying != nullptr ? &notifying : nullptr);
        
            DependencyPropertyMgr::Instance()->Register(&typeof(TOwner), result);
            return result;
        }

        template<class THost, class TOwner>
        static std::shared_ptr<BindProperty<TValue>> RegisterAttached(
            const char* name,
            TValue defaultValue = TValue(),
            bool inherits = false,
            BindingMode defaultBindingMode = BindingMode::OneWay,
            XamlCPP::Core::Dependency::IBindPropertyMetadata::ValidateFunction validate = nullptr,
            NotifyFunction notifying = nullptr,
            CoerceValueCallback onCoerceValue = nullptr)
        {
            System::ThrowIfFailed<System::ArgumentNullException>(name != nullptr);

            auto metadata = std::make_shared<BindPropertyMetadata>(
                defaultValue,
                validate,
                defaultBindingMode,
                onCoerceValue);

            auto result = std::make_shared<BindProperty<TValue>>(
                name,
                &typeof(TOwner),
                defaultValue,
                std::shared_ptr<PropertyMetadata>(metadata),
                inherits,
                notifying != nullptr ? &notifying : nullptr);

            result->SetIsAttached(true);

            DependencyPropertyMgr::Instance()->Register(&typeof(THost), result);
            return result;
        }
    };
}

#endif // !__MODEL_DATA_PROPERTY_H__
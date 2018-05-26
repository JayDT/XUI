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

#ifndef __MODEL_DIRECT_PROPERTY_H__
#define __MODEL_DIRECT_PROPERTY_H__

#include "Core/Dependencies/Models/PropertyPrototype.h"
#include "Core/Dependencies/Models/DirectPropertyMetadata.h"
#include "Core/Dependencies/DependencyObject.h"
#include "Core/Dependencies/DependencyPropertyMgr.h"
#include "Reflection/Runtime/Variant.h"
#include <functional>

#define XUI_DIRECT_PROPERTY_DECL(OwnerType, Type) std::shared_ptr<XUI::Core::Dependency::DirectProperty<OwnerType, Type>>
#define XUI_DIRECT_PROPERTY_REGISTER(OwnerType, Type, Name, Getter, Setter) Core::Dependency::DirectProperty<OwnerType, Type>::RegisterDirect(Name, Getter, Setter)

namespace XUI::Core::Dependency
{
    template<typename TOwner, typename TValue>
    class TC_XUI_API DirectProperty : public DependencyProperty, public XamlCPP::Core::Dependency::IDirectPropertyAccessor
    {
    public:
        typedef std::function<TValue(TOwner*)> Action_Getter;
        typedef std::function<void(TOwner*, TValue)> Action_Setter;

    private:
        Action_Getter _getter;
        Action_Setter _setter;

        bool _isReadOnly = false;

    public:
        PROPERTY_(bool, IsDirect);
        bool PROPERTY_GET(IsDirect)() override { return true; }

        DirectProperty(std::string name,
            System::Reflection::meta::Type const* owner_type,
            Action_Getter getter,
            Action_Setter setter,
            PropertyMetadata* metadata,
            NotifyFunction* Notifying = nullptr) 
            : DependencyProperty(name, owner_type, &typeof(TValue), metadata->shared_from_this(), Notifying)
        {
            _getter = getter;
            _setter = setter;
        }

        DirectProperty(DependencyProperty* source,
            System::Reflection::meta::Type const* owner_type,
            Action_Getter getter,
            Action_Setter setter,
            PropertyMetadata* metadata = nullptr) 
            : DependencyProperty(source, owner_type, &typeof(TValue), metadata->shared_from_this())
        {
            _getter = getter;
            _setter = setter;
        }

        virtual ~DirectProperty()
        {}

        // Inherited via DependencyProperty
        virtual System::Reflection::meta::Variant GetValue(XamlCPP::Core::Dependency::IDependencyObject * instance) const override
        {
            if (_getter)
            {
                if (typeid(System::Reflection::Variant) == typeid(TValue))
                    return _getter(static_cast<TOwner*>(instance));
                if (std::is_pointer<TValue>::value)
                    return _getter(static_cast<TOwner*>(instance));
                if (std::is_reference<TValue>::value)
                    return System::Reflection::Variant(_getter(static_cast<TOwner*>(instance)), (System::Reflection::variant_policy::NoCopy*)nullptr);
                return  System::Reflection::Variant(_getter(static_cast<TOwner*>(instance)), (System::Reflection::variant_policy::Copy*)nullptr);
            }
            return System::Reflection::meta::Variant::Empty;
        }

        virtual void SetValue(XamlCPP::Core::Dependency::IDependencyObject * instance, System::Reflection::meta::Variant const & value) override
        {
            if (_setter && value.GetBase())
            {
                if (typeid(System::Reflection::Variant) == typeid(TValue))
                    _setter(static_cast<TOwner*>(instance), (TValue&)value);
                else
                    _setter(static_cast<TOwner*>(instance), *static_cast<TValue*>(value.getPtr()));
            }
        }

        virtual void OverrideDefaultValue(System::Reflection::meta::Type const * type, System::Reflection::meta::Variant const & defaultValue) override
        {
            OverrideMetadata(type, std::make_shared<DirectPropertyMetadata>(defaultValue));
        }

        System::Reflection::meta::Variant const& GetDefaultValue(System::Reflection::meta::Type const* type) const override
        {
            return GetMetadata(type)->ToIDirectPropertyMetadata()->UnsetValue();
        }

        XamlCPP::Core::Dependency::IDirectPropertyAccessor const* ToIDirectPropertyAccessor() const override final { return this; }

        PROPERTY_(bool, IsReadOnly);
        bool PROPERTY_GET(IsReadOnly)() override { return _isReadOnly; }

        template<class TNewOwner>
        DirectProperty<TNewOwner, TValue>* AddOwner(
            std::function<TValue(TNewOwner*)> getter,
            std::function<void(TNewOwner*, TValue)> setter = nullptr,
            TValue unsetValue = TValue(),
            BindingMode defaultBindingMode = BindingMode::OneWay,
            bool enableDataValidation = false,
            CoerceValueCallback onCoerceValue = nullptr)
        {
            std::shared_ptr<DirectPropertyMetadata> metadata = std::make_shared<DirectPropertyMetadata>(
                unsetValue,
                defaultBindingMode,
                enableDataValidation,
                onCoerceValue);

            auto result = new DirectProperty<TNewOwner, TValue>(
                this,
                getter,
                setter,
                metadata);
        
            DependencyPropertyMgr::Instance()->Register(&typeof(TNewOwner), result);
            return result;
        }

        static std::shared_ptr<DirectProperty<TOwner, TValue>> RegisterDirect(
                const char* name,
                Action_Getter getter,
                Action_Setter setter,
                TValue unsetValue = TValue(),
                BindingMode defaultBindingMode = BindingMode::OneWay,
                bool enableDataValidation = false,
                CoerceValueCallback onCoerceValue = nullptr)
        {
            System::ThrowIfFailed<System::ArgumentNullException>(name != nullptr);
        
            std::shared_ptr<DirectPropertyMetadata> metadata = std::make_shared<DirectPropertyMetadata>(
                unsetValue,
                defaultBindingMode,
                enableDataValidation,
                onCoerceValue);
        
            auto result = std::make_shared<DirectProperty<TOwner, TValue>>(name, &typeof(TOwner), getter, setter, metadata.get());
            DependencyPropertyMgr::Instance()->Register(&typeof(TOwner), result);
            return result;
        }
    };
}

#endif //!__MODEL_DIRECT_PROPERTY_H__
/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2016 OmniGUI Platform
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

#ifndef __IPROPERTY_OBJECT_H__
#define __IPROPERTY_OBJECT_H__

#include "standard/BasicPrimities.h"
#include "standard/events.h"
#include "standard/misc.h"
#include "Reflection/Runtime/Variant.h"
#include "Reflection/Runtime/Object.h"
#include "XamlCPP/Interfaces/IMarkupExtension.h"
#include "RxExtension/Subject.h"

//#include "rxcpp/rx.hpp"

namespace XamlCPP::Core::Dependency
{
    struct IDependencyObject;
    struct IPropertyObject;

    typedef rxcpp::observable<System::Reflection::Variant> GenericObservable;
    typedef System::RX::Subject<System::Reflection::Variant> GenericSubject;

    struct BindingSubject : System::EnableSharedFromThisEx<BindingSubject>
    {
        GenericSubject Subject;
    };

    enum class DependencyObservableState
    {
        NONE = 0
    };

    /// <summary>
    /// Untyped interface to <see cref="DirectPropertyMetadata{TValue}"/>
    /// </summary>
    struct IDirectPropertyMetadata
    {
        virtual ~IDirectPropertyMetadata() {}

        /// <summary>
        /// Gets the to use when the property is set to <see cref="dependencyProperty.UnsetValue"/>.
        /// </summary>
        virtual System::Reflection::meta::Variant const& UnsetValue() const = 0;

        /// <summary>
        /// Gets a value indicating whether the property is interested in data validation.
        /// </summary>
        virtual bool GetEnableDataValidation() const = 0;
    };

    /// <summary>
    /// Untyped interface to <see cref="DataPropertyMetadata{TValue}"/>
    /// </summary>
    struct IBindPropertyMetadata
    {
        typedef std::function<System::Reflection::meta::Variant(XamlCPP::Core::Dependency::IDependencyObject*, System::Reflection::meta::Variant const&)> ValidateFunction;

        virtual ~IBindPropertyMetadata() {}

        /// <summary>
        /// Gets the default value for the property.
        /// </summary>
        virtual System::Reflection::meta::Variant const& GetDefaultValue(System::Reflection::meta::Type const* type) const = 0;

        /// <summary>
        /// Gets the property's validation function.
        /// </summary>
        virtual ValidateFunction const& GetValidationFunc(System::Reflection::meta::Type const* type) const = 0;
    };

    /// <summary>
    /// Provides a runtime interface for getting and setting 
    /// <see cref="DirectProperty{TOwner, TValue}"/> values.
    /// </summary>
    struct IDirectPropertyAccessor
    {
        virtual ~IDirectPropertyAccessor() {}

        /// <summary>
        /// Gets a value indicating whether the property is read-only.
        /// </summary>
        PURE_VIRTUAL_R_PROPERTY(bool, IsReadOnly);

        /// <summary>
        /// Gets the default value for the property for the specified type.
        /// </summary>
        /// <param name="type">The type.</param>
        /// <returns>
        /// The default value.
        /// </returns>
        virtual System::Reflection::meta::Variant const& GetDefaultValue(System::Reflection::meta::Type const* type) const = 0;

        /// <summary>
        /// Gets the value of the property on the instance.
        /// </summary>
        /// <param name="instance">The instance.</param>
        /// <returns>The property value.</returns>
        virtual System::Reflection::meta::Variant GetValue(XamlCPP::Core::Dependency::IDependencyObject* instance) const = 0;

        /// <summary>
        /// Sets the value of the property on the instance.
        /// </summary>
        /// <param name="instance">The instance.</param>
        /// <param name="value">The value.</param>
        virtual void SetValue(XamlCPP::Core::Dependency::IDependencyObject* instance, System::Reflection::meta::Variant const& value) = 0;
    };

    struct IBindPropertyAccessor
    {
        virtual ~IBindPropertyAccessor() {}

        /// <summary>
        /// Gets the default value for the property for the specified type.
        /// </summary>
        /// <param name="type">The type.</param>
        /// <returns>
        /// The default value.
        /// </returns>
        virtual System::Reflection::meta::Variant const& GetDefaultValue(System::Reflection::meta::Type const* type) const = 0;

        /// <summary>
        /// Gets a validation function for the property on the specified type.
        /// </summary>
        /// <param name="type">The type.</param>
        /// <returns>
        /// The validation function, or null if no validation function exists.
        /// </returns>
        virtual IBindPropertyMetadata::ValidateFunction const& GetValidationFunc(System::Reflection::meta::Type const* type) const = 0;
    };

    struct DependencyPropertyChangedEventArgs : System::Events::EventArg
    {
        DependencyPropertyChangedEventArgs(
            IDependencyObject* sender,
            IPropertyObject* _property,
            System::Reflection::meta::Variant const& oldValue,
            System::Reflection::meta::Variant const& newValue)
            : Sender(sender)
            , Property(_property)
            , OldValue(oldValue)
            , NewValue(newValue)
        {
        }

        IDependencyObject* Sender;
        IPropertyObject* Property;
        Meta(Disable) System::Reflection::meta::Variant const& OldValue;
        Meta(Disable) System::Reflection::meta::Variant const& NewValue;
        BindingLevel Level;
    };

    struct IPropertyObject : System::Reflection::Object, System::EnableSharedFromThisEx<IPropertyObject>
    {
        typedef std::function<void(IDependencyObject*, DependencyPropertyChangedEventArgs&)> NotifyFunction;
        typedef rxcpp::observer<DependencyPropertyChangedEventArgs, 
                    rxcpp::detail::stateless_observer_tag,
                    void(DependencyPropertyChangedEventArgs const&),
                    void(std::exception_ptr),
                    void()> property_change_observer_t;

        typedef System::RX::Subject<DependencyPropertyChangedEventArgs> property_change_subcriber_t;

        struct Hasher
        {
            std::size_t operator()(IPropertyObject const* code) const
            {
                static std::hash<intptr_t> hasher;
                if (!code->_hashCache)
                    code->_hashCache = hasher((intptr_t)code);
                return code->_hashCache;
            }
        };

        struct EqualTo
        {
            bool operator()(IPropertyObject const* lhs, IPropertyObject const* rhs) const
            {
                return lhs == rhs;
            }
        };

        explicit IPropertyObject()
        {}
        virtual ~IPropertyObject()
        {}

        PURE_VIRTUAL_R_PROPERTY(uint, Id);

        PURE_VIRTUAL_R_PROPERTY(bool, Inherits);
        PURE_VIRTUAL_R_PROPERTY(bool, IsAttached);
        PURE_VIRTUAL_R_PROPERTY(bool, IsReadOnly);
        PURE_VIRTUAL_R_PROPERTY(bool, IsDirect);

        PURE_VIRTUAL_R_PROPERTY(std::string const&, Name);
        PURE_VIRTUAL_R_PROPERTY(System::Reflection::meta::Type const*, PropertyType);
        PURE_VIRTUAL_R_PROPERTY(System::Reflection::meta::Type const*, OwnerType);

        PURE_VIRTUAL_R_PROPERTY(NotifyFunction const&, Notifying);
        PURE_VIRTUAL_R_PROPERTY(property_change_subcriber_t&, Initialized);
        PURE_VIRTUAL_R_PROPERTY(property_change_subcriber_t&, Changed);

        virtual System::Reflection::meta::Variant GetValue(IDependencyObject* instance) const = 0;
        virtual void SetValue(IDependencyObject* instance, System::Reflection::meta::Variant const& value) = 0;
        virtual BindingMode GetDefaultBindingMode(System::Reflection::Type const*) = 0;

        virtual XamlCPP::Core::Dependency::IBindPropertyAccessor const* ToIBindPropertyAccessor() const { return nullptr; }
        virtual XamlCPP::Core::Dependency::IDirectPropertyAccessor const* ToIDirectPropertyAccessor() const { return nullptr; }

    private:

        mutable std::size_t _hashCache = 0;
    };
}

#endif // !__IPROPERTY_OBJECT_H__
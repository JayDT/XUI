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

#ifndef __XAMLCPP_INSTANCED_BINDING_H__
#define __XAMLCPP_INSTANCED_BINDING_H__

#include "standard/BasicPrimities.h"
#include "standard/events.h"
#include "standard/misc.h"
#include "Reflection/Runtime/Variant.h"
#include "Reflection/Runtime/Object.h"

#include "XamlCPP/DataBind/Binding.h"

#include "rxcpp/rx.hpp"

namespace XamlCPP::DataBind
{
    class BindingNotification;

    /// <summary>
    /// Holds the result of calling <see cref="IBinding.Initiate"/>.
    /// </summary>
    /// <remarks>
    /// Whereas an <see cref="IBinding"/> holds a description of a binding such as "Bind to the X
    /// property on a control's DataContext"; this class represents a binding that has been 
    /// *instanced* by calling <see cref="IBinding.Initiate(IAvaloniaObject, AvaloniaProperty, object, bool)"/>
    /// on a target object.
    /// 
    /// When a binding is initiated, it can return one of 3 possible sources for the binding:
    /// - An <see cref="ISubject{Object}"/> which can be used for any type of binding.
    /// - An <see cref="IObservable{Object}"/> which can be used for all types of bindings except
    ///  <see cref="BindingMode.OneWayToSource"/> and <see cref="BindingMode.TwoWay"/>.
    /// - A plain object, which can only represent a <see cref="BindingMode.OneTime"/> binding.
    /// </remarks>
    class TC_XAMLCPP_API InstancedBinding : public System::EnableSharedFromThisEx<InstancedBinding>
    {
        BindingMode _mode;
        BindingLevel _priority = BindingLevel::LocalValue;
        System::Reflection::meta::Variant _value;
        System::Reflection::meta::Variant _fallbackValue;
        rxcpp::subjects::subject<BindingNotification*>* _errors = nullptr;
        std::shared_ptr<Core::Dependency::GenericObservable> Observer;

        // Way One
        rxcpp::subscription _innerListenerSubcribe;
        Core::Dependency::GenericSubject* _innerSubject = nullptr;
        std::shared_ptr<SourceObserver> _inner;

        // Way Two
        rxcpp::subscription _secondaryListenerSubcribe;
        Core::Dependency::GenericSubject* _secondarySubject = nullptr;
        //std::shared_ptr<Observer::SourceObserver> _secondary;


    public:
        /// <summary>
        /// Initializes a new instance of the <see cref="InstancedBinding"/> class.
        /// </summary>
        /// <param name="value">
        /// The value used for the <see cref="BindingMode.OneTime"/> binding.
        /// </param>
        /// <param name="priority">The binding priority.</param>
        InstancedBinding(System::Reflection::meta::Variant const& value,
            BindingLevel priority = BindingLevel::LocalValue);

        /// <summary>
        /// Initializes a new instance of the <see cref="InstancedBinding"/> class.
        /// </summary>
        /// <param name="observable">The observable for a one-way binding.</param>
        /// <param name="mode">The binding mode.</param>
        /// <param name="priority">The binding priority.</param>
        InstancedBinding(
            std::shared_ptr<SourceObserver> observable,
            System::Reflection::meta::Variant const& fallbackValue,
            BindingMode mode = BindingMode::OneWay,
            BindingLevel priority = BindingLevel::LocalValue);

        /// <summary>
        /// Initializes a new instance of the <see cref="InstancedBinding"/> class.
        /// </summary>
        /// <param name="subject">The subject for a two-way binding.</param>
        /// <param name="mode">The binding mode.</param>
        /// <param name="priority">The binding priority.</param>
        InstancedBinding(
            std::shared_ptr<Core::Dependency::GenericObservable> const& subject,
            BindingMode mode = BindingMode::OneWay,
            BindingLevel priority = BindingLevel::LocalValue);

        virtual ~InstancedBinding();

        rxcpp::composite_subscription UnregisterSubcription;

        void CreatePrimaryWay();
        rxcpp::subscription CreateSecondaryWay(Core::Dependency::IDependencyObject * target, Core::Dependency::IPropertyObject * property);

        /// <summary>
        /// Gets the binding mode with which the binding was initiated.
        /// </summary>
        PROPERTY_G(_mode, Mode);

        /// <summary>
        /// Gets the binding priority.
        /// </summary>
        PROPERTY_G(_priority, Priority);

        /// <summary>
        /// Gets the value used for a <see cref="BindingMode.OneTime"/> binding.
        /// </summary>
        PROPERTY_(System::Reflection::meta::Variant const&, Value);
        System::Reflection::meta::Variant const& GetValue() const { return _value; }
        void GetValue(System::Reflection::meta::Variant const& value);

        /// <summary>
        /// Gets the subject for a OneWay binding.
        /// </summary>
        PROPERTY_GET_ONLY(Core::Dependency::GenericSubject*, Subject);
        Core::Dependency::GenericSubject* GetSubject();

        /// <summary>
        /// Gets the subject for a OneWay binding.
        /// </summary>
        PROPERTY_GET_ONLY(Core::Dependency::GenericSubject*, SecondarySubject);
        Core::Dependency::GenericSubject* GetSecondarySubject();

        /// Converter

        System::Reflection::meta::Variant ConverterParameter;
        Interfaces::IValueConverter* Converter = nullptr;
        Core::Dependency::IPropertyObject * TargetProperty = nullptr;

        /// Subriber Event Handlers

        /// <inheritdoc/>
        void OnCompleted()
        {
            if (_innerSubject && _innerSubject->has_observers())
                _innerSubject->get_observer().on_completed();
        }

        /// <inheritdoc/>
        void OnError(std::exception_ptr e)
        {
            if (_innerSubject && _innerSubject->has_observers())
                _innerSubject->get_observer().on_error(e);
        }

        /// <inheritdoc/>
        void OnNext(System::Reflection::meta::Variant const& value);

        /// <inheritdoc/>
        void OnSecondWayCompleted()
        {
            if (_secondarySubject && _secondarySubject->has_observers())
                _secondarySubject->get_observer().on_completed();
        }

        /// <inheritdoc/>
        void OnSecondWayError(std::exception_ptr e)
        {
            if (_secondarySubject && _secondarySubject->has_observers())
                _secondarySubject->get_observer().on_error(e);
        }

        /// <inheritdoc/>
        void OnSecondWayNext(System::Reflection::meta::Variant const& value);

        void UpdateSource(Core::Dependency::IDependencyObject* dp);
        void UpdateTarget();
    };
}

#endif // !__XAMLCPP_INSTANCED_BINDING_H__
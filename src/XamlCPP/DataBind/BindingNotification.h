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

#ifndef __XAMLCPP_BINDING_NOTIFICATION_H__
#define __XAMLCPP_BINDING_NOTIFICATION_H__

#include "standard/Errors.h"
#include "standard/BasicPrimities.h"
#include "Reflection/Runtime/Variant.h"
#include "Reflection/Runtime/Object.h"
#include "XamlCPP/Interfaces/IMarkupExtension.h"

namespace XamlCPP::DataBind
{
    /// <summary>
    /// Represents a binding notification that can be a valid binding value, or a binding or
    /// data validation error.
    /// </summary>
    class TC_XAMLCPP_API BindingNotification : public System::Reflection::meta::Object
    {
    private:
        System::Reflection::meta::Variant _value;

        std::shared_ptr<std::exception> _error;

    public:

        /// <summary>
        /// A binding notification representing the null value.
        /// </summary>
        //static const BindingNotification Null; // BindingNotification Null = BindingNotification(null)
        //static const BindingNotification UnsetValue; // = new BindingNotification(DependencyPropertyMgr.UnsetValue);

        // Null cannot be held in WeakReference as it's indistinguishable from an expired value so
        // use this value in its place.
        //static const System::Reflection::meta::Object* NullValue; // = new System::Reflection::meta::Object();

        BindingNotification();

        /// <summary>
        /// Initializes a new instance of the <see cref="BindingNotification"/> class.
        /// </summary>
        /// <param name="value">The binding value.</param>
        BindingNotification(System::Reflection::meta::Variant const& value);

        /// <summary>
        /// Initializes a new instance of the <see cref="BindingNotification"/> class.
        /// </summary>
        /// <param name="error">The binding error.</param>
        /// <param name="errorType">The type of the binding error.</param>
        BindingNotification(std::exception const* error, BindingErrorType errorType);


        /// <summary>
        /// Initializes a new instance of the <see cref="BindingNotification"/> class.
        /// </summary>
        /// <param name="error">The binding error.</param>
        /// <param name="errorType">The type of the binding error.</param>
        BindingNotification(std::exception const& error, BindingErrorType errorType);

        /// <summary>
        /// Initializes a new instance of the <see cref="BindingNotification"/> class.
        /// </summary>
        /// <param name="error">The binding error.</param>
        /// <param name="errorType">The type of the binding error.</param>
        /// <param name="fallbackValue">The fallback value.</param>
        BindingNotification(std::exception const* error, BindingErrorType errorType, System::Reflection::meta::Variant const& fallbackValue);

        /// <summary>
        /// Gets a value indicating whether <see cref="Value"/> should be pushed to the target.
        /// </summary>
        bool HasValue() const;

        PROPERTY_GET_ONLY(System::Reflection::meta::Variant, Value);
        System::Reflection::meta::Variant PROPERTY_GET(Value)() const;

        /// <summary>
        /// Gets the error that occurred on the source, if any.
        /// </summary>
        std::exception const* Error;

        /// <summary>
        /// Gets the type of error that <see cref="Error"/> represents, if any.
        /// </summary>
        BindingErrorType ErrorType;

        /// <summary>
        /// Compares two instances of <see cref="BindingNotification"/> for equality.
        /// </summary>
        /// <param name="a">The first instance.</param>
        /// <param name="b">The second instance.</param>
        /// <returns>true if the two instances are equal; otherwise false.</returns>
        bool operator==(BindingNotification const& b) const
        {
            if (this == &b)
            {
                return true;
            }

            return HasValue() == b.HasValue() &&
                ErrorType == b.ErrorType &&
                (!HasValue() || Value == b.Value) &&
                (ErrorType == BindingErrorType::None || typeid(Error) == typeid(b.Error));
        }

        /// <summary>
        /// Compares two instances of <see cref="BindingNotification"/> for inequality.
        /// </summary>
        /// <param name="a">The first instance.</param>
        /// <param name="b">The second instance.</param>
        /// <returns>true if the two instances are unequal; otherwise false.</returns>
        bool operator !=(BindingNotification const& b)
        {
            return !(*this == b);
        }

        /// <summary>
        /// Adds an error to the <see cref="BindingNotification"/>.
        /// </summary>
        /// <param name="e">The error to add.</param>
        /// <param name="type">The error type.</param>
        void AddError(std::exception const* e, BindingErrorType type);

        /// <summary>
        /// Removes the <see cref="Value"/> and makes <see cref="HasValue"/> return null.
        /// </summary>
        void ClearValue();

        /// <summary>
        /// Sets the <see cref="Value"/>.
        /// </summary>
        void SetValue(System::Reflection::meta::Variant const& value);

        /// <inheritdoc/>
        std::string ToString() const override;
    };
}

#endif // !__XAMLCPP_BINDING_NOTIFICATION_H__

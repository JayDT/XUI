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

#ifndef __IVALUE_CONVERTER_H__
#define __IVALUE_CONVERTER_H__

#include "Reflection/Runtime/Variant.h"

namespace XamlCPP::Interfaces
{
    /// <summary>
    /// Converts a binding value.
    /// </summary>
    struct IValueConverter : System::Reflection::Object
    {
        /// <summary>
        /// Converts a value.
        /// </summary>
        /// <param name="value">The value to convert.</param>
        /// <param name="targetType">The type of the target.</param>
        /// <param name="parameter">A user-defined parameter.</param>
        /// <param name="culture">The culture to use.</param>
        /// <returns>The converted value.</returns>
        /// <remarks>
        /// This method should not throw exceptions. If the value is not convertible, return
        /// a <see cref="BindingNotification"/> in an error state. Any exceptions thrown will be
        /// treated as an application exception.
        /// </remarks>
        virtual System::Reflection::meta::Variant Convert(System::Reflection::meta::Variant const& value, System::Reflection::meta::Type const* targetType, System::Reflection::meta::Variant const& parameter, uint32 culture) = 0;

        /// <summary>
        /// Converts a value.
        /// </summary>
        /// <param name="value">The value to convert.</param>
        /// <param name="targetType">The type of the target.</param>
        /// <param name="parameter">A user-defined parameter.</param>
        /// <param name="culture">The culture to use.</param>
        /// <returns>The converted value.</returns>
        /// <remarks>
        /// This method should not throw exceptions. If the value is not convertible, return
        /// a <see cref="BindingNotification"/> in an error state. Any exceptions thrown will be
        /// treated as an application exception.
        /// </remarks>
        virtual System::Reflection::meta::Variant ConvertBack(System::Reflection::meta::Variant const& value, System::Reflection::meta::Type const* targetType, System::Reflection::meta::Variant const& parameter, uint32 culture) = 0;
    };

    /// <summary>
    /// Converts multi-binding inputs to a final value.
    /// </summary>
    struct IMultiValueConverter : System::Reflection::Object
    {
        /// <summary>
        /// Converts multi-binding inputs to a final value.
        /// </summary>
        /// <param name="values">The values to convert.</param>
        /// <param name="targetType">The type of the target.</param>
        /// <param name="parameter">A user-defined parameter.</param>
        /// <param name="culture">The culture to use.</param>
        /// <returns>The converted value.</returns>
        /// <remarks>
        /// This method should not throw exceptions. If the value is not convertible, return
        /// <see cref="AvaloniaProperty.UnsetValue"/>. Any exception thrown will be treated as
        /// an application exception.
        /// </remarks>
        virtual System::Reflection::meta::Variant Convert(std::vector<System::Reflection::meta::Variant> const& value, System::Reflection::meta::Type const* targetType, System::Reflection::meta::Variant const& parameter, uint32 culture) = 0;
    };
}

#endif // !__IVALUE_CONVERTER_H__

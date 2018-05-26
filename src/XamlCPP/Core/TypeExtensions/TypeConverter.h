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

#ifndef __XUI_TYPE_CONVERTER_H__
#define __XUI_TYPE_CONVERTER_H__

#include "Reflection/Runtime/Variant.h"
#include "XamlCPP/Interfaces/IValueConverter.h"

namespace XamlCPP::Core::TypeExtensions
{
    struct IConvertible
    {
        virtual int ToInt(void) const = 0;
        virtual bool ToBool(void) const = 0;
        virtual float ToFloat(void) const = 0;
        virtual double ToDouble(void) const = 0;
        virtual long long ToLong(void) const = 0;
        virtual std::string ToString(void) const = 0;
    };

    struct TC_XAMLCPP_API TypeConverter
    {
        static bool TryCast(System::Reflection::meta::Type const* type, System::Reflection::meta::Variant const& value, System::Reflection::meta::Variant& result);
        static System::Reflection::meta::Variant CastOrDefault(System::Reflection::meta::Variant const& value, System::Reflection::meta::Type const* type);
    };

    /// <summary>
    /// Provides a default set of value conversions for bindings that do not specify a value
    /// converter.
    /// </summary>
    struct TC_XAMLCPP_API DefaultValueConverter : Interfaces::IValueConverter
    {
        /// <summary>
        /// Gets an instance of a <see cref="DefaultValueConverter"/>.
        /// </summary>
        static DefaultValueConverter Instance;

        /// <summary>
        /// Converts a value.
        /// </summary>
        /// <param name="value">The value to convert.</param>
        /// <param name="targetType">The type of the target.</param>
        /// <param name="parameter">A user-defined parameter.</param>
        /// <param name="culture">The culture to use.</param>
        /// <returns>The converted value.</returns>
        System::Reflection::meta::Variant Convert(System::Reflection::meta::Variant const& value, System::Reflection::meta::Type const* targetType, System::Reflection::meta::Variant const& parameter, uint32 culture);

        /// <summary>
        /// Converts a value.
        /// </summary>
        /// <param name="value">The value to convert.</param>
        /// <param name="targetType">The type of the target.</param>
        /// <param name="parameter">A user-defined parameter.</param>
        /// <param name="culture">The culture to use.</param>
        /// <returns>The converted value.</returns>
        System::Reflection::meta::Variant ConvertBack(System::Reflection::meta::Variant const& value, System::Reflection::meta::Type const* targetType, System::Reflection::meta::Variant const& parameter, uint32 culture)
        {
            return Convert(value, targetType, parameter, culture);
        }
    };
}

#endif // !__XUI_TYPE_CONVERTER_H__

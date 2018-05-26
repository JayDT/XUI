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

#ifndef __XAMLCPP_COMMONVALUECONVERSION_H__
#define __XAMLCPP_COMMONVALUECONVERSION_H__

#include <System/System.h>
#include "XamlCPP/Interfaces/ITypeConverter.h"
#include "XamlCPP/Interfaces/IValueContext.h"

namespace XamlCPP::Core::Proto
{
    class XamlType;
}

namespace XamlCPP::Core::ReflEx::TypeConversion
{
    class TC_XAMLCPP_API CommonValueConversion final
    {
    public:
        static bool TryConvert(System::Reflection::Variant const& value, Proto::XamlType *targetType, Interfaces::IValueContext *valueContext, System::Reflection::Variant &result);

        static bool EnumStepTryConvert(System::Reflection::Variant const& value, Proto::XamlType *xamlType, Interfaces::IValueContext *valueContext, System::Reflection::Variant &result);
        static bool SpecialTypesStepTryConvert(System::Reflection::Variant const& value, Proto::XamlType *targetType, Interfaces::IValueContext *valueContext, System::Reflection::Variant &result);
        static bool NonConversionStepTryConvert(System::Reflection::Variant const& value, Proto::XamlType *targetType, Interfaces::IValueContext *valueContext, System::Reflection::Variant &result);
        static bool ConverterStepTryConvert(System::Reflection::Variant const& value, Proto::XamlType *targetType, Interfaces::IValueContext *valueContext, System::Reflection::Variant &result);
    };
}


#endif	//#ifndef __XAMLCPP_COMMONVALUECONVERSION_H__

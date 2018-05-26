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

#pragma once
#ifndef __XAMLCPP_TYPECONVERTER_BUILDIN_H__
#define __XAMLCPP_TYPECONVERTER_BUILDIN_H__

#include <System/System.h>
#include "XamlCPP/Interfaces/ITypeConverter.h"
#include "XamlCPP/Interfaces/IValueContext.h"

namespace XamlCPP::Core::ReflEx::TypeConversion
{
    struct StringTypeConverter : public Interfaces::ITypeConverter
    {
    public:
        // Inherited via ITypeConverter
        virtual System::Reflection::Variant ConvertFrom(Interfaces::IValueContext * context, System::Reflection::Variant const& value) override;
        virtual System::Reflection::Variant ConvertTo(Interfaces::IValueContext * context, System::Reflection::Variant const& value, const System::Reflection::Type * destinationType) override;
        virtual bool CanConvertTo(Interfaces::IValueContext * context, const System::Reflection::Type * destinationType) override;
        virtual bool CanConvertFrom(Interfaces::IValueContext * context, const System::Reflection::Type * sourceType) override;
    };

    struct WStringTypeConverter : public Interfaces::ITypeConverter
    {
    public:
        // Inherited via ITypeConverter
        virtual System::Reflection::Variant ConvertFrom(Interfaces::IValueContext * context, System::Reflection::Variant const& value) override;
        virtual System::Reflection::Variant ConvertTo(Interfaces::IValueContext * context, System::Reflection::Variant const& value, const System::Reflection::Type * destinationType) override;
        virtual bool CanConvertTo(Interfaces::IValueContext * context, const System::Reflection::Type * destinationType) override;
        virtual bool CanConvertFrom(Interfaces::IValueContext * context, const System::Reflection::Type * sourceType) override;
    };

    struct IntTypeConverter : public Interfaces::ITypeConverter
    {
        // Inherited via ITypeConverter
        virtual System::Reflection::Variant ConvertFrom(Interfaces::IValueContext * context, System::Reflection::Variant const & value) override;
        virtual System::Reflection::Variant ConvertTo(Interfaces::IValueContext * context, System::Reflection::Variant const & value, const System::Reflection::Type * destinationType) override;
        virtual bool CanConvertTo(Interfaces::IValueContext * context, const System::Reflection::Type * destinationType) override;
        virtual bool CanConvertFrom(Interfaces::IValueContext * context, const System::Reflection::Type * sourceType) override;
    };

    struct UIntTypeConverter : public Interfaces::ITypeConverter
    {
        // Inherited via ITypeConverter
        virtual System::Reflection::Variant ConvertFrom(Interfaces::IValueContext * context, System::Reflection::Variant const & value) override;
        virtual System::Reflection::Variant ConvertTo(Interfaces::IValueContext * context, System::Reflection::Variant const & value, const System::Reflection::Type * destinationType) override;
        virtual bool CanConvertTo(Interfaces::IValueContext * context, const System::Reflection::Type * destinationType) override;
        virtual bool CanConvertFrom(Interfaces::IValueContext * context, const System::Reflection::Type * sourceType) override;
    };

    struct DoubleTypeConverter : public Interfaces::ITypeConverter
    {
        // Inherited via ITypeConverter
        virtual System::Reflection::Variant ConvertFrom(Interfaces::IValueContext * context, System::Reflection::Variant const & value) override;
        virtual System::Reflection::Variant ConvertTo(Interfaces::IValueContext * context, System::Reflection::Variant const & value, const System::Reflection::Type * destinationType) override;
        virtual bool CanConvertTo(Interfaces::IValueContext * context, const System::Reflection::Type * destinationType) override;
        virtual bool CanConvertFrom(Interfaces::IValueContext * context, const System::Reflection::Type * sourceType) override;
    };

    struct FloatTypeConverter : public Interfaces::ITypeConverter
    {
        // Inherited via ITypeConverter
        virtual System::Reflection::Variant ConvertFrom(Interfaces::IValueContext * context, System::Reflection::Variant const & value) override;
        virtual System::Reflection::Variant ConvertTo(Interfaces::IValueContext * context, System::Reflection::Variant const & value, const System::Reflection::Type * destinationType) override;
        virtual bool CanConvertTo(Interfaces::IValueContext * context, const System::Reflection::Type * destinationType) override;
        virtual bool CanConvertFrom(Interfaces::IValueContext * context, const System::Reflection::Type * sourceType) override;
    };

    struct BooleanTypeConverter : public Interfaces::ITypeConverter
    {
        // Inherited via ITypeConverter
        virtual System::Reflection::Variant ConvertFrom(Interfaces::IValueContext * context, System::Reflection::Variant const & value) override;
        virtual System::Reflection::Variant ConvertTo(Interfaces::IValueContext * context, System::Reflection::Variant const & value, const System::Reflection::Type * destinationType) override;
        virtual bool CanConvertTo(Interfaces::IValueContext * context, const System::Reflection::Type * destinationType) override;
        virtual bool CanConvertFrom(Interfaces::IValueContext * context, const System::Reflection::Type * sourceType) override;
    };

    struct TypeTypeConverter : public Interfaces::ITypeConverter
    {
        // Inherited via ITypeConverter
        virtual System::Reflection::Variant ConvertFrom(Interfaces::IValueContext * context, System::Reflection::Variant const & value) override;
        virtual System::Reflection::Variant ConvertTo(Interfaces::IValueContext * context, System::Reflection::Variant const & value, const System::Reflection::Type * destinationType) override;
        virtual bool CanConvertTo(Interfaces::IValueContext * context, const System::Reflection::Type * destinationType) override;
        virtual bool CanConvertFrom(Interfaces::IValueContext * context, const System::Reflection::Type * sourceType) override;
    };

    struct PropertyObjectTypeConverter : public Interfaces::ITypeConverter
    {
        // Inherited via ITypeConverter
        virtual System::Reflection::Variant ConvertFrom(Interfaces::IValueContext * context, System::Reflection::Variant const & value) override;
        virtual System::Reflection::Variant ConvertTo(Interfaces::IValueContext * context, System::Reflection::Variant const & value, const System::Reflection::Type * destinationType) override;
        virtual bool CanConvertTo(Interfaces::IValueContext * context, const System::Reflection::Type * destinationType) override;
        virtual bool CanConvertFrom(Interfaces::IValueContext * context, const System::Reflection::Type * sourceType) override;
    };

    struct SetterTriggerConditionalValueConverter : public Interfaces::ITypeConverter
    {
        // Inherited via ITypeConverter
        virtual System::Reflection::Variant ConvertFrom(Interfaces::IValueContext * context, System::Reflection::Variant const & value) override;
        virtual System::Reflection::Variant ConvertTo(Interfaces::IValueContext * context, System::Reflection::Variant const & value, const System::Reflection::Type * destinationType) override;
        virtual bool CanConvertTo(Interfaces::IValueContext * context, const System::Reflection::Type * destinationType) override;
        virtual bool CanConvertFrom(Interfaces::IValueContext * context, const System::Reflection::Type * sourceType) override;
    };

    struct EventHookValueConverter : public Interfaces::ITypeConverter
    {
        // Inherited via ITypeConverter
        virtual System::Reflection::Variant ConvertFrom(Interfaces::IValueContext * context, System::Reflection::Variant const & value) override;
        virtual System::Reflection::Variant ConvertTo(Interfaces::IValueContext * context, System::Reflection::Variant const & value, const System::Reflection::Type * destinationType) override;
        virtual bool CanConvertTo(Interfaces::IValueContext * context, const System::Reflection::Type * destinationType) override;
        virtual bool CanConvertFrom(Interfaces::IValueContext * context, const System::Reflection::Type * sourceType) override;
    };

	struct EventHookInstanceValueConverter : public Interfaces::ITypeConverter
	{
		// Inherited via ITypeConverter
		virtual System::Reflection::Variant ConvertFrom(Interfaces::IValueContext * context, System::Reflection::Variant const & value) override;
		virtual System::Reflection::Variant ConvertTo(Interfaces::IValueContext * context, System::Reflection::Variant const & value, const System::Reflection::Type * destinationType) override;
		virtual bool CanConvertTo(Interfaces::IValueContext * context, const System::Reflection::Type * destinationType) override;
		virtual bool CanConvertFrom(Interfaces::IValueContext * context, const System::Reflection::Type * sourceType) override;
	};
}
#endif //!__XAMLCPP_TYPECONVERTER_BUILDIN_H__
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
#include <System/System.h>
#include <standard/misc.h>

//Forward Declaration
namespace XamlCPP
{
    namespace Interfaces
    {
        struct ITypeRepository;
        struct ITypeFeatureProvider;
        struct IValueContext;
    }
}

namespace XamlCPP::Interfaces
{
    struct ITypeConverter : System::EnableSharedFromThisEx<ITypeConverter>
    {
        virtual System::Reflection::Variant ConvertFrom(IValueContext* context, System::Reflection::Variant const& value) = 0;
        virtual System::Reflection::Variant ConvertTo(IValueContext* context, System::Reflection::Variant const& value, const System::Reflection::Type* destinationType) = 0;
        virtual bool CanConvertTo(IValueContext* context, const System::Reflection::Type* destinationType) = 0;
        virtual bool CanConvertFrom(IValueContext* context, const System::Reflection::Type* sourceType) = 0;
    };

    template<typename TValue>
    class StringCompatibleTypeConverter : public XamlCPP::Interfaces::ITypeConverter
    {
    public:
        // Inherited via ITypeConverter
        virtual System::Reflection::Variant ConvertFrom(XamlCPP::Interfaces::IValueContext * context, System::Reflection::Variant const& value) override
        {
            return System::Reflection::Variant(TValue::Parse(value.ToString()), CopyVariantPolicy);
        }
        virtual System::Reflection::Variant ConvertTo(XamlCPP::Interfaces::IValueContext * context, System::Reflection::Variant const& value, const System::Reflection::Type * destinationType) override
        {
            return value.GetValue<TValue>().ToString();
        }
        virtual bool CanConvertTo(XamlCPP::Interfaces::IValueContext * context, const System::Reflection::Type * destinationType) override
        {
            return *destinationType == typeid(std::string);
        }
        virtual bool CanConvertFrom(XamlCPP::Interfaces::IValueContext * context, const System::Reflection::Type * sourceType) override
        {
            return *sourceType == typeid(std::string);
        }
    };

    template<typename TValue, typename TValueConverter = TValue>
    class StringCompatiblePointerTypeConverter : public XamlCPP::Interfaces::ITypeConverter
    {
    public:
        // Inherited via ITypeConverter
        virtual System::Reflection::Variant ConvertFrom(XamlCPP::Interfaces::IValueContext * context, System::Reflection::Variant const& value) override
        {
            System::Reflection::Variant result = std::static_pointer_cast<TValue>(TValueConverter::Parse(value.ToString()));
            return result;
        }
        virtual System::Reflection::Variant ConvertTo(XamlCPP::Interfaces::IValueContext * context, System::Reflection::Variant const& value, const System::Reflection::Type * destinationType) override
        {
            return value.GetValue<std::shared_ptr<TValue>>().get()->ToString();
        }
        virtual bool CanConvertTo(XamlCPP::Interfaces::IValueContext * context, const System::Reflection::Type * destinationType) override
        {
            return *destinationType == typeid(std::string);
        }
        virtual bool CanConvertFrom(XamlCPP::Interfaces::IValueContext * context, const System::Reflection::Type * sourceType) override
        {
            return *sourceType == typeid(std::string);
        }
    };

    template<typename TValue, typename TValueConverter = TValue>
    class EnumTypeConverter : public XamlCPP::Interfaces::ITypeConverter
    {
    public:
        // Inherited via ITypeConverter
        virtual System::Reflection::Variant ConvertFrom(XamlCPP::Interfaces::IValueContext * context, System::Reflection::Variant const& value) override
        {
            return TValueConverter::Parse(value.ToString());
        }
        virtual System::Reflection::Variant ConvertTo(XamlCPP::Interfaces::IValueContext * context, System::Reflection::Variant const& value, const System::Reflection::Type * destinationType) override
        {
            return TValueConverter::ToString(value.GetValue<TValue>());
        }
        virtual bool CanConvertTo(XamlCPP::Interfaces::IValueContext * context, const System::Reflection::Type * destinationType) override
        {
            return *destinationType == typeid(std::string);
        }
        virtual bool CanConvertFrom(XamlCPP::Interfaces::IValueContext * context, const System::Reflection::Type * sourceType) override
        {
            return *sourceType == typeid(std::string);
        }
    };
}

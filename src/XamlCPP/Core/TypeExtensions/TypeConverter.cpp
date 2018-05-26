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

#include "TypeConverter.h"
#include "Reflection/Runtime/CXXReflection.h"

#include "ReflEx/TypeConversion/BuildInConverters.h"
#include "Core/Dependencies/IDependencyObject.h"
#include "DataBind/BindingNotification.h"

#include "System/BaseTypes.h"
#include "Log/Log.h"

XamlCPP::Core::TypeExtensions::DefaultValueConverter XamlCPP::Core::TypeExtensions::DefaultValueConverter::Instance;
std::unordered_map<std::size_t, std::shared_ptr<XamlCPP::Interfaces::ITypeConverter>> converters;

void Register(std::size_t typehash, std::shared_ptr<XamlCPP::Interfaces::ITypeConverter> const & converter)
{
    converters[typehash] = converter;
}

bool RegisterBuiltIn()
{
    using namespace XamlCPP::Core::ReflEx;

    Register(typeid(std::string).hash_code(), std::make_shared<TypeConversion::StringTypeConverter>());
    Register(typeid(std::wstring).hash_code(), std::make_shared<TypeConversion::WStringTypeConverter>());
    Register(typeid(unsigned long).hash_code(), std::make_shared<TypeConversion::UIntTypeConverter>());
    Register(typeid(unsigned int).hash_code(), std::make_shared<TypeConversion::UIntTypeConverter>());
    Register(typeid(unsigned long).hash_code(), std::make_shared<TypeConversion::UIntTypeConverter>());
    Register(typeid(unsigned short).hash_code(), std::make_shared<TypeConversion::UIntTypeConverter>());
    Register(typeid(byte).hash_code(), std::make_shared<TypeConversion::UIntTypeConverter>());
    Register(typeid(long).hash_code(), std::make_shared<TypeConversion::IntTypeConverter>());
    Register(typeid(int).hash_code(), std::make_shared<TypeConversion::IntTypeConverter>());
    Register(typeid(long).hash_code(), std::make_shared<TypeConversion::IntTypeConverter>());
    Register(typeid(short).hash_code(), std::make_shared<TypeConversion::IntTypeConverter>());
    Register(typeid(sbyte).hash_code(), std::make_shared<TypeConversion::IntTypeConverter>());
    Register(typeid(double).hash_code(), std::make_shared<TypeConversion::DoubleTypeConverter>());
    Register(typeid(float).hash_code(), std::make_shared<TypeConversion::FloatTypeConverter>());
    Register(typeid(bool).hash_code(), std::make_shared<TypeConversion::BooleanTypeConverter>());
    Register(typeid(System::Reflection::Type).hash_code(), std::make_shared<TypeConversion::TypeTypeConverter>());
    Register(typeid(XamlCPP::Core::Dependency::IPropertyObject).hash_code(), std::make_shared<TypeConversion::PropertyObjectTypeConverter>());
    Register(typeid(TypeConversion::SetterTriggerConditionalValueConverter).hash_code(), std::make_shared<TypeConversion::SetterTriggerConditionalValueConverter>());
	

    return true;
}

bool IsNullable(System::Reflection::Type const & type)
{
    return type.ElaborateType && type.ElaborateType->IsValid();
}

XamlCPP::Interfaces::ITypeConverter * GetTypeConverter(std::size_t typehash)
{
    XamlCPP::Interfaces::ITypeConverter *converter = nullptr;
    System::Reflection::Type const& type = typeofbytypeid(typehash);
    if (IsNullable(type))
        typehash = type.ElaborateType->GetHashCode();

    auto converters_iterator = converters.find(typehash);
    return converters_iterator != converters.end() ? converters_iterator->second.get() : nullptr;
}

static bool InitializedTypeConverters = RegisterBuiltIn();

bool XamlCPP::Core::TypeExtensions::TypeConverter::TryCast(System::Reflection::meta::Type const * type, System::Reflection::meta::Variant const& value, System::Reflection::meta::Variant & result)
{
    if (!type || !value.GetType())
        return false; // Need fully implement native type support

    if (type == value.GetType())
    {
        if (&result != &value)
            result = value;
        return true;
    }

    if (*type == typeid(System::Reflection::Variant))
    {
        if (&result != &value)
            result = value;
        return true; // this is joker :]
    }

    if (type->IsEnum() && (value.GetType()->IsPrimitive() || value.GetType()->IsEnum()))
    {
        //if (!type->GetEnum()->GetKey(value.ToLong()).empty())
        result = type->TypeDescriptor->EnumValue(value.ToLong());
        return true;
    }

    if ((type->IsPrimitive()) && (value.GetType()->IsPrimitive() || value.GetType()->IsEnum()))
    {
        if (*type == typeid(bool))
            result = value.ToBool();
        else if (*type == typeid(char) ||
            *type == typeid(unsigned char) ||
            *type == typeid(short) ||
            *type == typeid(unsigned short) ||
            *type == typeid(int) ||
            *type == typeid(unsigned int))
            result = value.ToInt();
        else if (*type == typeid(long long))
            result = (long long)value.ToLong();
        else if (*type == typeid(unsigned long long))
            result = (unsigned long long)value.ToLong();
        else if (*type == typeid(float))
            result = value.ToFloat();
        else if (*type == typeid(double))
            result = value.ToDouble();
        else
            result = value;
        return true;
    }

    if (type->IsPointer() && value.IsPointer() && type->ElaborateType)
    {
        if (type->ElaborateType->DerivesFrom(*value.GetType()) || value.GetType()->DerivesFrom(*type->ElaborateType))
        {
            result = value;
            return true;
        }
    }
    else if (type->DerivesFrom(*value.GetType()) || value.GetType()->DerivesFrom(*type))
    {
        result = value;
        return true;
    }

    if (type->IsPrimitive())
    {
        static auto const& iconvertibletype = typeof(IConvertible);
        if (iconvertibletype.IsAssignableFrom(*value.GetType()))
        {
            if (System::Reflection::meta::Object* object = value.ToObject())
            {
                if (IConvertible* converter = (IConvertible*)(object))
                {
                    if (*type == typeid(bool))
                        result = converter->ToBool();
                    else if (*type == typeid(char) ||
                        *type == typeid(unsigned char) ||
                        *type == typeid(short) ||
                        *type == typeid(unsigned short) ||
                        *type == typeid(int) ||
                        *type == typeid(unsigned int))
                        result = converter->ToInt();
                    else if (*type == typeid(long long) ||
                        *type == typeid(unsigned long long))
                        result = converter->ToLong();
                    else if (*type == typeid(float))
                        result = converter->ToFloat();
                    else if (*type == typeid(double))
                        result = converter->ToDouble();
                    else
                        return false;

                    return true;
                }
            }
        }
    }

    return false;
}

System::Reflection::meta::Variant XamlCPP::Core::TypeExtensions::TypeConverter::CastOrDefault(System::Reflection::meta::Variant const & value, System::Reflection::meta::Type const * type)
{
    System::Reflection::meta::Variant result;
    TryCast(type, value, result);
    return result;
}

System::Reflection::meta::Variant XamlCPP::Core::TypeExtensions::DefaultValueConverter::Convert(System::Reflection::meta::Variant const& value, System::Reflection::meta::Type const* targetType, System::Reflection::meta::Variant const& parameter, uint32 culture)
{
    System::Reflection::meta::Variant result;

    if (!value.GetBase())
        return result;

    if (targetType == value.GetType())
        return value;

    if (TypeExtensions::TypeConverter::TryCast(
        targetType,
        value,
        result))
    {
        return result;
    }

    auto typeConverter = GetTypeConverter(targetType->native_hash_code);
    if (typeConverter != nullptr)
    {
        if (typeConverter->CanConvertFrom(nullptr, value.GetType()))
            return typeConverter->ConvertFrom(nullptr, value);
    }

    // ToDo: fix nullptr issues
    if (IsNullable(*targetType) && !value.GetType()->IsValid())
        return value;

    if (*targetType == typeid(System::Reflection::Object))
    {
        if (value.GetType()->IsPrimitive())
        {
            if (*value.GetType() == typeid(char))
            {
                return std::make_shared<System::Int8>(value.GetValue<int8>());
            }
            if (*value.GetType() == typeid(unsigned char))
            {
                return std::make_shared<System::UInt8>(value.GetValue<uint8>());
            }
            if (*value.GetType() == typeid(short))
            {
                return std::make_shared<System::Int16>(value.GetValue<int16>());
            }
            if (*value.GetType() == typeid(unsigned short))
            {
                return std::make_shared<System::UInt16>(value.GetValue<uint16>());
            }
            if (*value.GetType() == typeid(int))
            {
                return std::make_shared<System::Int32>(value.GetValue<int32>());
            }
            else if (*value.GetType() == typeid(unsigned int))
            {
                return std::make_shared<System::UInt32>(value.GetValue<uint32>());
            }
            if (*value.GetType() == typeid(long long))
            {
                return std::make_shared<System::Int64>(value.GetValue<int64>());
            }
            else if (*value.GetType() == typeid(unsigned long long))
            {
                return std::make_shared<System::UInt64>(value.GetValue<uint64>());
            }
            else if (*value.GetType() == typeid(bool))
            {
                return std::make_shared<System::UInt8>(uint8(value.GetValue<bool>()));
            }
            else if (*value.GetType() == typeid(float))
            {
                return std::make_shared<System::Float>(value.GetValue<float>());
            }
            else if (*value.GetType() == typeid(double))
            {
                return std::make_shared<System::Double>(value.GetValue<double>());
            }
        }
        else if (*value.GetType() == typeid(std::string))
        {
            return std::make_shared<System::String>(value.ToString());
        }
        else if (*value.GetType() == typeid(std::wstring))
        {
            return std::make_shared<System::String>(value.GetValue<std::wstring>());
        }
    }

    System::Console()->WriteErrorLine("Could not convert '%s' to '%s'.", value.GetType() ? value.GetType()->fullname : "<unknown>", targetType ? targetType->fullname : "<unset>");
    return std::make_shared<DataBind::BindingNotification>(new System::InvalidCastException("Could not convert '{value}' to '{targetType.Name}'."), BindingErrorType::Error);
}

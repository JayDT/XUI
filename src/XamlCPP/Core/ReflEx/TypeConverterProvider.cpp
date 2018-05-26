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

#include "TypeConverterProvider.h"
#include "Core/Dependencies/IPropertyObject.h"
#include "Core/TypeExtensions/TypeConverter.h"
#include "ReflEx/TypeConverterRegistration.h"
#include "ReflEx/TypeConversion/BuildInConverters.h"

#include <standard/events.h>

extern void Register(std::size_t typehash, std::shared_ptr<XamlCPP::Interfaces::ITypeConverter> const & converter);
extern XamlCPP::Interfaces::ITypeConverter * GetTypeConverter(std::size_t typehash);
extern std::unordered_map<std::size_t, std::shared_ptr<XamlCPP::Interfaces::ITypeConverter>> converters;

XamlCPP::Core::ReflEx::TypeConverterProvider::TypeConverterProvider()
{
    RegisterBuiltIn();
}

void XamlCPP::Core::ReflEx::TypeConverterProvider::RegisterBuiltIn()
{
    //Register(typeid(std::string).hash_code(), std::make_shared<TypeConversion::StringTypeConverter>());
    //Register(typeid(std::wstring).hash_code(), std::make_shared<TypeConversion::WStringTypeConverter>());
    //Register(typeid(unsigned long).hash_code(), std::make_shared<TypeConversion::UIntTypeConverter>());
    //Register(typeid(unsigned int).hash_code(), std::make_shared<TypeConversion::UIntTypeConverter>());
    //Register(typeid(unsigned long).hash_code(), std::make_shared<TypeConversion::UIntTypeConverter>());
    //Register(typeid(unsigned short).hash_code(), std::make_shared<TypeConversion::UIntTypeConverter>());
    //Register(typeid(byte).hash_code(), std::make_shared<TypeConversion::UIntTypeConverter>());
    //Register(typeid(long).hash_code(), std::make_shared<TypeConversion::IntTypeConverter>());
    //Register(typeid(int).hash_code(), std::make_shared<TypeConversion::IntTypeConverter>());
    //Register(typeid(long).hash_code(), std::make_shared<TypeConversion::IntTypeConverter>());
    //Register(typeid(short).hash_code(), std::make_shared<TypeConversion::IntTypeConverter>());
    //Register(typeid(sbyte).hash_code(), std::make_shared<TypeConversion::IntTypeConverter>());
    //Register(typeid(double).hash_code(), std::make_shared<TypeConversion::DoubleTypeConverter>());
    //Register(typeid(float).hash_code(), std::make_shared<TypeConversion::FloatTypeConverter>());
    //Register(typeid(bool).hash_code(), std::make_shared<TypeConversion::BooleanTypeConverter>());
    //Register(typeid(System::Reflection::Type).hash_code(), std::make_shared<TypeConversion::TypeTypeConverter>());
    //Register(typeid(Core::Dependency::IPropertyObject).hash_code(), std::make_shared<TypeConversion::PropertyObjectTypeConverter>());
    //Register(typeid(TypeConversion::SetterTriggerConditionalValueConverter).hash_code(), std::make_shared<TypeConversion::SetterTriggerConditionalValueConverter>());
    Register(typeid(TypeConversion::EventHookValueConverter).hash_code(), std::make_shared<TypeConversion::EventHookValueConverter>());
    Register(typeid(System::Events::IEventHook).hash_code(), std::make_shared<TypeConversion::EventHookValueConverter>());
	Register(typeid(TypeConversion::EventHookInstanceValueConverter).hash_code(), std::make_shared<TypeConversion::EventHookInstanceValueConverter>());
}

void XamlCPP::Core::ReflEx::TypeConverterProvider::Register(std::size_t typehash, std::shared_ptr<Interfaces::ITypeConverter> const & converter)
{
	::Register(typehash, converter);
    //converters[typehash] = converter;
}

XamlCPP::Interfaces::ITypeConverter * XamlCPP::Core::ReflEx::TypeConverterProvider::GetTypeConverter(std::size_t typehash)
{
    //Interfaces::ITypeConverter *converter = nullptr;
    //System::Reflection::Type const& type = typeofbytypeid(typehash);
    //if (IsNullable(type))
    //    typehash = type.ElaborateType->GetHashCode();
	//
    //auto converters_iterator = converters.find(typehash);
    //return converters_iterator != converters.end() ? converters_iterator->second.get() : nullptr;
	return ::GetTypeConverter(typehash);
}

const std::unordered_map<std::size_t, std::shared_ptr<XamlCPP::Interfaces::ITypeConverter>>& XamlCPP::Core::ReflEx::TypeConverterProvider::GetConverters() const
{
	return converters;
}

bool XamlCPP::Core::ReflEx::TypeConverterProvider::IsNullable(System::Reflection::Type const & type)
{
    return type.ElaborateType && type.ElaborateType->IsValid();
}

void XamlCPP::Core::ReflEx::TypeConverterProvider::Add(Core::TypeConverterRegistration const & item)
{
    Register(item.Typehash, item.getTypeConverter());
}

std::vector<XamlCPP::Core::TypeConverterRegistration> XamlCPP::Core::ReflEx::TypeConverterProvider::getTypeConverters()
{
    std::vector<Core::TypeConverterRegistration> result;
    for (auto const& conv : converters)
    {
        System::Reflection::Type const& type = typeofbytypeid(conv.first);
        result.emplace_back(&type, conv.second, conv.first);
    }
    return result;
}

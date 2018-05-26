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

#include "StdAfx.h"
#include "BuildInConverters.h"
#include "StateCommuter.h"
#include "StoredInstance.h"
#include "Interfaces/ITypeRepository.h"
#include "Proto/XamlType.h"
#include "Interfaces/IParser.h"
#include "XamlCPP.h"
#include "XamlCPP/Core/Dependencies/IDependencyObject.h"
#include "Interfaces/ISetter.h"
#include "Interfaces/ITriggerBase.h"
#include "ReflEx/TypeConversion/CommonValueConversion.h"
#include "Log/Log.h"

#include <standard/Errors.h>
#include <standard/events.h>

#include <boost/lexical_cast.hpp>

std::locale loc;

System::Reflection::Variant XamlCPP::Core::ReflEx::TypeConversion::StringTypeConverter::ConvertFrom(Interfaces::IValueContext * context, System::Reflection::Variant const& value)
{
    if (value.GetType()->IsEnum())
    {
        std::string enumKey = value.GetType()->GetEnum()->GetKey(value.ToLong());
        return enumKey;
    }
    else if (*value.GetType() == typeid(std::string))
    {
        return value;
    }
    if (*value.GetType() == typeid(std::wstring))
    {
        std::wstring str = value.GetValue<std::wstring>();
        return std::string(str.begin(), str.end());
    }
    if (*value.GetType() == typeid(const char*) || *value.GetType() == typeid(char*))
    {
        std::string str(*(const char**)value.toPointer());
        return System::Reflection::Variant(str);
    }
    if (*value.GetType() == typeid(const wchar_t*) || *value.GetType() == typeid(wchar_t*))
    {
        std::wstring str(*(const wchar_t**)value.toPointer());
        return std::string(str.begin(), str.end());
    }
    if (value.GetType()->IsPrimitive())
    {
        return value.ToString();
    }
    return System::Reflection::Variant::Empty;
}

System::Reflection::Variant XamlCPP::Core::ReflEx::TypeConversion::StringTypeConverter::ConvertTo(Interfaces::IValueContext * context, System::Reflection::Variant const& value, const System::Reflection::Type * destinationType)
{
    if (*destinationType == typeid(int))
    {
        auto str = value.GetValue<std::string>();
        if (!str.empty())
        {
            int value = atoi(str.c_str());
            return System::Reflection::Variant(value);
        }
    }
    else if (*destinationType == typeid(double))
    {
        auto str = value.GetValue<std::string>();
        if (!str.empty())
        {
            float value = std::stod(str.c_str());
            return System::Reflection::Variant(value);
        }
    }
    else if (*destinationType == typeid(float))
    {
        auto str = value.GetValue<std::string>();
        if (!str.empty())
        {
            float _value = std::stof(str.c_str());
            return System::Reflection::Variant(_value);
        }
    }
    else if (*destinationType == typeid(std::string))
    {
        return value;
    }

    return System::Reflection::Variant::Empty;
}

bool XamlCPP::Core::ReflEx::TypeConversion::StringTypeConverter::CanConvertTo(Interfaces::IValueContext * context, const System::Reflection::Type * destinationType)
{
    if (*destinationType == typeid(std::string) || *destinationType == typeid(int) || *destinationType == typeid(double) || *destinationType == typeid(float))
    {
        return true;
    }
    return false;
}

bool XamlCPP::Core::ReflEx::TypeConversion::StringTypeConverter::CanConvertFrom(Interfaces::IValueContext * context, const System::Reflection::Type * sourceType)
{
    return *sourceType == typeid(int) || *sourceType == typeid(double) || *sourceType == typeid(float) || *sourceType == typeid(std::wstring) || sourceType->IsEnum();
}


System::Reflection::Variant XamlCPP::Core::ReflEx::TypeConversion::WStringTypeConverter::ConvertFrom(Interfaces::IValueContext * context, System::Reflection::Variant const& value)
{
    static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    if (value.GetType()->IsEnum())
    {
        std::string enumKey = value.GetType()->GetEnum()->GetKey(value.ToLong());
        return converter.from_bytes(enumKey);
    }
    else if (*value.GetType() == typeid(std::string))
    {
        std::string str = value.GetValue<std::string>();
        //return std::wstring(str.begin(), str.end());
        return converter.from_bytes(str);
    }
    if (*value.GetType() == typeid(std::wstring))
    {
        return value;
    }
    if (*value.GetType() == typeid(const char*) || *value.GetType() == typeid(char*))
    {
        std::string str(*(const char**)value.toPointer());
        //return std::wstring(str.begin(), str.end());
        return converter.from_bytes(str);
    }
    if (*value.GetType() == typeid(const wchar_t*) || *value.GetType() == typeid(wchar_t*))
    {
        std::wstring str(*(const wchar_t**)value.toPointer());
        return str;
    }
    if (typeof(System::String).IsAssignableFrom(*value.GetType()))
    {
        auto string = (System::String*)value.toPointer();
        //return string->ToWString();
        return converter.from_bytes(*string);
    }
    if (value.GetType()->IsPrimitive())
    {
        std::string str = value.ToString();
        //return std::wstring(str.begin(), str.end());
        return converter.from_bytes(str);
    }
    return System::Reflection::Variant::Empty;
}

System::Reflection::Variant XamlCPP::Core::ReflEx::TypeConversion::WStringTypeConverter::ConvertTo(Interfaces::IValueContext * context, System::Reflection::Variant const& value, const System::Reflection::Type * destinationType)
{
    if (*destinationType == typeid(int))
    {
        auto str = value.GetValue<std::wstring>();
        if (!str.empty())
        {
            int value = std::stoi(str);
            return System::Reflection::Variant(value);
        }
    }
    else if (*destinationType == typeid(double))
    {
        auto str = value.GetValue<std::wstring>();
        if (!str.empty())
        {
            float value = std::stod(str);
            return System::Reflection::Variant(value);
        }
    }
    else if (*destinationType == typeid(float))
    {
        auto str = value.GetValue<std::wstring>();
        if (!str.empty())
        {
            float _value = std::stof(str);
            return System::Reflection::Variant(_value);
        }
    }

    return System::Reflection::Variant::Empty;
}

bool XamlCPP::Core::ReflEx::TypeConversion::WStringTypeConverter::CanConvertTo(Interfaces::IValueContext * context, const System::Reflection::Type * destinationType)
{
    if (*destinationType == typeid(std::wstring) || *destinationType == typeid(int) || *destinationType == typeid(double) || *destinationType == typeid(float))
    {
        return true;
    }
    return false;
}

bool XamlCPP::Core::ReflEx::TypeConversion::WStringTypeConverter::CanConvertFrom(Interfaces::IValueContext * context, const System::Reflection::Type * sourceType)
{
    return *sourceType == typeid(int) || *sourceType == typeid(double) || *sourceType == typeid(float) || *sourceType == typeid(std::string) || typeof(System::String).IsAssignableFrom(*sourceType) || sourceType->IsEnum();
}

System::Reflection::Variant XamlCPP::Core::ReflEx::TypeConversion::IntTypeConverter::ConvertFrom(Interfaces::IValueContext * context, System::Reflection::Variant const & value)
{
    if (*value.GetType() == typeid(std::string))
    {
        auto str = value.GetValue<std::string>();
        if (!str.empty())
        {
            int value = atoi(str.c_str());
            return System::Reflection::Variant(value);
        }
    }
    else if (*value.GetType() == typeid(std::wstring))
    {
        auto str = value.GetValue<std::wstring>();
        if (!str.empty())
        {
            try
            {
                int value = std::stoi(str);
                return System::Reflection::Variant(value);
            }
            catch (...)
            {
                return 0;
            }
        }
    }
    else if (*value.GetType() == typeid(int))
    {
        return value;
    }
    else if (*value.GetType() == typeid(long))
    {
        int destvalue = int(value.GetValue<long>());
        return System::Reflection::Variant(destvalue);
    }
    else if (*value.GetType() == typeid(short))
    {
        int destvalue = int(value.GetValue<short>());
        return System::Reflection::Variant(destvalue);
    }
    else if (*value.GetType() == typeid(float))
    {
        int destvalue = int(value.GetValue<float>());
        return System::Reflection::Variant(destvalue);
    }
    else if (*value.GetType() == typeid(bool))
    {
        int destvalue = int(value.GetValue<bool>());
        return System::Reflection::Variant(destvalue);
    }
    else if (*value.GetType() == typeid(sbyte))
    {
        int destvalue = int(value.GetValue<sbyte>());
        return System::Reflection::Variant(destvalue);
    }
    return value.ToInt();
}

System::Reflection::Variant XamlCPP::Core::ReflEx::TypeConversion::IntTypeConverter::ConvertTo(Interfaces::IValueContext * context, System::Reflection::Variant const & value, const System::Reflection::Type * destinationType)
{
    if (*destinationType == typeid(std::string))
    {
        int destvalue = value.ToInt();
        return System::Reflection::Variant(std::to_string(destvalue));
    }
    return System::Reflection::Variant::Empty;
}

bool XamlCPP::Core::ReflEx::TypeConversion::IntTypeConverter::CanConvertTo(Interfaces::IValueContext * context, const System::Reflection::Type * destinationType)
{
    return true;
}

bool XamlCPP::Core::ReflEx::TypeConversion::IntTypeConverter::CanConvertFrom(Interfaces::IValueContext * context, const System::Reflection::Type * sourceType)
{
    return *sourceType == typeid(std::string) || *sourceType == typeid(std::wstring) || *sourceType == typeid(long) || *sourceType == typeid(int) || *sourceType == typeid(short) || *sourceType == typeid(float) || *sourceType == typeid(bool) || *sourceType == typeid(sbyte);
}

System::Reflection::Variant XamlCPP::Core::ReflEx::TypeConversion::UIntTypeConverter::ConvertFrom(Interfaces::IValueContext * context, System::Reflection::Variant const & value)
{
    if (*value.GetType() == typeid(std::string))
    {
        auto str = value.GetValue<std::string>();
        if (!str.empty())
        {
            int value = atoi(str.c_str());
            return System::Reflection::Variant(value);
        }
    }
    else if (*value.GetType() == typeid(std::wstring))
    {
        auto str = value.GetValue<std::wstring>();
        if (!str.empty())
        {
            try
            {
                uint value = std::stoul(str);
                return System::Reflection::Variant(value);
            }
            catch (...)
            {
                return 0;
            }
        }
    }
    else if (*value.GetType() == typeid(unsigned int))
    {
        return value;
    }
    else if (*value.GetType() == typeid(unsigned long))
    {
        unsigned int destvalue = unsigned int(value.GetValue<unsigned long>());
        return System::Reflection::Variant(destvalue);
    }
    else if (*value.GetType() == typeid(unsigned short))
    {
        unsigned int destvalue = unsigned int(value.GetValue<unsigned short>());
        return System::Reflection::Variant(destvalue);
    }
    else if (*value.GetType() == typeid(float))
    {
        unsigned int destvalue = unsigned int(value.GetValue<float>());
        return System::Reflection::Variant(destvalue);
    }
    else if (*value.GetType() == typeid(bool))
    {
        unsigned int destvalue = unsigned int(value.GetValue<bool>());
        return System::Reflection::Variant(destvalue);
    }
    else if (*value.GetType() == typeid(byte))
    {
        unsigned int destvalue = unsigned int(value.GetValue<byte>());
        return System::Reflection::Variant(destvalue);
    }
    return (unsigned int)value.ToInt();
}

System::Reflection::Variant XamlCPP::Core::ReflEx::TypeConversion::UIntTypeConverter::ConvertTo(Interfaces::IValueContext * context, System::Reflection::Variant const & value, const System::Reflection::Type * destinationType)
{
    if (*destinationType == typeid(std::string))
    {
        uint destvalue = value.GetValue<unsigned int>();
        return System::Reflection::Variant(std::to_string(destvalue));
    }
    return System::Reflection::Variant::Empty;
}

bool XamlCPP::Core::ReflEx::TypeConversion::UIntTypeConverter::CanConvertTo(Interfaces::IValueContext * context, const System::Reflection::Type * destinationType)
{
    return true;
}

bool XamlCPP::Core::ReflEx::TypeConversion::UIntTypeConverter::CanConvertFrom(Interfaces::IValueContext * context, const System::Reflection::Type * sourceType)
{
    return *sourceType == typeid(std::string) || *sourceType == typeid(std::wstring) || *sourceType == typeid(unsigned long) || *sourceType == typeid(unsigned int) || *sourceType == typeid(unsigned short) || *sourceType == typeid(float) || *sourceType == typeid(bool) || *sourceType == typeid(byte);
}

System::Reflection::Variant XamlCPP::Core::ReflEx::TypeConversion::DoubleTypeConverter::ConvertFrom(Interfaces::IValueContext * context, System::Reflection::Variant const & value)
{
    if (*value.GetType() == typeid(std::string))
    {
        auto str = value.GetValue<std::string>();
        if (!str.empty())
        {
            std::stringstream ss(str);
            ss.imbue(loc);
            double value;
            ss >> value;
            return System::Reflection::Variant(value);
        }
    }
    else if (*value.GetType() == typeid(std::wstring))
    {
        auto str = value.GetValue<std::wstring>();
        if (!str.empty())
        {
            std::wstringstream ss(str);
            ss.imbue(loc);
            double value;
            ss >> value;
            return System::Reflection::Variant(value);
        }
    }
    return value.ToDouble();
}

System::Reflection::Variant XamlCPP::Core::ReflEx::TypeConversion::DoubleTypeConverter::ConvertTo(Interfaces::IValueContext * context, System::Reflection::Variant const & value, const System::Reflection::Type * destinationType)
{
    if (*destinationType == typeid(std::string))
    {
        double destvalue = value.ToDouble();
        return System::Reflection::Variant(std::to_string(destvalue));
    }
    return System::Reflection::Variant::Empty;
}

bool XamlCPP::Core::ReflEx::TypeConversion::DoubleTypeConverter::CanConvertTo(Interfaces::IValueContext * context, const System::Reflection::Type * destinationType)
{
    return true;
}

bool XamlCPP::Core::ReflEx::TypeConversion::DoubleTypeConverter::CanConvertFrom(Interfaces::IValueContext * context, const System::Reflection::Type * sourceType)
{
    return *sourceType == typeid(std::string) || *sourceType == typeid(std::wstring);
}

System::Reflection::Variant XamlCPP::Core::ReflEx::TypeConversion::FloatTypeConverter::ConvertFrom(Interfaces::IValueContext * context, System::Reflection::Variant const & value)
{
    if (*value.GetType() == typeid(std::string))
    {
        auto str = value.GetValue<std::string>();
        if (!str.empty())
        {
            std::stringstream ss(str);
            ss.imbue(loc);
            float value;
            ss >> value;
            return System::Reflection::Variant(value);
        }
    }
    else if (*value.GetType() == typeid(std::wstring))
    {
        auto str = value.GetValue<std::wstring>();
        if (!str.empty())
        {
            std::wstringstream ss(str);
            ss.imbue(loc);
            float value;
            ss >> value;
            return System::Reflection::Variant(value);
        }
    }
    return value.ToDouble();
}

System::Reflection::Variant XamlCPP::Core::ReflEx::TypeConversion::FloatTypeConverter::ConvertTo(Interfaces::IValueContext * context, System::Reflection::Variant const & value, const System::Reflection::Type * destinationType)
{
    if (*destinationType == typeid(std::string))
    {
        float destvalue = value.ToFloat();
        return System::Reflection::Variant(std::to_string(destvalue));
    }
    if (*destinationType == typeid(std::wstring))
    {
        float destvalue = value.ToFloat();
        return System::Reflection::Variant(std::to_wstring(destvalue));
    }
    return System::Reflection::Variant::Empty;
}

bool XamlCPP::Core::ReflEx::TypeConversion::FloatTypeConverter::CanConvertTo(Interfaces::IValueContext * context, const System::Reflection::Type * destinationType)
{
    return true;
}

bool XamlCPP::Core::ReflEx::TypeConversion::FloatTypeConverter::CanConvertFrom(Interfaces::IValueContext * context, const System::Reflection::Type * sourceType)
{
    return *sourceType == typeid(std::string) || *sourceType == typeid(std::wstring);
}

System::Reflection::Variant XamlCPP::Core::ReflEx::TypeConversion::BooleanTypeConverter::ConvertFrom(Interfaces::IValueContext * context, System::Reflection::Variant const & value)
{
    if (*value.GetType() == typeid(std::string))
    {
        auto str = value.GetValue<std::string>();
        if (str.empty())
            return false;

        if (isdigit(str[0]))
            return boost::lexical_cast<bool>(str);

        if (System::String(str).ToLower() == "true")
            return true;
        return false;
    }
    else if (*value.GetType() == typeid(std::wstring))
    {
        auto str = value.GetValue<std::wstring>();
        if (str.empty())
            return false;

        if (iswdigit(str[0]))
            return boost::lexical_cast<bool>(str);

        if (System::String(str).ToLower() == "true")
            return true;
        return false;
    }
    return value.ToBool();
}

System::Reflection::Variant XamlCPP::Core::ReflEx::TypeConversion::BooleanTypeConverter::ConvertTo(Interfaces::IValueContext * context, System::Reflection::Variant const & value, const System::Reflection::Type * destinationType)
{
    if (*destinationType == typeid(std::string))
    {
        bool destvalue = value.ToBool();
        return System::Reflection::Variant(std::to_string(destvalue));
    }
    return System::Reflection::Variant::Empty;
}

bool XamlCPP::Core::ReflEx::TypeConversion::BooleanTypeConverter::CanConvertTo(Interfaces::IValueContext * context, const System::Reflection::Type * destinationType)
{
    return true;
}

bool XamlCPP::Core::ReflEx::TypeConversion::BooleanTypeConverter::CanConvertFrom(Interfaces::IValueContext * context, const System::Reflection::Type * sourceType)
{
    return *sourceType == typeid(std::string) || *sourceType == typeid(std::wstring);
}

System::Reflection::Variant XamlCPP::Core::ReflEx::TypeConversion::TypeTypeConverter::ConvertFrom(Interfaces::IValueContext * context, System::Reflection::Variant const & value)
{
    auto qualifiedTypeName = value.GetValue<std::string>();
    auto xamlType = context->getTypeRepository()->GetByQualifiedName(qualifiedTypeName);
    if (xamlType)
        return xamlType->UnderlyingType();
    return System::Reflection::Variant::Empty;
}

System::Reflection::Variant XamlCPP::Core::ReflEx::TypeConversion::TypeTypeConverter::ConvertTo(Interfaces::IValueContext * context, System::Reflection::Variant const & value, const System::Reflection::Type * destinationType)
{
    throw System::NotImplementedException();
    return System::Reflection::Variant();
}

bool XamlCPP::Core::ReflEx::TypeConversion::TypeTypeConverter::CanConvertTo(Interfaces::IValueContext * context, const System::Reflection::Type * destinationType)
{
    return *destinationType == typeid(std::string);
}

bool XamlCPP::Core::ReflEx::TypeConversion::TypeTypeConverter::CanConvertFrom(Interfaces::IValueContext * context, const System::Reflection::Type * sourceType)
{
    return *sourceType == typeid(std::string);
}

System::Reflection::Variant XamlCPP::Core::ReflEx::TypeConversion::PropertyObjectTypeConverter::ConvertFrom(Interfaces::IValueContext * context, System::Reflection::Variant const & value)
{
    auto PropertyMgr = Xaml::UIServiceContext->Get<XamlCPP::Core::Dependency::IDependencyPropertyMgr>();

    std::string targetPropertyName = value.ToString();
    if (targetPropertyName.find('.') != std::string::npos)
    {
        auto parts = System::String(targetPropertyName).Split('.');
        auto xamlType = context->getTypeRepository()->GetByQualifiedName(parts[0]);
        if (xamlType)
        {
            auto Property = PropertyMgr->FindRegistered(xamlType->UnderlyingType(), parts[1]);
            if (Property)
                return Property;
        }
    }

    static std::string StyleName = "XUI::Styling::Style";
    static std::string ControlTemplateName = "XUI::UI::DataTemplates::ControlTemplate";
    //static std::string TriggerName = "XUI::Styling::Trigger";

    auto storage = context->m_StateCommuter->getStack();
    for (auto iInstance = storage->rbegin(); iInstance != storage->rend(); ++iInstance)
    {
        auto lvl = *iInstance;
        if (lvl->GetXamlType() && (StyleName == lvl->GetXamlType()->UnderlyingType()->GetFullName() || ControlTemplateName == lvl->GetXamlType()->UnderlyingType()->GetFullName()))
        {
            auto const& rtProperty = lvl->GetXamlType()->UnderlyingType()->GetProperty("TargetType");
            if (rtProperty.IsValid())
            {
                System::Reflection::Type const* objectType = rtProperty.GetValue(lvl->GetInstance().ToObject()).GetValue<System::Reflection::Type const*>();
                if (!objectType)
                    continue;

                auto Property = PropertyMgr->FindRegistered(objectType, targetPropertyName);
                if (Property)
                    return Property;
            }
        }
    }
    throw XamlCPP::ParseException("Invalid Property");
    //return System::Reflection::Variant();
}

System::Reflection::Variant XamlCPP::Core::ReflEx::TypeConversion::PropertyObjectTypeConverter::ConvertTo(Interfaces::IValueContext * context, System::Reflection::Variant const & value, const System::Reflection::Type * destinationType)
{
    throw System::NotImplementedException();
    return System::Reflection::Variant();
}

bool XamlCPP::Core::ReflEx::TypeConversion::PropertyObjectTypeConverter::CanConvertTo(Interfaces::IValueContext * context, const System::Reflection::Type * destinationType)
{
    return *destinationType == typeid(std::string);
}

bool XamlCPP::Core::ReflEx::TypeConversion::PropertyObjectTypeConverter::CanConvertFrom(Interfaces::IValueContext * context, const System::Reflection::Type * sourceType)
{
    return *sourceType == typeid(std::string);
}

System::Reflection::Variant XamlCPP::Core::ReflEx::TypeConversion::SetterTriggerConditionalValueConverter::ConvertFrom(Interfaces::IValueContext * context, System::Reflection::Variant const & value)
{
    // TODO: implement orderware (currentyl need first property field)
    if (context->m_StateCommuter)
    {
        System::Reflection::Object* instance = context->m_StateCommuter->getPrevious()->getInstance().ToObject();

        if (instance && context->m_StateCommuter->getPrevious()->getMember() && (instance->Is<XamlCPP::Interfaces::ISetter>() || instance->Is<XamlCPP::Interfaces::ITriggerBase>()) && *value.GetType() == typeid(std::string))
        {
            if (context->m_StateCommuter->getPrevious()->getMember()->Name == "Value")
            {
                Proto::XamlType* xamlType = nullptr;
                XamlCPP::Core::Dependency::IPropertyObject * property = instance->GetType()->GetRuntimeProperty("Property").GetValue(instance).GetValue<XamlCPP::Core::Dependency::IPropertyObject *>();
                if (property && property->PropertyType->IsValid())
                    xamlType = context->getTypeRepository()->GetByType(property->PropertyType);

                if (xamlType)
                {
                    auto typeConverter = xamlType->getTypeConverter();
                    if (typeConverter != nullptr)
                    {
                        if (typeConverter->CanConvertFrom(context, value.GetType()))
                        {
                            System::Reflection::Variant result = typeConverter->ConvertFrom(context, value);
                            return result;
                        }
                    }
                    else if (xamlType->UnderlyingType()->IsEnum())
                    {
                        System::Reflection::Variant result = System::Reflection::Enum::Parse(*xamlType->UnderlyingType(), value.ToString());
                        return result;
                    }
                }
            }
        }
    }
    return value;
}

System::Reflection::Variant XamlCPP::Core::ReflEx::TypeConversion::SetterTriggerConditionalValueConverter::ConvertTo(Interfaces::IValueContext * context, System::Reflection::Variant const & value, const System::Reflection::Type * destinationType)
{
    return System::Reflection::Variant();
}

bool XamlCPP::Core::ReflEx::TypeConversion::SetterTriggerConditionalValueConverter::CanConvertTo(Interfaces::IValueContext * context, const System::Reflection::Type * destinationType)
{
    return true;
}

bool XamlCPP::Core::ReflEx::TypeConversion::SetterTriggerConditionalValueConverter::CanConvertFrom(Interfaces::IValueContext * context, const System::Reflection::Type * sourceType)
{
    return true;
}

/// Event Hook Converter

System::Reflection::Variant XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter::ConvertFrom(Interfaces::IValueContext * context, System::Reflection::Variant const & value)
{
    if (context->m_StateCommuter && context->m_StateCommuter->GetTopLevel())
    {
        std::string methodName = value.ToString();
        const System::Reflection::Method& method = context->m_StateCommuter->GetTopLevel()->GetType()->GetRuntimeMethod(methodName);
        if (method.IsValid())
            return System::make_shared_eventhandler(&method, context->m_StateCommuter->GetTopLevel().get());
		else
			System::Console()->WriteErrorLine("[XAMLCPP]: Not found method (%s) from (%s)", methodName.c_str(), context->m_StateCommuter->GetTopLevel()->GetType()->GetName());
	}
    return nullptr;
}

System::Reflection::Variant XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter::ConvertTo(Interfaces::IValueContext * context, System::Reflection::Variant const & value, const System::Reflection::Type * destinationType)
{
    return System::Reflection::Variant();
}

bool XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter::CanConvertTo(Interfaces::IValueContext * context, const System::Reflection::Type * destinationType)
{
    return *destinationType == typeid(std::string);
}

bool XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter::CanConvertFrom(Interfaces::IValueContext * context, const System::Reflection::Type * sourceType)
{
    return *sourceType == typeid(std::string);
}

System::Reflection::Variant XamlCPP::Core::ReflEx::TypeConversion::EventHookInstanceValueConverter::ConvertFrom(Interfaces::IValueContext * context, System::Reflection::Variant const & value)
{
	if (context->m_StateCommuter && context->m_StateCommuter->GetTopLevel())
	{
		std::string methodName = value.ToString();
		const System::Reflection::Method& method = context->m_StateCommuter->GetTopLevel()->GetType()->GetRuntimeMethod(methodName);
		if (method.IsValid())
			return System::make_eventhandler(&method, context->m_StateCommuter->GetTopLevel().get());
		else
			System::Console()->WriteErrorLine("[XAMLCPP]: Not found method (%s) from (%s)", methodName.c_str(), context->m_StateCommuter->GetTopLevel()->GetType()->GetName());
	}
	return nullptr;
}

System::Reflection::Variant XamlCPP::Core::ReflEx::TypeConversion::EventHookInstanceValueConverter::ConvertTo(Interfaces::IValueContext * context, System::Reflection::Variant const & value, const System::Reflection::Type * destinationType)
{
	return System::Reflection::Variant();
}

bool XamlCPP::Core::ReflEx::TypeConversion::EventHookInstanceValueConverter::CanConvertTo(Interfaces::IValueContext * context, const System::Reflection::Type * destinationType)
{
	return *destinationType == typeid(std::string);
}

bool XamlCPP::Core::ReflEx::TypeConversion::EventHookInstanceValueConverter::CanConvertFrom(Interfaces::IValueContext * context, const System::Reflection::Type * sourceType)
{
	return *sourceType == typeid(std::string);
}
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

#include "CommonValueConversion.h"
#include "XamlType.h"
#include "Runtime/Enum.h"

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

#include <System/BaseTypes.h>

using namespace XamlCPP;
using namespace XamlCPP::Core;
using namespace XamlCPP::Core::ReflEx::TypeConversion;

struct pCommonConverterHandle
{
    bool (*Handler)(System::Reflection::Variant const& value, Proto::XamlType *xamlType, Interfaces::IValueContext *valueContext, System::Reflection::Variant &result);
};

pCommonConverterHandle ConverterHandlers[] =
{
    { &CommonValueConversion::NonConversionStepTryConvert },
    { &CommonValueConversion::SpecialTypesStepTryConvert },
    { &CommonValueConversion::EnumStepTryConvert },
    { &CommonValueConversion::ConverterStepTryConvert },
    { nullptr }
};

bool CommonValueConversion::TryConvert(System::Reflection::Variant const& value, Proto::XamlType *targetType, Interfaces::IValueContext *valueContext, System::Reflection::Variant &result)
{
    result = nullptr;
    int8 stage = 0;
    while (true)
    {
        auto handler = ConverterHandlers[stage];
        if (!handler.Handler)
            break;

        if (handler.Handler(value, targetType, valueContext, result))
            return true;

        ++stage;
    }

    if (*targetType->UnderlyingType() == typeid(System::Reflection::Variant))
    {
        result = value;
        if (result.GetBase())
            return true;
    }

    return false;
}

bool CommonValueConversion::EnumStepTryConvert(System::Reflection::Variant const& value, Proto::XamlType *xamlType, Interfaces::IValueContext *valueContext, System::Reflection::Variant &result)
{
    auto targetType = xamlType->UnderlyingType();
    
    result = nullptr;
    if (targetType->ElaborateType)
    {
        auto typeInfo = targetType->ElaborateType;
        if (typeInfo != nullptr && typeInfo->IsEnum())
        {
            result = System::Reflection::meta::Enum::Parse(*typeInfo, value.ToString());
            if (result.GetBase())
                return true;
        }
    }
    
    if (targetType->IsEnum())
    {
        result = System::Reflection::meta::Enum::Parse(*targetType, value.ToString());
        if (result.GetBase())
            return true;
    }

    return false;
}

bool CommonValueConversion::SpecialTypesStepTryConvert(System::Reflection::Variant const& value, Proto::XamlType *targetType, Interfaces::IValueContext *valueContext, System::Reflection::Variant &result)
{
    auto type = value.GetType();
    
    if (*type == typeid(std::string) && *targetType->UnderlyingType() == typeid(std::string))
    {
        result = value;
        return true;
    }
    
    if (*type == typeid(std::wstring) && *targetType->UnderlyingType() == typeid(std::wstring))
    {
        result = value;
        return true;
    }

    if (*targetType->UnderlyingType() == typeid(System::Reflection::Object))
    {
        if (type->IsPrimitive())
        {
            if (*type == typeid(char))
            {
                result = std::make_shared<System::Int8>(value.GetValue<int8>());
                return true;
            }
            if (*type == typeid(unsigned char))
            {
                result = std::make_shared<System::UInt8>(value.GetValue<uint8>());
                return true;
            }
            if (*type == typeid(short))
            {
                result = std::make_shared<System::Int16>(value.GetValue<int16>());
                return true;
            }
            if (*type == typeid(unsigned short))
            {
                result = std::make_shared<System::UInt16>(value.GetValue<uint16>());
                return true;
            }
            if (*type == typeid(int))
            {
                result = std::make_shared<System::Int32>(value.GetValue<int32>());
                return true;
            }
            else if (*type == typeid(unsigned int))
            {
                result = std::make_shared<System::UInt32>(value.GetValue<uint32>());
                return true;
            }
            if (*type == typeid(long long))
            {
                result = std::make_shared<System::Int64>(value.GetValue<int64>());
                return true;
            }
            else if (*type == typeid(unsigned long long))
            {
                result = std::make_shared<System::UInt64>(value.GetValue<uint64>());
                return true;
            }
            else if (*type == typeid(bool))
            {
                result = std::make_shared<System::UInt8>(uint8(value.GetValue<bool>()));
                return true;
            }
            else if (*type == typeid(float))
            {
                result = std::make_shared<System::Float>(value.GetValue<float>());
                return true;
            }
            else if (*type == typeid(double))
            {
                result = std::make_shared<System::Double>(value.GetValue<double>());
                return true;
            }
        }
        else if (*type == typeid(std::string))
        {
            result = std::make_shared<System::String>(value.ToString());
            return true;
        }
        else if (*type == typeid(std::wstring))
        {
            result = std::make_shared<System::String>(value.GetValue<std::wstring>());
            return true;
        }
    }

    result = nullptr;
    return false;
}

bool CommonValueConversion::NonConversionStepTryConvert(System::Reflection::Variant const& value, Proto::XamlType *targetType, Interfaces::IValueContext *valueContext, System::Reflection::Variant &result)
{
    result = nullptr;
    
    if (value == nullptr)
    {
        return true;
    }
    
    auto isAssignable = targetType->UnderlyingType()->IsAssignableFrom(*value.GetType()) || targetType->UnderlyingType()->DerivesFrom(*value.GetType());
    
    if (isAssignable)
    {
        result = value;
        return true;
    }
    else
    {
        return false;
    }
}

bool CommonValueConversion::ConverterStepTryConvert(System::Reflection::Variant const& value, Proto::XamlType *targetType, Interfaces::IValueContext *valueContext, System::Reflection::Variant &result)
{
    result = nullptr;
    
    if (valueContext->m_StateCommuter && valueContext->m_StateCommuter->getPrevious()->getMember() && valueContext->m_StateCommuter->getPrevious()->getMember()->ToMutableMember())
    {
        auto member = valueContext->m_StateCommuter->getPrevious()->getMember()->ToMutableMember();
        const System::Reflection::Field* field = member->IsField ? member->RuntimeField() : member->RuntimeProperty();
        if (field)
        {
            const XamlCPP::TypeConverterAttribute* converter = field->GetMeta().GetCustomAttribute<XamlCPP::TypeConverterAttribute>();
            if (converter)
            {
                auto typeConverter = targetType->getFeatureProvider()->GetTypeConverter(converter->GetConverterTypeIndex().hash_code());
                if (typeConverter != nullptr)
                {
                    if (typeConverter->CanConvertFrom(valueContext, value.GetType()))
                    {
                        {
                            result = typeConverter->ConvertFrom(valueContext, value);
                            return true;
                        }
                    }
                }
            }

            static const System::Reflection::Type& propertyEvent = typeof(System::Events::IPropertyEvent);

            if (propertyEvent.IsAssignableFrom(*field->GetFieldType()))
            {
                const System::PropertyEventType* eventProperty = propertyEvent.GetMeta().GetCustomAttribute<System::PropertyEventType>();
                if (eventProperty)
                {
                    auto typeConverter = targetType->getFeatureProvider()->GetTypeConverter(eventProperty->GetTypeIndex().hash_code());
                    if (typeConverter != nullptr)
                    {
                        if (typeConverter->CanConvertFrom(valueContext, value.GetType()))
                        {
                            {
                                result = typeConverter->ConvertFrom(valueContext, value);
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    auto typeConverter = targetType->getTypeConverter();
    if (typeConverter != nullptr)
    {
        if (typeConverter->CanConvertFrom(valueContext, value.GetType()))
        {
            result = typeConverter->ConvertFrom(valueContext, value);
            return true;
        }
    }

    return false;
}


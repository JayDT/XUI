/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2016 Austin Brunkhorst, All Rights Reserved.
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

#include "Precompiled.h"

#include "VariantContainer.h"
#include "Type.h"
#include "TypeData.h"
#include "ReflectionDatabase.h"
#include "Reflection/Runtime/ReflectionModule.h"
#include "Enum.h"

#include <algorithm>

namespace System::Reflection
{
    inline namespace meta
    {
#pragma region TypeSafe Converter
        template<typename T>
        T GenericConverter(Type const& type, void* ptr)
        {
            if (type.IsPrimitive())
            {
                if (type == typeid(char))
                    return static_cast<T>(*static_cast<char*>(ptr));
                if (type == typeid(unsigned char))
                    return static_cast<T>(*static_cast<unsigned char*>(ptr));
                if (type == typeid(short))
                    return static_cast<T>(*static_cast<short*>(ptr));
                if (type == typeid(unsigned short))
                    return static_cast<T>(*static_cast<unsigned short*>(ptr));
                if (type == typeid(int))
                    return static_cast<T>(*static_cast<int*>(ptr));
                else if (type == typeid(unsigned int)) // typeof(unsigned int))
                    return static_cast<T>(*static_cast<unsigned int*>(ptr));
                if (type == typeid(long long)) // typeof(long long))
                    return static_cast<T>(*static_cast<long long*>(ptr));
                else if (type == typeid(unsigned long long)) // typeof(unsigned long long))
                    return static_cast<T>(*static_cast<unsigned long long*>(ptr));
                else if (type == typeid(bool)) // typeof(bool))
                    return static_cast<T>(*static_cast<bool*>(ptr));
                else if (type == typeid(float)) // typeof(float))
                    return static_cast<T>(*static_cast<float*>(ptr));
                else if (type == typeid(double)) // typeof(double))
                    return static_cast<T>(*static_cast<double*>(ptr));
            }
            else if (type.IsEnum() && type.TypeDescriptor)
            {
                if (typeid(T) == typeid(long long)) 
                    return type.TypeDescriptor->ToLong(ptr);
                if (typeid(T) == typeid(unsigned long long))
                    return (unsigned long long)type.TypeDescriptor->ToLong(ptr);
                if (typeid(T) == typeid(int))
                    return type.TypeDescriptor->ToInt(ptr);
                if (typeid(T) == typeid(unsigned int))
                    return (unsigned int)type.TypeDescriptor->ToInt(ptr);
                else
                    return type.TypeDescriptor->ToLong(ptr);
            }

            return *static_cast<T*>(ptr);
        }

        template int GenericConverter<int>(Type const& type, void* ptr);
        template bool GenericConverter<bool>(Type const& type, void* ptr);
        template float GenericConverter<float>(Type const& type, void* ptr);
        template double GenericConverter<double>(Type const& type, void* ptr);

        template<>
        std::string GenericConverter<std::string>(Type const& type, void* ptr)
        {
            if (type.IsPrimitive())
            {
                if (type == typeid(char))
                    return std::to_string(*static_cast<char*>(ptr));
                if (type == typeid(unsigned char))
                    return std::to_string(*static_cast<unsigned char*>(ptr));
                if (type == typeid(short))
                    return std::to_string(*static_cast<short*>(ptr));
                if (type == typeid(unsigned short))
                    return std::to_string(*static_cast<unsigned short*>(ptr));
                if (type == typeid(int))
                    return std::to_string(*static_cast<int*>(ptr));
                else if (type == typeid(unsigned int)) // typeof(unsigned int))
                    return std::to_string(*static_cast<unsigned int*>(ptr));
                if (type == typeid(long long)) // typeof(long long))
                    return std::to_string(*static_cast<long long*>(ptr));
                else if (type == typeid(unsigned long long)) // typeof(unsigned long long))
                    return std::to_string(*static_cast<unsigned long long*>(ptr));
                else if (type == typeid(bool)) // typeof(bool))
                    return std::to_string(*static_cast<bool*>(ptr));
                else if (type == typeid(float)) // typeof(float))
                    return std::to_string(*static_cast<float*>(ptr));
                else if (type == typeid(double)) // typeof(double))
                    return std::to_string(*static_cast<double*>(ptr));
            }
            else if (type.IsEnum() && type.TypeDescriptor)
            {
                return type.GetEnum()->GetKey(type.TypeDescriptor->ToLong(ptr));
            }

            return "";
        }
#pragma endregion TypeSafe Converter

        int VariantBase::ToInt(void) const { return GenericConverter<int>(GetType(), (void*)GetPtr()); }
        bool VariantBase::ToBool(void) const { return GenericConverter<bool>(GetType(), (void*)GetPtr()); }
        float VariantBase::ToFloat(void) const { return GenericConverter<float>(GetType(), (void*)GetPtr()); }
        double VariantBase::ToDouble(void) const { return GenericConverter<double>(GetType(), (void*)GetPtr()); }
        long long VariantBase::ToLong(void) const { return GenericConverter<long long>(GetType(), (void*)GetPtr()); }

        std::string VariantBase::ToString(void) const
        {
            if (GetType().IsPrimitive())
            {
                if (GetType().IsPointer())
                    return std::string(*static_cast<const char**>((void*)GetPtr()));
                else
                    return GenericConverter<std::string>(GetType(), (void*)GetPtr());
            }
            else if (GetType().IsEnum() && GetType().TypeDescriptor)
            {
                return GetType().GetEnum()->GetKey(GetType().TypeDescriptor->ToLong((void*)GetPtr()));
            }
            else if (IsObject())
                return ToObject()->ToString();
            else if (GetType() == typeid(std::string))
                return (*static_cast<std::string*>((void*)GetPtr()));
            else if (GetType() == typeid(std::wstring))
                return System::String::FromWString(*static_cast<std::wstring*>((void*)GetPtr()));
            else if (GetType().IsValid())
                return GetType().fullname;
            return "<can't stringify>";
        }

        inline VariantRefObject::VariantRefObject()
            : m_type(&Type::Invalid())
            , m_value(nullptr)
            , m_isObject(false)
            , m_isPointer(false)
        { }

        inline VariantRefObject::VariantRefObject(Type const & source_type, void * source, bool isObject, bool isPointer)
            : m_type(&source_type)
            , m_value(source)
            , m_isObject(isObject)
            , m_isPointer(isPointer)
        { }

        VariantRefObject::~VariantRefObject()
        {
        }

        //int VariantRefObject::ToInt(void) const          { return GenericConverter<int>(GetType(), m_value); }
        //bool VariantRefObject::ToBool(void) const        { return GenericConverter<bool>(GetType(), m_value); }
        //float VariantRefObject::ToFloat(void) const      { return GenericConverter<float>(GetType(), m_value); }
        //double VariantRefObject::ToDouble(void) const    { return GenericConverter<double>(GetType(), m_value); }
        //long long VariantRefObject::ToLong(void) const   { return GenericConverter<long long>(GetType(), m_value); }

        bool VariantRefObject::IsArray(void) const { return m_type->IsArray(); }
        VariantBase * VariantRefObject::Clone(void) const
        {
            VariantRefObject * clone = new VariantRefObject(*m_type, m_value, m_isObject, m_isPointer);
            return clone;
        }

        bool VariantRefObject::Equal(VariantBase const & variant) const
        {
            if (ToPointer() == variant.ToPointer())
                return true;

            if (ToObject() && variant.ToObject())
                return ToObject()->Equal(variant);

            if (!*this || !variant)
                return false;

            if (GetType() != variant.GetType())
            {
                if (GetType().IsEnum() || variant.GetType().IsEnum())
                    return ToLong() == variant.ToLong();
                return false;
            }

            if (IsPointer() || variant.IsPointer())
            {
                if (IsPointer() != variant.IsPointer())
                    return false;

                if (!ToPointer() && !variant.ToPointer())
                    return true;
            }

            return memcmp(ToPointer(), variant.ToPointer(), GetType().GetTypeData().size) == 0 ? true : false;
        }
    }
}

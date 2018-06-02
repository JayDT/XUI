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

#pragma once

#include "Reflection/Runtime/TypeConfig.h"
#include "Reflection/Runtime/VariantContainer.h"
#include <standard/Errors.h>

namespace System::Reflection
{
    inline namespace meta
    {
        template<typename T>
        Variant::Variant(T& data, variant_policy::NoCopy*, typename std::enable_if<!std::is_base_of<Object, typename std::remove_pointer<typename std::remove_reference<T>::type>::type>::value>::type*)
            : m_isConst(false)
        {
            m_base = new VariantRefObject(
                CacheType<typename System::Reflection::meta::CleanedType<T>>(), //System::Reflection::type_of(typeid(typename System::Reflection::meta::CleanedType<T>)), 
                const_cast<void*>(reinterpret_cast<const void*>(std::addressof(data))), 
                false, 
                std::is_pointer<T>::value || std::is_null_pointer<T>::value);

            if (!m_base->GetType())
                m_nativetypeHash = typeid(typename std::remove_cv<T>::type).hash_code();
        }

        template<typename T>
        Variant::Variant(T const& data, variant_policy::NoCopy*, typename std::enable_if<!std::is_base_of<Object, typename std::remove_pointer<typename std::remove_reference<T>::type>::type>::value>::type*)
            : m_isConst(false)
        {
            m_base = new VariantRefObject(
                CacheType<typename System::Reflection::meta::CleanedType<T>>(), //System::Reflection::type_of(typeid(typename System::Reflection::meta::CleanedType<T>)),
                const_cast<void*>(reinterpret_cast<const void*>(std::addressof(data))), 
                false, 
                std::is_pointer<T>::value || std::is_null_pointer<T>::value);

            if (!m_base->GetType())
                m_nativetypeHash = typeid(typename std::remove_cv<T>::type).hash_code();
        }

        template<typename T>
        Variant::Variant(T const& data, typename std::enable_if<!std::is_base_of<Object, typename std::remove_pointer<typename std::remove_reference<T>::type>::type>::value>::type*)
            : m_isConst(false)
        {
            m_base = new VariantCopyObject<T>(
                CacheType<typename System::Reflection::meta::CleanedType<T>>(), //System::Reflection::type_of(typeid(typename System::Reflection::meta::CleanedType<T>)),
                data, //(void**)(std::addressof(data)), 
                sizeof(T), 
                std::is_base_of<Object, typename std::remove_pointer<typename std::remove_reference<T>::type>::type>::value, 
                std::is_pointer<T>::value || std::is_null_pointer<T>::value);

            if (!m_base->GetType())
                m_nativetypeHash = typeid(typename std::remove_cv<T>::type).hash_code();
        }

        template<typename T>
        Variant::Variant(T const& data, variant_policy::Copy*)
            : m_isConst(false)
        {
            m_base = new VariantCopyObject<T>(
                CacheType<typename System::Reflection::meta::CleanedType<T>>(), //System::Reflection::type_of(typeid(typename System::Reflection::meta::CleanedType<T>)),
                data, //(void**)(std::addressof(data)), 
                sizeof(T),
                std::is_base_of<Object, typename std::remove_pointer<typename std::remove_reference<T>::type>::type>::value,
                std::is_pointer<T>::value || std::is_null_pointer<T>::value);

            if (!m_base->GetType())
                m_nativetypeHash = typeid(typename std::remove_cv<T>::type).hash_code();
        }

        template<typename T>
        Variant::Variant(std::shared_ptr<T> const & data, variant_policy::Copy*)
            : m_isConst(false)
        {
            static const Type* ObjectTypeCache = &typeof(System::Reflection::Object);
            if (!ObjectTypeCache->IsValid())
                ObjectTypeCache = &typeof(System::Reflection::Object);

            const Type* type = &CacheType<typename std::remove_volatile<T>::type>(); // &System::Reflection::type_of(typeid(typename std::remove_volatile<T>::type));
            if (data && ObjectTypeCache->IsAssignableFrom(*type))
                type = ((System::Reflection::Object*)data.get())->GetType();
            else
                type = &System::Reflection::type_of(typeid(typename std::remove_volatile<T>::type));

            m_base = new VariantSharedObject<T>(
                *type,
                data,
                std::is_base_of<Object, typename std::remove_pointer<typename std::remove_reference<T>::type>::type>::value,
                true);

            if (!m_base->GetType())
                m_nativetypeHash = typeid(typename std::remove_cv<T>::type).hash_code();
        }

        ///////////////////////////////////////////////////////////////////////

        template<typename T>
        Variant &Variant::operator=(T const& rhs)
        {
            Variant(static_cast<T const&>(rhs)).Swap(*this);

            return *this;
        }

        ///////////////////////////////////////////////////////////////////////

        template <typename T, class Enabled = void>
        struct __VariantGetValueHelper
        {
            static T GetValue(Variant const& v)
            {
                if (v.GetBase() && !v.IsObject())
                    return *(typename std::remove_reference<T>::type*)(v.GetBase()->GetPtr());
                throw System::NotSupportedException("Invalid Value Conversion");
            }

            static T TryGetValue(Variant const& v)
            {
                if (v.GetBase() && !v.IsObject())
                {
                    // we can determinate base can't use dynamic_cast (ToDo: reflect_cast implement)
                    return *(typename std::remove_reference<T>::type*)(v.GetBase()->GetPtr());
                }
                throw System::NotSupportedException("Invalid Value Conversion");
            }
        };

        template <typename T>
        struct __VariantGetValueHelper<T, typename std::enable_if<std::is_pointer<typename std::remove_cv<typename std::remove_reference<T>::type>::type>::value && std::is_base_of<Object, typename std::remove_pointer<typename std::remove_reference<T>::type>::type>::value>::type>
        {
            static T GetValue(Variant const& v)
            {
                if (v.GetBase() && v.IsObject())
                    return (typename std::remove_pointer<typename std::remove_reference<T>::type>::type*)(static_cast<typename std::remove_cv<typename std::remove_reference<T>::type>::type>(v.GetBase()->ToObject()));
                throw System::NotSupportedException("Invalid Value Conversion");
            }

            static T TryGetValue(Variant const& v)
            {
                if (v.GetBase() && v.IsObject())
                    return (typename std::remove_pointer<typename std::remove_reference<T>::type>::type*)(dynamic_cast<typename std::remove_cv<typename std::remove_reference<T>::type>::type>(v.GetBase()->ToObject()));
                throw System::NotSupportedException("Invalid Value Conversion");
            }
        };

        template <typename T>
        struct __VariantGetValueHelper<T, typename std::enable_if<!std::is_pointer<typename std::remove_cv<typename std::remove_reference<T>::type>::type>::value && std::is_base_of<Object, typename std::remove_pointer<typename std::remove_reference<T>::type>::type>::value>::type>
        {
            static T GetValue(Variant const& v)
            {
                if (v.GetBase() && v.IsObject())
                    return *static_cast<typename std::remove_cv<typename std::remove_reference<T>::type>::type*>(v.GetBase()->ToObject());
                throw System::NotSupportedException("Invalid Value Conversion");
            }

            static T TryGetValue(Variant const& v)
            {
                if (v.GetBase() && v.IsObject())
                    return *dynamic_cast<typename std::remove_cv<typename std::remove_reference<T>::type>::type*>(v.GetBase()->ToObject());
                throw System::NotSupportedException("Invalid Value Conversion");
            }
        };

        template <typename T>
        struct __VariantGetValueHelper<std::shared_ptr<T>, typename std::enable_if<std::is_base_of<Object, typename std::remove_pointer<typename std::remove_reference<T>::type>::type>::value>::type>
        {
            static std::shared_ptr<typename std::remove_reference<T>::type> GetValue(Variant const& v)
            {
                if (v.GetBase())
                {
                    if (v.GetBase()->IsSharedPointer())
                    {
                        if (v.GetBase()->IsObject())
                            return (std::static_pointer_cast<T>(v.GetBase()->ToSharedObject()));
                    }
                }
                return nullptr;
            }

            static std::shared_ptr<typename std::remove_reference<T>::type> TryGetValue(Variant const& v)
            {
                if (v.GetBase())
                {
                    if (v.GetBase()->IsSharedPointer())
                    {
                        if (v.GetBase()->IsObject())
                            return (std::dynamic_pointer_cast<T>(v.GetBase()->ToSharedObject()));
                    }
                }
                return nullptr;
            }
        };

        template <typename T>
        struct __VariantGetValueHelper<std::shared_ptr<T>, typename std::enable_if<!std::is_base_of<Object, typename std::remove_pointer<typename std::remove_reference<T>::type>::type>::value>::type>
        {
            static std::shared_ptr<typename std::remove_reference<T>::type> GetValue(Variant const& v)
            {
                if (v.GetBase())
                {
                    if (v.GetBase()->IsSharedPointer())
                    {
                        return (std::static_pointer_cast<T>(v.GetBase()->ToSharedPointer()));
                    }
                }
                return nullptr;
            }
            
            static std::reference_wrapper<std::shared_ptr<typename std::remove_reference<T>::type>> TryGetValue(Variant const& v)
            {
                throw System::NotSupportedException("Invalid Value Conversion");
            }
        };

        template<typename T>
        typename T Variant::GetValue() const
        {
            return __VariantGetValueHelper<T>::GetValue(*this);
        }

        template<typename T>
        typename T Variant::TryGetValue() const
        {
            return __VariantGetValueHelper<T>::TryGetValue(*this);
        }
    }
}

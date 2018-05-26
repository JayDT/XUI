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

#include "VariantBase.h"
#include "MetaTraits.h"
#include "Object.h"
#include "Variant.h"

#include "standard/Platform/Define.h"

namespace System::Reflection
{
    inline namespace meta
    {
        template <typename T, class Enabled = void>
        struct __VariantToObjectHelper;

        template <typename T>
        struct __VariantToObjectHelper<std::shared_ptr<T>, typename std::enable_if<std::is_base_of<Object, typename std::remove_pointer<typename std::remove_reference<T>::type>::type>::value>::type>
        {
            static std::shared_ptr<Object> ToObject(std::shared_ptr<T> const& v)
            {
                return std::static_pointer_cast<Object>(v);
            }

            static std::shared_ptr<Object> ToDynamicObject(std::shared_ptr<T> const& v)
            {
                return std::dynamic_pointer_cast<Object>(v);
            }

            static Object* ToObject(T* v)
            {
                return static_cast<Object*>(v);
            }

            static Object* ToDynamicObject(T* v)
            {
                return dynamic_cast<Object*>(v);
            }
        };

        template <typename T>
        struct __VariantToObjectHelper<std::shared_ptr<T>, typename std::enable_if<!std::is_base_of<Object, typename std::remove_pointer<typename std::remove_reference<T>::type>::type>::value>::type>
        {
            static std::shared_ptr<Object> ToObject(std::shared_ptr<T> const& v)
            {
                return nullptr;
            }

            static std::shared_ptr<Object> ToDynamicObject(std::shared_ptr<T> const& v)
            {
                return nullptr;
            }

            static Object* ToObject(T* v)
            {
                return nullptr;
            }

            static Object* ToDynamicObject(T* v)
            {
                return nullptr;
            }

        };

        template<typename TObject>
        class VariantCopyObject : public VariantBase
        {
        public:
            VariantCopyObject(Type const& source_type, TObject const& source, size_t source_size, bool isObject, bool isPointer)
                : m_type(source_type)
                , m_value(source)
                , source_size(source_size)
                , m_isPointer(isPointer)
                , m_isObject(isObject)
            {

            }

            virtual ~VariantCopyObject()
            {

            }

            virtual VariantStorageType StorageType() const { return VariantStorageType::Copy; }

            Type const& GetType(void) const override
            {
                return m_type;
            }

            void *GetPtr(void) const override
            {
                return (void*)&m_value;
            }

            Object* ToObject(void) const override 
            {
                typedef typename std::remove_cv<typename std::remove_pointer<TObject>::type>::type CleanType;
                if (m_isObject)
                    return __VariantToObjectHelper<std::shared_ptr<CleanType>>::ToObject((CleanType*)ToPointer());
                    //static_cast<Object*>((TObject*)ToPointer());
                return nullptr; 
            }
            void* ToPointer(void) const override 
            { 
                if (m_isPointer)
                    return *(void**)(&m_value);
                return (void*)(&m_value);
            }
            std::shared_ptr<void> ToSharedPointer(void) const override
            {
                return std::shared_ptr<void>(nullptr);
            }
            std::shared_ptr<Object> ToSharedObject(void) const override
            {
                return std::shared_ptr<Object>(nullptr);
            }
            VariantBase * Clone(void) const
            {
                VariantCopyObject * clone = new VariantCopyObject(m_type, m_value, source_size, m_isObject, m_isPointer);
                return clone;
            }

            bool Equal(VariantBase const & variant) const
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

                    if (GetType().IsStringType())
                        return ToString() == variant.ToString();

                    return false;
                }

                if (IsPointer() || variant.IsPointer())
                {
                    if (IsPointer() != variant.IsPointer())
                        return false;

                    if (!ToPointer() && !variant.ToPointer())
                        return true;
                }

                if (GetType().IsPrimitive() || variant.GetType().IsPrimitive())
                {
                    if (GetType() == typeid(float))
                        return ToFloat() == variant.ToFloat();
                    else if (GetType() == typeid(double))
                        return ToDouble() == variant.ToDouble();
                    else
                        return ToLong() == variant.ToLong();
                }

                if (GetType().IsStringType())
                    return ToString() == variant.ToString();

                if (variant.ToPointer() && ToPointer())
                    return memcmp(ToPointer(), variant.ToPointer(), GetType().GetTypeData().size) == 0 ? true : false;
                return false;
            }

            bool IsArray(void) const override { return m_type.IsArray(); }
            bool IsPointer(void) const override { return m_isPointer; }
            bool IsObject(void) const override { return m_isObject; }
            bool IsSharedPointer(void) const override { return false; }

            void OnSerialize(json11::Json::object &output) const override {}
            void OnDeserialize(const json11::Json &input) override {}

            virtual size_t GetHashCode() const override
            {
                if (IsObject())
                    return ToObject()->GetHashCode();
                return std::hash<size_t>()(reinterpret_cast<uintptr_t>(ToPointer()));
            }

        private:
            friend class Variant;

            Type const& m_type;
            TObject m_value;
            size_t source_size;
            bool m_isObject : 1;
            bool m_isPointer : 1;
        };

        class TC_METARUNTIME_API VariantRefObject : public VariantBase
        {
        public:
            VariantRefObject();
            VariantRefObject(Type const& source_type, void* source, bool isObject, bool isPointer);
            virtual ~VariantRefObject();

            virtual VariantStorageType StorageType() const { return VariantStorageType::Reference; }

            Type const& GetType(void) const override
            {
                return *m_type;
            }

            void* GetPtr(void) const override
            {
                return m_value;
            }

            Object* ToObject(void) const override
            {
                if (m_isObject)
                    return static_cast<Object*>(ToPointer());
                return nullptr;
            }
            void* ToPointer(void) const override
            {
                if (m_isPointer)
                    return *(void**)(m_value);
                return (void*)(m_value);
            }
            std::shared_ptr<void> ToSharedPointer(void) const override
            {
                return std::shared_ptr<void>(nullptr);
            }
            std::shared_ptr<Object> ToSharedObject(void) const override
            {
                return std::shared_ptr<Object>(nullptr);
            }
            //int ToInt(void) const override;
            //bool ToBool(void) const override;
            //float ToFloat(void) const override;
            //double ToDouble(void) const override;
            //long long ToLong(void) const override;

            bool IsArray(void) const override;
            bool IsPointer(void) const override { return m_isPointer; }
            bool IsObject(void) const override { return m_isObject; }
            bool IsSharedPointer(void) const override { return false; }

            VariantBase *Clone(void) const override;
            virtual bool Equal(VariantBase const&) const override;

            void OnSerialize(json11::Json::object &output) const override {}
            void OnDeserialize(const json11::Json &input) override {}

            virtual size_t GetHashCode() const override
            {
                if (IsObject())
                    return ToObject()->GetHashCode();
                return std::hash<size_t>()(reinterpret_cast<uintptr_t>(ToPointer()));
            }
        private:
            friend class Variant;

            Type const* m_type;
            void* m_value;
            bool m_isObject : 1;
            bool m_isPointer : 1;
        };

        template<typename T>
        class VariantSharedObject : public VariantBase
        {
        public:
            VariantSharedObject()
                : m_type(&Type::Invalid())
                , m_value(nullptr)
                , m_isObject(false)
                , m_isPointer(false)
            {}

            VariantSharedObject(Type const& source_type, std::shared_ptr<T> const& source, bool isObject, bool isPointer)
                : m_type(&source_type)
                , m_value(source)
                , m_isObject(isObject)
                , m_isPointer(true)
            {}

            virtual ~VariantSharedObject()
            {
            }

            virtual VariantStorageType StorageType() const { return VariantStorageType::SharedPointer; }

            Type const& GetType(void) const override
            {
                if (!m_type->IsValid() && m_isObject)
                    return *ToObject()->GetType();
                return *m_type;
            }

            void* GetPtr(void) const override
            {
                return (void*)&m_value;
            }

            Object* ToObject(void) const override
            {
                if (m_isObject)
                    return __VariantToObjectHelper<std::shared_ptr<T>>::ToObject(m_value.get());
                return nullptr;
            }
            void* ToPointer(void) const override
            {
                if (m_isPointer)
                    return reinterpret_cast<void*>(m_value.get());
                return nullptr;
            }
            std::shared_ptr<void> ToSharedPointer(void) const override
            {
                if (m_isPointer)
                    return std::static_pointer_cast<void>(m_value);
                return std::shared_ptr<void>(nullptr);
            }
            std::shared_ptr<Object> ToSharedObject(void) const override
            {
                return __VariantToObjectHelper<std::shared_ptr<T>>::ToObject(m_value);
            }

            // @ToDo: this unsupported currently
            bool IsArray(void) const override { return false; }
            bool IsPointer(void) const override { return m_isPointer; }
            bool IsObject(void) const override { return m_isObject; }
            bool IsSharedPointer(void) const override { return true; }

            bool Equal(VariantBase const& variant) const override
            {
                if (m_isObject && ToObject())
                    return ToObject()->Equal(variant);
                return ToPointer() == variant.ToPointer();
            }

            VariantBase *Clone(void) const override
            {
                VariantSharedObject<T> * clone = new VariantSharedObject<T>(*m_type, m_value, m_isObject, m_isPointer);
                return clone;
            }

            void OnSerialize(json11::Json::object &output) const override {}
            void OnDeserialize(const json11::Json &input) override {}

            virtual size_t GetHashCode() const override
            {
                if (IsObject())
                    return ToObject()->GetHashCode();
                return std::hash<size_t>()(reinterpret_cast<uintptr_t>(m_value.get()));
            }
        private:
            friend class Variant;

            Type const* m_type;
            std::shared_ptr<T> m_value;
            bool m_isObject : 1;
            bool m_isPointer : 1;
        };
    }
}

#undef DEFAULT_TYPE_HANDLER

#include "Impl/VariantContainer.hpp"
#include "Impl/VariantContainerStandardTypes.hpp"
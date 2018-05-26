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
#include "VariantPolicy.h"

namespace System::Reflection
{
    inline namespace meta
    {
        class Type;
        class Object;
        class Argument;
        class ArrayWrapper;
        class VariantMemDumpObject;
        class VariantRefObject;

        class TC_METARUNTIME_API Variant
        {
        public:
            static Variant Empty;

            Variant(void);

            // copy constructor
            Variant(const Variant &rhs);
            Variant(Variant& instance, variant_policy::NoCopy*)
                : Variant(instance)
            {
            }

            // r-value
            Variant(Variant &&rhs);

            Variant(Object const& instance, variant_policy::NoCopy* = nullptr);
            Variant(Object& instance, variant_policy::NoCopy* = nullptr);
            Variant(Object*& instance, variant_policy::NoCopy* = nullptr);
            Variant(Object const* const& instance, variant_policy::NoCopy*);
            Variant(Object* const& instance, variant_policy::Copy* = nullptr);
            Variant(Object const* const& instance, variant_policy::Copy* = nullptr);

            Variant(ArrayWrapper const& _array);

            template<typename T>
            Variant(T& instance, variant_policy::NoCopy*, typename std::enable_if<!std::is_base_of<Object, typename std::remove_pointer<typename std::remove_reference<T>::type>::type>::value>::type* = nullptr);
            template<typename T>
            Variant(T const& instance, variant_policy::NoCopy*, typename std::enable_if<!std::is_base_of<Object, typename std::remove_pointer<typename std::remove_reference<T>::type>::type>::value>::type* = nullptr);
            template<typename T>
            Variant(T const& data, typename std::enable_if<!std::is_base_of<Object, typename std::remove_pointer<typename std::remove_reference<T>::type>::type>::value>::type* = nullptr);
            template<typename T>
            Variant(T const& data, variant_policy::Copy*);

            template<typename T>
            Variant(std::shared_ptr<T> const& data, variant_policy::Copy* = nullptr);

            ~Variant(void);

            template<typename T>
            Variant &operator=(T const& rhs);

            Variant &operator=(const Variant &rhs);

            Variant &operator=(Variant &&rhs);

            bool operator==(const Variant &rhs) const;
            bool operator!=(const Variant &rhs) const;

            Type const* GetType(void) const;

            void Swap(Variant &other);

            Object* ToObject(void) const;
            std::shared_ptr<void> ToSharedPointer(void) const;
            std::shared_ptr<Object> ToSharedObject(void) const;
            int ToInt(void) const;
            bool ToBool(void) const;
            float ToFloat(void) const;
            double ToDouble(void) const;
            long long ToLong(void) const;
            std::string ToString(void) const;
            void *ToPtr(void) const;
            ArrayWrapper* GetArray(void) const;

            template<typename TObject>
            TObject* ToPtrObject(void) const
            {
                auto object = ToObject();
                if (object)
                    return object->Static_As<TObject>();
                return nullptr;
            }

            template<typename TObject>
            std::shared_ptr<TObject> ToSharedObject(void) const
            {
                auto object = ToSharedObject();
                if (object && object->Is<TObject>())
                    return std::static_pointer_cast<TObject>(object);
                return nullptr;
            }

            json11::Json SerializeJson(void) const;

            template<typename T>
            T GetValue() const;

            template<typename T>
            T TryGetValue() const;

            bool IsValid(void) const;
            bool IsConst(void) const;
            bool IsArray(void) const;
            bool IsPointer(void) const;
            bool IsObject(void) const;
            bool Valid(void) const { return m_base != nullptr; }
            bool HasType(void) const;

            void *getPtr(void) const;
            void *toPointer(void) const;

            bool IsFastEqual(Variant const&) const;
            std::size_t GetNativeTypeHash() const;

            inline VariantBase * GetBase() const { return m_base; }

            bool operator<(const System::Reflection::meta::Variant &__variant) const
            {
                return GetBase() && __variant.GetBase() && GetBase()->GetHashCode() < __variant.GetBase()->GetHashCode();
            }

            bool operator>(const System::Reflection::meta::Variant &__variant) const
            {
                return GetBase() && __variant.GetBase() && GetBase()->GetHashCode() > __variant.GetBase()->GetHashCode();
            }

            explicit operator bool() const
            {
                return IsValid();
            }

        private:
            friend class Type;
            friend class Argument;
            friend class Destructor;

            VariantBase *m_base;
            std::size_t m_nativetypeHash;
            bool m_isConst;
        };
    }
}

namespace std
{
    template<>
    struct hash<System::Reflection::meta::Variant>
    {
        size_t operator()(const System::Reflection::meta::Variant &__variant) const
        {
            return __variant.GetBase() ? __variant.GetBase()->GetHashCode() : 0;
        }
    };
}

#include "Impl/Variant.hpp"

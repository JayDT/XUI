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

#include "standard/Platform/Define.h"

#include <string>
#include "JsonConfig.h"

namespace System::Reflection
{
    inline namespace meta
    {
        class Type;
        class Object;
        class ArrayWrapper;

        enum class VariantStorageType
        {
            Copy,
            Reference,
            SharedPointer,
            Array
        };

        class TC_METARUNTIME_API VariantBase
        {
        public:
            explicit VariantBase() { }
            virtual ~VariantBase() { }

            virtual Type const& GetType(void) const = 0;

            virtual void *GetPtr(void) const = 0;

            virtual Object* ToObject(void) const = 0;
            virtual void* ToPointer(void) const = 0;
            virtual std::shared_ptr<void> ToSharedPointer(void) const = 0;
            virtual std::shared_ptr<Object> ToSharedObject(void) const = 0;
            virtual int ToInt(void) const;
            virtual bool ToBool(void) const;
            virtual float ToFloat(void) const;
            virtual double ToDouble(void) const;
            virtual long long ToLong(void) const;
            virtual std::string ToString(void) const;

            virtual bool IsArray(void) const { return false; }
            virtual bool IsPointer(void) const = 0;
            virtual bool IsObject(void) const = 0;
            virtual bool IsSharedPointer(void) const = 0;
            virtual VariantStorageType StorageType() const = 0;
            virtual ArrayWrapper* GetArray(void) const { return nullptr; }

            virtual VariantBase *Clone(void) const = 0;
            virtual bool Equal(VariantBase const&) const = 0;

            virtual void OnSerialize(json11::Json::object &output) const { }
            virtual void OnDeserialize(const json11::Json &input) { }

            // Operators
            virtual size_t GetHashCode() const = 0;

            operator bool() const { return GetPtr() != nullptr; }
            bool operator!() const { return GetPtr() == nullptr; }

            bool operator==(VariantBase& other) const { return (GetPtr() == other.GetPtr()); }
            bool operator!=(VariantBase& other) const { return (GetPtr() != other.GetPtr()); }

            Type const& operator->() { return GetType(); }
            void* operator*() { return GetPtr(); }
        };
    }
}
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
#include "ArrayWrapper.h"

namespace System::Reflection
{
    inline namespace meta
    {
        class ArrayWrapper;

        class ArrayVariantContainer : public VariantBase
        {
        public:
            ArrayVariantContainer(Type const* elementType, ArrayWrapper const& array);
            virtual ~ArrayVariantContainer();

            virtual VariantStorageType StorageType() const { return VariantStorageType::Array; }

            Type const& GetType(void) const override;
            void *GetPtr(void) const override;

            Object* ToObject(void) const override { return nullptr; }
            std::shared_ptr<void> ToSharedPointer(void) const override
            {
                return std::shared_ptr<void>(nullptr);
            }
            std::shared_ptr<Object> ToSharedObject(void) const override
            {
                return std::shared_ptr<Object>(nullptr);
            }
            int ToInt(void) const override;
            bool ToBool(void) const override;
            float ToFloat(void) const override;
            double ToDouble(void) const override;
            std::string ToString(void) const override;

            bool IsPointer(void) const { return false; }
            bool IsObject(void) const { return false; }
            bool IsSharedPointer(void) const { return false; }
            void* ToPointer(void) const { return nullptr; }

            bool IsArray(void) const override;
            ArrayWrapper* GetArray(void) const override;

            VariantBase *Clone(void) const override;

            virtual size_t GetHashCode() const
            {
                return std::hash<size_t>()(reinterpret_cast<uintptr_t>(m_array.GetPtr()));
            }

        private:
            ArrayWrapper m_array;
            Type const* m_elementType;

            // Inherited via VariantBase
            virtual bool Equal(VariantBase const &) const override;
        };
    }
}

#include "Impl/ArrayVariantContainer.hpp"
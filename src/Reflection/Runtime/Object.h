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
#ifndef __REFL_META_OBJECT_H__
#define __REFL_META_OBJECT_H__

#include "standard/Platform/Define.h"
#include "Reflection/Runtime/Macros.h"
#include "TypeConfig.h"
#include <typeinfo>
#include "json11.hpp"

class Enable;

namespace System::Reflection
{
    namespace Assembly
    {
        class AssemblyModulDefault;
    }

    inline namespace meta
    {
        class TC_METARUNTIME_API Type;
        class VariantBase;

        class 
            Meta(Enable)
            TC_METARUNTIME_API 
            Object
        {
            friend class Assembly::AssemblyModulDefault;
            //mutable System::Reflection::meta::Type const* m_type = nullptr;

        public:
            explicit Object() {}
            virtual ~Object(void) { }

            virtual Type const* GetType(void) const;
            virtual size_t GetHashCode() const;
            virtual Object *Clone(void) const;
            virtual std::string ToString() const;
            virtual bool Equal(VariantBase const&) const;

            virtual void OnSerialize(json11::Json::object &output) const { }
            virtual void OnDeserialize(const json11::Json &input) { }

            template<class T>
            T* As()
            {
                if (!this)
                    return nullptr;

                // Target Cache
                static const Type* TargetType = &typeof(T);
                if (!TargetType || !TargetType->IsValid())
                    TargetType = &typeof(T);

                const Type* selfType = GetType();

                // Todo virtual parent detect
                if (TargetType->IsAssignableFrom(*selfType) || selfType->DerivesFrom(*TargetType))
                    return (T*)(this);
                return nullptr;
            }

            template<class T>
            T* Static_As()
            {
                if (!this)
                    return nullptr;

                // Target Cache
                static const Type* TargetType = &typeof(T);
                if (!TargetType || !TargetType->IsValid())
                    TargetType = &typeof(T);

                const Type* selfType = GetType();

                // Todo virtual parent detect
                if (TargetType->IsAssignableFrom(*selfType) || selfType->DerivesFrom(*TargetType))
                    return static_cast<T*>(this);
                return nullptr;
            }

            template<class T>
            T* Dynamic_As()
            {
                if (!this)
                    return nullptr;

                // Target Cache
                static const Type* TargetType = &typeof(T);
                if (!TargetType || !TargetType->IsValid())
                    TargetType = &typeof(T);

                const Type* selfType = GetType();

                // Todo virtual parent detect
                if (TargetType->IsAssignableFrom(*selfType) || selfType->DerivesFrom(*TargetType))
                    return dynamic_cast<T*>(this);
                return nullptr;
            }

            template<class T>
            bool Is()
            {
                if (!this)
                    return false;

                // Target Cache
                static const Type* TargetType = &typeof(T);
                if (!TargetType || !TargetType->IsValid())
                    TargetType = &typeof(T);

                const Type* selfType = GetType();

                // Todo virtual parent detect
                return (TargetType->IsAssignableFrom(*selfType) || selfType->DerivesFrom(*TargetType));
            }

            bool Is(Type const& TargetType);
        };

        using RefObject = std::shared_ptr<System::Reflection::Object>;

        class
            Meta(Enable)
            TC_METARUNTIME_API
            ISerializable : public Object
        {
        public:
            explicit ISerializable() : Object() {}
            virtual ~ISerializable(void) { }

            virtual Object *Clone(void) const override;

            virtual void OnSerialize(json11::Json::object &output) const override;
            virtual void OnDeserialize(const json11::Json &input) override;
        };
    }
}

#endif // __REFL_META_OBJECT_H__
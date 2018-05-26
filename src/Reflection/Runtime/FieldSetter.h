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

#include "FieldSetterBase.h"

namespace System::Reflection
{
    inline namespace meta
    {
        template<typename ClassType, typename FieldType, typename ReturnType, bool IsAssignable>
        class PropertyValue : public FieldSetterBase, public FieldGetterBase
        {
        public:
            void SetValue(uint32 uid, Variant &obj, const Variant &value) override;
            Variant GetValue(uint32 uid, const Variant &obj) override;
            Variant GetValueReference(uint32 uid, const Variant &obj) override;
        };

        template<typename FieldType, bool IsAssignable>
        class FieldValue : public FieldSetterBase, public FieldGetterBase
        {
        public:
            void SetValue(uint32 uid, Variant &obj, const Variant &value) override;
            Variant GetValue(uint32 uid, const Variant &obj) override;
            Variant GetValueReference(uint32 uid, const Variant &obj) override;
        };
    }
}

#include "Impl/FieldSetter.hpp"
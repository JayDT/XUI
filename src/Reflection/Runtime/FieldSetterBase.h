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

#include "IMethod.h"

namespace System::Reflection
{
    inline namespace meta
    {
        struct IField;
        class Variant;

        class TC_METARUNTIME_API FieldSetterBase : public IMethod
        {
        public:
            virtual ~FieldSetterBase(void) { }

            virtual void SetValue(uint32_t uid, Variant &obj, const Variant &value) = 0;

            IField* m_declaringField = nullptr;

            // Inherited via IMethod
            virtual Type const & DeclaringType(void) const override;
            virtual const char * GetName(void) const override;
            virtual CXXAccessSpecifier GetAccessLevel() const override;
            virtual bool CanCall(const ArgHashStorage & signature) const override;
            virtual bool IsValid(void) const override;
            virtual MethodDesc const & GetMethodInfo() const override;
            virtual Variant InternalInvoke(Variant & instance, ArgumentList & arguments) const override;

            // Inherited via IMethod
            virtual uint32_t GetUID() const override;
            virtual bool IsStatic() const override { return false; }
        };
    }
}
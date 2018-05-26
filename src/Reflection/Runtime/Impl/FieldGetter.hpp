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

namespace System::Reflection
{
    inline namespace meta
    {
        ///////////////////////////////////////////////////////////////////////
        // Getter from Method
        ///////////////////////////////////////////////////////////////////////

        class PropertyGetter : public FieldGetterBase
        {
        public:
            typedef Variant invoker_impl(size_t methodId, Variant&& obj);

            invoker_impl* invoker = nullptr;
			bool isStatic = false;

            PropertyGetter(invoker_impl method)
                : invoker(method) { }

            Variant GetValue(uint32_t uid, const Variant &obj) override
            {
                return invoker(uid, std::move(const_cast<Variant &>(obj)));
            }

            Variant GetValueReference(uint32_t uid, const Variant &obj) override
            {
                return invoker(uid, std::move(const_cast<Variant &>(obj))); //getValueReference(obj);
            }

			bool IsStatic() const override { return isStatic; }
        };
    }
}
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

#include "TypeConfig.h"
#include "Variant.h"
#include "VariantContainer.h"

#include <vector>

namespace System::Reflection
{
    inline namespace meta
    {
        class Object;
        class Type;

        class TC_METARUNTIME_API Argument
        {
        public:
            Argument(void);
            Argument(const Argument &rhs);
            Argument(Variant &obj);
            Argument(const Variant &obj);
            Argument(Variant &&obj)
                : m_data(std::move(obj))
            {}
            Argument(const Variant &&obj)
                : m_data(std::move(obj))
            {}

            template<typename T>
            Argument(const T &data);

            template<typename T>
            Argument(const T *data);

            Argument &operator=(const Argument &rhs);

            Type const* GetType(void) const;

            Object* ToObject(void) const;
            RefObject ToSharedObject(void) const;
            std::shared_ptr<void> ToSharedPointer(void) const;
            void *GetPtr(void) const;

            template<typename T>
            typename std::remove_reference<T>::type& GetValue() const;

        private:
            Variant m_data;
        };
    }
}

#include "Impl/Argument.hpp"
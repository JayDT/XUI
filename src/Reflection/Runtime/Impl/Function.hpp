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

namespace System::Reflection
{
    inline namespace meta
    {
        ///////////////////////////////////////////////////////////////////////

        template<typename ...Args>
        Variant Function::Invoke(Args &&...args) const
        {
            static Type::List signatureTypes;
            static bool initial = true;

            if (initial)
            {
                TypeUnpacker<Args...>::Apply(signatureTypes);
                initial = false;
            }

            ArgumentList arguments;
            ArgUnpacker().Apply(arguments, signatureTypes, 0, std::forward<Args>(args)...);

            auto ret = InvokeVariadic( arguments );

            ArgUnpacker().Free<int(sizeof...(Args)-1)>(arguments);

            return ret;
        }
    }
}

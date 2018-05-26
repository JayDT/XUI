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
        //template<typename F>
        //typename MethodWrapper<F>::Signature MethodWrapper<F>::m_method = nullptr;

        //template<typename F>
        //Variant MethodWrapper<F>::Invoke(Variant &obj, const ArgumentList &arguments)
        //{
        //    //UAssert(arguments.size() == THIS_ARG_COUNT,
        //    //    "Invalid method arguments.\nExpected %i args but got %i.",
        //    //    THIS_ARG_COUNT,
        //    //    arguments.size()
        //    //);
        //
        //    return method_invoke_impl<return_t>::invoke(std::move(*static_cast<class_t*>(obj.getPtr())), std::move(m_method), std::move(arguments), arg_index_sequence());
        //}
    }
}

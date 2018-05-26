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

#include "FunctionInvokerBase.h"

namespace System::Reflection
{
    inline namespace meta
    {
        /////////////////////////////////////////////////////////////////////////////////////
        class FunctionWrapper : public FunctionInvokerBase
        {
        public:
            typedef Variant invoker_impl(size_t methodId, Variant&& obj, const ArgumentList&& arguments);
            typedef MethodDesc const& methodinfo_impl(size_t methodId);

            invoker_impl* invoker = nullptr;
            methodinfo_impl* methodinfo = nullptr;

            Variant Invoke(uint32_t UID, const ArgumentList &arguments) override
            {
                if (invoker)
                    return invoker(UID, std::move(Variant::Empty), std::move(arguments));
                return {};
            }

            MethodDesc const& GetMethodInfo(uint32_t UID) override
            {
                return methodinfo(UID);
            }
        };


        template<typename F>
        class FunctionDynamicWrapper : public FunctionInvokerBase
        {
        public:
            typedef F Signature;
            Signature m_method;

            using class_t = typename function_traits<F>::class_type;
            using return_t = typename function_traits<F>::return_type;
            using arg_index_sequence = std::make_index_sequence<function_traits<Signature>::arg_count>;

            Variant Invoke(uint32_t UID, const ArgumentList &arguments)
            {
                return function_invoke_impl<return_t>::invoke(std::move(m_method), std::move(arguments), arg_index_sequence());
            }

            MethodDesc const& GetMethodInfo(uint32_t UID) override
            {
                return *(MethodDesc*)nullptr;
            }
        };
    }
}

#include "Impl/FunctionInvoker.hpp"
#include "Impl/VoidFunctionInvoker.hpp"
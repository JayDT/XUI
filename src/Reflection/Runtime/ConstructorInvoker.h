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

#include "ConstructorInvokerBase.h"

namespace System::Reflection
{
    inline namespace meta
    {
        class Variant;
        class Argument;

        /////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////
        class ConstructorWrapper : public ConstructorInvokerBase
        {
        public:
            typedef Variant invoker_impl(size_t methodId, Variant&& obj, const ArgumentList&& arguments);
            typedef MethodDesc const& methodinfo_impl(size_t methodId);

            std::size_t methodId = 0;
            invoker_impl* invoker = nullptr;
            methodinfo_impl* methodinfo = nullptr;

            Variant Invoke(const ArgumentList &arguments) override
            {
                if (invoker)
                    return invoker(methodId, std::move(Variant::Empty), std::move(arguments));
                return {};
            }

            MethodDesc const& GetMethodInfo() override
            {
                return methodinfo(methodId);
            }
        };


        template<typename F>
        class ConstructorDynamicWrapper : public ConstructorInvokerBase
        {
        public:
            typedef F Signature;
            Signature m_method;

            using class_t = typename function_traits<F>::class_type;
            using return_t = typename function_traits<F>::return_type;
            using arg_index_sequence = std::make_index_sequence<function_traits<Signature>::arg_count>;

            Variant Invoke(const ArgumentList &arguments)
            {
                return nullptr; //function_invoke_impl<return_t>::invoke(std::move(m_method), std::move(arguments), arg_index_sequence());
            }
        };

        //template<typename ClassType, bool IsDynamic, bool IsObjectWrapped, typename ...ArgTypes>
        //class ConstructorInvoker : public ConstructorInvokerBase
        //{
        //public:
        //    static_assert( THIS_ARG_COUNT <= MaxArgumentCount,
        //        "Constructor has too many arguments. It's time to generate more overloads." 
        //    );
        //
        //    Variant Invoke(const ArgumentList &arguments) override;
        //};
    }
}

#include "Impl/ConstructorInvoker.hpp"
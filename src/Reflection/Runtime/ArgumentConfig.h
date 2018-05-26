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

#include <vector>
#include <functional>

#include "Variant.h"

namespace System::Reflection
{
    inline namespace meta
    {
        class Argument;

        typedef std::vector<Argument*> ArgumentList;

        /////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////

        template <typename T>
        struct function_traits : function_traits< decltype(&T::operator()) > {};

        template<typename R, typename... Args>
        struct function_traits<R(Args...)>
        {
            static constexpr size_t arg_count = sizeof...(Args);

            using return_type = R;
            using arg_types = std::tuple<Args...>;
        };

        template<typename R, typename... Args>
        struct function_traits<R(*)(Args...)> : function_traits<R(Args...)> { };

        template<typename R, typename... Args>
        struct function_traits<R(&)(Args...)> : function_traits<R(Args...)> { };

        template<typename R, typename C, typename... Args>
        struct function_traits<R(C::*)(Args...)> : function_traits<R(Args...)> { using class_type = C; };

        template<typename R, typename C, typename... Args>
        struct function_traits<R(C::*)(Args...) const> : function_traits<R(Args...)> { using class_type = C; };

        template<typename R, typename C, typename... Args>
        struct function_traits<R(C::*)(Args...) volatile> : function_traits<R(Args...)> { using class_type = C; };

        template<typename R, typename C, typename... Args>
        struct function_traits<R(C::*)(Args...) const volatile> : function_traits<R(Args...)> { using class_type = C; };

        template<typename T>
        struct function_traits<std::function<T>> : function_traits<T> {};

        /////////////////////////////////////////////////////////////////////////////////////
        // use it like e.g:
        // param_types<F, 0>::type

        template<typename F, size_t Index>
        struct param_types
        {
            using type = typename std::tuple_element<Index, typename function_traits<F>::arg_types>::type;
        };

        template<typename F, size_t Index>
        using param_types_t = typename param_types<F, Index>::type;

        template<typename ReturnType>
        struct method_invoke_impl
        {
            template< typename ClassType, typename F, std::size_t ...S >
            __forceinline static Variant invoke(ClassType&& instance, F&& fn, const ArgumentList &&arguments, std::index_sequence<S...>)
            {
                return (std::forward<ClassType>(instance).*std::forward<F>(fn))((std::forward<const ArgumentList>(arguments)[S])-> template GetValue< param_types_t<F, S> >()...);
            }
        };

        template<>
        struct method_invoke_impl<void>
        {
            template< typename ClassType, typename F, std::size_t ...S >
            __forceinline static Variant invoke(ClassType&& instance, F&& fn, const ArgumentList &&arguments, std::index_sequence<S...>)
            {
                (std::forward<ClassType>(instance).*std::forward<F>(fn))((std::forward<const ArgumentList>(arguments)[S])-> template GetValue< param_types_t<F, S> >()...);
                return {};
            }
        };

        template<typename ReturnType>
        struct function_invoke_impl
        {
            template< typename F, std::size_t ...S >
            __forceinline static Variant invoke(F&& fn, const ArgumentList &&arguments, std::index_sequence<S...>)
            {
                return (std::forward<F>(fn))((std::forward<const ArgumentList>(arguments)[S])-> template GetValue< param_types_t<F, S> >()...);
            }
        };

        template<>
        struct function_invoke_impl<void>
        {
            template< typename F, std::size_t ...S >
            __forceinline static Variant invoke(F&& fn, const ArgumentList &&arguments, std::index_sequence<S...>)
            {
                (std::forward<F>(fn))((std::forward<const ArgumentList>(arguments)[S])-> template GetValue< param_types_t<F, S> >()...);
                return {};
            }
        };
    }
}
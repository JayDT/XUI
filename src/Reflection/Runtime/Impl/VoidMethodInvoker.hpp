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
        //class MethodWrapper<ClassType, void, ArgTypes...> : public MethodInvokerBase
        //{
        //public:
        //    typedef void(ClassType::*Signature)(ArgTypes...);
        //    static Signature m_method;
        //
        //    //using arg_index_sequence = std::make_index_sequence<function_traits<Signature>::arg_count>;
        //    //static constexpr arg_index_sequence arg_index_sequence_impl = arg_index_sequence();
        //
        //    Variant Invoke(Variant &obj, const ArgumentList &arguments) override
        //    {
        //        //UAssert(arguments.size() == THIS_ARG_COUNT,
        //        //    "Invalid method arguments.\nExpected %i args but got %i.",
        //        //    THIS_ARG_COUNT,
        //        //    arguments.size()
        //        //);
        //
        //        auto &instance = *static_cast<ClassType*>(obj.getPtr());
        //        void_method_invoke_impl(std::move(instance), std::move(m_method), std::move(arguments), std::make_index_sequence<sizeof...(ArgTypes)>());
        //        return { };
        //    }
        //};
        //
        //template<typename ClassType, typename ... ArgTypes>
        //typename MethodWrapper<ClassType, void, ArgTypes...>::Signature MethodWrapper<ClassType, void, ArgTypes...>::m_method = nullptr;
    }
}
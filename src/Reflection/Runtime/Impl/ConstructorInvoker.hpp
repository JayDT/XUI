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

#include "../Common/Logging.h"

#include "../Variant.h"
#include "../ArgumentConfig.h"
#include "../Argument.h"

namespace System::Reflection
{
    inline namespace meta
    {
        //////////////////////////////////////////////////////////////////THIS_ARG_COUNT///////
        //// Non Dynamic
        /////////////////////////////////////////////////////////////////////////
        //
        //template<typename ClassType, bool IsObjectWrapped, typename ...ArgTypes>
        //class ConstructorInvoker<ClassType, false, IsObjectWrapped, ArgTypes...> : public ConstructorInvokerBase
        //{
        //public:
        //    static_assert( THIS_ARG_COUNT <= MaxArgumentCount,
        //        "Constructor has too many arguments. It's time to generate more overloads."
        //    );
        //
        //    Variant Invoke(const ArgumentList &arguments) override
        //    {
        //        UAssert( arguments.size( ) == THIS_ARG_COUNT,
        //            "Invalid constructor arguments.\nExpected %i args but got %i.",
        //            THIS_ARG_COUNT,
        //            arguments.size( )
        //        );
        //
        //        return invoke<void, ArgTypes...>( arguments );
        //    }
        //
        //private:
        //    template<typename _>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return ClassType( );
        //    }
        //
        //    template<typename _, typename A1>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr())
        //        );
        //    }
        //
        //    template<typename _, typename A1, typename A2>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr()),
        //            *(typename std::remove_reference< A2 >::type*)(arguments[ 1 ]->GetPtr())
        //        );
        //    }
        //
        //    template<typename _, typename A1, typename A2, typename A3>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr()),
        //            *(typename std::remove_reference< A2 >::type*)(arguments[ 1 ]->GetPtr()),
        //            *(typename std::remove_reference< A3 >::type*)(arguments[ 2 ]->GetPtr())
        //        );
        //    }
        //
        //    template<typename _, typename A1, typename A2, typename A3, typename A4>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr()),
        //            *(typename std::remove_reference< A2 >::type*)(arguments[ 1 ]->GetPtr()),
        //            *(typename std::remove_reference< A3 >::type*)(arguments[ 2 ]->GetPtr()),
        //            *(typename std::remove_reference< A4 >::type*)(arguments[ 3 ]->GetPtr())
        //        );
        //    }
        //
        //    template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr()),
        //            *(typename std::remove_reference< A2 >::type*)(arguments[ 1 ]->GetPtr()),
        //            *(typename std::remove_reference< A3 >::type*)(arguments[ 2 ]->GetPtr()),
        //            *(typename std::remove_reference< A4 >::type*)(arguments[ 3 ]->GetPtr()),
        //            *(typename std::remove_reference< A5 >::type*)(arguments[ 4 ]->GetPtr())
        //        );
        //    }
        //
        //    template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr()),
        //            *(typename std::remove_reference< A2 >::type*)(arguments[ 1 ]->GetPtr()),
        //            *(typename std::remove_reference< A3 >::type*)(arguments[ 2 ]->GetPtr()),
        //            *(typename std::remove_reference< A4 >::type*)(arguments[ 3 ]->GetPtr()),
        //            *(typename std::remove_reference< A5 >::type*)(arguments[ 4 ]->GetPtr()),
        //            *(typename std::remove_reference< A6 >::type*)(arguments[ 5 ]->GetPtr())
        //        );
        //    }
        //
        //    template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr()),
        //            *(typename std::remove_reference< A2 >::type*)(arguments[ 1 ]->GetPtr()),
        //            *(typename std::remove_reference< A3 >::type*)(arguments[ 2 ]->GetPtr()),
        //            *(typename std::remove_reference< A4 >::type*)(arguments[ 3 ]->GetPtr()),
        //            *(typename std::remove_reference< A5 >::type*)(arguments[ 4 ]->GetPtr()),
        //            *(typename std::remove_reference< A6 >::type*)(arguments[ 5 ]->GetPtr()),
        //            *(typename std::remove_reference< A7 >::type*)(arguments[ 6 ]->GetPtr())
        //        );
        //    }
        //
        //    template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr()),
        //            *(typename std::remove_reference< A2 >::type*)(arguments[ 1 ]->GetPtr()),
        //            *(typename std::remove_reference< A3 >::type*)(arguments[ 2 ]->GetPtr()),
        //            *(typename std::remove_reference< A4 >::type*)(arguments[ 3 ]->GetPtr()),
        //            *(typename std::remove_reference< A5 >::type*)(arguments[ 4 ]->GetPtr()),
        //            *(typename std::remove_reference< A6 >::type*)(arguments[ 5 ]->GetPtr()),
        //            *(typename std::remove_reference< A7 >::type*)(arguments[ 6 ]->GetPtr()),
        //            *(typename std::remove_reference< A8 >::type*)(arguments[ 7 ]->GetPtr())
        //        );
        //    }
        //
        //    template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr()),
        //            *(typename std::remove_reference< A2 >::type*)(arguments[ 1 ]->GetPtr()),
        //            *(typename std::remove_reference< A3 >::type*)(arguments[ 2 ]->GetPtr()),
        //            *(typename std::remove_reference< A4 >::type*)(arguments[ 3 ]->GetPtr()),
        //            *(typename std::remove_reference< A5 >::type*)(arguments[ 4 ]->GetPtr()),
        //            *(typename std::remove_reference< A6 >::type*)(arguments[ 5 ]->GetPtr()),
        //            *(typename std::remove_reference< A7 >::type*)(arguments[ 6 ]->GetPtr()),
        //            *(typename std::remove_reference< A8 >::type*)(arguments[ 7 ]->GetPtr()),
        //            *(typename std::remove_reference< A9 >::type*)(arguments[ 8 ]->GetPtr())
        //        );
        //    }
        //
        //    template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr()),
        //            *(typename std::remove_reference< A2 >::type*)(arguments[ 1 ]->GetPtr()),
        //            *(typename std::remove_reference< A3 >::type*)(arguments[ 2 ]->GetPtr()),
        //            *(typename std::remove_reference< A4 >::type*)(arguments[ 3 ]->GetPtr()),
        //            *(typename std::remove_reference< A5 >::type*)(arguments[ 4 ]->GetPtr()),
        //            *(typename std::remove_reference< A6 >::type*)(arguments[ 5 ]->GetPtr()),
        //            *(typename std::remove_reference< A7 >::type*)(arguments[ 6 ]->GetPtr()),
        //            *(typename std::remove_reference< A8 >::type*)(arguments[ 7 ]->GetPtr()),
        //            *(typename std::remove_reference< A9 >::type*)(arguments[ 8 ]->GetPtr()),
        //            *(typename std::remove_reference< A10 >::type*)(arguments[ 9 ]->GetPtr())
        //        );
        //    }
        //};
        //
        /////////////////////////////////////////////////////////////////////////
        //// Dynamic, Non Wrapped
        /////////////////////////////////////////////////////////////////////////
        //
        //template<typename ClassType, typename ...ArgTypes>
        //class ConstructorInvoker<ClassType, true, false, ArgTypes...> : public ConstructorInvokerBase
        //{
        //public:
        //
        //    static_assert( THIS_ARG_COUNT <= MaxArgumentCount,
        //        "Constructor has too many arguments. It's time to generate more overloads."
        //    );
        //
        //    Variant Invoke(const ArgumentList &arguments) override
        //    {
        //        UAssert( arguments.size( ) == THIS_ARG_COUNT,
        //            "Invalid constructor arguments.\nExpected %i args but got %i.",
        //            THIS_ARG_COUNT,
        //            arguments.size( )
        //        );
        //
        //        return invoke<void, ArgTypes...>( arguments );
        //    }
        //
        //private:
        //    template<typename _>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return new ClassType( );
        //    }
        //
        //    template<typename _, typename A1>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return new ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr())
        //        );
        //    }
        //
        //    template<typename _, typename A1, typename A2>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return new ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr()),
        //            *(typename std::remove_reference< A2 >::type*)(arguments[ 1 ]->GetPtr())
        //        );
        //    }
        //
        //    template<typename _, typename A1, typename A2, typename A3>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return new ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr()),
        //            *(typename std::remove_reference< A2 >::type*)(arguments[ 1 ]->GetPtr()),
        //            *(typename std::remove_reference< A3 >::type*)(arguments[ 2 ]->GetPtr())
        //        );
        //    }
        //
        //    template<typename _, typename A1, typename A2, typename A3, typename A4>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return new ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr()),
        //            *(typename std::remove_reference< A2 >::type*)(arguments[ 1 ]->GetPtr()),
        //            *(typename std::remove_reference< A3 >::type*)(arguments[ 2 ]->GetPtr()),
        //            *(typename std::remove_reference< A4 >::type*)(arguments[ 3 ]->GetPtr())
        //        );
        //    }
        //
        //    template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return new ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr()),
        //            *(typename std::remove_reference< A2 >::type*)(arguments[ 1 ]->GetPtr()),
        //            *(typename std::remove_reference< A3 >::type*)(arguments[ 2 ]->GetPtr()),
        //            *(typename std::remove_reference< A4 >::type*)(arguments[ 3 ]->GetPtr()),
        //            *(typename std::remove_reference< A5 >::type*)(arguments[ 4 ]->GetPtr())
        //        );
        //    }
        //
        //    template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return new ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr()),
        //            *(typename std::remove_reference< A2 >::type*)(arguments[ 1 ]->GetPtr()),
        //            *(typename std::remove_reference< A3 >::type*)(arguments[ 2 ]->GetPtr()),
        //            *(typename std::remove_reference< A4 >::type*)(arguments[ 3 ]->GetPtr()),
        //            *(typename std::remove_reference< A5 >::type*)(arguments[ 4 ]->GetPtr()),
        //            *(typename std::remove_reference< A6 >::type*)(arguments[ 5 ]->GetPtr())
        //        );
        //    }
        //
        //    template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return new ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr()),
        //            *(typename std::remove_reference< A2 >::type*)(arguments[ 1 ]->GetPtr()),
        //            *(typename std::remove_reference< A3 >::type*)(arguments[ 2 ]->GetPtr()),
        //            *(typename std::remove_reference< A4 >::type*)(arguments[ 3 ]->GetPtr()),
        //            *(typename std::remove_reference< A5 >::type*)(arguments[ 4 ]->GetPtr()),
        //            *(typename std::remove_reference< A6 >::type*)(arguments[ 5 ]->GetPtr()),
        //            *(typename std::remove_reference< A7 >::type*)(arguments[ 6 ]->GetPtr())
        //        );
        //    }
        //
        //    template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return new ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr()),
        //            *(typename std::remove_reference< A2 >::type*)(arguments[ 1 ]->GetPtr()),
        //            *(typename std::remove_reference< A3 >::type*)(arguments[ 2 ]->GetPtr()),
        //            *(typename std::remove_reference< A4 >::type*)(arguments[ 3 ]->GetPtr()),
        //            *(typename std::remove_reference< A5 >::type*)(arguments[ 4 ]->GetPtr()),
        //            *(typename std::remove_reference< A6 >::type*)(arguments[ 5 ]->GetPtr()),
        //            *(typename std::remove_reference< A7 >::type*)(arguments[ 6 ]->GetPtr()),
        //            *(typename std::remove_reference< A8 >::type*)(arguments[ 7 ]->GetPtr())
        //        );
        //    }
        //
        //    template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return new ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr()),
        //            *(typename std::remove_reference< A2 >::type*)(arguments[ 1 ]->GetPtr()),
        //            *(typename std::remove_reference< A3 >::type*)(arguments[ 2 ]->GetPtr()),
        //            *(typename std::remove_reference< A4 >::type*)(arguments[ 3 ]->GetPtr()),
        //            *(typename std::remove_reference< A5 >::type*)(arguments[ 4 ]->GetPtr()),
        //            *(typename std::remove_reference< A6 >::type*)(arguments[ 5 ]->GetPtr()),
        //            *(typename std::remove_reference< A7 >::type*)(arguments[ 6 ]->GetPtr()),
        //            *(typename std::remove_reference< A8 >::type*)(arguments[ 7 ]->GetPtr()),
        //            *(typename std::remove_reference< A9 >::type*)(arguments[ 8 ]->GetPtr())
        //        );
        //    }
        //
        //    template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return new ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr()),
        //            *(typename std::remove_reference< A2 >::type*)(arguments[ 1 ]->GetPtr()),
        //            *(typename std::remove_reference< A3 >::type*)(arguments[ 2 ]->GetPtr()),
        //            *(typename std::remove_reference< A4 >::type*)(arguments[ 3 ]->GetPtr()),
        //            *(typename std::remove_reference< A5 >::type*)(arguments[ 4 ]->GetPtr()),
        //            *(typename std::remove_reference< A6 >::type*)(arguments[ 5 ]->GetPtr()),
        //            *(typename std::remove_reference< A7 >::type*)(arguments[ 6 ]->GetPtr()),
        //            *(typename std::remove_reference< A8 >::type*)(arguments[ 7 ]->GetPtr()),
        //            *(typename std::remove_reference< A9 >::type*)(arguments[ 8 ]->GetPtr()),
        //            *(typename std::remove_reference< A10 >::type*)(arguments[ 9 ]->GetPtr())
        //        );
        //    }
        //};
        //
        /////////////////////////////////////////////////////////////////////////
        //// Dynamic, Wrapped
        /////////////////////////////////////////////////////////////////////////
        //
        //template<typename ClassType, typename ...ArgTypes>
        //class ConstructorInvoker<ClassType, true, true, ArgTypes...> : public ConstructorInvokerBase
        //{
        //public:
        //    static_assert( THIS_ARG_COUNT <= MaxArgumentCount,
        //        "Constructor has too many arguments. It's time to generate more overloads."
        //    );
        //
        //    Variant Invoke(const ArgumentList &arguments) override
        //    {
        //        UAssert( arguments.size( ) == THIS_ARG_COUNT,
        //            "Invalid constructor arguments.\nExpected %i args but got %i.",
        //            THIS_ARG_COUNT,
        //            arguments.size( )
        //        );
        //
        //        return invoke<void, ArgTypes...>( arguments );
        //    }
        //
        //private:
        //    template<typename _>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return ObjectVariant( new ClassType( ) );
        //    }
        //
        //    template<typename _, typename A1>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return ObjectVariant( new ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr())
        //        ) );
        //    }
        //
        //    template<typename _, typename A1, typename A2>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return ObjectVariant( new ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr()),
        //            *(typename std::remove_reference< A2 >::type*)(arguments[ 1 ]->GetPtr())
        //        ) );
        //    }
        //
        //    template<typename _, typename A1, typename A2, typename A3>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return ObjectVariant( new ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr()),
        //            *(typename std::remove_reference< A2 >::type*)(arguments[ 1 ]->GetPtr()),
        //            *(typename std::remove_reference< A3 >::type*)(arguments[ 2 ]->GetPtr())
        //        ) );
        //    }
        //
        //    template<typename _, typename A1, typename A2, typename A3, typename A4>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return ObjectVariant( new ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr()),
        //            *(typename std::remove_reference< A2 >::type*)(arguments[ 1 ]->GetPtr()),
        //            *(typename std::remove_reference< A3 >::type*)(arguments[ 2 ]->GetPtr()),
        //            *(typename std::remove_reference< A4 >::type*)(arguments[ 3 ]->GetPtr())
        //        ) );
        //    }
        //
        //    template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return ObjectVariant( new ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr()),
        //            *(typename std::remove_reference< A2 >::type*)(arguments[ 1 ]->GetPtr()),
        //            *(typename std::remove_reference< A3 >::type*)(arguments[ 2 ]->GetPtr()),
        //            *(typename std::remove_reference< A4 >::type*)(arguments[ 3 ]->GetPtr()),
        //            *(typename std::remove_reference< A5 >::type*)(arguments[ 4 ]->GetPtr())
        //        ) );
        //    }
        //
        //    template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return ObjectVariant( new ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr()),
        //            *(typename std::remove_reference< A2 >::type*)(arguments[ 1 ]->GetPtr()),
        //            *(typename std::remove_reference< A3 >::type*)(arguments[ 2 ]->GetPtr()),
        //            *(typename std::remove_reference< A4 >::type*)(arguments[ 3 ]->GetPtr()),
        //            *(typename std::remove_reference< A5 >::type*)(arguments[ 4 ]->GetPtr()),
        //            *(typename std::remove_reference< A6 >::type*)(arguments[ 5 ]->GetPtr())
        //        ) );
        //    }
        //
        //    template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return ObjectVariant( new ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr()),
        //            *(typename std::remove_reference< A2 >::type*)(arguments[ 1 ]->GetPtr()),
        //            *(typename std::remove_reference< A3 >::type*)(arguments[ 2 ]->GetPtr()),
        //            *(typename std::remove_reference< A4 >::type*)(arguments[ 3 ]->GetPtr()),
        //            *(typename std::remove_reference< A5 >::type*)(arguments[ 4 ]->GetPtr()),
        //            *(typename std::remove_reference< A6 >::type*)(arguments[ 5 ]->GetPtr()),
        //            *(typename std::remove_reference< A7 >::type*)(arguments[ 6 ]->GetPtr())
        //        ) );
        //    }
        //
        //    template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return ObjectVariant( new ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr()),
        //            *(typename std::remove_reference< A2 >::type*)(arguments[ 1 ]->GetPtr()),
        //            *(typename std::remove_reference< A3 >::type*)(arguments[ 2 ]->GetPtr()),
        //            *(typename std::remove_reference< A4 >::type*)(arguments[ 3 ]->GetPtr()),
        //            *(typename std::remove_reference< A5 >::type*)(arguments[ 4 ]->GetPtr()),
        //            *(typename std::remove_reference< A6 >::type*)(arguments[ 5 ]->GetPtr()),
        //            *(typename std::remove_reference< A7 >::type*)(arguments[ 6 ]->GetPtr()),
        //            *(typename std::remove_reference< A8 >::type*)(arguments[ 7 ]->GetPtr())
        //        ) );
        //    }
        //
        //    template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return ObjectVariant( new ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr()),
        //            *(typename std::remove_reference< A2 >::type*)(arguments[ 1 ]->GetPtr()),
        //            *(typename std::remove_reference< A3 >::type*)(arguments[ 2 ]->GetPtr()),
        //            *(typename std::remove_reference< A4 >::type*)(arguments[ 3 ]->GetPtr()),
        //            *(typename std::remove_reference< A5 >::type*)(arguments[ 4 ]->GetPtr()),
        //            *(typename std::remove_reference< A6 >::type*)(arguments[ 5 ]->GetPtr()),
        //            *(typename std::remove_reference< A7 >::type*)(arguments[ 6 ]->GetPtr()),
        //            *(typename std::remove_reference< A8 >::type*)(arguments[ 7 ]->GetPtr()),
        //            *(typename std::remove_reference< A9 >::type*)(arguments[ 8 ]->GetPtr())
        //        ) );
        //    }
        //
        //    template<typename _, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
        //    Variant invoke(const ArgumentList &arguments) const
        //    {
        //        return ObjectVariant( new ClassType(
        //            *(typename std::remove_reference< A1 >::type*)(arguments[ 0 ]->GetPtr()),
        //            *(typename std::remove_reference< A2 >::type*)(arguments[ 1 ]->GetPtr()),
        //            *(typename std::remove_reference< A3 >::type*)(arguments[ 2 ]->GetPtr()),
        //            *(typename std::remove_reference< A4 >::type*)(arguments[ 3 ]->GetPtr()),
        //            *(typename std::remove_reference< A5 >::type*)(arguments[ 4 ]->GetPtr()),
        //            *(typename std::remove_reference< A6 >::type*)(arguments[ 5 ]->GetPtr()),
        //            *(typename std::remove_reference< A7 >::type*)(arguments[ 6 ]->GetPtr()),
        //            *(typename std::remove_reference< A8 >::type*)(arguments[ 7 ]->GetPtr()),
        //            *(typename std::remove_reference< A9 >::type*)(arguments[ 8 ]->GetPtr()),
        //            *(typename std::remove_reference< A10 >::type*)(arguments[ 9 ]->GetPtr())
        //        ) );
        //    }
        //};
    }
}

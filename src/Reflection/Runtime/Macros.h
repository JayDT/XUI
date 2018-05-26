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

#if defined(__REFLECTION_PARSER__)

    #define Meta(...) __attribute__((annotate(#__VA_ARGS__)))

    #define __META_EXTERNAL(type, guid)       \
        typedef type __META_EXTERNAL__##guid; \

    #define _META_EXTERNAL(type, guid) __META_EXTERNAL(type, guid)

    #define MetaExternal(type) _META_EXTERNAL(type, __COUNTER__)

    #define MetaAssemblyBegin namespace
    #define MetaAssemblyEnd {}

#else

    #define Meta(...) 

    #define MetaExternal(type)

    #define MetaInitialize(initializer)

    #define MetaAssemblyBegin
    #define MetaAssemblyEnd

#endif
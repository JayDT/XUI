/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2014 Steven Kirk <The Avalonia Project>
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

#ifndef __GLOBAL_INITIALIZER_SCHEDULER_H__
#define __GLOBAL_INITIALIZER_SCHEDULER_H__

#include "standard/callback.h"

#define STATIC_PRE_INITIALIZER(FUNC)                        \
bool __prepare_ ## FUNC()                                   \
{                                                           \
    XUI::Core::GlobalInitializer::PreInitializer += FUNC;   \
    return true;                                            \
}                                                           \
bool initialized_ ## FUNC = __prepare_ ## FUNC();

#define STATIC_INITIALIZER(FUNC)                        \
bool __prepare_ ## FUNC()                               \
{                                                       \
    XUI::Core::GlobalInitializer::Initializer += FUNC;  \
    return true;                                        \
}                                                       \
bool initialized_ ## FUNC = __prepare_ ## FUNC();

#define STATIC_POST_INITIALIZER(FUNC)                       \
bool __prepare_ ## FUNC()                                   \
{                                                           \
    XUI::Core::GlobalInitializer::PostInitializer += FUNC;  \
    return true;                                            \
}                                                           \
bool initialized_ ## FUNC = __prepare_ ## FUNC();

#define STATIC_CLASS_PRE_INITIALIZER(CLASS, FUNC)           \
bool __prepare_ ## CLASS()                                  \
{                                                           \
    XUI::Core::GlobalInitializer::PreInitializer += FUNC;   \
    return true;                                            \
}                                                           \
bool initialized_ ## CLASS = __prepare_ ## CLASS();

#define STATIC_CLASS_INITIALIZER(CLASS, FUNC)           \
bool __prepare_##CLASS()								\
{                                                       \
    XUI::Core::GlobalInitializer::Initializer += FUNC;  \
    return true;                                        \
}                                                       \
bool initialized_##CLASS = __prepare_##CLASS();			\

#define STATIC_CLASS_POST_INITIALIZER(CLASS, FUNC)          \
bool __prepare_ ## CLASS()                                  \
{                                                           \
    XUI::Core::GlobalInitializer::PostInitializer += FUNC;  \
    return true;                                            \
}                                                           \
bool initialized_ ## CLASS = __prepare_ ## CLASS();

#define STATIC_CLASS_AFTER_SERVICE_INITIALIZER(CLASS, FUNC)             \
bool __prepare_ ## CLASS()                                              \
{                                                                       \
    XUI::Core::GlobalInitializer::AfterBaseServiceInitializer += FUNC;  \
    return true;                                                        \
}                                                                       \
bool initialized_ ## CLASS = __prepare_ ## CLASS();

namespace XUI::Core
{
        struct GlobalInitializer
        {
            static TC_XUI_API System::Async::InvokeHandler PreInitializer;
            static TC_XUI_API System::Async::InvokeHandler Initializer;
            static TC_XUI_API System::Async::InvokeHandler PostInitializer;
            static TC_XUI_API System::Async::InvokeHandler AfterBaseServiceInitializer;
            static TC_XUI_API void Finalize();
            static TC_XUI_API void FinalizeAfterService();
        };

        struct UIDispatcher
        {
            enum class Levels
            {
                AfterRender = 0
            };

            static TC_XUI_API System::Async::InvokeHandler OnRender;

            static TC_XUI_API void Update(Levels level);
        };
}
#endif // !__GLOBAL_INITIALIZER_SCHEDULER_H__

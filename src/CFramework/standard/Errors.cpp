/*
 * Copyright (C) 2008-2015 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
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

#include "Errors.h"

#include <cstdio>
#include <cstdlib>
//#include <thread>
#include <cstdarg>

#include "lpt/call_stack.hpp"

typedef lpt::stack::call_stack<40> wstack_type; // We want max 40 call frames
typedef lpt::stack::call_stack_info< wstack_type
    , lpt::stack::extended_symbol_info // All possible symbol information for the platform
>     extended_call_stack_info_type;

namespace System::Debug {

void Assert(char const* file, int line, char const* function, char const* message)
{
    fprintf(stderr, "\n%s:%i in %s ASSERTION FAILED:\n  %s\n",
            file, line, function, message);

    DEBUG_BREAK;
}

void Assert(char const* file, int line, char const* function, char const* message, char const* format, ...)
{
    va_list args;
    va_start(args, format);

    fprintf(stderr, "\n%s:%i in %s ASSERTION FAILED:\n  %s ", file, line, function, message);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    fflush(stderr);

    va_end(args);
    DEBUG_BREAK;
} 

void Fatal(char const* file, int line, char const* function, char const* message, ...)
{
    va_list args;
    va_start(args, message);

    fprintf(stderr, "\n%s:%i in %s FATAL ERROR:\n  ", file, line, function);
    vfprintf(stderr, message, args);
    fprintf(stderr, "\n");
    fflush(stderr);

    //std::this_thread::sleep_for(std::chrono::seconds(10));
    DEBUG_BREAK;
    exit(1);
}

void Error(char const* file, int line, char const* function, char const* message)
{
    fprintf(stderr, "\n%s:%i in %s ERROR:\n  %s\n",
                   file, line, function, message);

    DEBUG_BREAK;
}

void Warning(char const* file, int line, char const* function, char const* message)
{
    fprintf(stderr, "\n%s:%i in %s WARNING:\n  %s\n",
                   file, line, function, message);
}

} // namespace Trinity

namespace boost
{
    void OutException(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);

        vfprintf(stderr, fmt, args);
        fprintf(stderr, "\n %s \n", System::Runtime::Backtrace().c_str());
        fflush(stderr);
    }
}


namespace System
{
    namespace Runtime
    {
        std::string Backtrace()
        {
            static std::mutex traceLock;

            std::lock_guard<std::mutex> g(traceLock);

            wstack_type here(true);
            std::stringstream ss;

            ss << "Stack: " << here.depth() << " frames:\n"
                << extended_call_stack_info_type(here) << std::endl;

            return ss.str();
        }
    }
}

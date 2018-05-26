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

#include <iostream>

namespace System::Reflection
{
    namespace logging
    {
        template<typename... Args>
        void Assert(
            const std::string &file,
            const std::string &function,
            unsigned line,
            const std::string &format,
            const Args&... args
        )
        {
            std::cerr << "----- Assertion Failed -----" << std::endl
                      << "----------------------------" << std::endl;

            std::fprintf( stderr, format.c_str( ), args... );

            std::cerr << std::endl;
            std::cerr << "----------------------------" << std::endl;
            std::cerr << "file: " << file << std::endl;
            std::cerr << "function: " << function << std::endl;
            std::cerr << "line: " << line << std::endl;
            std::cerr << "----------------------------" << std::endl;
            std::cerr << std::endl;

            std::abort( );
        }
    }
}
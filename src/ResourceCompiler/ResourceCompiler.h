/*
* Copyright (C) 2017-2018 Tauri JayD <https://www.>
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

#ifndef __RS_RESOURCE_COMPILER_H__
#define __RS_RESOURCE_COMPILER_H__

namespace System::Resource
{
    enum class ResourceTypes : uint8;
    class ResourceBuilder;

    class ResourceCompiler
    {
    public:
        constexpr static const size_t MaxChunkSize = 1024 * 1024;

        std::vector<std::vector<byte>*> m_input;
        std::vector<std::vector<byte>*> m_output;

        ResourceCompiler();

        void Compile(ResourceTypes type);
    };
}

#endif // !__RS_RESOURCE_COMPILER_H__

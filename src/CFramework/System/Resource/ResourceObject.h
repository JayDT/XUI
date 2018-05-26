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

#ifndef __CF_RESOURCE_OBJECT_H__
#define __CF_RESOURCE_OBJECT_H__

#include <standard/client/DataSource_Standard.h>

namespace System::Resource
{
    class TC_CFRAMEWORK_API ResourceObject
    {
        bool _created = false;
        bool _isCompressed = false;
        std::string _name;
        std::vector<byte> _data;

    public:

        ResourceObject(const std::string& modulName);
        virtual ~ResourceObject() {}

        std::string const& Name() const { return _name; }

        void LoadCompressRawData(const uint8_t* src, const size_t src_size, const size_t total_size);
        void CreateResource();

        std::string ToString() const;
        std::vector<byte> const& ToBytes() const;

        System::IO::MemoryStreamReader* ToMemoryStreamReader() const;
        std::shared_ptr<System::IO::MemoryStreamReader> ToSharedMemoryStreamReader() const;
    };
}

#endif //!__CF_RESOURCE_MANAGER_H__
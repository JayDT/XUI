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

#include "ResourceObject.h"

#include <zlib/zlib.h>

bool uncompress(void *dst, int total_size, void *src, int src_size)
{
    memset(dst, 0, total_size);

    z_stream c_stream;

    c_stream.zalloc = (alloc_func)0;
    c_stream.zfree = (free_func)0;
    c_stream.opaque = (voidpf)0;

    int z_res = inflateInit(&c_stream);

    c_stream.next_out = (Bytef*)dst;
    c_stream.avail_out = total_size;
    c_stream.next_in = (Bytef*)src;
    c_stream.avail_in = (uInt)src_size;

    z_res = inflate(&c_stream, Z_FINISH);
    return z_res >= Z_OK;
}

System::Resource::ResourceObject::ResourceObject(const std::string& modulName)
    : _name(modulName)
{
}

void System::Resource::ResourceObject::LoadCompressRawData(const uint8_t * src, const size_t src_size, const size_t total_size)
{
    _isCompressed = true;
    size_t wpos = _data.size();
    _data.resize(_data.size() + total_size);
    bool success = uncompress((void*)&_data[wpos], total_size, (void*)src, src_size);
    System::ThrowIfFailed<System::NotSupportedException>(success);
}

void System::Resource::ResourceObject::CreateResource()
{
    _created = true;
}

std::string System::Resource::ResourceObject::ToString() const
{
    return std::string((const char*)_data.data(), _data.size());
}

std::vector<byte> const& System::Resource::ResourceObject::ToBytes() const
{
    return _data;
}

System::IO::MemoryStreamReader * System::Resource::ResourceObject::ToMemoryStreamReader() const
{
    auto stream = new System::IO::MemoryStreamReader((byte*)_data.data(), _data.size());
    stream->FileName = _name;
    return stream;
}

std::shared_ptr<System::IO::MemoryStreamReader> System::Resource::ResourceObject::ToSharedMemoryStreamReader() const
{
    auto stream = std::make_shared<System::IO::MemoryStreamReader>((byte*)_data.data(), _data.size());
    stream->FileName = _name;
    return stream;
}

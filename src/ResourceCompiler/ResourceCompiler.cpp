#include "ResourceCompiler.h"
#include "ResourceBuilder.h"

#include "ResourceTypes/Font.h"
#include "ResourceTypes/Image.h"
#include "ResourceTypes/Shader.h"
#include "ResourceTypes/Xaml.h"

#include <zlib/zlib.h>

bool compress(void *dst, uint32 *dst_size, void *src, int src_size, int compression_level = Z_BEST_SPEED)
{
    z_stream c_stream;

    c_stream.zalloc = (alloc_func)0;
    c_stream.zfree = (free_func)0;
    c_stream.opaque = (voidpf)0;

    int z_res = deflateInit(&c_stream, compression_level);
    if (z_res != Z_OK)
    {
        *dst_size = 0;
        return false;
    }

    c_stream.next_out = (Bytef*)dst;
    c_stream.avail_out = *dst_size;
    c_stream.next_in = (Bytef*)src;
    c_stream.avail_in = (uInt)src_size;

    z_res = deflate(&c_stream, Z_FINISH);
    if (z_res != Z_STREAM_END || z_res == Z_STREAM_ERROR)
    {
        *dst_size = 0;
        return false;
    }

    if (c_stream.avail_in != 0)
    {
        *dst_size = 0;
        return false;
    }

    if (c_stream.msg != 0)
    {
        *dst_size = 0;
        return false;
    }

    z_res = deflateEnd(&c_stream);
    if (z_res != Z_OK)
    {
        *dst_size = 0;
        return false;
    }

    *dst_size = c_stream.total_out;
    return true;
}


System::Resource::ResourceCompiler::ResourceCompiler()
{
}

void System::Resource::ResourceCompiler::Compile(System::Resource::ResourceTypes type)
{
    if (type == System::Resource::ResourceTypes::RAW)
    {
        byte* buffer = new byte[ResourceCompiler::MaxChunkSize];
        for (int i = 0; i < m_input.size(); ++i)
        {
            uint32 dstSize;
            compress(buffer, &dstSize, m_input[i]->data(), m_input[i]->size(), 3);

            std::vector<byte>* out = new std::vector<byte>(dstSize);
            memcpy(out->data(), buffer, dstSize);
            m_output.push_back(out);
        }

        delete buffer;
    }
    else
    {
        //ToDo: intermediate compiler
    }
}

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

#ifndef __StreamEx_H__
#define __StreamEx_H__

#include <standard/Platform/Define.h>
#include <streambuf>
#include <vector>

namespace System
{
    class memorystreambuf : public ::std::streambuf
    {
        std::vector<byte>* buf_ = nullptr;

    public:

        memorystreambuf(std::vector<byte>* buf)
            : buf_(buf)
        {
            setbuf((char*)buf_->data(), buf_->size());
        }

        ::std::streambuf* setbuf(char_type* const s,
            ::std::streamsize const n) final
        {
            auto const begin(s);
            auto const end(s + n);

            setg(begin, begin, end);
            setp(begin, end);

            return this;
        }

        pos_type seekpos(pos_type const pos,
            ::std::ios_base::openmode const which = ::std::ios_base::in |
            ::std::ios_base::out) final
        {
            switch (which)
            {
                case ::std::ios_base::in:
                    if (pos < egptr() - eback())
                    {
                        setg(eback(), eback() + pos, egptr());

                        return pos;
                    }
                    else
                    {
                        break;
                    }

                case ::std::ios_base::out:
                    if (pos < epptr() - pbase())
                    {
                        setp(pbase(), epptr());
                        pbump(pos);

                        return pos;
                    }
                    else
                    {
                        break;
                    }

                default:
                    assert(0);
            }

            return pos_type(off_type(-1));
        }

        ::std::streamsize xsgetn(char_type* const s,
            ::std::streamsize const count) final
        {
            auto const size(::std::min(::std::streamsize(egptr() - gptr()), count));

            ::std::memcpy(s, gptr(), size);

            gbump(size);

            return egptr() == gptr() ? traits_type::eof() : size;
        }

        ::std::streamsize xsputn(char_type const* s,
            ::std::streamsize const count) final
        {
            auto const size(::std::min(::std::streamsize(epptr() - pptr()), count));

            ::std::memcpy(pptr(), s, size);

            pbump(size);

            return epptr() == pptr() ? traits_type::eof() : size;
        }
    };
}

#endif //!
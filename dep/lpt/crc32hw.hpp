/*
 *  Copyright 2012 Aurelian Melinte. 
 *  Released under GPL 3.0 or later. 
 *
 *  You need a C++0x compiler. 
 * 
 */
 

#include <cstdio>

#pragma once 

namespace lpt { namespace algo { namespace hardware {

// http://lwn.net/Articles/292984/
// http://www.strchr.com/crc32_popcnt
inline unsigned long int
crc32 (const void *buffer, size_t len, unsigned long int crc)
{
    const char *buf = static_cast<const char*>(buffer);
    while (len--) {
        __asm__ __volatile__(
        ".byte 0xf2, 0xf, 0x38, 0xf0, 0xf1"
        :"=S"(crc)
        :"0"(crc), "c"(*buf)
        );
        buf++;
    }

    return crc;
}

}}} //namespace


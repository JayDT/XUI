/*
 *  Copyright 2012 Aurelian Melinte. 
 *  Released under GPL 3.0 or later. 
 *
 *  You need a C++0x compiler. 
 * 
 */
 

#include <cstdio>

#include <lpt/crc32ace.hpp>
#include <lpt/crc32hw.hpp>


#pragma once 

namespace lpt { namespace algo { 

inline unsigned long int
crc32 (const void *buffer, size_t len, unsigned long int crc)
{
    //return lpt::algo::hardware::crc32(buffer, len, crc);

    // Otherwise, if the platform does not support hardware CRC: 
    return ace::crc32(buffer, len, crc);
}

}} //namespace


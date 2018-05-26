/*
 *  Copyright 2012 Aurelian Melinte.
 *  Released under GPL 3.0 or later.
 *
 *  You need a C++0x compiler.
 *
 *  Ref: http://mainisusuallyafunction.blogspot.com/2012/01/embedding-gdb-breakpoints-in-c-source.html
 */


#pragma once

namespace lpt { namespace intel {


#define breakpoint1  asm volatile ("int3;")


#define breakpoint2 \
    asm("0:"                              \
        ".pushsection embed-breakpoints;" \
        ".quad 0b;"                       \
        ".popsection;")


}} //namespace

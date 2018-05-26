/*
 *  Copyright 2012 Aurelian Melinte.
 *  Released under GPL 3.0 or later.
 *
 *  You need a C++0x compiler.
 *
 */


#pragma once

namespace lpt {

// S. Schurr
#define static_assert(...)  std::static_assert(__VA_ARGS__, #__VA_ARGS__)

} //namespace

/* *  Copyright 2012 Aurelian Melinte.  *  Released under GPL 3.0 or later.  * *  You need a C++0x compiler.  *  */#include <chrono>

#include <lpt/nocopy.hpp>

#pragma once 


namespace lpt { class timing : public nocopy{public:

    typedef std::chrono::milliseconds milliseconds;    timing()
        : _start(std::chrono::high_resolution_clock::now())    {
    }

    template < typename Units >
    unsigned long elapsed() const
    {
        Units us = std::chrono::duration_cast<Units>(std::chrono::high_resolution_clock::now() - _start);
        return us.count();
    }        ~timing()    {    }private:

    std::chrono::high_resolution_clock::time_point _start;};  } //namespace

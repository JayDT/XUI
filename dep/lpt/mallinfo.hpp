/*
 *  Copyright 2012 Aurelian Melinte. 
 *  Released under GPL 3.0 or later. 
 *
 *  You need a C++0x compiler. 
 * 
 */
 
#include <malloc.h>

 
#pragma once 


namespace lpt { namespace process {

 
class memory
{
public:
 
    memory() : _meminfo(::mallinfo()) {}
 
    int total() const
    {
        return _meminfo.hblkhd + _meminfo.uordblks;
    }
 
    bool operator==(memory const& other) const
    {
        return total() == other.total();
    }
 
    bool operator!=(memory const& other) const
    {
        return total() != other.total();
    }
 
    bool operator<(memory const& other) const
    {
        return total() < other.total();
    }
 
    bool operator<=(memory const& other) const
    {
        return total() <= other.total();
    }
 
    bool operator>(memory const& other) const
    {
        return total() > other.total();
    }
 
    bool operator>=(memory const& other) const
    {
        return total() >= other.total();
    }
 
private:
 
    struct mallinfo _meminfo;
};
 
} } //namespace

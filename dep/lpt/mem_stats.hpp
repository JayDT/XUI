/*
 *  Copyright 2012 Aurelian Melinte. 
 *  Released under GPL 3.0 or later. 
 *
 *  You need a C++0x compiler. 
 * 
 */
 
 
#include <cassert>

#include <map> 
#include <unordered_map>

#include <lpt/nocopy.hpp>
#include <lpt/allocator.hpp>
#include <lpt/call_stack_stats.hpp> //stack::key_type

#pragma once 


typedef unsigned long long memsize_type;

/*
 *
 */ 
namespace lpt { 


/*
 * Allocations tracking
 */

typedef struct _allocation {
    memsize_type num_bytes; 
    lpt::stack::key_type key; //stack
    bool is_new; //new allocation
} allocation;

typedef std::map< __ptr_t
                , allocation
                , std::less<__ptr_t>
                , lpt::allocator03<std::pair<const __ptr_t, allocation> >
                > allocation_map_type;

//typedef std::unordered_map<  __ptr_t
//                           , allocation
//                           , std::hash<__ptr_t >
//                           , std::equal_to<__ptr_t >
//                           , lpt::allocator<std::pair<const __ptr_t, allocation> > 
//                           > allocation_map_type;


} //namespace

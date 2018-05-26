/*
 *  Copyright 2012 Aurelian Melinte. 
 *  Released under GPL 3.0 or later. 
 *
 *  You need a C++0x compiler. 
 * 
 */
 
 
#include <cassert>

#include <algorithm>
#include <map> 
#include <unordered_map>

#include <lpt/nocopy.hpp>
#include <lpt/call_stack.hpp>
#include <lpt/allocator.hpp>

#include <lpt/crc32.hpp> // software or hardware impl?

#pragma once 


/*
 *
 */ 
namespace lpt { namespace stack {


// Unique stack id
typedef unsigned long int key_type;

// FIXME: how about collisions?
template < typename CallStack >
struct crc32key
{
    key_type operator()(const CallStack& stack)
    {
        //key_type key = lpt::algo::crc32(stack.raw_data(), stack.depth() * sizeof(void*), 0);
        key_type key(0);
        std::for_each(stack.begin(),
                      stack.end(), 
                      [&](const call_frame& frame) 
                      {
                          const address_type addr = frame.addr();
                          key = lpt::algo::crc32(&addr, 1*sizeof(address_type), key);
                      });
        return key;
    }
};


template < typename Data
         , typename CallStack //call_stack
         //, key_type (*KeyAlgo)(const CallStack&)   = crc32key
         , typename KeyAlgo
         >
class stack_stats : public Data
                  , public CallStack
{
public:

    typedef KeyAlgo key_algo_type;

    stack_stats() : CallStack(false), _key(0) {} // For containers
    stack_stats(bool capture_stack) : CallStack(capture_stack), _key(0) {}

    key_type key() const
    {
        if (!_key)
        {
            _key = KeyAlgo(static_cast<CallStack>(*this));
        }
        return _key;
    }

private:

    mutable key_type   _key; // Cache it
};

//
// Usage:
//   typedef lpt::stack::stack_stats<stack_mem_data, lpt::stack::bfd::stack>  stack_mem_stats;
//   lpt::stack::xstats_map<stack_mem_stats>::type  stack_mem_stats_map;
//

//template < template <typename, typename> class Stack_stats >
template <typename StackStats>
struct stats_map {
    typedef std::map< lpt::stack::key_type
                    , StackStats
                    , std::less<lpt::stack::key_type>
                    , lpt::allocator03<std::pair<const lpt::stack::key_type
                                                , StackStats
                                                >
                                       >
                    > type;

    //typedef std::unordered_map< lpt::stack::key_type
    //                          , StackStats
    //                          , std::hash<lpt::stack::key_type >
    //                          , std::equal_to<lpt::stack::key_type >
    //                          , lpt::allocator<std::pair<const lpt::stack::key_type
    //                                                    , StackStats
    //                                                    >
    //                                          >
    //                          > type;
};

//template < typename T >
//using map = std::mapstd::map< lpt::stack::key_type
//                            , T
//                            , std::less<lpt::stack::key_type>
//                            , lpt::allocator<std::pair<const lpt::stack::key_type
//                                                      , T
//                                                      >
//                                            >
//                            >;

}} //namespace

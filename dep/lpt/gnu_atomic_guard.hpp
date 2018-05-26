/*
*  Copyright 2012 Aurelian Melinte. 
*  Released under GPL 3.0 or later. 
*
*  You need a C++0x compiler. 
* 
*/

#include <lpt/nocopy.hpp>

#pragma once 

namespace lpt { 


template < typename T >
class gnu_atomic_guard : public nocopy
{
public:
    gnu_atomic_guard(T* ptr, T oldval, T newval)
	    : _ptr(ptr)
        , _oldval(oldval)
        , _newval(newval)
    {
	    _foundval = __sync_val_compare_and_swap(ptr, oldval, newval);
    }
    
    bool acquired() volatile const
    {
        return _foundval == _oldval;
    }
	
	~gnu_atomic_guard()
	{
        if (acquired()) {
		    T oldval = __sync_val_compare_and_swap(_ptr, _newval, _oldval);
            oldval = oldval; // Keep compiler happy
            //assert(oldval == __newval && *_ptr == _oldval); // Only if single threaded
        }
    }
	
	private:
	
	    T* _ptr;
        T  _oldval;
        T  _newval;
        T  _foundval;
};

} //namespace

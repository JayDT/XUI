/*
 *  Copyright 2012 Aurelian Melinte. 
 *  Released under GPL 3.0 or later. 
 *
 *  Evil singleton. 
 * 
 */

#pragma once

namespace lpt { 

template <class T>
class singleton : private T
{
private:
    singleton()
    {
    }

    ~singleton()
    {
    }

public:
    static T& instance()
    {
        static singleton<T> _staticT;
        return(_staticT);
    }
};

} //lpt

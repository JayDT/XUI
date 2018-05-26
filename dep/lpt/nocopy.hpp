/*
 *  Copyright 2012 Aurelian Melinte. 
 *  Released under GPL 3.0 or later. 
 *
 *  You need a C++0x compiler. 
 * 
 */
 
 
#pragma once 

namespace lpt { 

class nocopy
{
protected:
   
    nocopy() {}
    ~nocopy() {}
      
private:  
      
    nocopy( const nocopy& );
    const nocopy& operator=( const nocopy& );
};
  
} //namespace

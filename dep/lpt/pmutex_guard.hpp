/* 
 *  Copyright 2012 Aurelian Melinte.  
 *  Released under GPL 3.0 or later.
 *  
 *  You need a C++0x compiler. 
 *  
 */
 
 
 #include <pthread.h>
 #include <lpt/nocopy.hpp> 
 
 #pragma once 
 
 namespace lpt { 
 
 class pmutex_guard : public nocopy
 {
 public:
     pmutex_guard(pthread_mutex_t* mutex)
             : _mutex(mutex)
    {
          int ret = pthread_mutex_lock(_mutex);
          if (-1 == ret)
              throw std::runtime_error("pthread_mutex_lock");
    }
    
    ~pmutex_guard()
    {
        int ret = pthread_mutex_unlock(_mutex);
    }
    
    
    private:
    
        pthread_mutex_t * _mutex;
};


} //namespace

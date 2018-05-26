/*
 *  Copyright 2012 Aurelian Melinte. 
 *  Released under GPL 3.0 or later. 
 *
 *  You need a C++0x compiler. 
 * 
 */
 
 
#include <cassert>

#include <list>
#include <limits>

#pragma once 


/*
 *
 */ 
namespace lpt { 


/*
 * Custom allocator (Josuttis)
 */
template <class T>
class allocator03 {
public:

       typedef T              value_type;
       typedef T*             pointer;
       typedef const T*       const_pointer;
       typedef T&             reference;
       typedef const T&       const_reference;
       typedef std::size_t    size_type;
       typedef std::ptrdiff_t difference_type;

       // rebind allocator to type U
       template <class U>
       struct rebind {
           typedef allocator03<U> other;
       };

       // return address of values
       pointer address (reference value) const {
           return &value;
       }
       const_pointer address (const_reference value) const {
           return &value;
       }


       static const int initial_num_items = 40 * 1024;
       allocator03() throw() {
           for (int i=0; i<initial_num_items; i++) {
               __heap.push_back( new T() );
           }
       }
       allocator03(const allocator03&) throw() {}
       template <class U> allocator03 (const allocator03<U>&) throw() {}
       ~allocator03() throw() {} //FIXME: leaks __heap

       // return maximum number of elements that can be allocated
       size_type max_size () const throw() {
           return std::numeric_limits<std::size_t>::max() / sizeof(T);
       }

       // allocate but don't initialize num elements of type T
       pointer allocate (size_type num, const void* = 0) {
           assert(num == 1);
           
           pointer ret = 0;
           
           if (__heap.empty()) {
               ret = (pointer)(::operator new(num*sizeof(T)));
           }
           else {
               ret = __heap.front();
               __heap.pop_front();
           }
           
           return ret;
       }

       // deallocate storage p of deleted elements
       void deallocate (pointer p, size_type num) {
           //::operator delete((void*)p);
           assert(num ==0 || num ==1);
           __heap.push_back((pointer)p);
       }
       
       // initialize elements of allocated storage p with value value
       void construct (pointer p, const T& value) {
           // initialize memory with placement new
           new((void*)p)T(value);
       }

       // destroy elements of initialized storage p
       void destroy (pointer p) {
           // destroy objects by calling their destructor
           p->~T();
       }
       
private:

      std::list<T*> __heap;
};

// return that all specializations of this allocator are interchangeable
template <class T1, class T2>
bool operator== (const allocator03<T1>&,
                 const allocator03<T2>&) throw() {
   return true;
}
template <class T1, class T2>
bool operator!= (const allocator03<T1>&,
                 const allocator03<T2>&) throw() {
   return false;
}



template <class T>
class allocator11 {
public:
    typedef T value_type;

    allocator11();
    template <class U>
    allocator11(const allocator11<U>&);

    T* allocate(std::size_t);

    void deallocate(T*, std::size_t);
};

template <class T, class U>
bool operator==(const allocator11<T>&, const allocator11<U>&);

template <class T, class U>
bool operator!=(const allocator11<T>&, const allocator11<U>&);



} //namespace

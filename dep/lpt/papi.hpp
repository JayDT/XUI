/*
 *  C++ Performance API wrapper.
 *
 *  Copyright 2012 Aurelian Melinte. 
 *  Released under GPL 3.0 or later. 
 *
 *  You need a C++0x compiler. 
 * 
 */


#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <vector>
#include <list>
#include <cassert>
#include <atomic>
#include <mutex>

#include <lpt/call_stack.hpp>
#include <lpt/stackonly.hpp>
#include <lpt/singleton.hpp>

#include <papi.h>

# pragma once

namespace lpt { namespace papi {

/*
 *
 */

class event
{
public:

    event(int num, const char* info) 
        : _init(false)
        , _num(num)
        , _info(info)
    {}
    
   
    int num() const { return _num; }
    const char * name() const { init(); return _name; }
    const char * info() const 
    { 
        init(); 
        return _info ? _info : _einfo.short_descr;
    }

private: 

    void init() const 
    {
        if (!_init) {
            PAPI_event_code_to_name(_num, _name);
            PAPI_get_event_info(_num, &_einfo);
            
            _init = true;
        }
    }
    
private: 

    mutable bool _init;

    int _num;
    const char* _info;
    mutable char _name[PAPI_MAX_STR_LEN+1];
    mutable PAPI_event_info_t _einfo;
};

/*
 * What will PAPI be watching over
 */

typedef std::vector<event> events_type;
static  const events_type events = {  
      {PAPI_TOT_INS, "Total instructions"} 
    , {PAPI_TOT_CYC, "Total cpu cycles"}
    , {PAPI_L1_DCM,  "L1 load  misses"}
//  , {PAPI_L1_STM,  "L1 store  missess"}
    , {PAPI_L2_DCM,  "L2 load  misses"}
//  , {PAPI_L2_STM,  "L2 store  missess"}
    , {PAPI_BR_MSP,  "Branch mispredictions"}
};
static const int num_events = events.size();



/*
 *
 */

class papi_error : public std::runtime_error
{
public: 

    typedef lpt::stack::call_stack<40> stack_type;

    explicit papi_error(std::string const& msg, int err) 
        : std::runtime_error(msg)
        , _msg(msg)
        , _err(err)
        , _where(true)
    {
        _msg += ": ";
        _msg += PAPI_strerror(_err);
    }
    
    papi_error(papi_error&& other) noexcept
        : std::runtime_error(std::move(other._msg.c_str()))
        , _msg(std::move(other._msg))
        , _err(other._err)
        , _where(other._where)
    {
        swap(*this, other);
    }
    
    papi_error& operator=(papi_error other) noexcept
    {
        swap(*this, other);
        return *this;
    }
    
    virtual ~papi_error() noexcept {}
    
    void swap(papi_error& left, papi_error& right) noexcept
    {
        std::swap(left._msg,   right._msg);
        std::swap(left._err,   right._err);
        std::swap(left._where, right._where);
    }

    const char *what() const noexcept
    {
        return _msg.c_str();
    }

    const stack_type& where() const
    {
        return _where;
    }

private: 

    std::string _msg;
    int         _err;
    stack_type  _where;
};



/*
 * Policies
 */

struct counting_per_thread
{
    struct process 
    {
        static void apply(int event_set)
        {
#if 0 
            int retval = PAPI_OK;
            if ( (retval = PAPI_start(event_set)) != PAPI_OK) {
                throw papi_error("PAPI_start", retval);
            }
#endif        
        }
    };
    struct thread 
    {
        static void apply(int event_set)
        {
            int retval = PAPI_OK;
            if ( (retval = PAPI_start(event_set)) != PAPI_OK) {
                throw papi_error("PAPI_start", retval);
            }
        }
    };

protected:
    ~counting_per_thread() {}
};


struct multiplex_none
{
    struct process 
    {
        static void apply(int event_set)
        {
#if 0
            int retval = PAPI_OK;
            if ( (retval = PAPI_multiplex_init()) != PAPI_OK) {
                throw papi_error("PAPI_multiplex_init", retval);
            }
#endif           
        }
    };
    struct thread 
    {
        static void apply(int event_set)
        {
#if 0 
            int retval = PAPI_OK;
            if ( (retval = PAPI_set_multiplex(event_set)) != PAPI_OK) {
                throw papi_error("PAPI_set_multiplex", retval);
            }
#endif
        }
    };

protected:
    ~multiplex_none() {}
};


/*
 * A tool to ensure only one PAPI library across all templates.
 */
class unique
{
public:

    unique()
        : _one(ATOMIC_FLAG_INIT)
    {
    }

    ~unique()
    {
    }

    bool check()
    {
        bool more_than_one = _one.test_and_set(std::memory_order_seq_cst);
        if (more_than_one)
        {
            throw papi_error("Multiple papi::library instances", PAPI_ENOIMPL);
        }
        
        return !more_than_one;
    }

private:

    std::atomic_flag _one;
};
typedef lpt::singleton< papi::unique > unique_papi_lib_type;


/*
 *
 */

template < class Counting 
         , class Multiplex 
         > 
class library
{
public:

    library()
        : _event_set(PAPI_NULL)
    {
        init();
    }

    ~library()
    {
        //FIXME:
        //  PAPI_cleanup_eventset
        //  PAPI_shutdown
    }

   
    int event_set()
    {
        return _event_set;
    }
    
private:

    bool init()
    {
        if (!_lib_init.load()) {
            std::unique_lock<std::mutex> lock(_lib_mutex);

            if (_lib_init.load())
                return true;
        
            // Only one library across all templates. 
            unique_papi_lib_type::instance().check();

            int num_hwcntrs = 0;
            int retval = 0;

            if ((retval = PAPI_library_init(PAPI_VER_CURRENT)) != PAPI_VER_CURRENT ) {
                throw papi_error("PAPI_library_init", retval);
            }
        
            Multiplex::process::apply(_event_set);

            if ( (retval = PAPI_thread_init( (unsigned long (*)(void))(pthread_self) )) != PAPI_OK) {
                throw papi_error("PAPI_thread_init", retval);
            }
           
            /*
             * PAPI_num_counters returns the number of hardware counters the platform
             * has or a negative number if there is an error 
             */
            if ((num_hwcntrs = PAPI_num_counters()) < PAPI_OK) {
                throw papi_error("PAPI_num_counters", retval);
            }

            const PAPI_hw_info_t *hwinfo = PAPI_get_hardware_info();
            if (hwinfo == nullptr) {
                throw papi_error("PAPI_get_hardware_info", 0);
            }
            
            std::cout <<  "There are " << num_hwcntrs 
                      << " counters for " << hwinfo->vendor_string 
                      << ":" << hwinfo->model_string 
                      << std::endl;
            std::cout <<  "Cache: " << hwinfo->mem_hierarchy.levels << " levels\n";
            for (int i = 0; i < hwinfo->mem_hierarchy.levels; ++i) {
                const PAPI_mh_level_t& level = hwinfo->mem_hierarchy.level[i];
                std::cout << " " << i+1 << ".1:  size=" << level.cache[0].size 
                          << " " << level.cache[0].num_lines << " lines * " 
                          << level.cache[0].line_size << " assoc " 
                          << level.cache[0].associativity << "\n"
                          ;
                std::cout << " " << i+1 << ".1:  size=" << level.cache[1].size 
                          << " " << level.cache[1].num_lines << " lines * " 
                          << level.cache[1].line_size << " assoc " 
                          << level.cache[1].associativity << "\n"
                          ;
            }
            std::cout << std::endl;

            if ( (retval = PAPI_create_eventset(&_event_set)) != PAPI_OK) {
                throw papi_error("PAPI_create_eventset", retval);
            }

            for (std::vector<event>::const_iterator it = events.begin();
                 it != events.end();
                 ++it) {
                if ((retval=PAPI_add_event(_event_set, it->num())) != PAPI_OK) {
                    throw papi_error("PAPI_add_event", retval);
                }
            }
            
            Counting::process::apply(_event_set);

            _lib_init.store(true);
        }
    
        return _lib_init.load();
    }

private:

    std::atomic<bool> _lib_init;// = false; //FIXME: not neeeded anymore
    std::mutex _lib_mutex;
    int  _event_set; 
};


/*
 *
 */

template < class Counting 
         , class Multiplex 
         > 
class thread
{
public:

    static const unsigned long _noTid = static_cast<unsigned long>(-1);

    thread()
        : _tid(_noTid)
        , _event_set(PAPI_NULL)
    {
        init();
    }

    ~thread()
    {
        //FIXME:
        //  PAPI_stop
        //  PAPI_cleanup_eventset
        //  PAPI_unregister_thread
    }

    int event_set()
    {
        return _event_set;
    }

private:

    void init()
    {
        typedef lpt::singleton< papi::library<Counting, Multiplex> > papi_lib_type;

        if (_noTid == _tid) {
            papi_lib_type::instance();
            
            int retval = 0;

            /*
             * Cf. PAPI Users Manual, unbounded threads use per process counters.
             * Still...
             */
            if ( (retval = PAPI_register_thread()) != PAPI_OK) {
                throw papi_error("PAPI_register_thread", retval);
            }

            if ( (retval = PAPI_create_eventset(&_event_set)) != PAPI_OK) {
                throw papi_error("PAPI_create_eventset", retval);
            }

            for (std::vector<event>::const_iterator it = events.begin();
                 it != events.end();
                 ++it) {
                if ((retval=PAPI_add_event(_event_set, it->num())) != PAPI_OK) {
                    throw papi_error("PAPI_add_event", retval);
                }
            }
        
            Multiplex::thread::apply(_event_set);

            Counting::thread::apply(_event_set);

            _tid = PAPI_thread_id();
        }
    }

private:

    thread(thread const&);
    thread& operator=(thread const&);

private:

    /*__thread*/ unsigned long _tid; 
    /*__thread*/ int  _event_set;
};


/*
 * Operations on counters
 */

struct nop
{
    template < typename Operand > 
    static void apply(Operand const& c)
    {
    }

protected:
    ~nop() {}
};

struct stdout_print
{
    template < typename Operand > 
    static void apply(Operand const& c)
    {
        std::cout << c;
    }

protected:
    ~stdout_print() {}
};



/*
 *
 */

class counters_const_iterator;
class counter
{
public:

    typedef long long                value_type;
    typedef std::vector<value_type>  values_type;

    explicit counter(const events_type::const_iterator& ev, 
                     const values_type::const_iterator& b, 
                     const values_type::const_iterator& e)
        : _event(ev)
        , _begin(b)
        , _end(e)
    {}
    
    int num() const  { return _event->num(); }
    const char * name() const { return _event->name(); }
    const char * info() const { return _event->info(); }
    value_type begin() const { return *_begin; }
    value_type end() const { return *_end; }
    
    void print(std::ostream& os) const
    {
        os << name() << " (" <<  info() << "): " << end() - begin() 
           << " (" << end() <<"-"<< begin() << ")";
    }
    
    friend std::ostream& operator<< (std::ostream&, counter const&);
    friend class counters_const_iterator;
    
private:

    events_type::const_iterator  _event;
    values_type::const_iterator  _begin;
    values_type::const_iterator  _end;
};


inline std::ostream& operator<< (std::ostream& os, counter const& c)
{
    c.print(os);
    return os;
}


class counters_const_iterator 
    : public std::iterator< std::bidirectional_iterator_tag
                          , ptrdiff_t 
                          >
{
    counter _counter;

public:
    counters_const_iterator( events_type::const_iterator const& ev
                           , counter::values_type::const_iterator const& b
                           , counter::values_type::const_iterator const& e
                           )
        : _counter(ev, b, e) 
        {}

    bool operator==(const counters_const_iterator& x) const 
    {
        return    _counter._event == x._counter._event 
               && _counter._begin == x._counter._begin 
               && _counter._end == x._counter._end;
    }

    bool operator!=(const counters_const_iterator& x) const 
    {
        return !(*this == x);
    }

    const counter& operator*() const 
    {
        return _counter;
    }
    const counter* operator->() const 
    {
        return &_counter;
    }

    counters_const_iterator& operator++() 
    {
        ++_counter._event;  
        ++_counter._begin;  
        ++_counter._end;
        return *this;
    }
    counters_const_iterator operator++(int) 
    {
        counters_const_iterator tmp = *this;
        ++_counter._event;  
        ++_counter._begin;  
        ++_counter._end;
        return tmp;
    }

    counters_const_iterator& operator--() 
    {
        --_counter._event;  
        --_counter._begin;  
        --_counter._end;
        return *this;
    }
    counters_const_iterator operator--(int) 
    {
        counters_const_iterator tmp = *this;
        --_counter._event;  
        --_counter._begin;  
        --_counter._end;
        return tmp;
    }
}; //iterator


template < typename Op
         , typename Counting = counting_per_thread
         , typename Multiplex = multiplex_none
         >
class counters 
    : public Op
    , public Counting
    , public Multiplex
    , public stackonly // Only on the stack for now. 
{

public: 

    typedef lpt::singleton< papi::thread<Counting, Multiplex> >  papi_thread_type;
    
    counters(std::string const& tag) : _tag(tag)
    {
        _start_vals.resize(num_events);
        _end_vals.resize(num_events);
        
        // Note: PAPI_read_counters() resets the counters.
        int retval = 0;    
        if ( (retval=PAPI_read(papi_thread_type::instance().event_set(), &_start_vals[0])) != PAPI_OK) {
            throw papi_error("PAPI_read", retval);
        }
    }
    
    ~counters()
    {
        int retval = 0;    
        if ( (retval=PAPI_read(papi_thread_type::instance().event_set(), &_end_vals[0])) != PAPI_OK) {
            throw papi_error("PAPI_read", retval);
        }
    
        Op::apply(*this);
    }
    
    
    typedef counters_const_iterator const_iterator;
    const_iterator begin() const { return const_iterator(events.begin(), 
                                                         _start_vals.begin(), 
                                                         _end_vals.begin()); 
                                 }
    const_iterator end() const   { return const_iterator(events.end(), 
                                                         _start_vals.end(), 
                                                         _end_vals.end()); 
                                 }

private:

    counters();
    counters(const counters<Op, Counting, Multiplex>&);
    counters& operator=(const counters<Op, Counting, Multiplex>&);
    
    template < class O 
             , class C 
             , class M 
             > 
    friend std::ostream& operator<< (std::ostream&, counters<O, C, M> const&);

private:

   counter::values_type  _start_vals;
   counter::values_type  _end_vals;
   std::string        _tag;
};


struct counter_printer {
    counter_printer(std::ostream& os) : _os(os) {}
        
    void operator()(counter const& c) 
    {
        _os << "  ";
        c.print(_os);
        _os << "\n";
    }
        
private:
    std::ostream& _os;
};
    
template < class Op
         , class Counting
         , class Multiplex 
         >
std::ostream& operator<< (std::ostream& os, counters<Op, Counting, Multiplex > const& cs)
{
    os << "\nDelta " << cs._tag << ":\n";
    std::for_each(cs.begin(),
                  cs.end(),
                  counter_printer(os) //[&] (counter const& c) { c.print(os);} 
                  );
    os << std::endl;
    return os;
}


} } //namespace lpt::papi


/*
 *  Copyright 2012 Aurelian Melinte. 
 *  Released under GPL 3.0 or later. 
 *
 *  You need a C++0x compiler. 
 * 
 */
 
 
#if defined(_WIN32)
#  include <lpt/detail/win_call_stack.hpp>
#elif defined(__GNUG__)
#  include <lpt/detail/glibc_call_stack.hpp>
#else
#  error "Unsupported platform"
#endif


#include <cassert>
#include <cstring>
#include <cstdlib>

#include <memory>
#include <iostream>
#include <string> 
#include <sstream>
#include <algorithm>

#include <lpt/nocopy.hpp>
#include <lpt/stackonly.hpp>

#pragma once 

/*
 *
 */ 
namespace lpt { namespace stack {

/*
 *
 */

static const std::size_t default_max_depth = 40;

typedef detail::address_type        address_type;
static const address_type           null_address_type = detail::null_address_type;

typedef detail::delta_type          delta_type;


class call_frame
{
public:

    call_frame(const address_type& addr = null_address_type)
        : _addr(addr)
    {}

    call_frame(call_frame&& other) noexcept 
        : call_frame()
    {
        swap(other);
    }

    call_frame(const call_frame& other) noexcept
        : _addr(other._addr)
    {
    }

    call_frame& operator=(call_frame other) noexcept
    {
        swap(other);
        return *this;
    }

    void swap(call_frame& other) noexcept
    {
        std::swap(_addr, other._addr);
    }

    const address_type addr() const noexcept { return _addr; }

private:

    // TODO: add other data like: list of arguments, registers.
    address_type _addr;
};

inline
void swap(call_frame& left, call_frame& right) noexcept
{
    left.swap(right);
}


static const call_frame null_frame(null_address_type);



/*
 *
 */

template < std::size_t MaxDepth = default_max_depth >
class call_stack
{
public:

    typedef std::size_t                                  depth_type;
    typedef std::array<call_frame, MaxDepth>             stack_type;

    typedef typename stack_type::size_type               size_type;
    typedef typename stack_type::difference_type         difference_type;
    typedef typename stack_type::value_type              value_type;
    typedef typename stack_type::const_reference         const_reference;
    typedef typename stack_type::const_pointer           const_pointer;
    typedef typename stack_type::const_iterator          const_iterator;
    typedef typename stack_type::const_reverse_iterator  const_reverse_iterator;

    call_stack(bool capture = false) noexcept 
        : _depth(0)
    {
        static_assert(MaxDepth <= default_max_depth, "Call Stack limit");

        if (capture)
        {
            get_backtrace();
            assert(_depth > 0 && _depth <= MaxDepth);
        }
    }

    // Only MSVC 2012 CTP supports delegating constructors 
    call_stack(call_stack&& other) noexcept 
        : call_stack()
    {
        swap(other);
    }

    call_stack(const call_stack& other) noexcept
        : _stack(other._stack)
        , _depth(other._depth)
    {
    }

    call_stack& operator=(call_stack other) noexcept
    {
        swap(other);
        return *this;
    }

    size_type depth()      const noexcept { return _depth; }
    size_type max_depth()  const noexcept { return MaxDepth; }

    size_type size()       const noexcept { return depth(); }
    size_type max_size()   const noexcept { return max_depth(); }

    bool empty()           const noexcept { return _depth == 0; }

    const_iterator begin()  const { return _stack.cbegin(); }
    const_iterator end()    const { return (begin() + _depth); }
    const_iterator cbegin() const { return begin(); }
    const_iterator cend()   const { return end(); }

    const_reverse_iterator crbegin() const { return _depth ? const_reverse_iterator(&_stack[_depth-1])
                                                           : _stack.crbegin(); }
    const_reverse_iterator crend()   const { _stack.crend(); }
    const_reverse_iterator rbegin()  const { return crbegin(); }
    const_reverse_iterator rend()    const { return crend(); }

    const_reference operator [] (size_type idx) const { return _stack[idx]; }
    const_reference at(size_type idx) const
    {
        if (!(idx < depth())) {
            throw std::out_of_range("Requested index out of range");
        }
        return _stack.at(idx);
    }

    bool operator ==(const call_stack& other) const { return _depth == other._depth && _stack == other._stack; }
    bool operator !=(const call_stack& other) const { return _depth != other._depth || _stack != other._stack; }

    void swap(call_stack& other) noexcept
    {
        std::swap(_stack, other._stack);
        std::swap(_depth, other._depth);
    }

private:

    depth_type get_backtrace()
    {
        address_type buffer[MaxDepth] = {0};

        int numFrames = detail::backtrace(buffer, MaxDepth);
        std::transform(&buffer[0], 
                       &buffer[numFrames], 
                       _stack.begin(),
                       [] (address_type addr) { return call_frame(addr); } );

        assert(numFrames > 0 && static_cast<depth_type>(numFrames) <= MaxDepth);
        _depth = static_cast<depth_type>(numFrames);

        return _depth;
    }

private:

    stack_type         _stack;
    depth_type         _depth;
}; //call_stack

typedef call_stack<default_max_depth> default_stack;


template < size_t Size > inline
void swap(call_stack<Size>& left, call_stack<Size>& right) noexcept
{
    left.swap(right);
}


/*
 *  
 */

template < typename Resolver >
class symbol_info : protected Resolver
{
public:

    symbol_info(const call_frame& frame = null_frame) noexcept
        : Resolver(frame.addr())
    {
    }

    symbol_info(symbol_info& other) noexcept 
        : symbol_info()
    {
        swap(other);
    }

    symbol_info(symbol_info&& other) noexcept 
        : symbol_info()
    {
        swap(other);
    }

    symbol_info& operator=(symbol_info other) noexcept
    {
        swap(other);
        return *this;
    }

    void resolve(const address_type& addr) noexcept
    {
        Resolver::resolve(addr);
    }

    const address_type     addr() const noexcept                     { return Resolver::addr(); }
    const char*            binary_file() const noexcept              { return Resolver::binary_file(); }
    const char*            raw_function_name() const noexcept        { return Resolver::raw_function_name(); }
    const char*            demangled_function_name() const noexcept  { return Resolver::demangled_function_name(); }
    // Displacement of _addr from symbol address. Some platforms have only a positive displacement.
    delta_type             delta() const noexcept                    { return Resolver::delta(); }
    const char*            source_file() const noexcept              { return Resolver::source_file(); }
    unsigned int           line_number() const noexcept              { return Resolver::line_number(); }

    void swap(symbol_info& other) noexcept
    {
        Resolver::swap(other);
    }


    template < typename OutputFormatter >
    friend inline std::ostream& operator<<(std::ostream& os,
                                           const symbol_info& frm)
    {
        OutputFormatter::print(frm, os);
        return os;
    }

    template < typename OutputFormatter >
    std::string as_string() const
    {
        std::ostringstream s;
        s << *this;
        return std::move(s.str());
    }
}; //symbol_info


template < typename Resolver > inline
void swap(symbol_info<Resolver>& left, symbol_info<Resolver>& right) noexcept
{
    assert(&left != &right);
    left.swap(right);
}


/*
 * How to print symbol_info
 */

struct terse_formatter
{
    template < typename SymbolInfo > 
    static void print(SymbolInfo const& frm, std::ostream& os)
    {
        os << "[" << std::hex << frm.addr() << "] "
           << frm.demangled_function_name()
           //FIXME: rework for negative deltas
           << (frm.delta() >= 0 ? '+' : '-')
           << "0x" << std::hex << frm.delta()
           << " (" << frm.binary_file() << ")"
           << " in " << frm.source_file() << ":" << std::dec << frm.line_number()
           ;
    }

protected:
    ~terse_formatter() {}
};

struct fancy_formatter
{
    template < typename SymbolInfo > 
    static void print(SymbolInfo const& frm, std::ostream& os)
    {
        os << "[" << std::hex << frm.addr() << "] "
           << frm.demangled_function_name()
           << (frm.delta() >= 0 ? '+' : '-')
           << "0x" << std::hex << frm.delta()
           << "\n\tAt " << frm.source_file() << ":" << std::dec << frm.line_number()
           << "\n\tIn " << frm.binary_file()
           ;
    }

protected:
    ~fancy_formatter() {}
};
// No VS2012 support for template alias:
//    template < typename OutputFormatter > 
//    using null_symbol_info = symbol_info<detail::null_symbol_info, OutputFormatter>;



/*
 *  Plain symbol info. No memory allocation permitted.
 */

typedef symbol_info<detail::null_symbol_info> null_symbol_info;


/*
 *  Best resolution offered by libraries always installed with the OS.
 */

typedef symbol_info<detail::basic_symbol_info> basic_symbol_info;


/*
 *  Best resolution for the platform.
 */

typedef symbol_info<detail::extended_symbol_info> extended_symbol_info;


/*
 * 
 */

template < typename AddrResolver       = basic_symbol_info
         , typename OutputFormatter    = fancy_formatter
         >
class call_frame_info
{
public:

    call_frame_info(const call_frame& frame = null_frame) noexcept
        : _frame(frame)
    {}

    call_frame_info(call_frame_info&& other) noexcept 
        : call_frame_info()
    {
        swap(other);
    }

    call_frame_info(const call_frame_info& other) noexcept 
        : _frame(other._frame) 
    {}

    call_frame_info& operator=(call_frame_info other) noexcept
    {
        swap(other);
        return *this;
    }

    void swap(call_frame_info& other) noexcept
    {
        std::swap(_frame, other._frame);
    }

    call_frame frame() const 
    {
        return _frame;
    }

    friend inline std::ostream& operator<<(std::ostream& os,
                                           const call_frame_info& frm)
    {
        AddrResolver addrInfo(frm._frame.addr());
        OutputFormatter::print(addrInfo, os);
        return os;
    }

    std::string as_string() const
    {
        std::ostringstream s;
        s << *this;
        return std::move(s.str());
    }

private:

    call_frame _frame;
};

template < typename AddrResolver
         , typename OutputFormatter
         > inline
void swap(call_frame_info< AddrResolver, OutputFormatter >& left, 
          call_frame_info< AddrResolver, OutputFormatter >& right) noexcept
{
    assert(&left != &right);
    left.swap(right);
}


/*
 *
 */

template < typename CallStack          = call_stack<default_max_depth>
         , typename AddrResolver       = basic_symbol_info
         , typename OutputFormatter    = fancy_formatter
         >
class call_stack_info
{
public:

    typedef CallStack              stack_type;
    typedef AddrResolver           symbol_info_type;

    // For containers
    call_stack_info() noexcept : _stack(false) {}

    call_stack_info(const stack_type& stack) noexcept 
        : _stack(stack) 
    {}

    call_stack_info(call_stack_info&& other) noexcept 
        : call_stack_info()
    {
        swap(other);
    }

    call_stack_info(const call_stack_info& other) noexcept 
        : _stack(other._stack) 
    {}

    call_stack_info& operator=(call_stack_info other) noexcept
    {
        swap(other);
        return *this;
    }

    void swap(call_stack_info& other) noexcept
    {
        std::swap(_stack, other._stack);
    }


    friend inline std::ostream& operator<<(std::ostream& os,
                                           const call_stack_info& stk)
    {
        for (const auto& frm : stk._stack)
        {
            //os << call_frame_info< AddrResolver, OutputFormatter >(frm) << "\n";
            AddrResolver addrInfo(frm.addr());
            OutputFormatter::print(addrInfo, os);
            os << "\n";
        }
        os << std::flush;
        return os;
    }

    std::string as_string() const
    {
        std::ostringstream s;
        s << *this;
        return std::move(s.str());
    }

    class const_iterator
            : public std::iterator< std::bidirectional_iterator_tag
                                  , ptrdiff_t
                                  >
    {
    public:

        const_iterator(const typename stack_type::const_iterator& it)
                : _it(it)
                , _frame(*it)
        {}

        bool operator==(const const_iterator& other) const
        {
            return _frame.addr() == other._frame.addr();
        }

        bool operator!=(const const_iterator& x) const
        {
            return !(*this == x);
        }

        const symbol_info_type& operator*() const
        {
            return _frame;
        }
        const symbol_info_type* operator->() const
        {
            return &_frame;
        }

        const_iterator& operator++()
        {
            ++_it;
            _frame.resolve(_it->addr());
            return *this;
        }
        const_iterator operator++(int)
        {
            const_iterator tmp = *this;
            ++_it;
            _frame.resolve(_it->addr());
            return tmp;
        }

        const_iterator& operator--()
        {
            --_it;
            _frame.resolve(_it->addr());
            return *this;
        }
        const_iterator operator--(int)
        {
            const_iterator tmp = *this;
            --_it;
            _frame.resolve(_it->addr());
            return tmp;
        }

    private:

        const_iterator();

    private:

        typename stack_type::const_iterator  _it;
        symbol_info_type                     _frame;
    }; //const_iterator

    const_iterator begin()  const  { return const_iterator(_stack.begin()); }
    const_iterator cbegin() const  { return begin(); }
    const_iterator end()    const  { return const_iterator(_stack.end()); }
    const_iterator cend()   const  { return end(); }

private:

    stack_type         _stack;
};

template < typename CallStack
         , typename AddrResolver
         , typename OutputFormatter
         > inline
void swap(call_stack_info<CallStack, AddrResolver, OutputFormatter >& left, 
          call_stack_info<CallStack, AddrResolver, OutputFormatter >& right) noexcept
{
    assert(&left != &right);
    left.swap(right);
}


/*
 * 
 */

typedef call_stack_info< default_stack, extended_symbol_info > default_call_stack_info;


}} //namespace lpt::stack


namespace std {

/*
inline
void swap(lpt::stack::call_frame& left, lpt::stack::call_frame& right) noexcept
{
    lpt::stack::swap(left, right);
}
*/

template < size_t Size > inline
void swap(lpt::stack::call_stack<Size>& left,
          lpt::stack::call_stack<Size>& right) noexcept
{
    lpt::stack::swap(left, right);
}

template < typename Resolver > inline
void swap(lpt::stack::symbol_info<Resolver>& left,
          lpt::stack::symbol_info<Resolver>& right) noexcept
{
    lpt::stack::swap(left, right);
}


template < typename AddrResolver
         , typename OutputFormatter
         > inline
void swap(lpt::stack::call_frame_info< AddrResolver, OutputFormatter >& left, 
          lpt::stack::call_frame_info< AddrResolver, OutputFormatter >& right) noexcept
{
    assert(&left != &right);
    lpt::stack::swap(left, right);
}

template < typename CallStack
         , typename AddrResolver
         , typename OutputFormatter
         > inline
void swap(lpt::stack::call_stack_info<CallStack, AddrResolver, OutputFormatter>& left, 
          lpt::stack::call_stack_info<CallStack, AddrResolver, OutputFormatter>& right) noexcept
{
    lpt::stack::swap(left, right);
}

} //namespace std

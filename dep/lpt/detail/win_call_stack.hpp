/*
 *  Copyright 2012 Aurelian Melinte. 
 *  Released under GPL 3.0 or later. 
 *
 *  You need a C++0x compiler. 
 * 
 */
 
 

#include <windows.h>
#include <WinBase.h>
#include <DbgHelp.h>

#include <cassert>
#include <cstring>
#include <cstdlib>

#include <memory>
#include <iostream>
#include <mutex> 
#include <array> 
#include <string> 
#include <map> 
#include <sstream>
#include <algorithm>

#include <lpt/nocopy.hpp>
#include <lpt/stackonly.hpp>
#include <lpt/singleton.hpp>

#pragma once 

#pragma comment(lib, "DbgHelp")


/*
 *
 */ 
namespace lpt { namespace stack { namespace detail {

/*
 * Supported compilers and tools.  Only VS2012 CTP.
 */

#if (_MSC_VER < 1700)
#  error "Unsupported compiler: only Visual Studio 2012 or later."
#elif (_MSC_VER == 1700)
#  define noexcept  throw() //VS 2012: not yet supported
#endif


/*
 * Supported OSs. Only tested on Win7.
 */

#if (WINVER < _WIN32_WINNT_WIN7) //0x0601
#  error "Unsupported Windows version."
#endif


/*
 * Supported CPUs and tools. 
 */

enum supported_platforms
{
    x86    = IMAGE_FILE_MACHINE_I386,
    amd64  = IMAGE_FILE_MACHINE_AMD64, 
    ia64   = IMAGE_FILE_MACHINE_IA64,
};
#if   defined(_M_IX86 ) //600
static const supported_platforms this_platform = x86;
#elif defined( _M_X64 ) //100
static const supported_platforms this_platform = amd64;
#elif defined( _M_IA64)
static const supported_platforms this_platform = ia64;
#else
#  error "Unsupported platform."
#endif


/*
 *
 */

template <typename T> 
struct platform { 
    static bool const is_x86        = (this_platform == x86);
    static bool const is_amd64      = (this_platform == amd64);
    static bool const is_ia64       = (this_platform == ia64);
    static bool const is_supported  = (is_amd64 || is_x86 || is_ia64);
};

template <typename Context>
void init_stackframe(STACKFRAME64& sf, 
                     typename std::enable_if<platform<Context>::is_x86, Context>::type &ctx) 
{
    static_assert(this_platform == x86, "Not x86");
    ::memset(&sf, 0, sizeof(STACKFRAME64));
    sf.AddrPC.Offset    = ctx.Eip;
    sf.AddrPC.Mode      = AddrModeFlat;
    sf.AddrFrame.Offset = ctx.Ebp;
    sf.AddrFrame.Mode   = AddrModeFlat;
    sf.AddrStack.Offset = ctx.Esp;
    sf.AddrStack.Mode   = AddrModeFlat;
}

template <typename Context>
void init_stackframe(STACKFRAME64& sf, 
                     typename std::enable_if<platform<Context>::is_amd64, Context>::type &ctx) 
{
    static_assert(this_platform == amd64, "Not AMD64");
    ::memset(&sf, 0, sizeof(STACKFRAME64));
    sf.AddrPC.Offset     = ctx.Rip;
    sf.AddrPC.Mode       = AddrModeFlat;
    sf.AddrFrame.Offset  = ctx.Rsp;
    sf.AddrFrame.Mode    = AddrModeFlat;
    sf.AddrStack.Offset  = ctx.Rsp;
    sf.AddrStack.Mode    = AddrModeFlat;
}

template <typename Context>
void init_stackframe(STACKFRAME64& sf, 
                     typename std::enable_if<platform<Context>::is_ia64, Context>::type &ctx) 
{
    static_assert(this_platform == ia64, "Not ia64");
    ::memset(&sf, 0, sizeof(STACKFRAME64));
    sf.AddrPC.Offset     = ctx.StIIP;
    sf.AddrPC.Mode       = AddrModeFlat;
    sf.AddrFrame.Offset  = ctx.IntSp;
    sf.AddrFrame.Mode    = AddrModeFlat;
    sf.AddrBStore.Offset = ctx.RsBSP;
    sf.AddrBStore.Mode   = AddrModeFlat;
    sf.AddrStack.Offset  = ctx.IntSp;
    sf.AddrStack.Mode    = AddrModeFlat;
}

template <typename Context>
void init_stackframe(STACKFRAME64& sf, 
                     typename std::enable_if<!platform<Context>::is_supported, Context>::type &ctx) 
{
    throw std::runtime_error("Unsupported platform");
}


/*
 *
 */

typedef DWORD64            address_type;
static address_type        null_address_type = address_type(0);

typedef DWORD64            delta_type;


/*
 *
 */

namespace dbghelp {

class symbol_info
{
public:
    symbol_info()
    {
        reset();
    }

    void reset()
    {
        ::memset(&_sym, 0, sizeof(_sym));
        SYMBOL_INFO* pSym = reinterpret_cast<SYMBOL_INFO*>(&_sym);
        pSym->SizeOfStruct = sizeof(SYMBOL_INFO);
        pSym->MaxNameLen = MAX_SYM_NAME;
    }

    operator PSYMBOL_INFO()
    {
        return reinterpret_cast<SYMBOL_INFO*>(&_sym);
    }

    const CHAR* name() const
    {
        const SYMBOL_INFO* pSym = reinterpret_cast<const SYMBOL_INFO*>(&_sym);
        return pSym->Name;
    }

    PDWORD64 sym_displacement()
    {
        return &_sym_displacement;
    }

private:
    CHAR     _sym[sizeof(SYMBOL_INFO) + MAX_SYM_NAME + 1];
    DWORD64  _sym_displacement;
};


class imagehlp_line
{
public:

    imagehlp_line()
        :_lineDisplacement(0)
    {
        reset();
    }

    void reset()
    {
        ::memset(&_line, 0, sizeof(IMAGEHLP_LINE64));
        _line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
    }

    operator PIMAGEHLP_LINE64()
    {
        return &_line;
    }

    DWORD line_number() const
    {
        return _line.LineNumber;
    }

    PDWORD line_displacement()
    {
        return &_lineDisplacement;
    }

    const CHAR* file_name() const
    {
        return _line.FileName;
    }

private:
    IMAGEHLP_LINE64    _line;
    DWORD             _lineDisplacement;
};


class imagehlp_module
{
public:

    imagehlp_module()
    {
        reset();
    }

    void reset()
    {
        ::memset(&_module, 0, sizeof(IMAGEHLP_MODULE64));
        _module.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
    }

    operator PIMAGEHLP_MODULE64()
    {
        return &_module;
    }

    const CHAR* loaded_module_name() const
    {
        return _module.LoadedImageName;
    }

private:
    IMAGEHLP_MODULE64  _module;
};


class library : public lpt::nocopy
              , public lpt::stackonly
{
public:

    library() 
        : _init_err(0)
    {
        std::unique_lock<std::mutex> lock(_lib_mutex);

        ::SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES | SYMOPT_EXACT_SYMBOLS);

        //MUST: before any other DbgHelp func call
        ::SetLastError(0);
        BOOL ignore = ::SymInitialize(::GetCurrentProcess(), NULL, TRUE);
        _init_err = ::GetLastError();

        assert(_init_err == 0);
    }

    ~library()
    {
        std::unique_lock<std::mutex> lock(_lib_mutex);
        BOOL ignore = ::SymCleanup(::GetCurrentProcess());
    }

    int backtrace(address_type* buffer, int size)
    {
        std::unique_lock<std::mutex> lock(_lib_mutex);
        
        int depth = 0;
        
        // Alternative: call ImageNtHeader() and use machine info from PE header
        CONTEXT ctx = {0};
        ctx.ContextFlags = CONTEXT_FULL;
        ::RtlCaptureContext(&ctx);
        
        STACKFRAME64 sf;
        init_stackframe<CONTEXT>(sf, ctx);
        
        HANDLE proc = ::GetCurrentProcess();
        HANDLE thread = ::GetCurrentThread();
        DWORD dwProcessId = ::GetCurrentProcessId();
        while (depth < size) {
            BOOL stackRet = ::StackWalk64(
                detail::this_platform, 
                proc, 
                thread, 
                &sf, 
                &ctx, 
                NULL, //::ReadProcessMemory,         //::ReadProcessMemoryProc64, 
                NULL, //::SymFunctionTableAccess64,  //::FunctionTableAccessProc64, 
                NULL, //::SymFunctionTableAccess64,  //::GetModuleBaseProc64, 
                NULL  //::TranslateAddressProc64
                );
            if (!stackRet) {
                break;
            }
            if (sf.AddrPC.Offset == sf.AddrReturn.Offset) {
                break; // endless stack?
            }
            if (sf.AddrPC.Offset == 0) {
                break;
            }
            
            //std::cerr << std::dec << _depth << ": " << std::hex << sf.AddrPC.Offset << std::dec << std::endl;
            buffer[depth++] = sf.AddrPC.Offset;
        }
        
        return depth;
    }

    void resolve(const address_type&  addr,
                 symbol_info&         sym_info,
                 imagehlp_line&       line_info,
                 imagehlp_module&     module_info)
    {
        sym_info.reset();
        line_info.reset();
        module_info.reset();

        {
            std::unique_lock<std::mutex> lock(_lib_mutex);

            BOOL ret  = ::SymFromAddr(::GetCurrentProcess(), addr, sym_info.sym_displacement(), sym_info); 
            BOOL ret2 = ::SymGetLineFromAddr64(::GetCurrentProcess(), addr, line_info.line_displacement(), line_info);
            BOOL ret3 = ::SymGetModuleInfo64(::GetCurrentProcess(), addr, module_info);
        }
    }

private:

    DWORD               _init_err;

    // MSDN: All DbgHelp functions, such as StackWalk64, are single threaded. 
    // Therefore, calls from more than one thread to this function will likely
    // result in unexpected behavior or memory corruption. 
    std::mutex   _lib_mutex;
};

typedef lpt::singleton<dbghelp::library> dbghelp_lib_type;

} //dbghelp

/*
 *
 */

static inline
int backtrace(address_type* buffer, int size)
{
    return dbghelp::dbghelp_lib_type::instance().backtrace(buffer, size);
}

/*
 *
 */

class null_symbol_info
{
public:

    null_symbol_info(const address_type& addr = null_address_type) noexcept
    {
        resolve(addr);
    }

    null_symbol_info(null_symbol_info& other) noexcept 
        /*: symbol_info_base(other._addr)*/
    {
        resolve(null_address_type);
        swap(other);
    }
    null_symbol_info& operator=(null_symbol_info other) noexcept
    {
        swap(other);
        return *this;
    }

    null_symbol_info(null_symbol_info&& other) noexcept 
        /*: symbol_info_base()*/
    {
        resolve(null_address_type);
        swap(other);
    }
    null_symbol_info& operator=(null_symbol_info&& other) noexcept
    {
        assert(this != &other);
        swap(other);
        return *this;
    }

    void resolve(const address_type& addr) noexcept
    {
        _addr = addr;
        _binary_name = nullptr;
        _func_name = nullptr;
        _demangled_func_name = nullptr;
        _delta = 0;
        _source_file_name = nullptr;
        _line_number = 0;
    }

    const address_type   addr() const noexcept              { return _addr; }
    const char*          binary_file() const noexcept       { return safe(_binary_name); }
    const char*          raw_function_name() const noexcept { return safe(_func_name); }
    const char*          demangled_function_name() const noexcept { return safe(_demangled_func_name); }
    delta_type           delta() const noexcept             { return _delta; }
    const char*          source_file() const noexcept       { return safe(_source_file_name); }
    unsigned int         line_number() const noexcept       { return  _line_number; }


    void swap(null_symbol_info& other) noexcept
    {
        std::swap(_addr,                 other._addr);
        std::swap(_binary_name,          other._binary_name);
        std::swap(_func_name,            other._func_name);
        std::swap(_demangled_func_name,  other._demangled_func_name);
        std::swap(_delta,                other._delta);
        std::swap(_source_file_name,     other._source_file_name);
        std::swap(_line_number,          other._line_number);
    }

protected:

    const char* safe(const char* p) const { return (p && *p) ? p : "??"; }

protected:

    address_type  _addr;
    const char*   _binary_name;
    const char*   _func_name;
    const char*   _demangled_func_name;
    delta_type    _delta;

    const char*   _source_file_name;
    unsigned int  _line_number;
}; //null_symbol_info

inline void swap(null_symbol_info& left, null_symbol_info& right) noexcept
{
    assert(&left != &right);
    left.swap(right);
}


/*
 *
 */

class basic_symbol_info : public null_symbol_info
{
public:

    typedef null_symbol_info  base_type;

    basic_symbol_info(const address_type& addr = null_address_type) noexcept : base_type(addr)
    {
        resolve(addr);
    }

    basic_symbol_info(basic_symbol_info& other) noexcept 
        /*: symbol_info_base(other._addr)*/
    {
        resolve(null_address_type);
        swap(other);
    }
    basic_symbol_info& operator=(basic_symbol_info other) noexcept
    {
        swap(other);
        return *this;
    }

    basic_symbol_info(basic_symbol_info&& other) noexcept 
        /*: symbol_info_base()*/
    {
        resolve(null_address_type);
        swap(other);
    }
    basic_symbol_info& operator=(basic_symbol_info&& other) noexcept
    {
        assert(this != &other);
        swap(other);
        return *this;
    }

    void resolve(const address_type& addr) noexcept
    {
        _resolve(addr);
    }

    void swap(basic_symbol_info& other) noexcept
    {
        base_type::swap(other);
        std::swap(_sym_info,    other._sym_info);
        std::swap(_line_info,   other._line_info);
        std::swap(_module_info, other._module_info);
    }

protected:

    void _resolve(const address_type& addr) noexcept
    {
        base_type::resolve(addr);

        if (addr == null_address_type)
            return;

        dbghelp::dbghelp_lib_type::instance().resolve(_addr, _sym_info, _line_info, _module_info);

        _delta               = static_cast<decltype(_delta)>(*_sym_info.sym_displacement());
        _func_name           = _sym_info.name();
        _demangled_func_name = _sym_info.name();
        _line_number         = static_cast<decltype(_line_number)>(_line_info.line_number());
        _source_file_name    = _line_info.file_name();
        _binary_name         = _module_info.loaded_module_name();
    }

protected:

    dbghelp::symbol_info      _sym_info;
    dbghelp::imagehlp_line    _line_info;
    dbghelp::imagehlp_module  _module_info;
}; //basic_symbol_info

inline void swap(basic_symbol_info& left, basic_symbol_info& right) noexcept
{
    assert(&left != &right);
    left.swap(right);
}


/*
 *
 */

class extended_symbol_info : public basic_symbol_info
{
public:

    typedef basic_symbol_info  base_type;

    extended_symbol_info(const address_type& addr = null_address_type) noexcept : base_type(addr)
    {
        resolve(addr);
    }

    extended_symbol_info(extended_symbol_info& other) noexcept 
        /*: symbol_info_base(other._addr)*/
    {
        resolve(null_address_type);
        swap(other);
    }
    extended_symbol_info& operator=(extended_symbol_info other) noexcept
    {
        swap(other);
        return *this;
    }

    extended_symbol_info(extended_symbol_info&& other) noexcept 
        /*: symbol_info_base()*/
    {
        resolve(null_address_type);
        swap(other);
    }
    extended_symbol_info& operator=(extended_symbol_info&& other) noexcept
    {
        assert(this != &other);
        swap(other);
        return *this;
    }

    void resolve(const address_type& addr) noexcept
    {
        base_type::resolve(addr);
    }

    void swap(extended_symbol_info& other) noexcept
    {
        base_type::swap(other);
    }
}; //extended_symbol_info


inline void swap(extended_symbol_info& left, extended_symbol_info& right) noexcept
{
    assert(&left != &right);
    left.swap(right);
}


}}}  //namespace lpt::stack::detail


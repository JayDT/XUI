#ifndef _CORE_MISC_H_
#define _CORE_MISC_H_

#include <cmath>
#include <standard\Preprocessor\pp_platform.h>
#include "Reflection/Runtime/Object.h"
#include <functional>

//#define foreach(v, a, t) for (t v=(a).begin(); v!=(a).end(); ++v)
//#define for_each(v, a) for (auto v=(a).begin(); v!=(a).end(); ++v)
//#define for_inc(v, a) for (auto v=0; v!=a; ++v)
//#define for_each_range(v, a) for (auto v=(a).first; v!=(a).second; ++v)

// we always use stdlibc++ std::max/std::min, undefine some not C++ standard defines (Win API and some pother platforms)
#ifdef max
#  undef max
#endif

#ifdef min
#  undef min
#endif

#ifndef M_PI
#  define M_PI          3.14159265358979323846
#endif

#ifndef M_PI_F
#  define M_PI_F        float(M_PI)
#endif

namespace MathUtils
{
    template<class _Ty> __forceinline
    _Ty sqr(_Ty _X)
    {
        return _X*_X;
    }

    template<typename _Ty> __forceinline
    _Ty Tabs(_Ty _X)
    {
        return (_X < _Ty(0)) ? _X * _Ty(-1) : _X;
    }

    inline int NumberOfSetBits(int i)
    {
        i = i - ((i >> 1) & 0x55555555);
        i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
        return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
    }
};

namespace System
{
    template <class T>
    inline void hash_combine(std::size_t& seed, const T& v)
    {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    template<typename TOwner>
    struct EnableSharedFromThisEx : public std::enable_shared_from_this<TOwner>
    {
        template <typename Derived>
        std::shared_ptr<Derived> shared_from_base_dynamic()
        {
            return std::dynamic_pointer_cast<Derived>(shared_from_this());
        }

        template <typename Derived>
        std::shared_ptr<Derived> shared_from_base_static()
        {
            return std::static_pointer_cast<Derived>(shared_from_this());
        }

        template <typename Derived>
        std::weak_ptr<Derived> weak_from_base_dynamic()
        {
            return std::dynamic_pointer_cast<Derived>(shared_from_this());
        }

        template <typename Derived>
        std::weak_ptr<Derived> weak_from_base_static()
        {
            return std::static_pointer_cast<Derived>(shared_from_this());
        }
    };

    struct ScopeGuard
    {
        std::function<void()> m_finally;

        ScopeGuard(std::function<void()> finally) : m_finally(finally) {}
        ~ScopeGuard()
        {
            if (m_finally)
                m_finally();
        }
    };

#ifdef WIN32
    __forceinline uint64_t GetCurrentModule()
    { // NB: XP+ solution!
        HMODULE hModule = NULL;
        GetModuleHandleEx(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
            (LPCTSTR)GetCurrentModule,
            &hModule);

        return uint64_t(hModule);
    }
#else
    __forceinline uint64_t GetCurrentModule()
    { // NB: XP+ solution!
        Dl_info dlinf;
        assert(dladdr(&__GetCurrent_RunTime__ReflectionModule, &dlinf) == 1); // not exported func use
        return uint64_t(dlinf.dli_fbase);
    }
#endif

    struct Invokable : System::Reflection::Object
    {
        // TODO: Generic Invoke example from reflection IMethod
    };

    // Fast implement for reflection
    template<typename TFunc>
    class Delegate : public Invokable
    {
    public:
        typedef std::function<TFunc> FuncHandler;

    private:
        mutable Meta(Disable) FuncHandler _func;

    public:

        Delegate(FuncHandler const& func)
            : _func(std::move(func)) {}
        Delegate(Delegate<TFunc> const& func)
            : _func(func._func) {}
        Delegate() {}
        virtual ~Delegate() {}

        Meta(Disable) void operator=(Delegate<TFunc> const& func)
        {
            _func = func._func;
        }

        Meta(Disable) bool operator==(Delegate<TFunc> const& func) const
        {
            return _func == func._func;
        }

        Meta(Disable) bool operator!=(Delegate<TFunc> const& func) const
        {
            return _func != func._func;
        }

        FuncHandler& GetDelegate() { return _func; }
        FuncHandler const& GetDelegate() const { return _func; }
    };

    template<typename T = unsigned int, typename E = unsigned int>
    struct FlagTemplate
    {
        FlagTemplate() : flag(0) { }
        FlagTemplate(E _flag) : flag(_flag) { }
        ~FlagTemplate() {}
    public:
        T& addFlag(E _flag) { return flag |= (T)_flag; }
        T& setFlag(E _flag) { return flag = (T)_flag; }
        T& removeFlag(E _flag) { return flag &= ~(T)_flag; }
        void clear() { flag = 0; }
        bool hasFlag(E _flag) const { return flag & (T)_flag; }

    public:
        T operator~() { return (T)~flag; }
        T& operator+(E _flag) { return addFlag(_flag); }
        T& operator-(E _flag) { return removeFlag(_flag); }
        void operator=(T _flag) { setFlag((E)_flag); }
        void operator+=(E _flag) { addFlag(_flag); }
        void operator-=(E _flag) { removeFlag(_flag); }
        void operator|=(E _flag) { addFlag(_flag); }
        void operator&=(E _flag) { flag &= (T)_flag; }
        //bool operator&(E _flag) const { return hasFlag(_flag); }
        bool operator()(E _flag) const { return hasFlag(_flag); }
        //bool operator==(E _flag) const { return hasFlag(_flag); }
        T operator[](E _bitIdx) { return flag & (1 << _bitIdx); }

        T operator ()() const { return flag; }
        operator T() const { return flag; }
    public:
        T flag;
    };

    template<typename E>
    typename std::underlying_type<E>::type AsUnderlyingType(E enumValue)
    {
        static_assert(std::is_enum<E>::value, "AsUnderlyingType can only be used with enums");
        return static_cast<typename std::underlying_type<E>::type>(enumValue);
    }

    //-----------------------------------------------------------------------------
    // MurmurHash2, 64-bit versions, by Austin Appleby

    // 64-bit hash for 64-bit platforms

    inline uint64 MurmurHash64A(const void * key, int len, unsigned int seed)
    {
        const uint64_t m = 0xc6a4a7935bd1e995;
        const int r = 47;

        uint64_t h = seed ^ (len * m);

        const uint64_t * data = (const uint64_t *)key;
        const uint64_t * end = data + (len / 8);

        while (data != end)
        {
            uint64_t k = *data++;

            k *= m;
            k ^= k >> r;
            k *= m;

            h ^= k;
            h *= m;
        }

        const unsigned char * data2 = (const unsigned char*)data;

        switch (len & 7)
        {
            case 7: h ^= uint64_t(data2[6]) << 48;
            case 6: h ^= uint64_t(data2[5]) << 40;
            case 5: h ^= uint64_t(data2[4]) << 32;
            case 4: h ^= uint64_t(data2[3]) << 24;
            case 3: h ^= uint64_t(data2[2]) << 16;
            case 2: h ^= uint64_t(data2[1]) << 8;
            case 1: h ^= uint64_t(data2[0]);
                h *= m;
        };

        h ^= h >> r;
        h *= m;
        h ^= h >> r;

        return h;
    }


    // 64-bit hash for 32-bit platforms

    inline uint64 MurmurHash64B(const void * key, int len, unsigned int seed)
    {
        const unsigned int m = 0x5bd1e995;
        const int r = 24;

        unsigned int h1 = seed ^ len;
        unsigned int h2 = 0;

        const unsigned int * data = (const unsigned int *)key;

        while (len >= 8)
        {
            unsigned int k1 = *data++;
            k1 *= m; k1 ^= k1 >> r; k1 *= m;
            h1 *= m; h1 ^= k1;
            len -= 4;

            unsigned int k2 = *data++;
            k2 *= m; k2 ^= k2 >> r; k2 *= m;
            h2 *= m; h2 ^= k2;
            len -= 4;
        }

        if (len >= 4)
        {
            unsigned int k1 = *data++;
            k1 *= m; k1 ^= k1 >> r; k1 *= m;
            h1 *= m; h1 ^= k1;
            len -= 4;
        }

        switch (len)
        {
            case 3: h2 ^= ((unsigned char*)data)[2] << 16;
            case 2: h2 ^= ((unsigned char*)data)[1] << 8;
            case 1: h2 ^= ((unsigned char*)data)[0];
                h2 *= m;
        };

        h1 ^= h2 >> 18; h1 *= m;
        h2 ^= h1 >> 22; h2 *= m;
        h1 ^= h2 >> 17; h1 *= m;
        h2 ^= h1 >> 19; h2 *= m;

        uint64_t h = h1;

        h = (h << 32) | h2;

        return h;
    }
};
#endif

#ifndef __C_STANDARD_EXT_ENUM_H__
#define __C_STANDARD_EXT_ENUM_H__

#include <cstddef>      // For size_t.
#include <cstring>      // For strcspn, strncpy.
#include <stdexcept>    // For runtime_error.
#include <type_traits>

#include <standard\Preprocessor\pp_platform.h>
#include <standard\Preprocessor\pp_seq_foreach.h>

template<typename ENUM,
    ENUM First,
    ENUM Last,
    int step = 1,
    typename std::enable_if<std::is_enum<ENUM>::value>::type* = nullptr>
struct EnumIterator
{
    ENUM current;

    EnumIterator() : current(First)
    {

    }

    EnumIterator(ENUM elem) : current(elem)
    {

    }

    EnumIterator& operator++() 
    {
        current = (ENUM)(std::underlying_type<ENUM>::type(current) + step);
        return *this;
    }

    ENUM operator*() { return current; }
    bool operator==(EnumIterator const& other)
    {
        return current == other.current;
    }
    bool operator!=(EnumIterator const& other)
    {
        return current != other.current;
    }
    EnumIterator begin() { return EnumIterator(First); }
    EnumIterator end() { return EnumIterator(Last); }
};


// The type "T" mentioned above that drops assignment operations.
template <typename U>
struct ignore_assign {
#if _MSC_VER < 1900
    explicit ignore_assign(U value) : _value(value) { }
    inline operator U() const { return _value; }

    inline const ignore_assign& operator =(int dummy) const
    {
        return *this;
    }
#else
    constexpr explicit ignore_assign(U value) : _value(value) { }
    constexpr operator U() const { return _value; }

    constexpr const ignore_assign& operator =(int dummy) const
        { return *this; }
#endif

    U   _value;
};

#define ENABLE_ENUM_CLASS_FLAG(__ENUM)                                                                                                                          \
inline bool operator & (__ENUM lhs, __ENUM rhs)                                                                                                                 \
{                                                                                                                                                               \
    return (static_cast<std::underlying_type_t<__ENUM>>(lhs) & static_cast<std::underlying_type_t<__ENUM>>(rhs));                                               \
}                                                                                                                                                               \
                                                                                                                                                                \
inline __ENUM operator | (__ENUM lhs, __ENUM rhs)                                                                                                               \
{                                                                                                                                                               \
    return (__ENUM)(static_cast<std::underlying_type_t<__ENUM>>(lhs) | static_cast<std::underlying_type_t<__ENUM>>(rhs));                                       \
}                                                                                                                                                               \
                                                                                                                                                                \
inline __ENUM& operator |=(__ENUM& lhs, __ENUM rhs)                                                                                                             \
{                                                                                                                                                               \
    return lhs = lhs | rhs;                                                                                                                                     \
}                                                                                                                                                               \
                                                                                                                                                                \
inline __ENUM& operator &=(__ENUM& lhs, __ENUM rhs)                                                                                                             \
{                                                                                                                                                               \
    return lhs = (__ENUM)(static_cast<std::underlying_type_t<__ENUM>>(lhs) & static_cast<std::underlying_type_t<__ENUM>>(rhs));                                 \
}                                                                                                                                                               

#endif //__C_STANDARD_EXT_ENUM_H__

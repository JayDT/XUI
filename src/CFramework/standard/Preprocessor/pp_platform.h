#ifndef __PP_PLATFORM_H__
#define __PP_PLATFORM_H__

// if this not working check (MSVC header yvals.h (_CONST_DATA))
#if _MSC_VER < 1900 && !defined(constexpr)
#   define constexpr static
#endif

// Is Empty External source (https://gustedt.wordpress.com/2010/06/08/detect-empty-macro-arguments/)
#define _ARG16(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, ...) _15
#define HAS_COMMA(...) IDENTITY(_ARG16(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0))
#define _TRIGGER_PARENTHESIS_(...) ,

#define ISEMPTY(...)                                                    \
_ISEMPTY(                                                               \
          /* test if there is just one argument, eventually an empty    \
             one */                                                     \
          HAS_COMMA(__VA_ARGS__),                                       \
          /* test if _TRIGGER_PARENTHESIS_ together with the argument   \
             adds a comma */                                            \
          HAS_COMMA(_TRIGGER_PARENTHESIS_ __VA_ARGS__),                 \
          /* test if the argument together with a parenthesis           \
             adds a comma */                                            \
          HAS_COMMA(__VA_ARGS__ (/*empty*/)),                           \
          /* test if placing it between _TRIGGER_PARENTHESIS_ and the   \
             parenthesis adds a comma */                                \
          IDENTITY(HAS_COMMA(_TRIGGER_PARENTHESIS_ __VA_ARGS__ (/*empty*/)))      \
          )

#define PASTE5(_0, _1, _2, _3, _4) _0 ## _1 ## _2 ## _3 ## _4
//#define _ISEMPTY_EXPAND(_0, _1, _2, _3) IDENTITY(PASTE5(_IS_EMPTY_CASE_, _0, _1, _2, _3))
#define _ISEMPTY(_0, _1, _2, _3) HAS_COMMA(PASTE5(_IS_EMPTY_CASE_, _0, _1, _2, _3))
#define _IS_EMPTY_CASE_0001 ,


#define IDENTITY(x) x
#define IDENTITY_VA(...) __VA_ARGS__
#define IDENTITY_WARP_ARG(x, ...) x

#define STRINGIFY( x ) #x
#define STRINGIFY_EXPAND( x ) STRINGIFY(x)

#define M_CONC(A, B) M_CONC_(A, B)
#define M_CONC_(A, B) A##B

// Conditional
#define IF_BRANCH1(TRUE, FALSE) IDENTITY(TRUE)
#define IF_BRANCH0(TRUE, FALSE) IDENTITY(FALSE)
#define IF_BRANCH(bit, TRUE, FALSE) IF_BRANCH ## bit(TRUE, FALSE)

#define PP_COND_IF(COND, TRUE, FALSE) IDENTITY(IF_BRANCH(COND, TRUE, FALSE)) // 

#define STRINGIFY_CONDITIONAL( x ) PP_COND_IF(ISEMPTY(x), "", STRINGIFY_EXPAND(x))


// Some helpers needed for _from_string.
constexpr const char    terminators[] = " =\t\r\n";

// The size of terminators includes the implicit '\0'.
constexpr bool is_terminator(char c, size_t index = 0)
{
    return
        index >= sizeof(terminators) ? false :
        c == terminators[index] ? true :
        is_terminator(c, index + 1);
}

constexpr bool matches_untrimmed(const char *untrimmed, const char *s,
    size_t index = 0)
{
    return
        is_terminator(untrimmed[index]) ? s[index] == '\0' :
        s[index] != untrimmed[index] ? false :
        matches_untrimmed(untrimmed, s, index + 1);
}

constexpr size_t GetConstantStringLength(const char* untrimmed, size_t index = 0)
{
    return is_terminator(untrimmed[index]) ? index : GetConstantStringLength(untrimmed, index + 1);
}

constexpr bool contains_untrimmed(const char *untrimmed, const char* s,
    size_t index = 0, size_t matched = 0, size_t matchedLength = 0)
{
    return index != 0 && matched == matchedLength ? true :
        is_terminator(untrimmed[index]) ? false :
        s[matched] != untrimmed[index] ? contains_untrimmed(untrimmed, s, index + 1, 0, matchedLength == 0 ? GetConstantStringLength(s) : matchedLength) :
        contains_untrimmed(untrimmed, s, index + 1, matched + 1, matchedLength == 0 ? GetConstantStringLength(s) : matchedLength);
}


#endif
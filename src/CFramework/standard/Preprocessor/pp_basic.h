#ifndef __PP_BASIC_H__
#define __PP_BASIC_H__

#include <standard\Preprocessor\pp_platform.h>

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

#define ADD_SPLITTER(...) (__VA_ARGS__),
#define REM(...) __VA_ARGS__
#define EAT(...)

#define REM_EXTRACT(x) IDENTITY(REM x)
#define REM_EXTRACT_VA(...) IDENTITY(REM __VA_ARGS__)

#define EAT_EXPAND(x) IDENTITY(EAT x)

#define TRIM_AFTER_FIRST_EXPAND(x, ...) REM(x)
#define TRIM_AFTER_FIRST(x, ...) IDENTITY(TRIM_AFTER_FIRST_EXPAND(x))

//////////////////////////////////////
//  COUNT BLOCKS IN VA_ARG EXAMPLE INPUT (1,2,2,3) RETURN 4 OR PP_VARADICAL_ENUM_COUNT_NULL 3
//////////////////////////////////////
#define PP_EVALUATE_COUNT( \
          _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
         _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
         _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
         _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
         _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
         _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
         _61,_62,_63,N,...) N

#define PP_VARADICAL_ENUM_COUNT(...) \
    IDENTITY(PP_EVALUATE_COUNT(__VA_ARGS__, \
                63, 62, 61, 60, \
                59, 58, 57, 56, 55, 54, 53, 52, 51, 50, \
                49, 48, 47, 46, 45, 44, 43, 42, 41, 40, \
                39, 38, 37, 36, 35, 34, 33, 32, 31, 30, \
                29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
                19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
                9, 8, 7, 6, 5, 4, 3, 2, 1, 0 ))


#define PP_VARADICAL_ENUM_COUNT_NULL(...) \
    IDENTITY(PP_EVALUATE_COUNT(__VA_ARGS__, \
                62, 61, 60, \
                59, 58, 57, 56, 55, 54, 53, 52, 51, 50, \
                49, 48, 47, 46, 45, 44, 43, 42, 41, 40, \
                39, 38, 37, 36, 35, 34, 33, 32, 31, 30, \
                29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
                19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
                9, 8, 7, 6, 5, 4, 3, 2, 1, 0 ))

#define PP_VARADICAL_ENUM_COUNT_EXPAND(...) IDENTITY(PP_VARADICAL_ENUM_COUNT(__VA_ARGS__))

//////////////////////////////////////
//  EXTRACT () BLOCK BY ARG NUM EXAMPLE INPUT (1) RETURN 1
//////////////////////////////////////
// ARG BRANCHIES
#define REM_EXPAND0(x0, ...) IDENTITY(REM x0)
#define REM_EXPAND1(x0, x1, ...) IDENTITY(REM x1)
#define REM_EXPAND2(x0, x1, x2, ...) IDENTITY(REM x2)
#define REM_EXPAND3(x0, x1, x2, x3, ...) IDENTITY(REM x3)
// ...etc

#define REM_BRANCH(ARG_NUM, ...) IDENTITY(REM_EXPAND ## ARG_NUM(__VA_ARGS__))
#define REM_PROBE(ARG_NUM, ...) IDENTITY(REM_BRANCH(ARG_NUM, __VA_ARGS__))
//#define REM_PROBE_COND(ARG_NUM, ...) IDENTITY(REM_BRANCH(ARG_NUM, __VA_ARGS__))

// ARG BRANCHIES
#define REM_COND_EXPAND0(x0, ...) IDENTITY(CONDITIONAL_REM( x0))
#define REM_COND_EXPAND1(x0, x1, ...) IDENTITY(CONDITIONAL_REM( x1))
#define REM_COND_EXPAND2(x0, x1, x2, ...) IDENTITY(CONDITIONAL_REM( x2))
#define REM_COND_EXPAND3(x0, x1, x2, x3, ...) IDENTITY(CONDITIONAL_REM( x3))
// ...etc

#define REM_COND_BRANCH(ARG_NUM, ...) IDENTITY(REM_COND_EXPAND ## ARG_NUM(__VA_ARGS__))
#define REM_PROBE_COND(ARG_NUM, ...) IDENTITY(REM_COND_BRANCH(ARG_NUM, __VA_ARGS__))

//////////////////////////////////////
//  CUT BLOCK BY ARG NUM EXAMPLE: ARGNUM 1 (0,|1|,2,3) RETURN 1
//////////////////////////////////////
// ARG BRANCHIES
#define CUT_EXPAND0(x0, ...) IDENTITY(REM(x0))
#define CUT_EXPAND1(x0, x1, ...) IDENTITY(REM(x1))
#define CUT_EXPAND2(x0, x1, x2, ...) IDENTITY(REM(x2))
#define CUT_EXPAND3(x0, x1, x2, x3, ...) IDENTITY(REM(x3))
// ...etc

#define CUT_BRANCH(ARG_NUM, ...) IDENTITY(CUT_EXPAND ## ARG_NUM(__VA_ARGS__))
#define CUT_ARG(ARG_NUM, ...) IDENTITY(CUT_BRANCH(ARG_NUM, __VA_ARGS__))

//////////////////////////////////////
//  CREATE BLOCK BY ARG NUM EXAMPLE: ARGNUM 1 (0)(1)(2)(3) RETURN 0,1,2,3
//////////////////////////////////////
#define MAKE_SPLITTER_EXPAND(x) IDENTITY(ADD_SPLITTER x)
#define MAKE_SPLITTER(x) IDENTITY(MAKE_SPLITTER_EXPAND(x))

// ARG BRANCHIES
#define SPLITTER_EXPAND0(x0, ...) IDENTITY(MAKE_SPLITTER(x0)) __VA_ARGS__
#define SPLITTER_EXPAND1(x0, x1, ...) x0, IDENTITY(MAKE_SPLITTER(x1)) __VA_ARGS__
#define SPLITTER_EXPAND2(x0, x1, x2, ...)  x0, x1, IDENTITY(MAKE_SPLITTER(x2)) __VA_ARGS__
#define SPLITTER_EXPAND3(x0, x1, x2, x3, ...) x0, x1, x2, IDENTITY(MAKE_SPLITTER(x3)) __VA_ARGS__
// ...etc

#define SPLITTER_BRANCH_II(ARG_NUM, ...) IDENTITY(SPLITTER_EXPAND ## ARG_NUM(__VA_ARGS__))
#define SPLITTER_BRANCH_I(ARG_NUM, ...) IDENTITY(SPLITTER_BRANCH_II(ARG_NUM, __VA_ARGS__))
#define SPLITTER_BRANCH(ARG_NUM, ...) IDENTITY(SPLITTER_BRANCH_I(ARG_NUM, __VA_ARGS__))
#define SPLITTER_EACH(...) IDENTITY(SPLITTER_BRANCH(PP_VARADICAL_ENUM_COUNT_NULL(__VA_ARGS__), __VA_ARGS__))

//////////////////////////////////////
//  CONDITION HAS () BLOCK
//////////////////////////////////////
#define COND_HASARG(...) 1
#define COND_HASARGCOND_HASARG 0,
#define COND_HASARG1 1,
#define COND_EXPAND_I(m, f) COND_HASARG ## m, f

#define COND_EXPAND(x) COND_EXPAND_I(x, 0)
#define TRIM_ARG(x) IDENTITY(TRIM_AFTER_FIRST(x 0))
#define COND_ARG_EXPAND(x) IDENTITY(COND_HASARG x)
#define COND_EXEC_HASARG(x) TRIM_ARG(COND_EXPAND(COND_ARG_EXPAND(x)))

// ARG BRANCHIES
#define COND_EXEC_LAST_HASARG_EXPAND0(x0, ...) IDENTITY(COND_EXEC_HASARG(x0))
#define COND_EXEC_LAST_HASARG_EXPAND1(x0, x1, ...) IDENTITY(COND_EXEC_HASARG(x1))
#define COND_EXEC_LAST_HASARG_EXPAND2(x0, x1, x2, ...)  IDENTITY(COND_EXEC_HASARG(x2)) 
#define COND_EXEC_LAST_HASARG_EXPAND3(x0, x1, x2, x3, ...) IDENTITY(COND_EXEC_HASARG(x3))
// ...etc


#define COND_EXEC_LAST_HASARG_BRANCH_II(ARG_NUM, ...) IDENTITY(COND_EXEC_LAST_HASARG_EXPAND ## ARG_NUM(__VA_ARGS__))
#define COND_EXEC_LAST_HASARG_BRANCH_I(ARG_NUM, ...) IDENTITY(COND_EXEC_LAST_HASARG_BRANCH_II(ARG_NUM, __VA_ARGS__))
#define COND_EXEC_LAST_HASARG_BRANCH(ARG_NUM, ...) IDENTITY(COND_EXEC_LAST_HASARG_BRANCH_I(ARG_NUM, __VA_ARGS__))
#define COND_EXEC_LAST_HASARG(...) IDENTITY(COND_EXEC_LAST_HASARG_BRANCH(PP_VARADICAL_ENUM_COUNT_NULL(__VA_ARGS__), __VA_ARGS__))


#endif // __PP_BASIC_H__
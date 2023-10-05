/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: A set of macro helpers
 */

#pragma once

// Stringify
#define _STRINGIFY(arg) #arg

// Concatenate arguments
#define _CONCATENATE(arg1, arg2) _CONCATENATE_(arg1, arg2) // Call the _CONCATENATE1 to expend potential macro arguments
#define _CONCATENATE_(arg1, arg2) arg1##arg2
#define _CONCATENATE2(arg1, arg2) _CONCATENATE2_(arg1, arg2) // Call the _CONCATENATE2 to expend potential macro arguments
#define _CONCATENATE2_(arg1, arg2) arg1##arg2
#define _CONCATENATE3(arg1, arg2) _CONCATENATE3_(arg1, arg2) // Call the _CONCATENATE3 to expend potential macro arguments
#define _CONCATENATE3_(arg1, arg2) arg1##arg2
#define _CONCATENATE4(arg1, arg2) _CONCATENATE4_(arg1, arg2) // Call the _CONCATENATE3 to expend potential macro arguments
#define _CONCATENATE4_(arg1, arg2) arg1##arg2
#define _CONCATENATE5(arg1, arg2) _CONCATENATE5_(arg1, arg2) // Call the _CONCATENATE3 to expend potential macro arguments
#define _CONCATENATE5_(arg1, arg2) arg1##arg2


// Merge 5 arguments together
#define _MERGE5(_0, _1, _2, _3, _4) _0 ## _1 ## _2 ## _3 ## _4

// Check if variadic list of arguments is empty
#define IS_EMPTY(...) _IS_EMPTY( \
        /* test if there is just one argument, eventually an empty one */ \
        _HAS_COMMA(__VA_ARGS__), \
        /* test if _TRIGGER_PARENTHESIS_ together with the argument adds a comma */ \
        _HAS_COMMA(_TRIGGER_PARENTHESIS_ __VA_ARGS__), \
        /* test if the argument together with a parenthesis adds a comma */ \
        _HAS_COMMA(__VA_ARGS__ (/*empty*/)), \
        /* test if placing it between _TRIGGER_PARENTHESIS_ and the parenthesis adds a comma */ \
        _HAS_COMMA(_TRIGGER_PARENTHESIS_ __VA_ARGS__ (/*empty*/)) \
    )

#define _IS_EMPTY(_0, _1, _2, _3) _HAS_COMMA(_MERGE5(_IS_EMPTY_CASE_, _0, _1, _2, _3))
#define _IS_EMPTY_CASE_0001 ,
#define _TRIGGER_PARENTHESIS_(...) ,

// Check if a variadic arguments of 64 or less parameters has a coma (one zero arguments)
#define _HAS_COMMA(...) _ARG32(__VA_ARGS__, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 \
)
#define _ARG32( \
     _0,  _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10, _11, _12, _13, _14, _15, \
    _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, \
    _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, \
    _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, \
    _64, ... \
) _64

// Helper to get the number of argument of variadic arguments that are inferior or equal to 64
#define VA_ARGC(...) _VA_ARGC_BRANCH(IS_EMPTY(__VA_ARGS__), __VA_ARGS__)
#define _VA_ARGC_BRANCH(N, ...) _CONCATENATE(_VA_ARGC_BRANCH_, N)(__VA_ARGS__)
#define _VA_ARGC_BRANCH_0(...) _VA_ARGC(__VA_ARGS__, _VA_ARGC_RSEQ_N())
#define _VA_ARGC_BRANCH_1(...) 0
#define _VA_ARGC(...) _VA_ARGC_N(__VA_ARGS__)
#define _VA_ARGC_N( \
     _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10, _11, _12, _13, _14, _15, _16, \
    _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, \
    _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, \
    _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, \
    N, ... \
) N
#define _VA_ARGC_RSEQ_N() \
    64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, \
    48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, \
    32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, \
    16, 15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1, \
    0


// Concatenate a list of variadic elements (up to 64)
#define ADD_PREFIX_EACH(prefix, ...)                _ADD_PREFIX_EACH(VA_ARGC(__VA_ARGS__), prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH(N, prefix, ...)            _CONCATENATE_ADD_PREFIX_EACH(N)(prefix __VA_OPT__(,) __VA_ARGS__)
#define _CONCATENATE_ADD_PREFIX_EACH(N)             _CONCATENATE_ADD_PREFIX_EACH_EXPAND(N) // Force macro expansion
#define _CONCATENATE_ADD_PREFIX_EACH_EXPAND(N)      _ADD_PREFIX_EACH_##N
#define _ADD_PREFIX_EACH_0(prefix)
#define _ADD_PREFIX_EACH_1(prefix, x)               _CONCATENATE2(prefix, x)
#define _ADD_PREFIX_EACH_2(prefix, x, ...)          _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_1(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_3(prefix, x, ...)          _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_2(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_4(prefix, x, ...)          _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_3(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_5(prefix, x, ...)          _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_4(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_6(prefix, x, ...)          _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_5(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_7(prefix, x, ...)          _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_6(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_8(prefix, x, ...)          _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_7(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_9(prefix, x, ...)          _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_8(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_10(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_9(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_11(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_10(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_12(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_11(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_13(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_12(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_14(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_13(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_15(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_14(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_16(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_15(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_17(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_16(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_18(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_17(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_19(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_18(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_20(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_19(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_21(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_20(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_22(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_21(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_23(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_22(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_24(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_23(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_25(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_24(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_26(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_25(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_27(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_26(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_28(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_27(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_29(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_28(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_30(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_29(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_31(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_30(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_32(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_31(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_33(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_32(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_34(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_33(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_35(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_34(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_36(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_35(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_37(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_36(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_38(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_37(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_39(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_38(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_40(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_39(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_41(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_40(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_42(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_41(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_43(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_42(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_44(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_43(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_45(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_44(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_46(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_45(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_47(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_46(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_48(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_47(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_49(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_48(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_50(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_49(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_51(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_50(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_52(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_51(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_53(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_52(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_54(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_53(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_55(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_54(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_56(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_55(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_57(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_56(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_58(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_57(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_59(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_58(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_60(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_59(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_61(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_60(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_62(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_61(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_63(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_62(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH_64(prefix, x, ...)         _CONCATENATE2(prefix, x) _ADD_PREFIX_EACH_63(prefix, __VA_ARGS__)

// Concatenate a list of variadic elements, this second version may be used when called from ADD_PREFIX_EACH to prevent recursion (up to 64)
#define ADD_PREFIX_EACH2(prefix, ...)               _ADD_PREFIX_EACH2(VA_ARGC(__VA_ARGS__), prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2(N, prefix, ...)           _CONCATENATE_ADD_PREFIX_EACH2(N)(prefix __VA_OPT__(,) __VA_ARGS__)
#define _CONCATENATE_ADD_PREFIX_EACH2(N)            _CONCATENATE_ADD_PREFIX_EACH2_EXPAND(N) // Force macro expansion
#define _CONCATENATE_ADD_PREFIX_EACH2_EXPAND(N)     _ADD_PREFIX_EACH2_##N
#define _ADD_PREFIX_EACH2_0(prefix)
#define _ADD_PREFIX_EACH2_1(prefix, x)              _CONCATENATE3(prefix, x)
#define _ADD_PREFIX_EACH2_2(prefix, x, ...)         _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_1(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_3(prefix, x, ...)         _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_2(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_4(prefix, x, ...)         _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_3(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_5(prefix, x, ...)         _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_4(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_6(prefix, x, ...)         _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_5(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_7(prefix, x, ...)         _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_6(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_8(prefix, x, ...)         _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_7(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_9(prefix, x, ...)         _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_8(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_10(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_9(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_11(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_10(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_12(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_11(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_13(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_12(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_14(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_13(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_15(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_14(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_16(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_15(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_17(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_16(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_18(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_17(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_19(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_18(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_20(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_19(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_21(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_20(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_22(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_21(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_23(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_22(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_24(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_23(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_25(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_24(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_26(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_25(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_27(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_26(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_28(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_27(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_29(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_28(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_30(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_29(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_31(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_30(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_32(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_31(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_33(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_32(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_34(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_33(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_35(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_34(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_36(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_35(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_37(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_36(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_38(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_37(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_39(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_38(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_40(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_39(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_41(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_40(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_42(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_41(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_43(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_42(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_44(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_43(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_45(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_44(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_46(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_45(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_47(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_46(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_48(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_47(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_49(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_48(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_50(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_49(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_51(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_50(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_52(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_51(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_53(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_52(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_54(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_53(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_55(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_54(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_56(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_55(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_57(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_56(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_58(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_57(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_59(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_58(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_60(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_59(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_61(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_60(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_62(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_61(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_63(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_62(prefix, __VA_ARGS__)
#define _ADD_PREFIX_EACH2_64(prefix, x, ...)        _CONCATENATE3(prefix, x) _ADD_PREFIX_EACH2_63(prefix, __VA_ARGS__)


// Concatenate a list of variadic elements separated by a comma (up to 64)
#define ADD_PREFIX_COMMA_EACH(prefix, ...)              _ADD_PREFIX_COMMA_EACH(VA_ARGC(__VA_ARGS__), prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH(N, prefix, ...)          _CONCATENATE_ADD_PREFIX_COMMA_EACH(N)(prefix __VA_OPT__(,) __VA_ARGS__)
#define _CONCATENATE_ADD_PREFIX_COMMA_EACH(N)           _CONCATENATE_ADD_PREFIX_COMMA_EACH_EXPAND(N) // Force macro expansion
#define _CONCATENATE_ADD_PREFIX_COMMA_EACH_EXPAND(N)    _ADD_PREFIX_COMMA_EACH_##N
#define _ADD_PREFIX_COMMA_EACH_0(prefix)
#define _ADD_PREFIX_COMMA_EACH_1(prefix, x)             _CONCATENATE4(prefix, x)
#define _ADD_PREFIX_COMMA_EACH_2(prefix, x, ...)        _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_1(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_3(prefix, x, ...)        _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_2(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_4(prefix, x, ...)        _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_3(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_5(prefix, x, ...)        _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_4(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_6(prefix, x, ...)        _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_5(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_7(prefix, x, ...)        _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_6(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_8(prefix, x, ...)        _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_7(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_9(prefix, x, ...)        _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_8(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_10(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_9(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_11(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_10(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_12(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_11(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_13(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_12(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_14(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_13(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_15(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_14(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_16(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_15(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_17(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_16(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_18(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_17(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_19(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_18(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_20(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_19(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_21(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_20(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_22(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_21(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_23(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_22(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_24(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_23(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_25(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_24(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_26(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_25(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_27(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_26(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_28(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_27(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_29(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_28(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_30(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_29(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_31(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_30(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_32(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_31(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_33(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_32(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_34(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_33(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_35(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_34(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_36(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_35(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_37(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_36(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_38(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_37(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_39(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_38(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_40(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_39(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_41(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_40(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_42(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_41(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_43(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_42(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_44(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_43(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_45(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_44(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_46(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_45(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_47(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_46(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_48(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_47(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_49(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_48(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_50(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_49(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_51(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_50(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_52(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_51(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_53(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_52(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_54(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_53(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_55(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_54(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_56(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_55(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_57(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_56(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_58(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_57(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_59(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_58(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_60(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_59(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_61(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_60(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_62(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_61(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_63(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_62(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH_64(prefix, x, ...)       _CONCATENATE4(prefix, x) , _ADD_PREFIX_COMMA_EACH_63(prefix, __VA_ARGS__)


// Concatenate a list of variadic elements separated by a comma, this second version may be used when called from ADD_PREFIX_COMMA_EACH to prevent recursion (up to 64)
#define ADD_PREFIX_COMMA_EACH2(prefix, ...)             _ADD_PREFIX_COMMA_EACH2(VA_ARGC(__VA_ARGS__), prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2(N, prefix, ...)               _CONCATENATE_ADD_PREFIX_COMMA_EACH2(N)(prefix __VA_OPT__(,) __VA_ARGS__)
#define _CONCATENATE_ADD_PREFIX_COMMA_EACH2(N)          _CONCATENATE_ADD_PREFIX_COMMA_EACH2_EXPAND(N) // Force macro expansion
#define _CONCATENATE_ADD_PREFIX_COMMA_EACH2_EXPAND(N)   _ADD_PREFIX_COMMA_EACH2_##N
#define _ADD_PREFIX_COMMA_EACH2_0(prefix)
#define _ADD_PREFIX_COMMA_EACH2_1(prefix, x)            _CONCATENATE5(prefix, x)
#define _ADD_PREFIX_COMMA_EACH2_2(prefix, x, ...)       _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_1(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_3(prefix, x, ...)       _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_2(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_4(prefix, x, ...)       _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_3(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_5(prefix, x, ...)       _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_4(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_6(prefix, x, ...)       _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_5(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_7(prefix, x, ...)       _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_6(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_8(prefix, x, ...)       _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_7(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_9(prefix, x, ...)       _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_8(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_10(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_9(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_11(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_10(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_12(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_11(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_13(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_12(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_14(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_13(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_15(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_14(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_16(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_15(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_17(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_16(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_18(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_17(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_19(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_18(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_20(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_19(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_21(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_20(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_22(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_21(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_23(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_22(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_24(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_23(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_25(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_24(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_26(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_25(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_27(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_26(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_28(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_27(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_29(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_28(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_30(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_29(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_31(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_30(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_32(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_31(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_33(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_32(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_34(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_33(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_35(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_34(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_36(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_35(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_37(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_36(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_38(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_37(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_39(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_38(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_40(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_39(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_41(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_40(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_42(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_41(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_43(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_42(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_44(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_43(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_45(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_44(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_46(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_45(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_47(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_46(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_48(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_47(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_49(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_48(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_50(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_49(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_51(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_50(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_52(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_51(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_53(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_52(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_54(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_53(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_55(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_54(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_56(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_55(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_57(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_56(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_58(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_57(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_59(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_58(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_60(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_59(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_61(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_60(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_62(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_61(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_63(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_62(prefix, __VA_ARGS__)
#define _ADD_PREFIX_COMMA_EACH2_64(prefix, x, ...)      _CONCATENATE5(prefix, x) , _ADD_PREFIX_COMMA_EACH2_63(prefix, __VA_ARGS__)
#pragma once

#define _GETARG_1(a1, ...) a1
#define _GETARG_2(a1, a2, ...) a2
#define _GETARG_3(a1, a2, a3, ...) a3
#define _GETARG_4(a1, a2, a3, a4, ...) a4
#define _GETARG_5(a1, a2, a3, a4, a5, ...) a5
#define _GETARG_6(a1, a2, a3, a4, a5, a6, ...) a6
#define _EXARG_34(a1,a2,a3,a4,...) a3,a4
#define _EXARG_12(a1,a2,...) a1,a2
#define _EXARG_13(a1,a2,a3,...) a1,a2,a3
#define _EXARG_23(a1,a2,a3,...) a2,a3
#define _EXARG_14(a1,a2,a3,a4,...) a1,a2,a3,a4
#define _EXARG_15(a1,a2,a3,a4,a5,...) a1,a2,a3,a4,a5
#define _EXARG_16(a1,a2,a3,a4,a5,a6...) a1,a2,a3,a4,a5,a6
#define _EXARG_26(a1,a2,a3,a4,a5,a6, ...) a2,a3,a4,a5,a6
#define _EXARG_37(a1,a2,a3,a4,a5,a6,a7, ...) a3,a4,a5,a6,a7
#define GETARG_1(...) _GETARG_1(__VA_ARGS__)
#define GETARG_2(...) _GETARG_2(__VA_ARGS__)
#define GETARG_3(...) _GETARG_3(__VA_ARGS__)
#define GETARG_4(...) _GETARG_4(__VA_ARGS__)
#define GETARG_5(...) _GETARG_5(__VA_ARGS__)
#define GETARG_6(...) _GETARG_6(__VA_ARGS__)
#define GETARG_12(...) _EXARG_12(__VA_ARGS__)
#define GETARG_13(...) _EXARG_13(__VA_ARGS__)
#define GETARG_14(...) _EXARG_14(__VA_ARGS__)
#define GETARG_15(...) _EXARG_15(__VA_ARGS__)
#define GETARG_16(...) _EXARG_16(__VA_ARGS__)
#define GETARG_34(...) _EXARG_34(__VA_ARGS__)
#define GETARG_26(...) _EXARG_26(__VA_ARGS__)
#define GETARG_23(...) _EXARG_23(__VA_ARGS__)
#define GETARG_37(...) _EXARG_37(__VA_ARGS__)

#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)
//Example: #pragma message(VAR_NAME_VALUE(DELAY2CHAR))


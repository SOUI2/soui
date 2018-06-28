#include <core-def.h>

#ifdef DLL_CORE
#ifdef UTILITIES_EXPORTS
#define UTILITIES_API __declspec(dllexport)
#else
#define UTILITIES_API __declspec(dllimport)
#endif
#else
#define UTILITIES_API
#endif


#ifdef _DEBUG
#include <crtdbg.h>
#   define SASSERT_FMTW(expr, format, ...) \
    (void) ((!!(expr)) || \
    (1 != _CrtDbgReportW(_CRT_ASSERT, _CRT_WIDE(__FILE__), __LINE__, NULL, format, __VA_ARGS__)) || \
    (_CrtDbgBreak(), 0))

#   define SASSERT_FMTA(expr, format, ...) \
    (void) ((!!(expr)) || \
    (1 != _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, format, __VA_ARGS__)) || \
    (_CrtDbgBreak(), 0))
#else
#   define SASSERT_FMTW
#   define SASSERT_FMTA
#endif

#ifdef _UNICODE 
#   define SASSERT_FMT    SASSERT_FMTW
#else
#   define SASSERT_FMT    SASSERT_FMTA
#endif//_UNICODE

#ifndef SASSERT
#include <assert.h>
#define SASSERT(x) assert(x)
#endif

#ifndef PURE
#define PURE                    = 0
#endif

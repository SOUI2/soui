
/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */


#ifndef SkPreConfig_DEFINED
#define SkPreConfig_DEFINED

//////////////////////////////////////////////////////////////////////

#if !defined(SK_DEBUG) && !defined(SK_RELEASE)
    #ifdef NDEBUG
        #define SK_RELEASE
    #else
        #define SK_DEBUG
    #endif
#endif

#ifdef SK_BUILD_FOR_WIN32
    #if !defined(SK_RESTRICT)
        #define SK_RESTRICT __restrict
    #endif
    #if !defined(SK_WARN_UNUSED_RESULT)
        #define SK_WARN_UNUSED_RESULT
    #endif
#endif

//////////////////////////////////////////////////////////////////////

#if !defined(SK_RESTRICT)
    #define SK_RESTRICT __restrict__
#endif


//////////////////////////////////////////////////////////////////////

#if !defined(SKIA_IMPLEMENTATION)
    #define SKIA_IMPLEMENTATION 0
#endif

#if defined(MATRIX_DLL)
    #if defined(WIN32)
        #if SKIA_IMPLEMENTATION
            #define SK_API __declspec(dllexport)
        #else
            #define SK_API __declspec(dllimport)
        #endif
    #else
        #define SK_API __attribute__((visibility("default")))
    #endif
#else
    #define SK_API
#endif

//////////////////////////////////////////////////////////////////////

/**
 * SK_HAS_ATTRIBUTE(<name>) should return true iff the compiler
 * supports __attribute__((<name>)). Mostly important because
 * Clang doesn't support all of GCC attributes.
 */
#if defined(__has_attribute)
#   define SK_HAS_ATTRIBUTE(x) __has_attribute(x)
#elif defined(__GNUC__)
#   define SK_HAS_ATTRIBUTE(x) 1
#else
#   define SK_HAS_ATTRIBUTE(x) 0
#endif

/**
 * SK_ATTRIBUTE_OPTIMIZE_O1 can be used as a function attribute
 * to specify individual optimization level of -O1, if the compiler
 * supports it.
 *
 * NOTE: Clang/ARM (r161757) does not support the 'optimize' attribute.
 */
#if SK_HAS_ATTRIBUTE(optimize)
#   define SK_ATTRIBUTE_OPTIMIZE_O1 __attribute__((optimize("O1")))
#else
#   define SK_ATTRIBUTE_OPTIMIZE_O1 /* nothing */
#endif

#ifndef SK_ALWAYSBREAK
#  ifdef SK_DEBUG
#    define SK_ALWAYSBREAK(cond) do { \
	if (cond) break; \
	SkNO_RETURN_HINT(); \
	SkDebugf("%s:%d: failed assertion \"%s\"\n", __FILE__, __LINE__, #cond); \
	SK_CRASH(); \
		} while (false)
#  else
#    define SK_ALWAYSBREAK(cond) do { if (cond) break; SK_CRASH(); } while (false)
#  endif
#endif

#define SK_WARN_UNUSED_RESULT

#  define SkNEW(type_name)                           (new type_name)
#  define SkNEW_ARGS(type_name, args)                (new type_name args)
#  define SkNEW_ARRAY(type_name, count)              (new type_name[(count)])
#  define SkNEW_PLACEMENT(buf, type_name)            (new (buf) type_name)
#  define SkNEW_PLACEMENT_ARGS(buf, type_name, args) (new (buf) type_name args)
#  define SkDELETE(obj)                              (delete (obj))
#  define SkDELETE_ARRAY(array)                      (delete[] (array))

#define NS_BEGIN namespace SOUI{
#define NS_END }//namespace SOUI
#endif

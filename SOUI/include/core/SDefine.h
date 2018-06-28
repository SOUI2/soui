#pragma once

// DISALLOW_COPY_AND_ASSIGN禁用拷贝和赋值构造函数.
// 需要在类的private:访问控制域中使用.
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&); \
    void operator=(const TypeName&)

#ifndef NOSMINMAX
#ifndef smax
#define smax(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef smin
#define smin(a,b)            (((a) < (b)) ? (a) : (b))
#endif
#endif

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lParam)    ((int)(short)LOWORD(lParam))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lParam)    ((int)(short)HIWORD(lParam))
#endif


#ifdef _DISABLE_NO_VTABLE
#define S_NO_VTABLE
#else
#define S_NO_VTABLE __declspec(novtable)
#endif


// SWindow Handle
typedef DWORD SWND;
typedef ULONG_PTR HSTREEITEM;

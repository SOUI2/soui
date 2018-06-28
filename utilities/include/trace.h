#pragma once

#include "utilities-def.h"

#include <windows.h>
namespace SOUI
{
    void UTILITIES_API STraceA(LPCSTR pstrFormat, ...);
    void UTILITIES_API STraceW(LPCWSTR pstrFormat, ...);
}//end of namespace SOUI

#define STRACEW SOUI::STraceW
#define STRACEA SOUI::STraceA

#ifdef _UNICODE
#define STRACE STRACEW
#else
#define STRACE STRACEA
#endif

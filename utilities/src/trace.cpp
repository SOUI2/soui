#include "trace.h"
#include <stdio.h>

#define MAX_TRACEBUF    502

namespace SOUI
{
void  STraceA(LPCSTR pstrFormat, ...)
{
#ifdef _DEBUG
    char szBuffer[MAX_TRACEBUF] = { 0 };
    va_list args;
    va_start(args, pstrFormat);
    int len = _vscprintf( pstrFormat, args ) // _vscprintf doesn't count
        + 1; // terminating '\0'
    if(len<MAX_TRACEBUF-1)
    {
        vsprintf_s(szBuffer, ARRAYSIZE(szBuffer)-1, pstrFormat, args);
        strcat_s(szBuffer, MAX_TRACEBUF, "\n");
        ::OutputDebugStringA(szBuffer);
    }
    va_end(args);
#endif
}

void  STraceW(LPCWSTR pstrFormat, ...)
{
#ifdef _DEBUG
    wchar_t szBuffer[MAX_TRACEBUF] = { 0 };
    va_list args;
    va_start(args, pstrFormat);
    int len = _vscwprintf( pstrFormat, args ) // _vscprintf doesn't count
        + 1; // terminating '\0'
    if(len<MAX_TRACEBUF-1)
    {
        vswprintf_s(szBuffer, ARRAYSIZE(szBuffer)-1, pstrFormat, args);
        wcscat_s(szBuffer, MAX_TRACEBUF, L"\n");
        ::OutputDebugStringW(szBuffer);
    }
    va_end(args);
#endif
}

}
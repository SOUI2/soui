#pragma  once

#define HIMETRIC_PER_INCH   2540
#define MAP_PIX_TO_LOGHIM(x,ppli)   MulDiv(HIMETRIC_PER_INCH, (x), (ppli))
#define MAP_LOGHIM_TO_PIX(x,ppli)   MulDiv((ppli), (x), HIMETRIC_PER_INCH)

#ifndef ATLTRACENOTIMPL
#define ATLENSURE_THROW(a,b)
#endif

#ifndef ATLTRACENOTIMPL
#define ATLTRACENOTIMPL(funcname)   return E_NOTIMPL
#endif

#ifndef ATLASSERT
#define ATLASSERT SASSERT
#endif // ATLASSERT

#ifndef ATLASSUME
#define ATLASSUME(expr) do { ATLASSERT(expr); __analysis_assume(!!(expr)); } while(0)
#endif // ATLASSUME

#ifndef ATL_NO_VTABLE
#define ATL_NO_VTABLE    __declspec(novtable)
#endif

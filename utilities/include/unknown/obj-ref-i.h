#pragma once

#include "utilities-def.h"
#include <com-def.h>

#ifdef LIB_SOUI_COM
#define SOUI_COM_API
#define SOUI_COM_C
#else
#define SOUI_COM_API __declspec(dllexport)
#define SOUI_COM_C  EXTERN_C
#endif//SOUI_COM_DLL


struct IObjRef
{
	virtual long AddRef() PURE;

	virtual long Release() PURE;
	
	virtual void OnFinalRelease() PURE;
};
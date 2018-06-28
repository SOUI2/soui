#pragma once
#include <unknown/obj-ref-i.h>
#include <sobject/sobject-i.h>
namespace SOUI
{
	interface SOUI_EXP IInterpolator : public IObjRef , public IObject
	{
		virtual float getInterpolation(float input) PURE;
	};
}
#pragma once

#include <unknown/obj-ref-i.h>

namespace SOUI
{
	struct IPathEffect : IObjRef
	{
		virtual void * GetRealPathEffect() PURE;
	};

	struct __declspec(uuid("3C4A07C2-2617-432a-ADFC-8921D8B2E2BD")) ICornerPathEffect : IPathEffect
	{
		virtual void Init(float radius) PURE;
	};

	struct __declspec(uuid("5FA8CF4C-8C83-4eac-82C0-EE160E04D78C")) IDashPathEffect : IPathEffect
	{
		virtual void Init(float intervals[], int count, float phase) PURE;
	};

	struct __declspec(uuid("51D32163-594E-4fbe-B2A5-8A07DEDDBEAB")) IDiscretePathEffect : IPathEffect
	{
		virtual void Init(float segmentLength, float deviation) PURE;
	};

}
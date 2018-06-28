#pragma once

#include <interface/SPathEffect-i.h>
#include <unknown/obj-ref-impl.hpp>
#include <effects/SkCornerPathEffect.h>
#include <effects/SkDashPathEffect.h>
#include <effects/SkDiscretePathEffect.h>

namespace SOUI
{
	class SPathEffect_Corner : public TObjRefImpl<ICornerPathEffect>
	{
	public:
		SPathEffect_Corner();
		~SPathEffect_Corner();

		virtual void Init(float radius);

		virtual void * GetRealPathEffect();
	private:
		SkCornerPathEffect * m_skPathEffect;
	};

	class SPathEffect_Dash : public TObjRefImpl<IDashPathEffect>
	{
	public:
		SPathEffect_Dash();
		~SPathEffect_Dash();

		virtual void Init(float intervals[],int count, float phase);

		virtual void * GetRealPathEffect();

	private:
		SkDashPathEffect *m_skPathEffect;
	};

	class SPathEffect_Discrete : public TObjRefImpl<IDiscretePathEffect>
	{
	public:
		SPathEffect_Discrete();
		~SPathEffect_Discrete();

		virtual void Init(float segmentLength, float deviation);

		virtual void * GetRealPathEffect();

	private:
		SkDiscretePathEffect *m_skPathEffect;
	};


}


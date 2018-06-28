#include "stdafx.h"
#include "PathEffect-Skia.h"

namespace SOUI
{

	SPathEffect_Corner::SPathEffect_Corner():m_skPathEffect(NULL)
	{
	}

	SPathEffect_Corner::~SPathEffect_Corner()
	{
		if(m_skPathEffect) m_skPathEffect->unref();
	}

	void SPathEffect_Corner::Init(float radius)
	{
		if(m_skPathEffect)
		{
			m_skPathEffect->unref();
			m_skPathEffect = NULL;
		}
		m_skPathEffect = SkCornerPathEffect::Create(radius);
	}

	void * SPathEffect_Corner::GetRealPathEffect()
	{
		return m_skPathEffect;
	}


	//////////////////////////////////////////////////////////////////////////
	SPathEffect_Dash::SPathEffect_Dash():m_skPathEffect(NULL)
	{

	}

	SPathEffect_Dash::~SPathEffect_Dash()
	{
		if(m_skPathEffect) m_skPathEffect->unref();
	}


	void SPathEffect_Dash::Init(float intervals[],int count, float phase)
	{
		if(m_skPathEffect)
		{
			m_skPathEffect->unref();
			m_skPathEffect = NULL;
		}
		m_skPathEffect = SkDashPathEffect::Create(intervals,count,phase);
	}

	void * SPathEffect_Dash::GetRealPathEffect()
	{
		return m_skPathEffect;
	}


	//////////////////////////////////////////////////////////////////////////

	void * SPathEffect_Discrete::GetRealPathEffect()
	{
		return m_skPathEffect;

	}

	void SPathEffect_Discrete::Init(float segmentLength, float deviation)
	{
		if(m_skPathEffect)
		{
			m_skPathEffect->unref();
			m_skPathEffect = NULL;
		}
		m_skPathEffect = SkDiscretePathEffect::Create(segmentLength,deviation);

	}

	SPathEffect_Discrete::~SPathEffect_Discrete()
	{
		if(m_skPathEffect) m_skPathEffect->unref();
	}

	SPathEffect_Discrete::SPathEffect_Discrete():m_skPathEffect(NULL)
	{

	}

}
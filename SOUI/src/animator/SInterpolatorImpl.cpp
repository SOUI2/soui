#include "souistd.h"
#include "animator\SInterpolatorImpl.h"
#include "layout/SLayoutSize.h" //for SLayoutSize::fequal
#pragma warning (push)
#pragma warning (disable: 4985) // disable the warning message during the include
#include <math.h>               // this is where I would normally get the warning message
#pragma warning (pop)

namespace SOUI
{
	static const float PI= 3.1415926f;

	//////////////////////////////////////////////////////////////////////////
	float SLinearInterpolator::getInterpolation(float input)
	{
		return input;
	}


	//////////////////////////////////////////////////////////////////////////
	float SAccelerateInterpolator::getInterpolation(float input)
	{
		if (SLayoutSize::fequal(mFactor , 1.0f)) {
			return input * input;
		} else {
			return (float)pow((double)input, mDoubleFactor);
		}
	}

	SAccelerateInterpolator::SAccelerateInterpolator(float factor):mFactor(factor),mDoubleFactor(2*factor)
	{
	}



	//////////////////////////////////////////////////////////////////////////

	float SDecelerateInterpolator::getInterpolation(float input)
	{
		float result;
		if (SLayoutSize::fequal(mFactor , 1.0f)) {
			result = (float)(1.0f - (1.0f - input) * (1.0f - input));
		} else {
			result = (float)(1.0f - pow((1.0f - input), 2 * mFactor));
		}
		return result;
	}

	SDecelerateInterpolator::SDecelerateInterpolator(float factor):mFactor(factor)
	{
	}

	//////////////////////////////////////////////////////////////////////////
	float SAccelerateDecelerateInterpolator::getInterpolation(float input)
	{
		return (float)(cos((input + 1) * PI) / 2.0f) + 0.5f;
	}


	//////////////////////////////////////////////////////////////////////////
	float SAnticipateInterpolator::getInterpolation(float t)
	{
		// a(t) = t * t * ((tension + 1) * t - tension)
		return t * t * ((mTension + 1) * t - mTension);
	}

	SAnticipateInterpolator::SAnticipateInterpolator(float tension):mTension(tension)
	{
	}

	SAnticipateInterpolator::SAnticipateInterpolator():mTension(2.0f)
	{
	}


	//////////////////////////////////////////////////////////////////////////
	float SAnticipateOvershootInterpolator::getInterpolation(float t)
	{
		// a(t, s) = t * t * ((s + 1) * t - s)
		// o(t, s) = t * t * ((s + 1) * t + s)
		// f(t) = 0.5 * a(t * 2, tension * extraTension), when t < 0.5
		// f(t) = 0.5 * (o(t * 2 - 2, tension * extraTension) + 2), when t <= 1.0
		if (t < 0.5f) return 0.5f * a(t * 2.0f, getTensionMulti());
		else return 0.5f * (o(t * 2.0f - 2.0f, getTensionMulti()) + 2.0f);
	}

	float SAnticipateOvershootInterpolator::o(float t, float s)
	{
		return t * t * ((s + 1) * t + s);
	}

	float SAnticipateOvershootInterpolator::a(float t, float s)
	{
		return t * t * ((s + 1) * t - s);
	}

	SAnticipateOvershootInterpolator::SAnticipateOvershootInterpolator(float tension, float extraTension):
	mTension(tension),mExtraTension(extraTension)
	{
	}


	//////////////////////////////////////////////////////////////////////////
	float SBounceInterpolator::getInterpolation(float t)
	{
		// _b(t) = t * t * 8
		// bs(t) = _b(t) for t < 0.3535
		// bs(t) = _b(t - 0.54719) + 0.7 for t < 0.7408
		// bs(t) = _b(t - 0.8526) + 0.9 for t < 0.9644
		// bs(t) = _b(t - 1.0435) + 0.95 for t <= 1.0
		// b(t) = bs(t * 1.1226)
		t *= 1.1226f;
		if (t < 0.3535f) return bounce(t);
		else if (t < 0.7408f) return bounce(t - 0.54719f) + 0.7f;
		else if (t < 0.9644f) return bounce(t - 0.8526f) + 0.9f;
		else return bounce(t - 1.0435f) + 0.95f;
	}

	float SBounceInterpolator::bounce(float t)
	{
		return t * t * 8.0f;
	}


	//////////////////////////////////////////////////////////////////////////
	float SCycleInterpolator::getInterpolation(float input)
	{
		return (float)(sin(2 * mCycles * PI * input));
	}

	SCycleInterpolator::SCycleInterpolator(float cycles):mCycles(cycles)
	{
	}

	//////////////////////////////////////////////////////////////////////////

	float SOvershootInterpolator::getInterpolation(float t)
	{
		// _o(t) = t * t * ((tension + 1) * t + tension)
		// o(t) = _o(t - 1) + 1
		t -= 1.0f;
		return t * t * ((mTension + 1) * t + mTension) + 1.0f;
	}

	SOvershootInterpolator::SOvershootInterpolator(float tension):mTension(tension)
	{
	}

}

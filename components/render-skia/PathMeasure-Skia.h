#pragma once

#include <interface/render-i.h>
#include <unknown/obj-ref-impl.hpp>
#include <core/SkPathMeasure.h>

namespace SOUI
{
	class SPathMeasure_Skia : public TObjRefImpl<IPathMeasure>
	{
	public:
		SPathMeasure_Skia(void);
		~SPathMeasure_Skia(void);

		virtual void setPath(IPath * path, bool forceClosed);

		virtual float getLength();

		virtual bool getPosTan(float distance, float pos[], float tan[]);

		virtual bool getSegment(float startD, float stopD, IPath * dst, bool startWithMoveTo);

	private:
		SkPathMeasure	m_pathMeasure;
	};


}

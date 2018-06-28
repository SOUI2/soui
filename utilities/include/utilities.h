#pragma once

#include "utilities-def.h"
#include "string/tstring.h"

namespace SOUI
{
    void UTILITIES_API SHiMetricToPixel(const SIZEL * lpSizeInHiMetric, LPSIZEL lpSizeInPix);
    void UTILITIES_API SPixelToHiMetric(const SIZEL * lpSizeInPix, LPSIZEL lpSizeInHiMetric);

}//end of namespace SOUI

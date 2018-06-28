#pragma  once

#ifndef CP_ACP
#define CP_ACP 0
#endif//CP_ACP

#include "utilities-def.h"
#include "tstring.h"

namespace SOUI
{

    class UTILITIES_API SStrCpCvt
    {
    public:
        static SStringA CvtW2A(const SStringW & str,unsigned int cp=CP_ACP);

        static SStringW CvtA2W(const SStringA & str,unsigned int cp=CP_ACP,unsigned int cp2=0);

        static SStringA CvtA2A(const SStringA & str,unsigned int cpFrom=CP_UTF8,unsigned int cpTo=CP_ACP);

        static SStringW CvtW2W(const SStringW &str,unsigned int cp=CP_ACP);

    };


}//end of namespace SOUI

#define S_CA2W SStrCpCvt::CvtA2W
#define S_CW2A SStrCpCvt::CvtW2A
#define S_CA2A SStrCpCvt::CvtA2A
#define S_CW2W SStrCpCvt::CvtW2W

#ifdef UNICODE
#define S_CA2T S_CA2W
#define S_CT2A S_CW2A
#define S_CW2T S_CW2W
#define S_CT2W S_CW2W
#else
#define S_CA2T S_CA2A
#define S_CT2A S_CA2A
#define S_CW2T S_CW2A
#define S_CT2W S_CA2W
#endif // UNICODE

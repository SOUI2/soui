//////////////////////////////////////////////////////////////////////////
//   File Name: SNamedValue.cpp
//////////////////////////////////////////////////////////////////////////

#include "souistd.h"
#include "res.mgr/SNamedValue.h"
#include "core/swndstyle.h"

namespace SOUI
{

//////////////////////////////////////////////////////////////////////////

const wchar_t KPrefixString[] = L"@string/";
const wchar_t KPrefixColor[] = L"@color/";


const int SIntParser::GetNullValue()
{
    return 0;
}

bool SIntParser::ParseValue(const SStringW & strValue,int & value)
{
    value = _wtoi(strValue);
    return true;
}


//////////////////////////////////////////////////////////////////////////
const SStringW SStringParser::GetNullValue()
{
    return SStringW();
}

bool SStringParser::ParseValue(const SStringW & strValue, SStringW & value)
{
    value = strValue;
    return true;
}

const SStringW & SNamedString::Get(const SStringW & strValue) const
{
    if(strValue.IsEmpty()) return strValue;
    if(strValue.Left(ARRAYSIZE(KPrefixString)-1)==KPrefixString)
    {
        return String2Value(strValue.Mid(ARRAYSIZE(KPrefixString)-1));
    }else
    {
        return strValue;
    }
}

//////////////////////////////////////////////////////////////////////////


const COLORREF SColorParser::GetNullValue()
{
    return CR_INVALID;
}

bool SColorParser::ParseValue(const SStringW & strValue, COLORREF & value)
{
	int r = 255, g = 255, b = 255, a = 255;
    int nSeg=0;
    SStringW strValueL = strValue;
    strValueL.MakeLower();
    if(strValueL.Left(1)==L"#")
    {
        nSeg = swscanf(strValueL,L"#%02x%02x%02x%02x",&r,&g,&b,&a);
    }else if(strValueL.Left(4).CompareNoCase(L"rgba")==0)
    {
        nSeg = swscanf(strValueL,L"rgba(%d,%d,%d,%d)",&r,&g,&b,&a);                
    }else if(strValueL.Left(3).CompareNoCase(L"rgb")==0)
    {
        nSeg = swscanf(strValueL,L"rgb(%d,%d,%d)",&r,&g,&b);                
    }
    if(nSeg!=3 && nSeg != 4)
    {
        SASSERT_FMT(FALSE,TEXT("ParseColor Failed with [%s]"),S_CW2T(strValue));
        return false;
    }else
    {
        value = RGBA(r,g,b,a);
        return true;
    }
}

const COLORREF & SNamedColor::Get(const SStringW & strValue) const
{
    if(strValue.Left(ARRAYSIZE(KPrefixColor)-1)==KPrefixColor)
    {
        return String2Value(strValue.Mid(ARRAYSIZE(KPrefixColor)-1));
    }else
    {
        static COLORREF crTmp;
        crTmp = SColorParser::GetNullValue();
        SColorParser::ParseValue(strValue,crTmp);
        return crTmp;
    }
}

void SNamedID::Init2(const NAMEDVALUE *pValue,int nCount,BOOL bSorted)
{
    m_lstNamedValue.RemoveAll();
    m_lstNamedValue.SetCount(nCount);
    for(int i=0;i<nCount;i++)
    {
        m_lstNamedValue.SetAt(i,pValue[i]);
    }
    if(!bSorted)
    {//自动排序
        qsort(m_lstNamedValue.GetData(),m_lstNamedValue.GetCount(),sizeof(NAMEDVALUE),Compare);
    }
}


}//namespace SOUI
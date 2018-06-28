#pragma once

namespace SOUI
{
    template<class T,class TC>
    size_t SplitString(const T &str, TC cSep ,SArray<T> & strLst)
    {
        int nBegin=0;
        int nEnd=0;
        while(nEnd!=str.GetLength())
        {
            if(str[nEnd]==cSep)
            {
                if(nEnd>nBegin)
                {
                    strLst.Add(str.Mid(nBegin,nEnd-nBegin));
                }
                nBegin=nEnd+1;
            }
            nEnd++;
        }
        if(nEnd>nBegin)
        {
            strLst.Add(str.Mid(nBegin,nEnd-nBegin));
        }
        return strLst.GetCount();
    }

    typedef SArray<SStringA> SStringAList;
    typedef SArray<SStringW> SStringWList;

    template size_t SplitString<SStringA,char>(const SStringA & str,char cSep, SStringAList & strLst);
    template size_t SplitString<SStringW,wchar_t>(const SStringW & str,wchar_t cSep, SStringWList & strLst);

#ifdef _UNICODE
#define SStringTList    SStringWList
#else
#define SStringTList    SStringAList
#endif//_UNICODE

    inline int ParseResID(const SStringT & str,SStringTList & strLst)
    {
        int nPos = str.Find(_T(':'));
        if(nPos==-1)
        {
            strLst.Add(str);
        }else
        {
            strLst.Add(str.Left(nPos));
            strLst.Add(str.Right(str.GetLength()-nPos-1));
        }
        return (int)strLst.GetCount();
    }

}
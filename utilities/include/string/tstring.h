#ifndef __TSTRING_H__
#define __TSTRING_H__

#pragma once

#include <windows.h>
#include <stdio.h>
#include <crtdbg.h>
#include <algorithm>
#ifndef _MBCS
#define _MBCS
#endif
#include <mbstring.h>

#ifndef TSTRING_PADDING
#define TSTRING_PADDING 0
#endif

#include "utilities-def.h"
#include <soui_mem_wrapper.h>

namespace SOUI
{

    struct TStringData
    {
        long nRefs;            // Reference count: negative == locked
        int nDataLength;    // Length of currently used data in XCHARs (not including terminating null)
        int nAllocLength;    // Length of allocated data in XCHARs (not including terminating null)

        inline void* data() const
        {
            return (void*)(this + 1);
        }

        inline void AddRef()
        {
            SASSERT(nRefs > 0);
            InterlockedIncrement(&nRefs);
        }
        inline void Release()
        {
            SASSERT(nRefs != 0);
            if (InterlockedDecrement(&nRefs) <= 0)
                soui_mem_wrapper::SouiFree(this);
        }
        inline bool IsShared() const
        {
            return nRefs > 1;
        }
        inline bool IsLocked() const
        {
            return nRefs < 0;
        }

        inline void Lock()
        {
            SASSERT(nRefs <= 1);
            nRefs--;  // Locked buffers can't be shared, so no interlocked operation necessary
            if (nRefs == 0)
                nRefs = -1;
        }
        inline void Unlock()
        {
            SASSERT(IsLocked());
            if (IsLocked())
            {
                nRefs++;  // Locked buffers can't be shared, so no interlocked operation necessary
                if (nRefs == 0)
                    nRefs = 1;
            }
        }
    };



    extern TStringData* _tstr_initDataNil;
    extern const void* _tstr_initPszNil;


    struct char_traits;
    struct wchar_traits;

    struct char_traits
    {
        static size_t StrLen(const char* psz)
        {
            return psz ? strlen(psz) : 0;
        }
        static int StrCmp(const char* psz1, const char* psz2)
        {
#ifdef _MBCS
            return _mbscmp((unsigned char*)psz1, (unsigned char*)psz2);
#else
            return strcmp(psz1, psz2);
#endif
        }
        static int StrICmp(const char* psz1, const char* psz2)
        {
#ifdef _MBCS
            return _mbsicmp((unsigned char*)psz1, (unsigned char*)psz2);
#else
            return stricmp(psz1, psz2);
#endif
        }
        static char* StrChr(const char* psz, char ch)
        {
#ifdef _MBCS
            return (char*)_mbschr((unsigned char*)psz, ch);
#else
            return strchr(psz, ch);
#endif
        }
        static char* StrRChr(const char* psz, char ch)
        {
#ifdef _MBCS
            return (char*)_mbsrchr((unsigned char*)psz, ch);
#else
            return strrchr(psz, ch);
#endif
        }
        static char* StrStr(const char* psz, const char* psz2)
        {
#ifdef _MBCS
            return (char*)_mbsstr((unsigned char*)psz, (unsigned char*)psz2);
#else
            return strstr(psz, psz2);
#endif
        }
        static char* StrUpper(char* psz)
        {
#ifdef _MBCS
            return (char*)_mbsupr((unsigned char*)psz);
#else
            return strupr(psz);
#endif
        }
        static char* StrLower(char* psz)
        {
#ifdef _MBCS
            return (char*)_mbslwr((unsigned char*)psz);
#else
            return strlwr(psz);
#endif
        }
        static int IsSpace(char ch)
        {
#ifdef _MBCS
            return _ismbcspace(ch);
#else
            return isspace(ch);
#endif
        }
        static char CharLower(char ch)
        {
            return (char)tolower(ch);
        }
        static char CharUpper(char ch)
        {
            return (char)toupper(ch);
        }
        static char* CharNext(char* psz)
        {
#ifdef _MBCS
            return (char*)_mbsinc((unsigned char*)psz);
#else
            return psz + 1;
#endif
        }
        static int Format(char** ppszDst, const char* pszFormat, va_list args)
        {
            int len = _vscprintf(pszFormat, args); // _vscprintf doesn't count terminating '\0'
            if (len == 0) return 0;
            *ppszDst = (char*)soui_mem_wrapper::SouiMalloc(len + 1);
            // vsprintf(*ppszDst, pszFormat, args);
            vsprintf_s(*ppszDst, len + 1, pszFormat, args);
            return len;
        }

        static int LoadString(HINSTANCE hInst,
            UINT uID,
            char* lpBuffer,
            int nBufferMax)
        {
            return ::LoadStringA(hInst, uID, lpBuffer, nBufferMax);
        }
    };

    struct wchar_traits
    {
        typedef wchar_t            this_type;
        typedef wchar_traits    this_traits;

        static size_t StrLen(const wchar_t* psz)
        {
            return psz ? wcslen(psz) : 0;
        }
        static int StrCmp(const wchar_t* psz1, const wchar_t* psz2)
        {
            return wcscmp(psz1, psz2);
        }
        static int StrICmp(const wchar_t* psz1, const wchar_t* psz2)
        {
            return _wcsicmp(psz1, psz2);
        }
        static wchar_t* StrChr(const wchar_t* psz, wchar_t ch)
        {
            return const_cast<wchar_t*>(wcschr(psz, ch));
        }
        static wchar_t* StrRChr(const wchar_t* psz, wchar_t ch)
        {
            return const_cast<wchar_t*>(wcsrchr(psz, ch));
        }
        static wchar_t* StrStr(const wchar_t* psz, const wchar_t* psz2)
        {
            return const_cast<wchar_t*>(wcsstr(psz, psz2));
        }
        static wchar_t* StrUpper(wchar_t* psz)
        {
            return _wcsupr(psz);
        }
        static wchar_t* StrLower(wchar_t* psz)
        {
            return _wcslwr(psz);
        }
        static int IsSpace(wchar_t ch)
        {
            return iswspace(ch);
        }
        static wchar_t CharLower(wchar_t ch)
        {
            return (wchar_t)towlower(ch);
        }
        static wchar_t CharUpper(wchar_t ch)
        {
            return (wchar_t)towupper(ch);
        }
        static wchar_t* CharNext(wchar_t* psz)
        {
            return psz + 1;
        }
        static int Format(wchar_t** ppszDst, const wchar_t* pszFormat, va_list args)
        {
            int len = _vscwprintf(pszFormat, args); // _vscprintf doesn't count terminating '\0'
            if (len == 0) return 0;
            *ppszDst = (wchar_t*)soui_mem_wrapper::SouiMalloc((len + 1) * sizeof(wchar_t));
            //vswprintf(*ppszDst, pszFormat, args);
            vswprintf_s(*ppszDst, len + 1, pszFormat, args);
            return len;
        }
        static int LoadString(HINSTANCE hInst,
            UINT uID,
            wchar_t* lpBuffer,
            int nBufferMax)
        {
            return ::LoadStringW(hInst, uID, lpBuffer, nBufferMax);
        }
    };

    template <class tchar, class tchar_traits>
    class TStringT
    {
    public:
        typedef tchar    _tchar;
        typedef const _tchar * pctstr;
        typedef tchar_traits    _tchar_traits;

        // Constructors
        TStringT()
        {
            Init();
        }
        TStringT(const TStringT& stringSrc)
        {
            SASSERT(stringSrc.GetData()->nRefs != 0);
            if (stringSrc.GetData()->nRefs >= 0)
            {
                SASSERT(stringSrc.GetData() != _tstr_initDataNil);
                m_pszData = stringSrc.m_pszData;
                GetData()->AddRef();
            }
            else
            {
                Init();
                *this = stringSrc.m_pszData;
            }
        }
        TStringT(tchar ch, int nLength = 1)
        {
            Init();
            if (nLength >= 1)
            {
                if (AllocBuffer(nLength))
                {
                    for (int i = 0; i < nLength; i++)
                        m_pszData[i] = ch;
                }
            }
        }
        TStringT(const tchar* psz, int nLength)
        {
            Init();
            if (nLength < 0) nLength = SafeStrlen(psz);
            if (nLength != 0)
            {
                if (AllocBuffer(nLength))
                    memcpy(m_pszData, psz, nLength * sizeof(tchar));
            }
        }
        TStringT(const tchar* psz)
        {
            Init();
            int nLength = SafeStrlen(psz);
            if (nLength != 0)
            {
                if (AllocBuffer(nLength))
                    memcpy(m_pszData, psz, nLength * sizeof(tchar));
            }
        }

        ~TStringT()
        {
            //  free any attached data
            TStringData* pData = GetData();
            if (pData != _tstr_initDataNil)
                pData->Release();
        }

        // Attributes & Operations
        // as an array of characters
        int GetLength() const
        {
            return GetData()->nDataLength;
        }
        bool IsEmpty() const
        {
            return GetData()->nDataLength == 0;
        }
        void Empty()    // free up the data
        {
            TStringData* pData = GetData();
            if (pData->nDataLength == 0)
                return;

            if (pData->nRefs >= 0)
                Release();
            else
            {
                tchar sz[1] = { 0 };
                *this = sz;
            }

            SASSERT(GetData()->nDataLength == 0);
            SASSERT(GetData()->IsLocked() || GetData()->nAllocLength == 0);
        }

        tchar GetAt(int nIndex) const
        {
            SASSERT(nIndex >= 0);
            SASSERT(nIndex < GetData()->nDataLength);
            return m_pszData[nIndex];
        }
        tchar operator[](int nIndex) const
        {
            // same as GetAt
            SASSERT(nIndex >= 0);
            SASSERT(nIndex < GetData()->nDataLength);
            return m_pszData[nIndex];
        }
        void SetAt(int nIndex, tchar ch)
        {
            SASSERT(nIndex >= 0);
            SASSERT(nIndex < GetData()->nDataLength);

            CopyBeforeWrite();
            m_pszData[nIndex] = ch;
        }
        operator const tchar*() const    // as a C string
        {
            return m_pszData;
        }

        // overloaded assignment
        TStringT& operator=(const TStringT& stringSrc)
        {
            if (m_pszData != stringSrc.m_pszData)
            {
                TStringData* pData = GetData();
                if ((pData->IsLocked() && pData != _tstr_initDataNil) || stringSrc.GetData()->IsLocked())
                {
                    // actual copy necessary since one of the strings is locked
                    AssignCopy(stringSrc.GetData()->nDataLength, stringSrc.m_pszData);
                }
                else
                {
                    // can just copy references around
                    Release();
                    SASSERT(stringSrc.GetData() != _tstr_initDataNil);
                    m_pszData = stringSrc.m_pszData;
                    GetData()->AddRef();
                }
            }
            return *this;
        }
        TStringT& operator=(const tchar* psz)
        {
            TStringT strCopy(psz);
            AssignCopy(strCopy.GetData()->nDataLength, strCopy.m_pszData);
            return *this;
        }
        const TStringT& operator=(tchar ch)
        {
            AssignCopy(1, &ch);
            return *this;
        }

        // string concatenation
        const TStringT& operator+=(const tchar* psz)
        {
            return Append(psz);
        }

        const TStringT& operator+=(tchar ch)
        {
            return Append(ch);
        }

        const TStringT& operator+=(const TStringT& src)
        {
            return Append(src);
        }

        const TStringT& Append(tchar ch)
        {
            ConcatInPlace(1, &ch);
            return *this;
        }

        const TStringT& Append(const tchar * psz)
        {
            TStringT strCopy(psz);
            ConcatInPlace(strCopy.GetData()->nDataLength, strCopy.m_pszData);
            return *this;
        }

        const TStringT& Append(const TStringT& src)
        {
            TStringT strCopy(src);
            ConcatInPlace(strCopy.GetData()->nDataLength, strCopy.m_pszData);
            return *this;
        }

        // string comparison
        int Compare(const tchar* psz) const
        {
            return tchar_traits::StrCmp(m_pszData, psz);
        }
        int CompareNoCase(const tchar* psz) const
        {
            return tchar_traits::StrICmp(m_pszData, psz);
        }

        // simple sub-string extraction
        TStringT Mid(int nFirst) const
        {
            return Mid(nFirst, GetData()->nDataLength - nFirst);
        }
        TStringT Mid(int nFirst, int nCount) const
        {
            // out-of-bounds requests return sensible things
            if (nFirst < 0)
                nFirst = 0;
            if (nCount < 0)
                nCount = 0;

            TStringData* pData = GetData();
            if (nFirst + nCount > pData->nDataLength)
                nCount = pData->nDataLength - nFirst;
            if (nFirst > pData->nDataLength)
                nCount = 0;

            TStringT dest;
            AllocCopy(dest, nCount, nFirst, 0);
            return dest;
        }
        TStringT Right(int nCount) const
        {
            TStringData* pData = GetData();
            if (nCount < 0)
                nCount = 0;
            else if (nCount > pData->nDataLength)
                nCount = pData->nDataLength;

            TStringT dest;
            AllocCopy(dest, nCount, pData->nDataLength - nCount, 0);
            return dest;
        }
        TStringT Left(int nCount) const
        {
            TStringData* pData = GetData();
            if (nCount < 0)
                nCount = 0;
            else if (nCount > pData->nDataLength)
                nCount = pData->nDataLength;

            TStringT dest;
            AllocCopy(dest, nCount, 0, 0);
            return dest;
        }

        //    string utilities
        TStringT& MakeUpper()
        {
            CopyBeforeWrite();

            if (m_pszData != NULL)
                tchar_traits::StrUpper(m_pszData);
            return *this;
        }
        TStringT& MakeLower()
        {
            CopyBeforeWrite();

            if (m_pszData != NULL)
                tchar_traits::StrLower(m_pszData);
            return *this;
        }

        // remove continuous occcurrences of characters in passed string, starting from right
        TStringT & TrimRight(tchar chTarget = VK_SPACE)
        {
            CopyBeforeWrite();

            // find beginning of trailing matches
            // by starting at beginning (DBCS aware)
            tchar* psz = m_pszData;
            tchar* pszLast = NULL;

            while (*psz != '\0')
            {
                if (*psz == chTarget)
                {
                    if (pszLast == NULL)
                        pszLast = psz;
                }
                else
                    pszLast = NULL;
                psz = tchar_traits::CharNext(psz);
            }

            if (pszLast != NULL)
            {
                // truncate at left-most matching character
                *pszLast = '\0';
                GetData()->nDataLength = (int)(pszLast - m_pszData);
            }
            return *this;
        }

        // remove continuous occurrences of chTarget starting from left
        TStringT & TrimLeft(tchar chTarget = VK_SPACE)
        {
            CopyBeforeWrite();

            // find first non-matching character
            tchar* psz = m_pszData;

            while (chTarget == *psz)
                psz = tchar_traits::CharNext(psz);

            if (psz != m_pszData)
            {
                // fix up data and length
                TStringData* pData = GetData();
                int nDataLength = pData->nDataLength - (int)(psz - m_pszData);
                memmove(m_pszData, psz, (nDataLength + 1) * sizeof(tchar));
                pData->nDataLength = nDataLength;
            }
            return *this;
        }

        TStringT & Trim(tchar ch = VK_SPACE)
        {
            TrimRight(ch);
            TrimLeft(ch);
            return *this;
        }


        static bool IsBlankChar(const tchar &c)
        {
            const tchar szBlank[] = { 0x0a,0x0d,0x20,0x09 };
            for (int i = 0; i < ARRAYSIZE(szBlank); i++)
            {
                if (c == szBlank[i]) return true;
            }
            return false;
        }

        void TrimBlank()
        {
            if (IsEmpty()) return;

            const tchar * pbuf = m_pszData;
            const tchar * p = pbuf;
            //look for start
            while (*p)
            {
                if (!IsBlankChar(*p)) break;
                p++;
            }
            const tchar * p1 = p;   //get start
            //look for end
            const tchar * p2 = pbuf + GetLength() - 1;
            while (p2 >= p1)
            {
                if (!IsBlankChar(*p2)) break;
                p2--;
            }
            if (p2 < p1)
                Empty();
            else
                (*this) = TStringT<tchar, tchar_traits>(p1, (int)(p2 - p1 + 1));
        }

		bool StartsWith(const TStringT& prefix, bool IgnoreCase = false) const
		{
			if (GetLength() >= prefix.GetLength())
			{
				TStringT _src = Left(prefix.GetLength());
				if (IgnoreCase)
					return 0 == _src.CompareNoCase(prefix);
				else
					return 0 == _src.Compare(prefix);
			}
			return false;
		}

		bool EndsWith(const TStringT& suffix, bool IgnoreCase = false) const
		{
			if (GetLength() >= suffix.GetLength())
			{
				TStringT _src = Right(suffix.GetLength());
				if (IgnoreCase)
					return 0 == _src.CompareNoCase(suffix);
				else
					return 0 == _src.Compare(suffix);
			}
			return false;
		}

        // insert character at zero-based index; concatenates if index is past end of string
        int Insert(int nIndex, tchar ch)
        {
            CopyBeforeWrite();

            if (nIndex < 0)
                nIndex = 0;

            TStringData* pData = GetData();
            int nNewLength = pData->nDataLength;
            if (nIndex > nNewLength)
                nIndex = nNewLength;
            nNewLength++;

            if (pData->nAllocLength < nNewLength)
                if (!ReallocBuffer(nNewLength))
                    return -1;

            // move existing bytes down
            memmove(m_pszData + nIndex + 1, m_pszData + nIndex, (nNewLength - nIndex) * sizeof(tchar));
            m_pszData[nIndex] = ch;
            GetData()->nDataLength = nNewLength;

            return nNewLength;
        }
        // insert substring at zero-based index; concatenates if index is past end of string
        inline int Insert(int nIndex, const tchar* psz)
        {
            if (nIndex < 0)
                nIndex = 0;

            int nInsertLength = SafeStrlen(psz);
            int nNewLength = GetData()->nDataLength;
            if (nInsertLength > 0)
            {
                CopyBeforeWrite();

                if (nIndex > nNewLength)
                    nIndex = nNewLength;
                nNewLength += nInsertLength;

                TStringData* pData = GetData();
                if (pData->nAllocLength < nNewLength)
                    if (!ReallocBuffer(nNewLength))
                        return -1;

                // move existing bytes down
                memmove(m_pszData + nIndex + nInsertLength, m_pszData + nIndex, (nNewLength - nIndex - nInsertLength + 1) * sizeof(tchar));
                memcpy(m_pszData + nIndex, psz, nInsertLength * sizeof(tchar));
                GetData()->nDataLength = nNewLength;
            }
            return nNewLength;
        }
        inline int Delete(int nIndex, int nCount = 1)
        {
            if (nIndex < 0)
                nIndex = 0;
            int nLength = GetData()->nDataLength;
            if (nCount > 0 && nIndex < nLength)
            {
                if((nIndex + nCount) > nLength)
                    nCount = nLength - nIndex;

                CopyBeforeWrite();

                int nBytesToCopy = nLength - (nIndex + nCount) + 1;
                memmove(m_pszData + nIndex, m_pszData + nIndex + nCount, nBytesToCopy * sizeof(tchar));
                nLength -= nCount;
                GetData()->nDataLength = nLength;
            }

            return nLength;
        }
        int Replace(tchar chOld, tchar chNew)
        {
            int nCount = 0;

            // short-circuit the nop case
            if (chOld != chNew)
            {
                CopyBeforeWrite();

                // otherwise modify each character that matches in the string
                tchar* psz = m_pszData;
                tchar* pszEnd = psz + GetData()->nDataLength;
                while (psz < pszEnd)
                {
                    // replace instances of the specified character only
                    if (*psz == chOld)
                    {
                        *psz = chNew;
                        nCount++;
                    }
                    psz = tchar_traits::CharNext(psz);
                }
            }
            return nCount;
        }
        inline int Replace(const tchar* pszOld, const tchar* pszNew)
        {
            // can't have empty or NULL pszOld
            int nSourceLen = SafeStrlen(pszOld);
            if (nSourceLen == 0)
                return 0;
            int nReplacementLen = SafeStrlen(pszNew);

            // loop once to figure out the size of the result string
            int nCount = 0;
            tchar* pszStart = m_pszData;
            tchar* pszEnd = m_pszData + GetData()->nDataLength;
            tchar* pszTarget;
            while (pszStart < pszEnd)
            {
                while ((pszTarget = tchar_traits::StrStr(pszStart, pszOld)) != NULL)
                {
                    nCount++;
                    pszStart = pszTarget + nSourceLen;
                }
                pszStart += tchar_traits::StrLen(pszStart) + 1;
            }

            // if any changes were made, make them
            if (nCount > 0)
            {
                CopyBeforeWrite();

                // if the buffer is too small, just
                //   allocate a new buffer (slow but sure)
                TStringData* pOldData = GetData();
                int nOldLength = pOldData->nDataLength;
                int nNewLength = nOldLength + (nReplacementLen - nSourceLen) * nCount;
                if (pOldData->nAllocLength < nNewLength || pOldData->IsShared())
                    if (!ReallocBuffer(nNewLength))
                        return -1;

                // else, we just do it in-place
                pszStart = m_pszData;
                pszEnd = m_pszData + GetData()->nDataLength;

                // loop again to actually do the work
                while (pszStart < pszEnd)
                {
                    while ((pszTarget = tchar_traits::StrStr(pszStart, pszOld)) != NULL)
                    {
                        int nBalance = nOldLength - ((int)(pszTarget - m_pszData) + nSourceLen);
                        memmove(pszTarget + nReplacementLen, pszTarget + nSourceLen, nBalance * sizeof(tchar));
                        memcpy(pszTarget, pszNew, nReplacementLen * sizeof(tchar));
                        pszStart = pszTarget + nReplacementLen;
                        pszStart[nBalance] = '\0';
                        nOldLength += (nReplacementLen - nSourceLen);
                    }
                    pszStart += tchar_traits::StrLen(pszStart) + 1;
                }
                SASSERT(m_pszData[nNewLength] == '\0');
                GetData()->nDataLength = nNewLength;
            }
            return nCount;
        }
        inline int Remove(tchar chRemove)
        {
            CopyBeforeWrite();

            tchar* pstrSource = m_pszData;
            tchar* pstrDest = m_pszData;
            tchar* pstrEnd = m_pszData + GetData()->nDataLength;

            while (pstrSource < pstrEnd)
            {
                if (*pstrSource != chRemove)
                {
                    *pstrDest = *pstrSource;
                    pstrDest = tchar_traits::CharNext(pstrDest);
                }
                pstrSource = tchar_traits::CharNext(pstrSource);
            }
            *pstrDest = '\0';
            int nCount = (int)(pstrSource - pstrDest);
            GetData()->nDataLength -= nCount;

            return nCount;
        }

        // searching (return starting index, or -1 if not found)
        // look for a single character match
        int Find(tchar ch, int nStart = 0) const
        {
            int nLength = GetData()->nDataLength;
            if (nStart >= nLength)
                return -1;

            // find first single character
            tchar* psz = tchar_traits::StrChr(m_pszData + nStart, ch);

            // return -1 if not found and index otherwise
            return (psz == NULL) ? -1 : (int)(psz - m_pszData);
        }
        int ReverseFind(tchar ch) const
        {
            // find last single character
            tchar* psz = tchar_traits::StrRChr(m_pszData, ch);

            // return -1 if not found, distance from beginning otherwise
            return (psz == NULL) ? -1 : (int)(psz - m_pszData);
        }

        // find a sub-string (like strstr)
        int Find(const tchar* pszSub, int nStart = 0) const
        {
            int nLength = GetData()->nDataLength;
            if (nStart > nLength)
                return -1;

            // find first matching substring
            tchar* psz = tchar_traits::StrStr(m_pszData + nStart, pszSub);

            // return -1 for not found, distance from beginning otherwise
            return (psz == NULL) ? -1 : (int)(psz - m_pszData);
        }

        BOOL LoadString(UINT nID,HINSTANCE hInst)
        {
            SASSERT_FMT(hInst, TEXT("hInstance is null"));
            tchar buf[1024 + 1];
            int nChar = tchar_traits::LoadString(hInst, nID, buf, 1024);
            if (nChar == 0) return FALSE;
            AssignCopy(nChar, buf);
            return TRUE;
        }

        BOOL __cdecl Format(HINSTANCE hInst,UINT nFormatID, ...)
        {
            TStringT strFormat;
            if (!strFormat.LoadString(nFormatID, hInst))
            {
                Empty();
                return FALSE;
            }

            va_list argList;
            va_start(argList, nFormatID);
            BOOL bRet = _Format(strFormat, argList);
            va_end(argList);
            return bRet;
        }

        void __cdecl AppendFormat(HINSTANCE hInst,UINT nFormatID, ...)
        {
            TStringT strFormat;
            if (!strFormat.LoadString(nFormatID, hInst))
                return;

            va_list argList;
            va_start(argList, nFormatID);
            _AppendFormat(strFormat, argList);
            va_end(argList);
        }

        // formatting (using sprintf style formatting)
        TStringT __cdecl Format(const tchar* pszFormat, ...)
        {
            va_list argList;
            va_start(argList, pszFormat);
            _Format(pszFormat, argList);
            va_end(argList);
            return *this;
        }
        // Append formatted data using format string 'pszFormat'
        TStringT  __cdecl AppendFormat(const tchar* pszFormat, ...)
        {
            va_list argList;
            va_start(argList, pszFormat);
            _AppendFormat(pszFormat, argList);
            va_end(argList);
            return *this;
        }

        // Access to string implementation buffer as "C" character array
        tchar* GetBuffer(int nMinBufLength)
        {
            SASSERT(nMinBufLength >= 0);

            TStringData* pData = GetData();
            if (pData->IsShared() || nMinBufLength > pData->nAllocLength)
            {
                // we have to grow the buffer
                int nOldLen = pData->nDataLength;
                if (nMinBufLength < nOldLen)
                    nMinBufLength = nOldLen;
                if (!ReallocBuffer(nMinBufLength))
                    return NULL;
            }
            SASSERT(GetData()->nRefs <= 1);

            // return a pointer to the character storage for this string
            SASSERT(m_pszData != NULL);
            return m_pszData;
        }
        void ReleaseBuffer(int nNewLength = -1)
        {
            CopyBeforeWrite();  // just in case GetBuffer was not called

            if (nNewLength == -1)
                nNewLength = SafeStrlen(m_pszData); // zero terminated

            TStringData* pData = GetData();
            SASSERT(nNewLength <= pData->nAllocLength);
            pData->nDataLength = nNewLength;
            m_pszData[nNewLength] = '\0';
        }
        tchar* GetBufferSetLength(int nNewLength)
        {
            SASSERT(nNewLength >= 0);

            if (GetBuffer(nNewLength) == NULL)
                return NULL;

            GetData()->nDataLength = nNewLength;
            m_pszData[nNewLength] = '\0';
            return m_pszData;
        }
        void SetLength(int nLength)
        {
            SASSERT(nLength >= 0);
            SASSERT(nLength <= GetData()->nAllocLength);

            if (nLength >= 0 && nLength < GetData()->nAllocLength)
            {
                GetData()->nDataLength = nLength;
                m_pszData[nLength] = 0;
            }
        }
        void Preallocate(int nLength)
        {
            int nOldLength = GetLength();
            GetBuffer(nLength);
            ReleaseBuffer(nOldLength);
        }
        void FreeExtra()
        {
            TStringData* pData = GetData();
            SASSERT(pData->nDataLength <= pData->nAllocLength);
            if (pData->nDataLength < pData->nAllocLength)
            {
                if (ReallocBuffer(pData->nDataLength))
                    SASSERT(m_pszData[GetData()->nDataLength] == '\0');
            }
            SASSERT(GetData() != NULL);
        }

        // Use LockBuffer/UnlockBuffer to turn refcounting off
        tchar* LockBuffer()
        {
            tchar* psz = GetBuffer(0);
            if (psz != NULL)
                GetData()->Lock();
            return psz;
        }
        void UnlockBuffer()
        {
            if (GetData() != _tstr_initDataNil)
                GetData()->Unlock();
        }

        friend inline bool __stdcall operator==(const TStringT& s1, const TStringT& s2)
        {
            return s1.Compare(s2) == 0;
        }
        friend inline bool __stdcall operator==(const TStringT& s1, const tchar* s2)
        {
            return s1.Compare(s2) == 0;
        }
        friend inline bool __stdcall operator==(const tchar* s1, const TStringT& s2)
        {
            return s2.Compare(s1) == 0;
        }

        friend inline bool __stdcall operator!=(const TStringT& s1, const TStringT& s2)
        {
            return s1.Compare(s2) != 0;
        }
        friend inline bool __stdcall operator!=(const TStringT& s1, const tchar* s2)
        {
            return s1.Compare(s2) != 0;
        }
        friend inline bool __stdcall operator!=(const tchar* s1, const TStringT& s2)
        {
            return s2.Compare(s1) != 0;
        }

        friend inline bool __stdcall operator<(const TStringT& s1, const TStringT& s2)
        {
            return s1.Compare(s2) < 0;
        }
        friend inline bool __stdcall operator<(const TStringT& s1, const tchar* s2)
        {
            return s1.Compare(s2) < 0;
        }
        friend inline bool __stdcall operator<(const tchar* s1, const TStringT& s2)
        {
            return s2.Compare(s1) > 0;
        }

        friend inline bool __stdcall operator>(const TStringT& s1, const TStringT& s2)
        {
            return s1.Compare(s2) > 0;
        }
        friend inline bool __stdcall operator>(const TStringT& s1, const tchar* s2)
        {
            return s1.Compare(s2) > 0;
        }
        friend inline bool __stdcall operator>(const tchar* s1, const TStringT& s2)
        {
            return s2.Compare(s1) < 0;
        }

        friend inline bool __stdcall operator<=(const TStringT& s1, const TStringT& s2)
        {
            return s1.Compare(s2) <= 0;
        }
        friend inline bool __stdcall operator<=(const TStringT& s1, const tchar* s2)
        {
            return s1.Compare(s2) <= 0;
        }
        friend inline bool __stdcall operator<=(const tchar* s1, const TStringT& s2)
        {
            return s2.Compare(s1) >= 0;
        }

        friend inline bool __stdcall operator>=(const TStringT& s1, const TStringT& s2)
        {
            return s1.Compare(s2) >= 0;
        }
        friend inline bool __stdcall operator>=(const TStringT& s1, const tchar* s2)
        {
            return s1.Compare(s2) >= 0;
        }
        friend inline bool __stdcall operator>=(const tchar* s1, const TStringT& s2)
        {
            return s2.Compare(s1) <= 0;
        }

        friend inline TStringT __stdcall operator+(const TStringT& string1, const TStringT& string2)
        {
            TStringT s;
            s.ConcatCopy(string1.GetData()->nDataLength, string1.m_pszData, string2.GetData()->nDataLength, string2.m_pszData);
            return s;
        }
        friend inline TStringT __stdcall operator+(const TStringT& string, const tchar* psz)
        {
            SASSERT(psz != NULL);
            TStringT s;
            s.ConcatCopy(string.GetData()->nDataLength, string.m_pszData, TStringT::SafeStrlen(psz), psz);
            return s;
        }
        friend inline TStringT __stdcall operator+(const tchar* psz, const TStringT& string)
        {
            SASSERT(psz != NULL);
            TStringT s;
            s.ConcatCopy(TStringT::SafeStrlen(psz), psz, string.GetData()->nDataLength, string.m_pszData);
            return s;
        }
        friend inline TStringT __stdcall operator+(const TStringT& string1, tchar ch)
        {
            TStringT s;
            s.ConcatCopy(string1.GetData()->nDataLength, string1.m_pszData, 1, &ch);
            return s;
        }
        friend inline TStringT __stdcall operator+(tchar ch, const TStringT& string)
        {
            TStringT s;
            s.ConcatCopy(1, &ch, string.GetData()->nDataLength, string.m_pszData);
            return s;
        }

        // Implementation
    public:
        inline int GetAllocLength() const
        {
            return GetData()->nAllocLength;
        }

        static int SafeStrlen(const tchar* psz)
        {
            return (psz == NULL) ? 0 : (int)tchar_traits::StrLen(psz);
        }

    protected:
        // implementation helpers
        inline TStringData* GetData() const
        {
            SASSERT(m_pszData != NULL);
            return ((TStringData*)m_pszData) - 1;
        }
        inline void Init()
        {
            m_pszData = (tchar*)_tstr_initPszNil;
        }

        BOOL _Format(const tchar* pszFormat, va_list args)
        {
            if (pszFormat == NULL || *pszFormat == '\0')
            {
                Empty();
                return FALSE;
            }

            tchar* pszBuffer = NULL;
            int nLength = tchar_traits::Format(&pszBuffer, pszFormat, args);
            if (nLength > 0 && pszBuffer != NULL)
            {
                *this = TStringT(pszBuffer, nLength);
                soui_mem_wrapper::SouiFree(pszBuffer);
                return TRUE;
            }
            else
            {
                Empty();
                return FALSE;
            }
        }

        void _AppendFormat(const tchar* pszFormat, va_list args)
        {
            if (pszFormat == NULL || *pszFormat == '\0')
                return;

            tchar* pszBuffer = NULL;
            int nLength = tchar_traits::Format(&pszBuffer, pszFormat, args);
            if (nLength > 0 && pszBuffer != NULL)
            {
                *this += TStringT(pszBuffer, nLength);
                soui_mem_wrapper::SouiFree(pszBuffer);
            }
        }

        // Assignment operators
        //  All assign a new value to the string
        //      (a) first see if the buffer is big enough
        //      (b) if enough room, copy on top of old buffer, set size and type
        //      (c) otherwise free old string data, and create a new one
        //
        //  All routines return the new string (but as a 'const TStringT&' so that
        //      assigning it again will cause a copy, eg: s1 = s2 = "hi there".
        //
        void AllocCopy(TStringT& dest, int nCopyLen, int nCopyIndex, int nExtraLen) const
        {
            // will clone the data attached to this string
            // allocating 'nExtraLen' characters
            // Places results in uninitialized string 'dest'
            // Will copy the part or all of original data to start of new string

            int nNewLen = nCopyLen + nExtraLen;
            if (nNewLen == 0)
            {
                dest.Init();
            }
            else
            {
                if (dest.ReallocBuffer(nNewLen))
                    memcpy(dest.m_pszData, m_pszData + nCopyIndex, nCopyLen * sizeof(tchar));
            }
        }

        void AssignCopy(int nSrcLen, const tchar* pszSrcData)
        {
            if (AllocBeforeWrite(nSrcLen))
            {
                memcpy(m_pszData, pszSrcData, nSrcLen * sizeof(tchar));
                GetData()->nDataLength = nSrcLen;
                m_pszData[nSrcLen] = '\0';
            }
        }

        // Concatenation
        // NOTE: "operator+" is done as friend functions for simplicity
        //      There are three variants:
        //          TStringT + TStringT
        // and for ? = tchar, const tchar*
        //          TStringT + ?
        //          ? + TStringT

        bool ConcatCopy(int nSrc1Len, const tchar* pszSrc1Data, int nSrc2Len, const tchar* pszSrc2Data)
        {
            // -- master concatenation routine
            // Concatenate two sources
            // -- assume that 'this' is a new TStringT object

            bool bRet = true;
            int nNewLength = nSrc1Len + nSrc2Len;
            if (nNewLength != 0)
            {
                bRet = ReallocBuffer(nNewLength);
                if (bRet)
                {
                    memcpy(m_pszData, pszSrc1Data, nSrc1Len * sizeof(tchar));
                    memcpy(m_pszData + nSrc1Len, pszSrc2Data, nSrc2Len * sizeof(tchar));
                }
            }
            return bRet;
        }
        void ConcatInPlace(int nSrcLen, const tchar* pszSrcData)
        {
            //  -- the main routine for += operators

            // concatenating an empty string is a no-op!
            if (nSrcLen == 0)
                return;

            // if the buffer is too small, or we have a width mis-match, just
            //   allocate a new buffer (slow but sure)
            TStringData* pData = GetData();
            if (pData->IsShared() || pData->nDataLength + nSrcLen > pData->nAllocLength)
            {
                // we have to grow the buffer
                int nOldDataLength = pData->nDataLength;
                int nNewLength = nOldDataLength + nSrcLen;
                if (ReallocBuffer(nNewLength))
                    memcpy(m_pszData + nOldDataLength, pszSrcData, nSrcLen * sizeof(tchar));
            }
            else
            {
                // fast concatenation when buffer big enough
                memcpy(m_pszData + pData->nDataLength, pszSrcData, nSrcLen * sizeof(tchar));
                pData->nDataLength += nSrcLen;
                SASSERT(pData->nDataLength <= pData->nAllocLength);
                m_pszData[pData->nDataLength] = '\0';
            }
        }
        void CopyBeforeWrite()
        {
            TStringData* pData = GetData();
            if (pData->IsShared())
            {
                Release();
                if (AllocBuffer(pData->nDataLength))
                    memcpy(m_pszData, pData->data(), (pData->nDataLength + 1) * sizeof(tchar));
            }
            SASSERT(GetData()->nRefs <= 1);
        }
        bool AllocBeforeWrite(int nLen)
        {
            bool bRet = true;
            TStringData* pData = GetData();
            if (pData->IsShared() || nLen > pData->nAllocLength)
            {
                Release();
                bRet = AllocBuffer(nLen);
            }
            SASSERT(GetData()->nRefs <= 1);
            return bRet;
        }

        bool AllocBuffer(int nLength)
        {
            TStringData* pData = AllocData(nLength);
            if (pData != NULL)
            {
                m_pszData = (tchar*)pData->data();
                return true;
            }
            return false;
        }

        bool ReallocBuffer(int nNewLength)
        {
#define TSTRING_REALLOC
#ifdef TSTRING_REALLOC
            TStringData* pData = GetData();
            if (!pData->IsShared() && pData != _tstr_initDataNil)
            {
                pData = AllocData(nNewLength, pData);
                if (pData != NULL)
                {
                    m_pszData = (tchar*)pData->data();
                    return true;
                }

                Init();
                return false;
            }
#endif
            TStringData* pOldData = GetData();
            tchar* psz = m_pszData;
            if (AllocBuffer(nNewLength))
            {
                int nLength = (std::min)(pOldData->nDataLength, nNewLength) + 1;
                memcpy(m_pszData, psz, nLength * sizeof(tchar));
                ReleaseData(pOldData);
                return true;
            }
            return false;
        }
        void Release()
        {
            TStringData* pData = GetData();
            if (pData != _tstr_initDataNil)
            {
                SASSERT(pData->nRefs != 0);
                pData->Release();
                Init();
            }
        }

        // always allocate one extra character for '\0' termination
        // assumes [optimistically] that data length will equal allocation length
        static TStringData* AllocData(int nLength, TStringData* pOldData = NULL)
        {
            SASSERT(nLength >= 0);
            SASSERT(nLength <= 0x7fffffff);    // max size (enough room for 1 extra)

            if (nLength == 0)
                return _tstr_initDataNil;

            int nSize = sizeof(TStringData) + (nLength + 1 + TSTRING_PADDING) * sizeof(tchar);
            TStringData* pData;
            if (pOldData == NULL)
                pData = (TStringData*)soui_mem_wrapper::SouiMalloc(nSize);
            else
                pData = (TStringData*)soui_mem_wrapper::SouiRealloc(pOldData, nSize);
            if (pData == NULL)
                return NULL;

            pData->nRefs = 1;
            pData->nDataLength = nLength;
            pData->nAllocLength = nLength;

            tchar* pchData = (tchar*)pData->data();
            pchData[nLength] = '\0';

#if TSTRING_PADDING > 0
            for (int i = 1; i <= TSTRING_PADDING; i++)
                pchData[nLength + i] = '\0';
#endif

            return pData;
        }
        static void ReleaseData(TStringData* pData)
        {
            if (pData != _tstr_initDataNil)
            {
                SASSERT(pData->nRefs != 0);
                pData->Release();
            }
        }

    protected:
        tchar* m_pszData;   // pointer to ref counted string data
    };

#ifdef UTILITIES_EXPORTS
#    define EXPIMP_TEMPLATE
#else
#    define EXPIMP_TEMPLATE extern
#endif

#pragma warning (disable : 4231)

    EXPIMP_TEMPLATE template class UTILITIES_API  TStringT<char, char_traits>;
    EXPIMP_TEMPLATE template class UTILITIES_API  TStringT<wchar_t, wchar_traits>;


    typedef TStringT<char, char_traits>        SStringA;
    typedef TStringT<wchar_t, wchar_traits>     SStringW;

#ifdef _UNICODE
    typedef SStringW                        SStringT;
#else
    typedef SStringA                        SStringT;
#endif


    template< typename T >
    class SStringElementTraits
    {
    public:
        typedef typename T::pctstr INARGTYPE;
        typedef T& OUTARGTYPE;

        static void __cdecl CopyElements(T* pDest, const T* pSrc, size_t nElements)
        {
            for (size_t iElement = 0; iElement < nElements; iElement++)
            {
                pDest[iElement] = pSrc[iElement];
            }
        }

        static void __cdecl RelocateElements(T* pDest, T* pSrc, size_t nElements)
        {
            memmove_s(pDest, nElements * sizeof(T), pSrc, nElements * sizeof(T));
        }

        static ULONG __cdecl Hash(INARGTYPE  str)
        {
            SASSERT(str != NULL);
            ULONG nHash = 0;
            const typename T::_tchar * pch = str;
            while (*pch != 0)
            {
                nHash = (nHash << 5) + nHash + (*pch);
                pch++;
            }

            return(nHash);
        }

        static bool __cdecl CompareElements(INARGTYPE str1, INARGTYPE str2)
        {
            return(T::_tchar_traits::StrCmp(str1, str2) == 0);
        }

        static int __cdecl CompareElementsOrdered(INARGTYPE str1, INARGTYPE str2)
        {
            return(T::_tchar_traits::StrCmp(str1, str2));
        }
    };

    template< typename T >
    class CElementTraits;

    template<>
    class CElementTraits< SStringA > :
        public SStringElementTraits< SStringA >
    {
    };

    template<>
    class CElementTraits< SStringW > :
        public SStringElementTraits< SStringW >
    {
    };


}//end of namespace


#endif    //    __TSTRING_H__

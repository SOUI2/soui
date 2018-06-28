//////////////////////////////////////////////////////////////////////////
//   File Name: sresprovider.h
// Description: Resource Provider
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "interface/SResProvider-i.h"
#include <helper/SResID.h>
#include <unknown/obj-ref-impl.hpp>

namespace SOUI
{
    //定义3种系统资源类型
    extern const  TCHAR KTypeBitmap[];
    extern const  TCHAR KTypeCursor[];
    extern const  TCHAR KTypeIcon[];  
    extern const  TCHAR KTypeHtml[];

    class SOUI_EXP SResLoadFromMemory
    {
    public:
        static IBitmap * LoadImage(LPVOID pBuf,size_t size);
        static IImgX   * LoadImgX(LPVOID pBuf,size_t size);
    };
    
    class SResProviderPE:public TObjRefImpl<IResProvider>
    {
    public:
        SResProviderPE();
        BOOL Init(WPARAM wParam,LPARAM lParam);
        HBITMAP    LoadBitmap(LPCTSTR pszResName);
        HICON   LoadIcon(LPCTSTR pszResName,int cx=0,int cy=0);
        HCURSOR   LoadCursor(LPCTSTR pszResName);
        IBitmap * LoadImage(LPCTSTR strType,LPCTSTR pszResName);
        IImgX   * LoadImgX(LPCTSTR strType,LPCTSTR pszResName);
        BOOL GetRawBuffer(LPCTSTR strType,LPCTSTR pszResName,LPVOID pBuf,size_t size);
        BOOL HasResource(LPCTSTR strType,LPCTSTR pszResName);
        size_t GetRawBufferSize(LPCTSTR strType,LPCTSTR pszResName);
        LPVOID GetRawBufferPtr(LPCTSTR strType,LPCTSTR pszResName);
        
#ifdef _DEBUG
        virtual void CheckResUsage(const SMap<SStringT,int> & mapResUsage);
#endif
        
    protected:
        HRSRC MyFindResource(LPCTSTR strType, LPCTSTR pszResName );


        HINSTANCE m_hResInst;
    };


    class SOUI_EXP SResLoadFromFile
    {
    public:
        static HBITMAP LoadBitmap(LPCTSTR pszFileName);
        static HICON   LoadIcon(LPCTSTR pszFileName,int cx=0,int cy=0);
        static HCURSOR LoadCursor(LPCTSTR pszFileName);
        static IBitmap * LoadImage(LPCTSTR pszFileName);
        static IImgX   * LoadImgX(LPCTSTR pszFileName);
        static size_t GetRawBufferSize(LPCTSTR pszFileName);
        static BOOL GetRawBuffer(LPCTSTR pszFileName,LPVOID pBuf,size_t size);
    };

    class SResProviderFiles:public TObjRefImpl<IResProvider>
    {
    public:

        SResProviderFiles();

        BOOL Init(WPARAM wParam,LPARAM lParam);

        BOOL HasResource(LPCTSTR strType,LPCTSTR pszResName);
        HBITMAP    LoadBitmap(LPCTSTR pszResName);
        HICON   LoadIcon(LPCTSTR pszResName,int cx=0,int cy=0);
        HCURSOR LoadCursor(LPCTSTR pszResName);
        IBitmap * LoadImage(LPCTSTR strType,LPCTSTR pszResName);
        IImgX   * LoadImgX(LPCTSTR strType,LPCTSTR pszResName);
        size_t GetRawBufferSize(LPCTSTR strType,LPCTSTR pszResName);
        BOOL GetRawBuffer(LPCTSTR strType,LPCTSTR pszResName,LPVOID pBuf,size_t size);
        
#ifdef _DEBUG
        void CheckResUsage(const SMap<SStringT,int> & mapResUsage);
#endif
    protected:
        SStringT GetRes( LPCTSTR strType,LPCTSTR pszResName );


        SStringT m_strPath;
        SMap<SResID,SStringT> m_mapFiles;
    };

    BOOL SOUI_EXP CreateResProvider(BUILTIN_RESTYPE resType,IObjRef **pObj);

}//namespace SOUI
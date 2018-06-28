//////////////////////////////////////////////////////////////////////////
//   File Name: sresprovider.cpp
// Description: Resource Provider
//////////////////////////////////////////////////////////////////////////
#include "souistd.h"
#include "res.mgr/Sresprovider.h"
#include "helper/mybuffer.h"
#include <io.h>

namespace SOUI
{
    //定义3种系统资源类型
    const TCHAR KTypeBitmap[] = _T("BITMAP");
    const TCHAR KTypeCursor[] = _T("CURSOR");
    const TCHAR KTypeIcon[]   = _T("ICON");
    const  TCHAR KTypeHtml[] =_T("HTML");



    //////////////////////////////////////////////////////////////////////////
    IBitmap * SResLoadFromMemory::LoadImage(LPVOID pBuf,size_t size)
    {
        IBitmap * pImg=NULL;
        GETRENDERFACTORY->CreateBitmap(&pImg);
        if(!pImg) return NULL;
        HRESULT hr=pImg->LoadFromMemory((LPBYTE)pBuf,size);

        if(!SUCCEEDED(hr))
        {
            pImg->Release();
            pImg=NULL;
        }

        return pImg;
    }

    IImgX * SResLoadFromMemory::LoadImgX(LPVOID pBuf,size_t size)
    {
        IImgX *pImgX=NULL;
        GETRENDERFACTORY->GetImgDecoderFactory()->CreateImgX(&pImgX);
        if(!pImgX) return NULL;
        if(0==pImgX->LoadFromMemory(pBuf,size))
        {
            pImgX->Release();
            pImgX=NULL;
        }
        return pImgX;
    }
    
    //////////////////////////////////////////////////////////////////////////
    SResProviderPE::SResProviderPE()
        : m_hResInst(0)
    {

    }


    BOOL SResProviderPE::Init( WPARAM wParam,LPARAM lParam )
    {
        m_hResInst = (HINSTANCE)wParam;
        return TRUE;
    }


    HBITMAP SResProviderPE::LoadBitmap(LPCTSTR pszResName )
    {
        return ::LoadBitmap(m_hResInst,pszResName);
    }

    HICON SResProviderPE::LoadIcon(LPCTSTR pszResName ,int cx/*=0*/,int cy/*=0*/)
    {
        return (HICON)::LoadImage(m_hResInst, pszResName, IMAGE_ICON, cx, cy, LR_DEFAULTCOLOR);
    }

    HCURSOR SResProviderPE::LoadCursor(LPCTSTR pszResName )
    {
        HRSRC hRes=::FindResource(m_hResInst,pszResName,RT_ANICURSOR);
        if(hRes)
        {
            DWORD dwSize=SizeofResource(m_hResInst, hRes);
            HGLOBAL hGlob=LoadResource(m_hResInst, hRes);
            LPBYTE pBytes=(LPBYTE)LockResource(hGlob); 
            return  (HCURSOR)CreateIconFromResource(pBytes,dwSize,FALSE,0x00030000);
        }else
        {
            return ::LoadCursor(m_hResInst,pszResName);    
        }    
    }

    IBitmap * SResProviderPE::LoadImage( LPCTSTR strType,LPCTSTR pszResName )
    {
        if(!HasResource(strType,pszResName)) return NULL;
        size_t szImgBuf= GetRawBufferSize(strType,pszResName);
        if(szImgBuf==0) return NULL;
        LPVOID pBuf=GetRawBufferPtr(strType,pszResName);
        return SResLoadFromMemory::LoadImage(pBuf,szImgBuf);
    }

    IImgX   * SResProviderPE::LoadImgX( LPCTSTR strType,LPCTSTR pszResName )
    {
        if(!HasResource(strType,pszResName)) return NULL;
        size_t szImgBuf= GetRawBufferSize(strType,pszResName);
        if(szImgBuf==0) return NULL;
        LPVOID pBuf=GetRawBufferPtr(strType,pszResName);
        return SResLoadFromMemory::LoadImgX(pBuf,szImgBuf);
    }

    size_t SResProviderPE::GetRawBufferSize( LPCTSTR strType,LPCTSTR pszResName )
    {
        HRSRC hRsrc = MyFindResource(strType,pszResName);

        if (NULL == hRsrc)
            return 0;

        return ::SizeofResource(m_hResInst, hRsrc);
    }

    BOOL SResProviderPE::GetRawBuffer( LPCTSTR strType,LPCTSTR pszResName,LPVOID pBuf,size_t size )
    {
        SASSERT(strType);
        HRSRC hRsrc = MyFindResource(strType,pszResName);

        if (NULL == hRsrc)
            return FALSE;

        size_t dwSize = ::SizeofResource(m_hResInst, hRsrc);
        if (0 == dwSize)
            return FALSE;

        if(size < dwSize)
        {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }
        HGLOBAL hGlobal = ::LoadResource(m_hResInst, hRsrc);
        if (NULL == hGlobal)
            return FALSE;

        LPVOID pBuffer = ::LockResource(hGlobal);
        if (NULL == pBuffer)
            return FALSE;

        memcpy(pBuf,pBuffer,dwSize);

        ::FreeResource(hGlobal);

        return TRUE;
    }


    LPVOID SResProviderPE::GetRawBufferPtr( LPCTSTR strType,LPCTSTR pszResName )
    {
        SASSERT(strType);
        HRSRC hRsrc = MyFindResource(strType,pszResName);

        if (NULL == hRsrc)
            return NULL;

        size_t dwSize = ::SizeofResource(m_hResInst, hRsrc);
        if (0 == dwSize)
            return NULL;

        HGLOBAL hGlobal = ::LoadResource(m_hResInst, hRsrc);
        if (NULL == hGlobal)
            return NULL;

        LPVOID pBuffer = ::LockResource(hGlobal);

        ::FreeResource(hGlobal);

        return pBuffer;
    }

    BOOL SResProviderPE::HasResource( LPCTSTR strType,LPCTSTR pszResName )
    {
        SASSERT(strType);
        return MyFindResource(strType,pszResName)!=NULL;
    }

    HRSRC SResProviderPE::MyFindResource( LPCTSTR strType, LPCTSTR pszResName )
    {
        if(_tcsicmp(strType,KTypeBitmap)==0) strType=RT_BITMAP;
        else if(_tcsicmp(strType,KTypeIcon)==0) strType=RT_GROUP_ICON;
        else if(_tcsicmp(strType,KTypeCursor)==0) strType=RT_GROUP_CURSOR;
        else if(_tcsicmp(strType,KTypeHtml) == 0) strType=RT_HTML;

        HRSRC hRet = ::FindResource(m_hResInst, pszResName, strType);
        if(!hRet && strType==RT_GROUP_CURSOR)
            hRet = ::FindResource(m_hResInst,pszResName,RT_ANICURSOR);
        return hRet;
    }

#ifdef _DEBUG
    static BOOL CALLBACK EnumResNameProc(          HMODULE hModule,
        LPCTSTR lpszType,
        LPTSTR lpszName,
        LONG_PTR lParam
        )
        {
            const SMap<SStringT,int> * pMapResUsage = (const SMap<SStringT,int> *)lParam;
            if(IS_INTRESOURCE(lpszName)) return TRUE;
            
            SStringT key = SStringT().Format(_T("%s:%s"),lpszType,lpszName);
            key.MakeLower();
            if(!pMapResUsage->Lookup(key))
            {//发现未使用资源
                STRACE(_T("resource of [%s] was not used."),key);
            }
            return TRUE;
        }

    static BOOL CALLBACK EnumResTypeProc(HMODULE hModule,
        LPTSTR lpszType,
        LONG_PTR lParam
        )
        {
            if(IS_INTRESOURCE(lpszType)) return TRUE;

            EnumResourceNames(hModule,lpszType,EnumResNameProc,lParam);
            return TRUE;
        }

    void SResProviderPE::CheckResUsage(const SMap<SStringT,int> & mapResUsage)
    {
        STRACEW(L"####Detecting File Resource Usage BEGIN");    
        EnumResourceTypes(m_hResInst,EnumResTypeProc,(LONG_PTR)&mapResUsage);
        STRACEW(L"!!!!Detecting Resource Usage END");    
    }
#endif

    //////////////////////////////////////////////////////////////////////////
    //

    HBITMAP SResLoadFromFile::LoadBitmap(LPCTSTR strPath )
    {
        return (HBITMAP)::LoadImage(NULL, strPath, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
    }

    HICON SResLoadFromFile::LoadIcon(LPCTSTR strPath ,int cx/*=0*/,int cy/*=0*/)
    {
        return (HICON)::LoadImage(NULL, strPath, IMAGE_ICON, cx, cy, LR_LOADFROMFILE);
    }

    HCURSOR SResLoadFromFile::LoadCursor(LPCTSTR strPath )
    {
        return (HCURSOR)::LoadImage(NULL, strPath, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
    }

    IBitmap * SResLoadFromFile::LoadImage(LPCTSTR strPath )
    {
        IBitmap * pImg=NULL;
        GETRENDERFACTORY->CreateBitmap(&pImg);

        HRESULT hr=pImg->LoadFromFile(strPath);
        if(!SUCCEEDED(hr))
        {
            pImg->Release();
            pImg=NULL;
        }
        return pImg;

    }

    IImgX   * SResLoadFromFile::LoadImgX(LPCTSTR strPath )
    {
        IImgX *pImgX=NULL;
        GETRENDERFACTORY->GetImgDecoderFactory()->CreateImgX(&pImgX);
        if(!pImgX) return NULL;
        if(0==pImgX->LoadFromFile(strPath))
        {
            pImgX->Release();
            pImgX=NULL;
        }
        return pImgX;
    }

    size_t SResLoadFromFile::GetRawBufferSize(LPCTSTR strPath )
    {
        WIN32_FIND_DATA wfd;
        HANDLE hf=FindFirstFile(strPath,&wfd);
        if(INVALID_HANDLE_VALUE==hf) return 0;
        FindClose(hf);
        return wfd.nFileSizeLow;
    }

    BOOL SResLoadFromFile::GetRawBuffer(LPCTSTR strPath,LPVOID pBuf,size_t size )
    {
        FILE *f=_tfopen(strPath,_T("rb"));
        if(!f) return FALSE;
        size_t len=_filelength(_fileno(f));
        if(len>size)
        {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            fclose(f);
            return FALSE;
        }
        BOOL bRet=(len==fread(pBuf,1,len,f));

        fclose(f);
        return bRet;
    }

    //////////////////////////////////////////////////////////////////////////
    // SResProviderFiles

    SResProviderFiles::SResProviderFiles()
    {
    }

    SStringT SResProviderFiles::GetRes( LPCTSTR strType,LPCTSTR pszResName )
    {
        SResID resID(strType,pszResName);
        SMap<SResID,SStringT>::CPair *p=m_mapFiles.Lookup(resID);
        if(!p) return _T("");
        SStringT strRet=m_strPath+_T("\\")+p->m_value;
        return strRet;
    }

    HBITMAP SResProviderFiles::LoadBitmap(LPCTSTR pszResName )
    {
        SStringT strPath=GetRes(KTypeBitmap,pszResName);
        if(strPath.IsEmpty()) return NULL;
        return SResLoadFromFile::LoadBitmap(strPath);
    }

    HICON SResProviderFiles::LoadIcon(LPCTSTR pszResName ,int cx/*=0*/,int cy/*=0*/)
    {
        SStringT strPath=GetRes(KTypeIcon,pszResName);
        if(strPath.IsEmpty()) return NULL;
        return SResLoadFromFile::LoadIcon(strPath,cx,cy);
    }

    HCURSOR SResProviderFiles::LoadCursor(LPCTSTR pszResName )
    {
        SStringT strPath=GetRes(KTypeCursor,pszResName);
        if(strPath.IsEmpty()) return NULL;
        return SResLoadFromFile::LoadCursor(strPath);
    }

    IBitmap * SResProviderFiles::LoadImage( LPCTSTR strType,LPCTSTR pszResName )
    {
        SStringT strPath=GetRes(strType,pszResName);
        if(strPath.IsEmpty()) return NULL;
        return SResLoadFromFile::LoadImage(strPath);    
    }

    IImgX   * SResProviderFiles::LoadImgX( LPCTSTR strType,LPCTSTR pszResName )
    {
        SStringT strPath=GetRes(strType,pszResName);
        if(strPath.IsEmpty()) return NULL;
        return SResLoadFromFile::LoadImgX(strPath);
    }

    size_t SResProviderFiles::GetRawBufferSize( LPCTSTR strType,LPCTSTR pszResName )
    {
        SStringT strPath=GetRes(strType,pszResName);
        if(strPath.IsEmpty()) return 0;
        return SResLoadFromFile::GetRawBufferSize(strPath);
    }

    BOOL SResProviderFiles::GetRawBuffer( LPCTSTR strType,LPCTSTR pszResName,LPVOID pBuf,size_t size )
    {
        SStringT strPath=GetRes(strType,pszResName);
        if(strPath.IsEmpty()) return FALSE;
        return SResLoadFromFile::GetRawBuffer(strPath,pBuf,size);
    }


    BOOL SResProviderFiles::Init( WPARAM wParam,LPARAM lParam )
    {
        LPCTSTR pszPath = (LPCTSTR)wParam;

        SStringT strPathIndex=pszPath;
        strPathIndex+=_T("\\");
        strPathIndex+=UIRES_INDEX;

        pugi::xml_document xmlDoc;
        SStringT strFileName;
        if(!xmlDoc.load_file(strPathIndex,pugi::parse_default,pugi::encoding_utf8)) return FALSE;

        pugi::xml_node xmlResource=xmlDoc.child(L"resource");
        if(!xmlResource) return FALSE;
        pugi::xml_node xmlType=xmlResource.first_child();
        while(xmlType)
        {
            SStringT strType=S_CW2T(xmlType.name());
            pugi::xml_node xmlFile=xmlType.child(L"file");
            while(xmlFile)
            {
                SResID id(strType,S_CW2T(xmlFile.attribute(L"name").value()));
                SStringT strFile=S_CW2T(xmlFile.attribute(L"path").value());
                //再次Init时会因为此行代码导致资源无法加载
				//if(!m_strPath.IsEmpty()) strFile.Format(_T("%s\\%s"),(LPCTSTR)m_strPath,(LPCTSTR)strFile);
                m_mapFiles[id]=strFile;
                xmlFile=xmlFile.next_sibling(L"file");
            }
            xmlType=xmlType.next_sibling();
        }

        TCHAR szFullPath[1025];
        GetFullPathName(pszPath,1024,szFullPath,NULL);
        m_strPath=szFullPath;
        return TRUE;
    }

    BOOL SResProviderFiles::HasResource( LPCTSTR strType,LPCTSTR pszResName )
    {
        SResID resID(strType,pszResName);
        SMap<SResID,SStringT>::CPair *p=m_mapFiles.Lookup(resID);
        return (p!=NULL);
    }
    
#ifdef _DEBUG
    void SResProviderFiles::CheckResUsage(const SMap<SStringT,int> & mapResUsage)
    {
        STRACEW(L"####Detecting File Resource Usage BEGIN");    
        SPOSITION pos = m_mapFiles.GetStartPosition();
        while(pos)
        {
            const SResID & resid = m_mapFiles.GetNext(pos)->m_key;
            SStringT key = SStringT().Format(_T("%s:%s"),resid.szType,resid.szName);
            key.MakeLower();
            if(!mapResUsage.Lookup(key))
            {//发现未使用资源
                STRACE(_T("resource of [%s] was not used."),key);
            }
        }
        STRACEW(L"!!!!Detecting Resource Usage END");    
    }
#endif

    BOOL CreateResProvider( BUILTIN_RESTYPE resType,IObjRef **pObj )
    {
        *pObj = NULL;
        switch(resType)
        {
        case RES_PE:*pObj = new SResProviderPE;break;
        case RES_FILE: *pObj = new SResProviderFiles;break;
        default:break;
        }
        return *pObj != NULL;
    }


}//namespace SOUI
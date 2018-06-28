#include "stdafx.h"
#pragma warning(disable:4251)

#include "SResProvider7Zip.h"
#include <pugixml/pugixml.hpp>

extern HICON CURSORICON_LoadFromBuf(const BYTE * bits,DWORD filesize,INT width, INT height,BOOL fCursor, UINT loadflags);
extern HICON CURSORICON_LoadFromFile( LPCWSTR filename,
                              INT width, INT height,
                              BOOL fCursor, UINT loadflags);

namespace SOUI{

    SResProvider7Zip::SResProvider7Zip():m_renderFactory(NULL)
    {
    }

    SResProvider7Zip::~SResProvider7Zip(void)
    {
    }

    HBITMAP SResProvider7Zip::LoadBitmap(LPCTSTR pszResName )
    {
        SStringT strPath=_GetFilePath(pszResName,_T("BITMAP"));
        if(strPath.IsEmpty()) return NULL;
        CZipFile zf;
        if(!m_zipFile.GetFile(strPath,zf)) return NULL;

        HDC hDC = GetDC(NULL);
        //读取位图头
        BITMAPFILEHEADER *pBmpFileHeader=(BITMAPFILEHEADER *)zf.GetData(); 
        //检测位图头
        if (pBmpFileHeader->bfType != ((WORD) ('M'<<8)|'B')) 
        {
            return NULL; 
        } 
        //判断位图长度
        if (pBmpFileHeader->bfSize > (UINT)zf.GetSize()) 
        { 
            return NULL; 
        } 
        LPBITMAPINFO lpBitmap=(LPBITMAPINFO)(pBmpFileHeader+1); 
        LPVOID lpBits=(LPBYTE)zf.GetData()+pBmpFileHeader->bfOffBits;
        HBITMAP hBitmap= CreateDIBitmap(hDC,&lpBitmap->bmiHeader,CBM_INIT,lpBits,lpBitmap,DIB_RGB_COLORS);
        ReleaseDC(NULL,hDC);
        
        return hBitmap;
    }

    HICON SResProvider7Zip::LoadIcon(LPCTSTR pszResName ,int cx/*=0*/,int cy/*=0*/)
    {
        SStringT strPath=_GetFilePath(pszResName,_T("ICON"));
        if(strPath.IsEmpty()) return NULL;
        CZipFile zf;
        if(!m_zipFile.GetFile(strPath,zf)) return NULL;

        return CURSORICON_LoadFromBuf(zf.GetData(),zf.GetSize(),cx,cy,FALSE,LR_DEFAULTSIZE|LR_DEFAULTCOLOR);
    }

    HCURSOR SResProvider7Zip::LoadCursor( LPCTSTR pszResName )
    {
        SStringT strPath=_GetFilePath(pszResName,_T("CURSOR"));
        if(strPath.IsEmpty()) return NULL;
        CZipFile zf;
        if(!m_zipFile.GetFile(strPath,zf)) return NULL;
        return (HCURSOR)CURSORICON_LoadFromBuf(zf.GetData(),zf.GetSize(),0,0,TRUE,LR_DEFAULTSIZE|LR_DEFAULTCOLOR);
    }

    IBitmap * SResProvider7Zip::LoadImage( LPCTSTR strType,LPCTSTR pszResName)
    {
        SStringT strPath=_GetFilePath(pszResName,strType);
        if(strPath.IsEmpty()) return NULL;
        CZipFile zf;
        if(!m_zipFile.GetFile(strPath,zf)) return NULL;
        IBitmap * pBmp=NULL;
        m_renderFactory->CreateBitmap(&pBmp);
        if(!pBmp) return NULL;
        pBmp->LoadFromMemory(zf.GetData(),zf.GetSize());
        return pBmp;
    }

    IImgX   * SResProvider7Zip::LoadImgX( LPCTSTR strType,LPCTSTR pszResName )
    {
        SStringT strPath=_GetFilePath(pszResName,strType);
        if(strPath.IsEmpty()) return NULL;
        CZipFile zf;
        if(!m_zipFile.GetFile(strPath,zf)) return NULL;

        IImgX *pImgX=NULL;
        m_renderFactory->GetImgDecoderFactory()->CreateImgX(&pImgX);
        if(!pImgX) return NULL;

        if(0==pImgX->LoadFromMemory(zf.GetData(),zf.GetSize()))
        {
            pImgX->Release();
            pImgX=NULL;
        }
        return pImgX;
    }

    BOOL SResProvider7Zip::_Init( LPCTSTR pszZipFile ,LPCSTR pszPsw)
    {
		if (!m_zipFile.Open(pszZipFile, pszPsw)) return FALSE;
        return _LoadSkin();
    }

    BOOL SResProvider7Zip::_Init( HINSTANCE hInst,LPCTSTR pszResName,LPCTSTR pszType  ,LPCSTR pszPsw)
    {
        if(!m_zipFile.Open(hInst,pszResName,pszType)) return FALSE;
        m_zipFile.SetPassword(pszPsw);
        return _LoadSkin();
    }

    BOOL SResProvider7Zip::Init( WPARAM wParam,LPARAM lParam )
    {
		ZIP7RES_PARAM *zipParam = (ZIP7RES_PARAM*)wParam;
        m_renderFactory = zipParam->pRenderFac;
		m_childDir = zipParam->pszChildDir;
		if (!m_childDir.IsEmpty())
		{
			m_childDir.TrimRight(L'\\');
			m_childDir.TrimRight(L'/');
			m_childDir += L"\\";
		}
		if (zipParam->type == ZIP7RES_PARAM::ZIPFILE)
            return _Init(zipParam->pszZipFile,zipParam->pszPsw);
        else
            return _Init(zipParam->peInfo.hInst,zipParam->peInfo.pszResName,zipParam->peInfo.pszResType,zipParam->pszPsw);
    }

    SStringT SResProvider7Zip::_GetFilePath( LPCTSTR pszResName,LPCTSTR pszType )
    {
        SResID resID(pszType,pszResName);
        SMap<SResID,SStringT>::CPair *p = m_mapFiles.Lookup(resID);
        if(!p) return _T("");
        return p->m_value;
    }

    size_t SResProvider7Zip::GetRawBufferSize( LPCTSTR strType,LPCTSTR pszResName )
    {
        SStringT strPath=_GetFilePath(pszResName,strType);
        if(strPath.IsEmpty()) return FALSE;
        
		return m_zipFile.GetFileSize(strPath);
    }

    BOOL SResProvider7Zip::GetRawBuffer( LPCTSTR strType,LPCTSTR pszResName,LPVOID pBuf,size_t size )
    {
        SStringT strPath=_GetFilePath(pszResName,strType);
        if(strPath.IsEmpty()) return FALSE;
        CZipFile zf;
        if(!m_zipFile.GetFile(strPath,zf)) return NULL;
        if(size<zf.GetSize())
        {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return FALSE;
        }
        memcpy(pBuf,zf.GetData(),zf.GetSize());
        return TRUE;
    }

    BOOL SResProvider7Zip::HasResource( LPCTSTR strType,LPCTSTR pszResName )
    {
        SResID resID(strType,pszResName);
        SMap<SResID,SStringT>::CPair *p = m_mapFiles.Lookup(resID);
        return p!=NULL;
    }

    BOOL SResProvider7Zip::_LoadSkin()
    {
        CZipFile zf;
        BOOL bIdx=m_zipFile.GetFile(m_childDir + UIRES_INDEX,zf);
        if(!bIdx) return FALSE;

        pugi::xml_document xmlDoc;
        SStringA strFileName;
        if(!xmlDoc.load_buffer_inplace(zf.GetData(),zf.GetSize(),pugi::parse_default,pugi::encoding_utf8)) return FALSE;
        pugi::xml_node xmlElem=xmlDoc.child(L"resource");
        if(!xmlElem) return FALSE;
        pugi::xml_node resType=xmlElem.first_child();
        while(resType)
        {
            pugi::xml_node resFile=resType.first_child();
            while(resFile)
            {
                SResID id(S_CW2T(resType.name()),S_CW2T(resFile.attribute(L"name").value()));
                m_mapFiles[id] = m_childDir + S_CW2T(resFile.attribute(L"path").value());
                resFile=resFile.next_sibling();
            }
            resType = resType.next_sibling();
        }
        return TRUE;
    }

    namespace RESPROVIDER_7ZIP
    {
        BOOL SCreateInstance( IObjRef ** ppObj )
        {
            *ppObj = new SResProvider7Zip;
            return TRUE;
        }
    }

}//namespace SOUI
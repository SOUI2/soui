#include "souistd.h"
#include "res.mgr/SResProviderMgr.h"
#include "res.mgr/SResProvider.h"


#include "helper/mybuffer.h"
#include "helper/SplitString.h"

namespace SOUI
{
    const static TCHAR KTypeFile[]      = _T("file");  //从文件加载资源时指定的类型


    SResProviderMgr::SResProviderMgr()
    {
    }

    SResProviderMgr::~SResProviderMgr(void)
    {
        RemoveAll();
    }

    void SResProviderMgr::RemoveAll()
    {
        SAutoLock lock(m_cs);
        SPOSITION pos=m_lstResPackage.GetHeadPosition();
        while(pos)
        {
            IResProvider *pResProvider=m_lstResPackage.GetNext(pos);
#ifdef _DEBUG//检查资源使用情况
            pResProvider->CheckResUsage(m_mapResUsageCount);
#endif            
            pResProvider->Release();
        }
        m_lstResPackage.RemoveAll();
        
        pos = m_mapCachedCursor.GetStartPosition();
        while(pos)
        {
            CURSORMAP::CPair *pPair=m_mapCachedCursor.GetNext(pos);
            DestroyCursor(pPair->m_value);
        }
        m_mapCachedCursor.RemoveAll();
    }


    IResProvider * SResProviderMgr::GetMatchResProvider( LPCTSTR pszType,LPCTSTR pszResName )
    {
        if(!pszType) return NULL;

        SAutoLock lock(m_cs);
        SPOSITION pos=m_lstResPackage.GetTailPosition();
        while(pos)
        {
            IResProvider * pResProvider = m_lstResPackage.GetPrev(pos);
            if(pResProvider->HasResource(pszType,pszResName)) 
            {
#ifdef _DEBUG
                m_mapResUsageCount[SStringT().Format(_T("%s:%s"),pszType,pszResName).MakeLower()] ++;
#endif
                return pResProvider;
            }
        }
        return NULL;
    }
   
    void SResProviderMgr::AddResProvider( IResProvider * pResProvider ,LPCTSTR pszUidef)
    {
        SAutoLock lock(m_cs);
        m_lstResPackage.AddTail(pResProvider);
		pResProvider->AddRef();
		if(pszUidef) 
		{
			IUiDefInfo * pUiDef = SUiDef::getSingleton().CreateUiDefInfo(pResProvider,pszUidef);
			SUiDef::getSingleton().SetUiDef(pUiDef);
			pUiDef->Release();
		}
    }

    void SResProviderMgr::RemoveResProvider( IResProvider * pResProvider )
    {
        SAutoLock lock(m_cs);
        SPOSITION pos = m_lstResPackage.GetTailPosition();
        while(pos)
        {
            SPOSITION posPrev=pos;
            IResProvider *pResProvierT = m_lstResPackage.GetPrev(pos);
            if(pResProvierT == pResProvider)
            {
                m_lstResPackage.RemoveAt(posPrev);
                pResProvierT->Release();
                break;
            }
        }
    }

	IResProvider * SResProviderMgr::GetTailResProvider() const
	{
		return m_lstResPackage.GetTail();
	}

    LPCTSTR SResProviderMgr::SysCursorName2ID( LPCTSTR pszCursorName )
    {
		struct Cursor2ID
		{
			LPCTSTR szName;
			LPCTSTR szID;
		};
		static const Cursor2ID cursorMap[] = {
			{ _T("arrow") ,IDC_ARROW },
			{ _T("ibeam") ,IDC_IBEAM },
			{ _T("wait") ,IDC_WAIT },
			{ _T("cross") ,IDC_CROSS },
			{ _T("uparrow") ,IDC_UPARROW },
			{ _T("size") ,IDC_SIZE },
			{ _T("sizenwse") ,IDC_SIZENWSE },
			{ _T("sizenesw") ,IDC_SIZENESW },
			{ _T("sizewe") ,IDC_SIZEWE },
			{ _T("sizens") ,IDC_SIZENS },
			{ _T("sizeall") ,IDC_SIZEALL },
			{ _T("no") ,IDC_NO },
			{ _T("hand") ,IDC_HAND },
			{ _T("help") ,IDC_HELP },
		};
        SAutoLock lock(m_cs);
		for (int i = 0; i < ARRAYSIZE(cursorMap); i++)
		{
			if (0==_tcsicmp(cursorMap[i].szName, _T("arrow")))
				return cursorMap[i].szID;
		}
        return NULL;
   }

    BOOL SResProviderMgr::GetRawBuffer( LPCTSTR strType,LPCTSTR pszResName,LPVOID pBuf,size_t size )
    {
        SAutoLock lock(m_cs);
        if(IsFileType(strType))
        {
            return SResLoadFromFile::GetRawBuffer(pszResName,pBuf,size);
        }else
        {
#ifdef _DEBUG
            m_mapResUsageCount[SStringT().Format(_T("%s:%s"),strType,pszResName).MakeLower()] ++;
#endif
            IResProvider *pResProvider=GetMatchResProvider(strType,pszResName);
            if(!pResProvider) return FALSE;
            return pResProvider->GetRawBuffer(strType,pszResName,pBuf,size);
        }
    }

    size_t SResProviderMgr::GetRawBufferSize( LPCTSTR strType,LPCTSTR pszResName )
    {
        SAutoLock lock(m_cs);
        if(IsFileType(strType))
        {
            return SResLoadFromFile::GetRawBufferSize(pszResName);
        }else
        {
#ifdef _DEBUG
            m_mapResUsageCount[SStringT().Format(_T("%s:%s"),strType,pszResName).MakeLower()] ++;
#endif

            IResProvider *pResProvider=GetMatchResProvider(strType,pszResName);
            if(!pResProvider) return 0;
            return pResProvider->GetRawBufferSize(strType,pszResName);
        }
    }

    IImgX * SResProviderMgr::LoadImgX( LPCTSTR strType,LPCTSTR pszResName )
    {
        SAutoLock lock(m_cs);
        if(IsFileType(strType))
        {
            return SResLoadFromFile::LoadImgX(pszResName);
        }else
        {
#ifdef _DEBUG
            m_mapResUsageCount[SStringT().Format(_T("%s:%s"),strType,pszResName).MakeLower()] ++;
#endif

            IResProvider *pResProvider=GetMatchResProvider(strType,pszResName);
            if(!pResProvider) return NULL;
            return pResProvider->LoadImgX(strType,pszResName);
        }
    }

    IBitmap * SResProviderMgr::LoadImage( LPCTSTR pszType,LPCTSTR pszResName )
    {
        if(!pszType) return NULL;
        SAutoLock lock(m_cs);
        if(IsFileType(pszType))
        {
            return SResLoadFromFile::LoadImage(pszResName);
        }else
        {
#ifdef _DEBUG
            m_mapResUsageCount[SStringT().Format(_T("%s:%s"),pszType,pszResName).MakeLower()] ++;
#endif

            IResProvider *pResProvider=GetMatchResProvider(pszType,pszResName);
            if(!pResProvider) return NULL;
            return pResProvider->LoadImage(pszType,pszResName);
        }
    }

    HBITMAP SResProviderMgr::LoadBitmap( LPCTSTR pszResName ,BOOL bFromFile /*= FALSE*/)
    {
        SAutoLock lock(m_cs);
        if(bFromFile)
        {
            return SResLoadFromFile::LoadBitmap(pszResName);
        }else
        {
#ifdef _DEBUG
            m_mapResUsageCount[SStringT().Format(_T("bitmap:%s"),pszResName).MakeLower()] ++;
#endif

            IResProvider *pResProvider=GetMatchResProvider(KTypeBitmap,pszResName);
            if(!pResProvider) return NULL;
            return pResProvider->LoadBitmap(pszResName);
        }
    }

    HCURSOR SResProviderMgr::LoadCursor( LPCTSTR pszResName ,BOOL bFromFile /*= FALSE*/)
    {
        SAutoLock lock(m_cs);
        if(IS_INTRESOURCE(pszResName))
            return ::LoadCursor(NULL, pszResName);
        else 
        {
            LPCTSTR pszCursorID=SysCursorName2ID(pszResName);
            if(pszCursorID)
                return ::LoadCursor(NULL, pszCursorID);
        }
        const CURSORMAP::CPair * pPair  = m_mapCachedCursor.Lookup(pszResName);
        if(pPair) return pPair->m_value;
        
        HCURSOR hRet = NULL;
        if(bFromFile)
        {
            hRet = SResLoadFromFile::LoadCursor(pszResName);
        }else
        {
        
#ifdef _DEBUG
            m_mapResUsageCount[SStringT().Format(_T("cursor:%s"),pszResName).MakeLower()] ++;
#endif

            IResProvider *pResProvider=GetMatchResProvider(KTypeCursor,pszResName);
            if(pResProvider)
                hRet =pResProvider->LoadCursor(pszResName);
        }
        if(hRet)
        {
            m_mapCachedCursor[pszResName]=hRet;
        }
        return hRet;
    }

    HICON SResProviderMgr::LoadIcon( LPCTSTR pszResName,int cx/*=0*/,int cy/*=0*/ ,BOOL bFromFile /*= FALSE*/)
    {
        SAutoLock lock(m_cs);
        if(bFromFile)
        {
            return SResLoadFromFile::LoadIcon(pszResName,cx,cy);
        }else
        {
#ifdef _DEBUG
            m_mapResUsageCount[SStringT().Format(_T("icon:%s"),pszResName).MakeLower()] ++;
#endif
            IResProvider *pResProvider=GetMatchResProvider(KTypeIcon,pszResName);
            if(!pResProvider) return NULL;
            return pResProvider->LoadIcon(pszResName,cx,cy);
        }
    }

    BOOL SResProviderMgr::HasResource( LPCTSTR pszType,LPCTSTR pszResName )
    {
        SAutoLock lock(m_cs);
        if(IsFileType(pszType))
        {
            return ::GetFileAttributes(pszResName) != INVALID_FILE_ATTRIBUTES;
        }else
        {
            return NULL != GetMatchResProvider(pszType,pszResName);
        }
    }

    IBitmap * SResProviderMgr::LoadImage2(const SStringW & strImgID)
    {
        SStringT strImgID2 = S_CW2T(strImgID);
        SStringTList strLst;
        int nSegs = ParseResID(strImgID2,strLst);
        if(nSegs == 2) return LoadImage(strLst[0],strLst[1]);
        else return NULL;
    }

    HICON SResProviderMgr::LoadIcon2(const SStringW & strIconID)
    {
        SStringT strIconID2 = S_CW2T(strIconID);
        SStringTList strLst;
        int nSegs = ParseResID(strIconID2,strLst);
        if(nSegs == 2)
        {
            int cx = _ttoi(strLst[1]);
            return LoadIcon(strLst[0],cx,cx);
        }
        else 
        {
            return LoadIcon(strLst[0]);
        }
    }

    BOOL SResProviderMgr::IsFileType( LPCTSTR pszType )
    {
        if(!pszType) return FALSE;
        return _tcsicmp(pszType,KTypeFile) == 0;
    }

	static SNamedColor emptyColor;
	static SNamedString emptyString;
	static SNamedDimension emptyDim;

    COLORREF SResProviderMgr::GetColor(const SStringW & strColor)
    {
		if(SUiDef::getSingleton().GetUiDef() == NULL)
			return emptyColor.Get(strColor);
		return SUiDef::getSingleton().GetUiDef()->GetNamedColor().Get(strColor);
    }

    COLORREF SResProviderMgr::GetColor(int idx)
    {
		if(SUiDef::getSingleton().GetUiDef() == NULL)
			return emptyColor.Get(idx);

		return SUiDef::getSingleton().GetUiDef()->GetNamedColor().Get(idx);
	}


    SStringW SResProviderMgr::GetString(const SStringW & strString)
    {
		if(SUiDef::getSingleton().GetUiDef() == NULL)
			return emptyString.Get(strString);

		return SUiDef::getSingleton().GetUiDef()->GetNamedString().Get(strString);
    }

    SStringW SResProviderMgr::GetString(int idx)
    {
		if(SUiDef::getSingleton().GetUiDef() == NULL)
			return emptyString.Get(idx);

		return SUiDef::getSingleton().GetUiDef()->GetNamedString().Get(idx);
    }

	SLayoutSize SResProviderMgr::GetLayoutSize(const SStringW & strSize)
	{
		if (SUiDef::getSingleton().GetUiDef() == NULL)
			return emptyDim.Get(strSize);

		return SUiDef::getSingleton().GetUiDef()->GetNamedDimension().Get(strSize);
	}

	SLayoutSize SResProviderMgr::GetLayoutSize(int idx)
	{
		if (SUiDef::getSingleton().GetUiDef() == NULL)
			return emptyDim.Get(idx);

		return SUiDef::getSingleton().GetUiDef()->GetNamedDimension().Get(idx);
	}
}

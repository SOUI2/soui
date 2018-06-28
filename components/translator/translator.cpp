// translator.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "translator.h"
#include <search.h>
#include <ObjBase.h>
#include <tchar.h>

using namespace pugi;

namespace SOUI
{
    int StringCmp(const SStringW &str1,const SStringW &str2)
    {
        if(str1 == str2) return 0;
        else return str1<str2?-1:1;
    }

    class SStrMap
    {
        friend class STranslatorMgr;
    public:
        SStringW strSource;
        SStringW strTranslation;

        static int  Compare(const void * e1, const void * e2);
        static int  CompareInSearch(const void * e1, const void * e2);
    };

    class SStrMapEntry
    {
        friend class STranslatorMgr;
    public:
        ~SStrMapEntry();
        SStringW strCtx;
        SArray<SStrMap*> m_arrStrMap;
        static int  Compare(const void * e1, const void * e2);
        static int  CompareInSearch(const void * e1, const void * e2);
    };


    int SStrMap::Compare( const void * e1, const void * e2)
    {
        SStrMap **p1=(SStrMap**) e1;
        SStrMap **p2=(SStrMap**) e2;
        return StringCmp((*p1)->strSource,(*p2)->strSource);
    }

    int SStrMap::CompareInSearch( const void * e1, const void * e2 )
    {
        SStringW * pKey=(SStringW *)e1;
        SStrMap **p2=(SStrMap**) e2;
        return StringCmp(*pKey,(*p2)->strSource);     
    }


    int SStrMapEntry::Compare( const void * e1, const void * e2 )
    {
        SStrMapEntry **p1=(SStrMapEntry**) e1;
        SStrMapEntry **p2=(SStrMapEntry**) e2;
        return StringCmp((*p1)->strCtx,(*p2)->strCtx);
    }
    
    int SStrMapEntry::CompareInSearch( const void * e1, const void * e2 )
    {
        SStringW *pKey=(SStringW*) e1;
        SStrMapEntry **p2=(SStrMapEntry**) e2;
        return StringCmp(*pKey,(*p2)->strCtx);
    }

    SStrMapEntry::~SStrMapEntry()
    {
        for(UINT i=0;i<m_arrStrMap.GetCount();i++)
            delete m_arrStrMap.GetAt(i);
    }


    //////////////////////////////////////////////////////////////////////////
    // SLang
    STranslator::STranslator()
    {
        m_arrEntry = new SArray<SStrMapEntry*>;
    }

    STranslator::~STranslator()
    {
        for(UINT i=0;i<m_arrEntry->GetCount();i++)
            delete m_arrEntry->GetAt(i);
        delete m_arrEntry;
    }

    SStringW STranslator::name()
    {
        return m_strLang;
    }

    GUID STranslator::guid()
    {
        return m_guid;
    }

    BOOL STranslator::Load( LPVOID pData,UINT uType )
    {
        switch(uType)
        {
        case LD_XML:
            return LoadFromXml((*(pugi::xml_node*)pData));
        }
        return FALSE;
    }
    
    BOOL STranslator::LoadFromXml( pugi::xml_node xmlLang )
    {
        m_strLang=xmlLang.attribute(L"name").value();
        
        OLECHAR szIID[100] = { 0 };
        wcscpy(szIID,xmlLang.attribute(L"guid").value());

        IIDFromString(szIID,&m_guid);
        
        int ctxCount=0;
        xml_node nodeCtx=xmlLang.child(L"context");
        while(nodeCtx)
        {
            ctxCount++;
            nodeCtx=nodeCtx.next_sibling(L"context");
        }
        m_arrEntry->SetCount(ctxCount);
        nodeCtx=xmlLang.child(L"context");
        for(int i=0;i<ctxCount;i++)
        {
            SASSERT(nodeCtx);
            int strCount=0;
            xml_node nodeStr=nodeCtx.child(L"message");
            while(nodeStr)
            {
                strCount++;
                nodeStr=nodeStr.next_sibling(L"message");
            }
            
            SStrMapEntry * strMapEntry = new SStrMapEntry;
            strMapEntry->strCtx=nodeCtx.attribute(L"name").value();
            strMapEntry->m_arrStrMap.SetCount(strCount);
            nodeStr=nodeCtx.child(L"message");
            for(int j=0;j<strCount;j++)
            {
                SASSERT(nodeStr);
                SStrMap * strMap= new SStrMap;
                strMap->strSource=nodeStr.child(L"source").text().get();
                strMap->strTranslation=nodeStr.child(L"translation").text().get();
                strMapEntry->m_arrStrMap.SetAt(j,strMap);
                nodeStr=nodeStr.next_sibling(L"message");
            }
            qsort(strMapEntry->m_arrStrMap.GetData(),strMapEntry->m_arrStrMap.GetCount(),sizeof(SStrMap*),SStrMap::Compare);
            m_arrEntry->SetAt(i,strMapEntry);
            nodeCtx=nodeCtx.next_sibling(L"context");
        }
        
        qsort(m_arrEntry->GetData(),m_arrEntry->GetCount(),sizeof(SStrMapEntry*),SStrMapEntry::Compare);


		//加载字体更换信息
		pugi::xml_node nodeFonts = xmlLang.child(L"fonts");
		pugi::xml_node nodeFont = nodeFonts.child(L"font");
		while(nodeFont)
		{
			SStringW strName = nodeFont.attribute(L"name").as_string();
			TrFontInfo info;
			info.lfCharset =charsetFromString(nodeFont.attribute(L"charset").as_string());
			info.strFaceName = nodeFont.attribute(L"face").as_string();
			m_mapFonts[strName] = info;
			nodeFont = nodeFont.next_sibling(L"font");
		}
        return TRUE;
    }

    BOOL STranslator::tr( const SStringW & strSrc,const SStringW & strCtx,SStringW & strRet )
    {
        SStrMapEntry** pEntry = (SStrMapEntry**)bsearch(&strCtx,m_arrEntry->GetData(),m_arrEntry->GetCount(),sizeof(SStrMapEntry*),SStrMapEntry::CompareInSearch);
        if(pEntry)
        {
            SStrMap ** pMap=(SStrMap**)bsearch(&strSrc,(*pEntry)->m_arrStrMap.GetData(),(*pEntry)->m_arrStrMap.GetCount(),sizeof(SStrMap*),SStrMap::CompareInSearch);
            if(pMap)
            {//从指定的上下文中查找翻译
                strRet=(*pMap)->strTranslation;
                return TRUE;
            }
        }
		if(!strCtx.IsEmpty())
		{//从空白上下文中查找
			return tr(strSrc,SStringW(),strRet);
		}
        return FALSE;
    }

	BOOL STranslator::updateLogfont(const SStringW & strName,LOGFONT *pFont)
	{
		if(!m_mapFonts.Lookup(strName))
			return FALSE;
		TrFontInfo trFontInfo = m_mapFonts[strName];
		SASSERT(trFontInfo.strFaceName.GetLength()<LF_FACESIZE);
		pFont->lfCharSet = trFontInfo.lfCharset;
		_tcscpy(pFont->lfFaceName,trFontInfo.strFaceName);
		return TRUE;
	}

	BYTE STranslator::charsetFromString(const SStringW & strCharset) const
	{
		const struct {
			wchar_t name[40];
			BYTE    value;
		}CharsetMap []={
			{L"ANSI",0},
			{L"DEFAULT",1},
			{L"SYMBOL",2},
			{L"SHIFTJIS",128},
			{L"HANGEUL",129},
			{L"HANGUL",129},
			{L"GB2312",134},
			{L"CHINESEBIG5",136},
			{L"OEM",255},
			{L"JOHAB",130},
			{L"HEBREW",177},
			{L"ARABIC",178},
			{L"GREEK",161},
			{L"TURKISH",162},
			{L"VIETNAMESE",163},
			{L"THAI",222},
			{L"EASTEUROPE",238},
			{L"RUSSIAN",204},
			{L"MAC",77},
			{L"BALTIC",186}
		};
		
			for(int i=0;i<ARRAYSIZE(CharsetMap);i++)
			{
				if(strCharset.CompareNoCase(CharsetMap[i].name) == 0)
					return CharsetMap[i].value;
			}
			return _wtoi(strCharset);
	}


    //////////////////////////////////////////////////////////////////////////
    //  STranslator
    BOOL STranslatorMgr::InstallTranslator(ITranslator *pTranslator)
    {
		if (m_strLang.IsEmpty())
			m_strLang = pTranslator->name();
		if (pTranslator->name() != m_strLang)
			return FALSE;

        SPOSITION pos=m_lstLang->GetHeadPosition();
        while(pos)
        {
            ITranslator *p=m_lstLang->GetNext(pos);
            if(IsEqualGUID(pTranslator->guid(),p->guid()))
            {
                return FALSE;
            }
        }
        m_lstLang->AddHead(pTranslator);
        pTranslator->AddRef();

        return TRUE;
    }

    BOOL STranslatorMgr::UninstallTranslator(REFGUID id)
    {
        SPOSITION pos=m_lstLang->GetHeadPosition();
        while(pos)
        {
            SPOSITION posBackup=pos;
            ITranslator *p=m_lstLang->GetNext(pos);
            if(IsEqualGUID(id,p->guid()))
            {
                m_lstLang->RemoveAt(posBackup);
                p->Release();

                return TRUE;
            }
        }
        return FALSE;
    }

    STranslatorMgr::STranslatorMgr( void )
    {
        m_lstLang=new SList<ITranslator*>;
    }

    STranslatorMgr::~STranslatorMgr( void )
    {
        SPOSITION pos=m_lstLang->GetHeadPosition();
        while(pos)
        {
            ITranslator *pLang=m_lstLang->GetNext(pos);
            pLang->Release();
        }
        delete m_lstLang;
    }

    SStringW STranslatorMgr::tr(const SStringW & strSrc,const SStringW & strCtx)
    {
        if(strSrc.IsEmpty()) return strSrc;
        SStringW strRet;
        SPOSITION pos=m_lstLang->GetHeadPosition();
        while(pos)
        {
            ITranslator *pLang=m_lstLang->GetNext(pos);
            if(pLang->tr(strSrc,strCtx,strRet)) return strRet;
        }
        return strSrc;
    }

    BOOL STranslatorMgr::CreateTranslator( ITranslator ** ppTranslator )
    {
        *ppTranslator = new STranslator;
        return TRUE;
    }


	void STranslatorMgr::SetLanguage(const SStringW & strLang)
	{
		if (m_strLang != strLang)
		{
			SPOSITION pos = m_lstLang->GetHeadPosition();
			while (pos)
			{
				ITranslator *pTrans = m_lstLang->GetNext(pos);
				pTrans->Release();
			}
			m_lstLang->RemoveAll();
		}
		m_strLang = strLang;
	}

	SStringW STranslatorMgr::GetLanguage() const
	{
		return m_strLang;
	}

	BOOL STranslatorMgr::updateLogfont(const SStringW & strName,LOGFONT * pfont)
	{
		if(m_lstLang->IsEmpty()) return FALSE;
		return m_lstLang->GetHead()->updateLogfont(strName,pfont);
	}

    //////////////////////////////////////////////////////////////////////////
    //  
    namespace TRANSLATOR
    {
        BOOL SCreateInstance( IObjRef **ppTrans )
        {
            *ppTrans = new STranslatorMgr;
            return TRUE;
        }
    }
}

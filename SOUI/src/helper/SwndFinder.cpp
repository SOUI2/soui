#include "souistd.h"
#include <helper/swndfinder.h>
#include <core/Swnd.h>

namespace SOUI
{
    SWindowFinder * SSingleton<SWindowFinder>::ms_Singleton = NULL;
    
	SFindInfo::SFindInfo(SWindow *pParent,const SStringW & _strName,int _nDeep)
		:hParent(pParent->GetSwnd()),strName(_strName),nDeep(_nDeep),findByName(true)
	{

	}

	SFindInfo::SFindInfo(SWindow *pParent,int _nID,int _nDeep)
		:hParent(pParent->GetSwnd()),nID(_nID),nDeep(_nDeep),findByName(false)
	{

	}

	//////////////////////////////////////////////////////////////////////////
    SWindow * SWindowFinder::FindChildByName(SWindow *pParent,const SStringW &  strName,int nDeep)
    {
        SFindInfo fi(pParent,strName,nDeep);
        return FindChildByKey(pParent,fi);
    }

    SWindow * SWindowFinder::FindChildByID(SWindow *pParent,int nID,int nDeep)
    {
        SFindInfo fi(pParent,nID,nDeep);
        return FindChildByKey(pParent,fi);
    }

    SWindow * SWindowFinder::FindChildByKey(SWindow *pParent,const SFindInfo &fi)
    {
        FINDCACHE::CPair *pFind = m_findCache.Lookup(fi);
        if(!pFind)
        {
			return NULL;
        }else
        {
			SWindow *pRet = SWindowMgr::GetWindow(pFind->m_value);
			if(!pRet) 
			{
				m_findCache.RemoveKey(fi);
			}
			return pRet;
        }

    }

	void SWindowFinder::CacheResultForName(SWindow *pParent,const SStringW & strName,int nDeep,SWindow *pResult)
	{
		SFindInfo fi(pParent,strName,nDeep);
		SASSERT(m_findCache.Lookup(fi)==NULL);
		SASSERT(pResult);
		m_findCache[fi]=pResult->GetSwnd();
	}

	void SWindowFinder::CacheResultForID(SWindow *pParent,int nID,int nDeep,SWindow *pResult)
	{
		SFindInfo fi(pParent,nID,nDeep);
		SASSERT(m_findCache.Lookup(fi)==NULL);
		SASSERT(pResult);
		m_findCache[fi]=pResult->GetSwnd();
	}


}

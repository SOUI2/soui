#include "souistd.h"
#include <helper/swndfinder.h>

namespace SOUI
{
    SWindowFinder * SSingleton<SWindowFinder>::ms_Singleton = NULL;
    
    SWindow * SWindowFinder::FindChildByName(SWindow *pParent,SStringW strName,int nDeep)
    {
        SFindInfo fi;
        fi.hParent = pParent->GetSwnd();
        fi.findByName = true;
        fi.strName = strName;
        fi.nDeep = nDeep;
        return FindChildByKey(pParent,fi);
    }

    SWindow * SWindowFinder::FindChildByID(SWindow *pParent,int nID,int nDeep)
    {
        SFindInfo fi;
        fi.hParent = pParent->GetSwnd();
        fi.findByName = false;
        fi.nID = nID;
        fi.nDeep = nDeep;
        return FindChildByKey(pParent,fi);
    }

    SWindow * SWindowFinder::FindChildByKey(SWindow *pParent,const SFindInfo &fi)
    {
        FINDCACHE::CPair *pFind = m_findCache.Lookup(fi);
        if(!pFind)
        {
            SWindow *pRet = NULL;
            if(fi.findByName) pRet=pParent->FindChildByName(fi.strName,fi.nDeep);
            else pRet=pParent->FindChildByID(fi.nID,fi.nDeep);
            m_findCache[fi]=pRet?pRet->GetSwnd():0;
            return pRet;
        }else
        {
            SWindow *pRet = pFind->m_value!=0?SWindowMgr::GetWindow(pFind->m_value):NULL;
            if(pRet)
            {
                pFind->m_value = pRet->GetSwnd();
            }
            return pRet;
        }

    }


    SWindowFinder::SFindInfo::operator ULONG_PTR() const
    {
        ULONG_PTR lRet = 0;
        if(findByName)
            lRet =SStringElementTraits<SStringW>::Hash(strName);
        else
            lRet = nID;

        union KEY{
            LONG key;
            struct long_sep
            {
                LONG hParent:24;
                LONG nDeep:7;
                LONG findByName:1;
            }sep;
        }key;
        key.sep.hParent=hParent;
        key.sep.nDeep = nDeep;
        key.sep.findByName=findByName;
        
        lRet += key.key;
        
        return lRet;
    }

}

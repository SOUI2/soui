#pragma once

#include <core/SSingleton2.h>
#include <core/swnd.h>

namespace SOUI
{
    class SOUI_EXP SWindowFinder : public SSingleton2<SWindowFinder>
    {
		SINGLETON2_TYPE(SINGLETON_WINDOWFINDER)
        class SFindInfo
        {
        public:
            SWND hParent;
            bool findByName;
            SStringW strName;
            int      nID;
            int nDeep;

            operator ULONG_PTR() const;
        };
    public:
        SWindow * FindChildByName(SWindow *pParent,SStringW strName,int nDeep);
        template<class T>
        SWindow * FindChildByName2(SWindow *pParent,SStringW strName,int nDeep)
        {
            SWindow *pTarget = FindChildByName(pParent,strName,nDeep);
            if(!pTarget || !pTarget->IsClass(T::GetClassName()))
            {
                SLOGFMTW(_T("FindChildByName2 Failed, no window of class [%s] with name of [%s] was found within [%d] levels"),T::GetClassName(),pszName,nDeep);
                return NULL;
            }
            return (T*)pTarget;
        }
        
        SWindow * FindChildByID(SWindow *pParent,int nID,int nDeep);
        template<class T>
        SWindow * FindChildByID2(SWindow *pParent,int nID,int nDeep)
        {
            SWindow *pTarget = FindChildByID(pParent,nID,nDeep);
            if(!pTarget || !pTarget->IsClass(T::GetClassName()))
            {
                SLOGFMTW(_T("FindChildByID2 Failed, no window of class [%s] with id of [%d] was found within [%d] levels"),T::GetClassName(),nID,nDeep);
                return NULL;
            }
            return (T*)pTarget;
        }
    protected:
        SWindow * FindChildByKey(SWindow *pParent,const SFindInfo &fi);

        typedef SMap<SFindInfo,SWND> FINDCACHE;
        FINDCACHE m_findCache;
    };


}
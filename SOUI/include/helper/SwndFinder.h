#pragma once

#include <core/SSingleton2.h>

namespace SOUI
{
	class SFindInfo
	{
	public:
		SFindInfo(SWindow *pParent,const SStringW & strName,int nDeep);
		SFindInfo(SWindow *pParent,int nID,int nDeep);
		SWND hParent;
		bool findByName;
		SStringW strName;
		int      nID;
		int nDeep;
	};

	template<>
	class CElementTraits<SFindInfo > :
		public CElementTraitsBase<SFindInfo >
	{
	public:
		static ULONG Hash(INARGTYPE fi)
		{
			ULONG_PTR lRet = 0;
			if(fi.findByName)
				lRet =SStringElementTraits<SStringW>::Hash(fi.strName);
			else
				lRet = fi.nID;

			union KEY{
				LONG key;
				struct long_sep
				{
					LONG hParent:24;
					LONG nDeep:7;
					LONG findByName:1;
				}sep;
			}key;
			key.sep.hParent=fi.hParent;
			key.sep.nDeep = fi.nDeep;
			key.sep.findByName=fi.findByName;

			lRet += key.key^0x12345678;
			return lRet;
		}

		static bool CompareElements(INARGTYPE element1, INARGTYPE element2)
		{
			bool bRet = element1.findByName == element2.findByName;
			if(bRet) bRet = element1.hParent == element2.hParent;
			if(bRet)
			{
				if(element1.findByName)
					bRet = element1.strName == element2.strName;
				else
					bRet = element1.nID == element2.nID;
			}			
			return bRet;
		}

		static int CompareElementsOrdered(INARGTYPE element1, INARGTYPE element2)
		{
			int nRet = element1.findByName - element2.findByName;
			if(nRet == 0) nRet = element1.hParent - element2.hParent;
			if(nRet == 0)
			{
				if(element1.findByName)
					nRet = element1.strName.Compare(element2.strName);
				else
					nRet = element1.nID - element2.nID;
			}			
			return nRet;
		}
	};

    class SWindowFinder : public SSingleton2<SWindowFinder>
    {
		SINGLETON2_TYPE(SINGLETON_WINDOWFINDER)
 

		friend class SWindow;
    protected:
        SWindow * FindChildByName(SWindow *pParent,const SStringW & strName,int nDeep);
		SWindow * FindChildByID(SWindow *pParent,int nID,int nDeep);

		void CacheResultForName(SWindow *pParent,const SStringW & strName,int nDeep,SWindow *pResult);
		void CacheResultForID(SWindow *pParent,int nID,int nDeep,SWindow *pResult);
    protected:
        SWindow * FindChildByKey(SWindow *pParent,const SFindInfo &fi);

        typedef SMap<SFindInfo,SWND> FINDCACHE;
        FINDCACHE m_findCache;
    };


}
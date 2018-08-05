#pragma once
#include "core/SSingleton.h"
#include <unknown/obj-ref-i.h>
#include <unknown/obj-ref-impl.hpp>
#include "interface/SResProvider-i.h"

#include "res.mgr/SSkinPool.h"
#include "res.mgr/SStylePool.h"
#include "res.mgr/SFontPool.h"
#include "res.mgr/SObjDefAttr.h"
#include "res.mgr/SNamedValue.h"

#define GETCSS(p1) SUiDef::getSingleton().GetUiDef()->GetObjDefAttr().GetDefAttribute(p1)

namespace SOUI
{

	struct IUiDefInfo : IObjRef
	{
		virtual SSkinPool * GetSkinPool() =0;
		virtual SStylePool * GetStylePool() =0;
		virtual SNamedColor & GetNamedColor()  =0;
		virtual SNamedString & GetNamedString()  =0;
		virtual SNamedDimension & GetNamedDimension() = 0;
		virtual SObjDefAttr & GetObjDefAttr() = 0;
		virtual FontInfo & GetDefFontInfo() = 0;
	};

	typedef BOOL (*FunFontCheck)(const SStringT & strFontName);

	class SOUI_EXP SUiDef : public SSingleton<SUiDef>
	{
	public:
		SUiDef(void);
		~SUiDef(void);

		static IUiDefInfo * CreateUiDefInfo(IResProvider *pResProvider, LPCTSTR pszUiDef);

		
		IUiDefInfo * GetUiDef(){return m_pCurUiDef;}
		
		void SetUiDef(IUiDefInfo* pUiDefInfo);

		static void SetFontChecker(FunFontCheck fontCheck);

		static BOOL CheckFont(const SStringT & strFontName);
	protected:

		static FunFontCheck	s_funFontCheck;

		CAutoRefPtr<IUiDefInfo> m_pCurUiDef;
	};


}

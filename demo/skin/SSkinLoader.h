#pragma once
#include "pugixml\pugixml.hpp"
#include "res.mgr\SSkinPool.h"
#include "helper\SplitString.h"
#include "resprovider-zip\zipresprovider-param.h"
#include "resprovider-zip\SResProviderZip.h"
namespace SOUI
{
#define RT_SKIN _T("SkinXml")
	class SSkinLoader :public SSingleton<SSkinLoader>
	{
		CAutoRefPtr<IResProvider> m_pResProvider; 
		CAutoRefPtr<SSkinPool>  m_privateSkinPool;
		SApplication *m_theApp;
		BOOL CreateResProvider_ZIP(IObjRef **ppObj)
		{
			return RESPROVIDER_ZIP::SCreateInstance(ppObj);
		}
	public:
		SSkinLoader(SApplication*);
		~SSkinLoader();
		void LoadSkinFormZip(SStringT respath, const TCHAR *strXmlSkin=_T("LoadSkinXml"))
		{
			if (m_pResProvider == NULL)
			{
				if(CreateResProvider_ZIP((IObjRef**)&m_pResProvider))
					m_theApp->AddResProvider(m_pResProvider,NULL);
			}
			SASSERT(m_pResProvider);
			ZIPRES_PARAM param;
			param.ZipFile(m_theApp->GetRenderFactory(), respath, "www.bukengnikengshui.com");
			if (!m_pResProvider->Init((WPARAM)&param, 0))
			{
				SASSERT(0);
			}
			pugi::xml_document xmlDoc;
			SStringTList strLst;
			BOOL bLoad = FALSE;
			if (2 == ParseResID(strXmlSkin, strLst))
			{
				bLoad = LOADXML(xmlDoc, strLst[1], strLst[0]);
			}
			else
			{
				bLoad = LOADXML(xmlDoc, strLst[0], RT_SKIN);
			}
			if (bLoad)
			{
				if (m_privateSkinPool->GetCount() > 0)
				{
					pugi::xml_node xmlSkin = xmlDoc.child(L"skin").first_child();
					SStringW strSkinName, strTypeName;
					while (xmlSkin)
					{
						strTypeName = xmlSkin.name();
						strSkinName = xmlSkin.attribute(L"name").value();

						if (strSkinName.IsEmpty() || strTypeName.IsEmpty())
						{
							xmlSkin = xmlSkin.next_sibling();
							continue;
						}
						xmlSkin.attribute(L"name").set_userdata(1);
						ISkinObj *pSkin = m_privateSkinPool->GetSkin(strSkinName,100);
						if (pSkin)
						{
							pSkin->InitFromXml(xmlSkin);
						}
						else
						{
							SASSERT_FMTW(FALSE, L"load skin error,type=%s,name=%s", strTypeName, strSkinName);
						}
						xmlSkin.attribute(L"name").set_userdata(0);
						xmlSkin = xmlSkin.next_sibling();
					}
				}
				else
					m_privateSkinPool->LoadSkins(xmlDoc.child(L"skin"));
			}
		}
		void LoadSkin(SStringT respath,const TCHAR *strXmlSkin = _T("LoadSkinXml"))
		{			
// 			if (m_pResProvider)
// 			{
// 				m_theApp->RemoveResProvider(m_pResProvider);				
// 				m_pResProvider = NULL;
// 			}
// 			CreateResProvider(RES_FILE, (IObjRef**)&m_pResProvider);				
// 			m_theApp->AddResProvider(m_pResProvider,NULL);			
			if (m_pResProvider == NULL)
			{
				if(CreateResProvider(RES_FILE, (IObjRef**)&m_pResProvider))
					m_theApp->AddResProvider(m_pResProvider, NULL);
			}
			SASSERT(m_pResProvider);
			if (!m_pResProvider->Init((WPARAM)respath.GetBuffer(0), NULL))
			{
				SASSERT(0);
			}
			pugi::xml_document xmlDoc;
			SStringTList strLst;
			BOOL bLoad=FALSE;
			if (2 == ParseResID(strXmlSkin, strLst))
			{
				bLoad=LOADXML(xmlDoc, strLst[1], strLst[0]);
			}
			else
			{
				bLoad=LOADXML(xmlDoc, strLst[0], RT_SKIN);
			}
			if (bLoad)
			{
				if (m_privateSkinPool->GetCount() > 0)
				{
					pugi::xml_node xmlSkin = xmlDoc.child(L"skin").first_child();
					SStringW strSkinName, strTypeName;
					while (xmlSkin)
					{
						strTypeName = xmlSkin.name();
						strSkinName = xmlSkin.attribute(L"name").value();

						if (strSkinName.IsEmpty() || strTypeName.IsEmpty())
						{
							xmlSkin = xmlSkin.next_sibling();
							continue;
						}
						xmlSkin.attribute(L"name").set_userdata(1);
						ISkinObj *pSkin = m_privateSkinPool->GetSkin(strSkinName,100);
						if (pSkin)
						{
							pSkin->InitFromXml(xmlSkin);
						}
						else
						{
							SASSERT_FMTW(FALSE, L"load skin error,type=%s,name=%s", strTypeName, strSkinName);
						}
						xmlSkin.attribute(L"name").set_userdata(0);
						xmlSkin = xmlSkin.next_sibling();
					}
				}
				else 
					m_privateSkinPool->LoadSkins(xmlDoc.child(L"skin"));
			}
		}
	};
}//END SOUI

#include "souistd.h"
#include "SApp.h"
#include "core/SimpleWnd.h"
#include "core/SWindowMgr.h"

#include "res.mgr/sfontpool.h"
#include "res.mgr/SUiDef.h"

#include "helper/STimerEx.h"
#include "helper/SScriptTimer.h"
#include "helper/mybuffer.h"
#include "helper/SToolTip.h"
#include "helper/AppDir.h"
#include "helper/SwndFinder.h"

#include "control/Smessagebox.h"
#include "updatelayeredwindow/SUpdateLayeredWindow.h"
#include "helper/splitstring.h"
#include "res.mgr/SObjDefAttr.h"

#include "core/SSkin.h"
#include "control/souictrls.h"
#include "layout/SouiLayout.h"
#include "layout/SLinearLayout.h"
#include "layout/SGridLayout.h"
#include "animator/SInterpolatorImpl.h"


namespace SOUI
{
	class SObjectDefaultRegister : public TObjRefImpl<ISystemObjectRegister>
	{
	public:

		void RegisterWindows(SObjectFactoryMgr *objFactory);
		void RegisterSkins(SObjectFactoryMgr *objFactory);
		void RegisterLayouts(SObjectFactoryMgr *objFactory);
		void RegisterInterpolator(SObjectFactoryMgr *objFactory);
	};

class SNullTranslator : public TObjRefImpl<ITranslatorMgr>
{
public:
    BOOL CreateTranslator(ITranslator **pLang){return FALSE;}
    BOOL InstallTranslator(ITranslator * pLang){return FALSE;}
    BOOL UninstallTranslator(REFGUID id){return FALSE;}
    SStringW tr(const SStringW & strSrc,const SStringW & strCtx)
    {
        return strSrc;
    } 


	virtual void SetLanguage(const SStringW & strLang) override
	{
	}

	virtual SStringW GetLanguage() const override
	{
		return SStringW();
	}

};

class SDefToolTipFactory : public TObjRefImpl<IToolTipFactory>
{
public:
    /*virtual */IToolTip * CreateToolTip(HWND hHost)
    {
        STipCtrl *pTipCtrl = new STipCtrl;
        if(!pTipCtrl->Create())
        {
            delete pTipCtrl;
            return NULL;
        }
        return pTipCtrl;
    }

    /*virtual */void DestroyToolTip(IToolTip *pToolTip)
    {
        if(pToolTip)
        {
            STipCtrl *pTipCtrl= (STipCtrl *)pToolTip;
            pTipCtrl->DestroyWindow();
        }
    }
};

class SDefMsgLoopFactory : public TObjRefImpl<IMsgLoopFactory>
{
public:
    virtual SMessageLoop * CreateMsgLoop()
    {
        return new SMessageLoop;
    }

    virtual void DestoryMsgLoop(SMessageLoop * pMsgLoop)
    {
        delete pMsgLoop;
    }
};

//////////////////////////////////////////////////////////////////////////
// SApplication

template<> SApplication* SSingleton<SApplication>::ms_Singleton = 0;

SApplication::SApplication(IRenderFactory *pRendFactory,HINSTANCE hInst,LPCTSTR pszHostClassName,ISystemObjectRegister *pSysObjRegister,BOOL bImeApp)
    :m_hInst(hInst)
    ,m_RenderFactory(pRendFactory)
    ,m_hMainWnd(NULL)
{
    SWndSurface::Init();
    _CreateSingletons();

    CSimpleWndHelper::Init(m_hInst,pszHostClassName,bImeApp);
    STextServiceHelper::Init();
    SRicheditMenuDef::Init();
    m_translator.Attach(new SNullTranslator);
    m_tooltipFactory.Attach(new SDefToolTipFactory);
    m_msgLoopFactory.Attach(new SDefMsgLoopFactory);
    
    SAppDir appDir(hInst);
    m_strAppDir = appDir.AppDir();
    
    m_pMsgLoop = GetMsgLoopFactory()->CreateMsgLoop();

	ISystemObjectRegister *pRegister = pSysObjRegister ? pSysObjRegister : new SObjectDefaultRegister();
	pRegister->RegisterLayouts(this);
	pRegister->RegisterSkins(this);
	pRegister->RegisterWindows(this);
	pRegister->RegisterInterpolator(this);
	if (pSysObjRegister == NULL)
		pRegister->Release();

}

SApplication::~SApplication(void)
{
    GetMsgLoopFactory()->DestoryMsgLoop(m_pMsgLoop);
    
    _DestroySingletons();
    CSimpleWndHelper::Destroy();
    STextServiceHelper::Destroy();
    SRicheditMenuDef::Destroy();
}

void SApplication::_CreateSingletons()
{
	new SUiDef();
    new SWindowMgr();
    new STimer2();
    new SScriptTimer();
    new SFontPool(m_RenderFactory);
    new SSkinPoolMgr();
    new SStylePoolMgr();
    new SWindowFinder();
}

void SApplication::_DestroySingletons()
{
    SResProviderMgr::RemoveAll();
    delete SWindowFinder::getSingletonPtr();
    delete SStylePoolMgr::getSingletonPtr();
    delete SSkinPoolMgr::getSingletonPtr();
    delete SFontPool::getSingletonPtr();
    delete SScriptTimer::getSingletonPtr();
    delete STimer2::getSingletonPtr();
    delete SWindowMgr::getSingletonPtr();
	delete SUiDef::getSingletonPtr();
}

BOOL SApplication::_LoadXmlDocment( LPCTSTR pszXmlName ,LPCTSTR pszType ,pugi::xml_document & xmlDoc,IResProvider *pResProvider/* = NULL*/)
{
    if(!pResProvider) 
    {
        if(IsFileType(pszType))
        {
            pugi::xml_parse_result result= xmlDoc.load_file(pszXmlName,pugi::parse_default,pugi::encoding_utf8);
            SASSERT_FMTW(result,L"parse xml error! xmlName=%s,desc=%s,offset=%d",pszXmlName,result.description(),result.offset);
            return result;
        }else
        {
            pResProvider = GetMatchResProvider(pszType,pszXmlName);
        }
    }
    if(!pResProvider) return FALSE;
    
    size_t dwSize=pResProvider->GetRawBufferSize(pszType,pszXmlName);
    if(dwSize==0) return FALSE;

    CMyBuffer<char> strXml;
    strXml.Allocate(dwSize);
    pResProvider->GetRawBuffer(pszType,pszXmlName,strXml,dwSize);

    pugi::xml_parse_result result= xmlDoc.load_buffer(strXml,strXml.size(),pugi::parse_default,pugi::encoding_utf8);
    SASSERT_FMTW(result,L"parse xml error! xmlName=%s,desc=%s,offset=%d",pszXmlName,result.description(),result.offset);
    return result;
}

BOOL SApplication::LoadXmlDocment( pugi::xml_document & xmlDoc,LPCTSTR pszXmlName ,LPCTSTR pszType )
{
    return _LoadXmlDocment(pszXmlName,pszType,xmlDoc);
}

BOOL SApplication::LoadXmlDocment(pugi::xml_document & xmlDoc, const SStringT & strXmlTypeName)
{
    SStringTList strLst;
    if(2!=ParseResID(strXmlTypeName,strLst)) return FALSE;
    return LoadXmlDocment(xmlDoc,strLst[1],strLst[0]);
}

UINT SApplication::LoadSystemNamedResource( IResProvider *pResProvider )
{
    UINT uRet=0;
    AddResProvider(pResProvider,NULL);
    //load system skins
    {
        pugi::xml_document xmlDoc;
        if(_LoadXmlDocment(_T("SYS_XML_SKIN"),_T("XML"),xmlDoc,pResProvider))
        {
            SSkinPool * p= SSkinPoolMgr::getSingletonPtr()->GetBuiltinSkinPool();
            p->LoadSkins(xmlDoc.child(L"skin"));
        }else
        {
            uRet |= 0x01;
        }
    }
    //load edit context menu
    {
        pugi::xml_document xmlDoc;
        if(_LoadXmlDocment(_T("SYS_XML_EDITMENU"),_T("XML"),xmlDoc,pResProvider))
        {
            SRicheditMenuDef::getSingleton().SetMenuXml(xmlDoc.child(L"editmenu"));
        }else
        {
            uRet |= 0x02;
        }
    }
    //load messagebox template
    {
        pugi::xml_document xmlDoc;
        if(!_LoadXmlDocment(_T("SYS_XML_MSGBOX"),_T("XML"),xmlDoc,pResProvider)
        || !SetMsgTemplate(xmlDoc.child(L"SOUI")))
        {
            uRet |= 0x04;
        }
    }
    RemoveResProvider(pResProvider);
    return uRet;
}

int SApplication::Run( HWND hMainWnd )
{
    m_hMainWnd = hMainWnd;
    int nRet = m_pMsgLoop->Run();
	if(::IsWindow(m_hMainWnd))
	{
		DestroyWindow(m_hMainWnd);
	}
	return nRet;
}

HINSTANCE SApplication::GetInstance()
{
	return m_hInst;
}

void SApplication::SetTranslator(ITranslatorMgr * pTrans)
{
	m_translator = pTrans;
}

ITranslatorMgr * SApplication::GetTranslator()
{
	return m_translator;
}

void SApplication::SetScriptFactory(IScriptFactory *pScriptFactory)
{
	m_pScriptFactory = pScriptFactory;
}


HRESULT SApplication::CreateScriptModule( IScriptModule **ppScriptModule )
{
    if(!m_pScriptFactory) return E_FAIL;
    return m_pScriptFactory->CreateScriptModule(ppScriptModule);
}

IRenderFactory * SApplication::GetRenderFactory()
{
	return m_RenderFactory;
}

void SApplication::SetRealWndHandler( IRealWndHandler *pRealHandler )
{
    m_pRealWndHandler = pRealHandler;
}

IRealWndHandler * SApplication::GetRealWndHander()
{
    return m_pRealWndHandler;
}

IToolTipFactory * SApplication::GetToolTipFactory()
{
    return m_tooltipFactory;
}

void SApplication::SetToolTipFactory( IToolTipFactory* pToolTipFac )
{
    m_tooltipFactory = pToolTipFac;
}

HWND SApplication::GetMainWnd()
{
    return m_hMainWnd;
}

BOOL SApplication::SetMsgLoopFactory(IMsgLoopFactory *pMsgLoopFac)
{
    if(m_pMsgLoop->IsRunning()) return FALSE;
    m_msgLoopFactory->DestoryMsgLoop(m_pMsgLoop);
    m_msgLoopFactory = pMsgLoopFac;
    m_pMsgLoop = m_msgLoopFactory->CreateMsgLoop();
    return TRUE;
}

IMsgLoopFactory * SApplication::GetMsgLoopFactory()
{
    return m_msgLoopFactory;
}

void SApplication::InitXmlNamedID(const SNamedID::NAMEDVALUE *pNamedValue,int nCount,BOOL bSorted)
{
    m_namedID.Init2(pNamedValue,nCount,bSorted);
}

int SApplication::Str2ID(const SStringW & str)
{
    return m_namedID.String2Value(str);
}

SWindow * SApplication::CreateWindowByName(LPCWSTR pszWndClass) const
{
	return (SWindow*)CreateObject(SObjectInfo(pszWndClass, Window));
}

ISkinObj * SApplication::CreateSkinByName(LPCWSTR pszSkinClass) const
{
	return (ISkinObj*)CreateObject(SObjectInfo(pszSkinClass, Skin));
}


IInterpolator * SApplication::CreateInterpolatorByName(LPCWSTR pszName) const
{
	return (IInterpolator*)CreateObject(SObjectInfo(pszName, Interpolator));
}


void SApplication::SetLogManager(ILog4zManager * pLogMgr)
{
    m_logManager = pLogMgr;
}

ILog4zManager * SApplication::GetLogManager()
{
    return m_logManager;
}

SStringT SApplication::GetAppDir() const
{
	return m_strAppDir;
}

void SApplication::SetAppDir(const SStringT & strAppDir)
{
	m_strAppDir = strAppDir;
}

IAttrStorageFactory * SApplication::GetAttrStorageFactory()
{
	return m_pAttrStroageFactory;
}

void SApplication::SetAttrStorageFactory(IAttrStorageFactory * pAttrStorageFactory)
{
	m_pAttrStroageFactory = pAttrStorageFactory;
}



void SObjectDefaultRegister::RegisterWindows(SObjectFactoryMgr *objFactory)
{
	objFactory->TplRegisterFactory<SWindow>();
	objFactory->TplRegisterFactory<SPanel>();
	objFactory->TplRegisterFactory<SStatic>();
	objFactory->TplRegisterFactory<SButton>();
	objFactory->TplRegisterFactory<SImageWnd>();
	objFactory->TplRegisterFactory<SProgress>();
	objFactory->TplRegisterFactory<SImageButton>();
	objFactory->TplRegisterFactory<SLine>();
	objFactory->TplRegisterFactory<SCheckBox>();
	objFactory->TplRegisterFactory<SIconWnd>();
	objFactory->TplRegisterFactory<SRadioBox>();
	objFactory->TplRegisterFactory<SLink>();
	objFactory->TplRegisterFactory<SGroup>();
	objFactory->TplRegisterFactory<SAnimateImgWnd>();
	objFactory->TplRegisterFactory<SScrollView>();
	objFactory->TplRegisterFactory<SRealWnd>();
	objFactory->TplRegisterFactory<SToggle>();
	objFactory->TplRegisterFactory<SCaption>();
	objFactory->TplRegisterFactory<STabCtrl>();
	objFactory->TplRegisterFactory<STabPage>();
	objFactory->TplRegisterFactory<SActiveX>();
	objFactory->TplRegisterFactory<SFlashCtrl>();
	objFactory->TplRegisterFactory<SSplitPane>();
	objFactory->TplRegisterFactory<SSplitWnd>();
	objFactory->TplRegisterFactory<SSplitWnd_Col>();
	objFactory->TplRegisterFactory<SSplitWnd_Row>();
	objFactory->TplRegisterFactory<SSliderBar>();
	objFactory->TplRegisterFactory<STreeCtrl>();
	objFactory->TplRegisterFactory<SScrollBar>();
	objFactory->TplRegisterFactory<SHeaderCtrl>();
	objFactory->TplRegisterFactory<SListCtrl>();
	objFactory->TplRegisterFactory<SListBox>();
	objFactory->TplRegisterFactory<SRichEdit>();
	objFactory->TplRegisterFactory<SEdit>();
	objFactory->TplRegisterFactory<SHotKeyCtrl>();
	objFactory->TplRegisterFactory<SComboBox>();
	objFactory->TplRegisterFactory<SCalendar>();
	objFactory->TplRegisterFactory<SSpinButtonCtrl>();
	objFactory->TplRegisterFactory<SSpinButton>();
	objFactory->TplRegisterFactory<SListView>();
	objFactory->TplRegisterFactory<SComboView>();
	objFactory->TplRegisterFactory<SMCListView>();
	objFactory->TplRegisterFactory<STileView>();
	objFactory->TplRegisterFactory<STreeView>();
	objFactory->TplRegisterFactory<SMenuBar>();
	objFactory->TplRegisterFactory<SCalendarEx>();
	objFactory->TplRegisterFactory<SDateTimePicker>();
}

void SObjectDefaultRegister::RegisterSkins(SObjectFactoryMgr *objFactory)
{
	objFactory->TplRegisterFactory<SSkinImgList>();
	objFactory->TplRegisterFactory<SSkinImgFrame>();
	objFactory->TplRegisterFactory<SSkinImgFrame2>();
	objFactory->TplRegisterFactory<SSkinButton>();
	objFactory->TplRegisterFactory<SSkinGradation>();
	objFactory->TplRegisterFactory<SSkinScrollbar>();
	objFactory->TplRegisterFactory<SSkinColorRect>();
	objFactory->TplRegisterFactory<SSkinShape>();
	objFactory->TplRegisterFactory<SSKinGroup>();
}

void SObjectDefaultRegister::RegisterLayouts(SObjectFactoryMgr *objFactory)
{
	objFactory->TplRegisterFactory<SouiLayout>();
	objFactory->TplRegisterFactory<SLinearLayout>();
	objFactory->TplRegisterFactory<SHBox>();
	objFactory->TplRegisterFactory<SVBox>();
	objFactory->TplRegisterFactory<SGridLayout>();
}

void SObjectDefaultRegister::RegisterInterpolator(SObjectFactoryMgr *objFactory)
{
	objFactory->TplRegisterFactory<SLinearInterpolator>();
	objFactory->TplRegisterFactory<SAccelerateInterpolator>();
	objFactory->TplRegisterFactory<SDecelerateInterpolator>();
 	objFactory->TplRegisterFactory<SAccelerateDecelerateInterpolator>();
 	objFactory->TplRegisterFactory<SAnticipateInterpolator>();
 	objFactory->TplRegisterFactory<SAnticipateOvershootInterpolator>();
 	objFactory->TplRegisterFactory<SBounceInterpolator>();
 	objFactory->TplRegisterFactory<SCycleInterpolator>();
 	objFactory->TplRegisterFactory<SOvershootInterpolator>();
}

}//namespace SOUI
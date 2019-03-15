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
#include "core/SwndAccessible.h"

#include "msaa/SAccProxyWindow.h"
#include "msaa/SAccProxyCmnCtrl.h"

namespace SOUI
{

class SNullTranslator : public TObjRefImpl<ITranslatorMgr>
{
public:
    BOOL CreateTranslator(ITranslator **pLang){return FALSE;}
    BOOL InstallTranslator(ITranslator * pLang){return FALSE;}
    BOOL UninstallTranslator(REFGUID id){return FALSE;}
    int tr(const SStringW & strSrc,const SStringW & strCtx, wchar_t *pszOut,int nBufLen) const 
    {
		return 0;
    } 


	virtual void SetLanguage(const SStringW & strLang)
	{
	}

	virtual void GetLanguage(wchar_t szOut[TR_MAX_NAME_LEN]) const
	{
		szOut[0] = 0;
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
	objFactory->TplRegisterFactory<SSkinImgCenter>();
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
//////////////////////////////////////////////////////////////////////////
// SApplication

template<> SApplication* SSingleton<SApplication>::ms_Singleton = 0;

SApplication::SApplication(IRenderFactory *pRendFactory,HINSTANCE hInst,LPCTSTR pszHostClassName, ISystemObjectRegister & sysObjRegister,BOOL bImeApp)
    :m_hInst(hInst)
    ,m_RenderFactory(pRendFactory)
    ,m_hMainWnd(NULL)
{
    SWndSurface::Init();
	memset(m_pSingletons, 0, sizeof(m_pSingletons));
    _CreateSingletons(hInst,pszHostClassName,bImeApp);

    m_translator.Attach(new SNullTranslator);
    m_tooltipFactory.Attach(new SDefToolTipFactory);
    m_msgLoopFactory.Attach(new SDefMsgLoopFactory);
    
    SAppDir appDir(hInst);
    m_strAppDir = appDir.AppDir();
    
    m_pMsgLoop = GetMsgLoopFactory()->CreateMsgLoop();

	sysObjRegister.RegisterLayouts(this);
	sysObjRegister.RegisterSkins(this);
	sysObjRegister.RegisterWindows(this);
	sysObjRegister.RegisterInterpolator(this);
}

SApplication::~SApplication(void)
{
    GetMsgLoopFactory()->DestoryMsgLoop(m_pMsgLoop);
    
	SResProviderMgr::RemoveAll();
    _DestroySingletons();
}

void SApplication::_CreateSingletons(HINSTANCE hInst,LPCTSTR pszHostClassName,BOOL bImeApp)
{
	m_pSingletons[SUiDef::GetType()] = new SUiDef();
	m_pSingletons[SWindowMgr::GetType()] = new SWindowMgr();
	m_pSingletons[STimer2::GetType()] = new STimer2();
	m_pSingletons[SScriptTimer::GetType()] = new SScriptTimer();
	m_pSingletons[SFontPool::GetType()] = new SFontPool(m_RenderFactory);
	m_pSingletons[SSkinPoolMgr::GetType()] =  new SSkinPoolMgr();
	m_pSingletons[SStylePoolMgr::GetType()] =  new SStylePoolMgr();
	m_pSingletons[STemplatePoolMgr::GetType()] = new STemplatePoolMgr();
	m_pSingletons[SWindowFinder::GetType()] = new SWindowFinder();
	m_pSingletons[STextServiceHelper::GetType()] = new STextServiceHelper();
	m_pSingletons[SRicheditMenuDef::GetType()] = new SRicheditMenuDef();
	m_pSingletons[CSimpleWndHelper::GetType()] =   new CSimpleWndHelper(hInst, pszHostClassName, bImeApp);
}

#define DELETE_SINGLETON(x) \
 delete (x*)m_pSingletons[x::GetType()];\
 m_pSingletons[x::GetType()] = NULL;

void SApplication::_DestroySingletons()
{
	DELETE_SINGLETON(CSimpleWndHelper);
	DELETE_SINGLETON(SRicheditMenuDef);
	DELETE_SINGLETON(STextServiceHelper);
	DELETE_SINGLETON(SWindowFinder);
	DELETE_SINGLETON(SStylePoolMgr);
	DELETE_SINGLETON(STemplatePoolMgr);
	DELETE_SINGLETON(SSkinPoolMgr);
	DELETE_SINGLETON(SFontPool);
	DELETE_SINGLETON(SScriptTimer);
	DELETE_SINGLETON(STimer2);
	DELETE_SINGLETON(SWindowMgr);
	DELETE_SINGLETON(SUiDef);
}


IAccProxy * SApplication::CreateAccProxy(SWindow* pWnd) const
{
#ifdef SOUI_ENABLE_ACC
	if(pWnd->IsClass(SProgress::GetClassName()))
	{
		return new SAccProxyProgress(pWnd);
	}else if(pWnd->IsClass(SSliderBar::GetClassName()))
	{
		return new SAccProxySlideBar(pWnd);
	}else if(pWnd->IsClass(SButton::GetClassName()) || pWnd->IsClass(SImageButton::GetClassName()))
	{
		return new SAccProxyButton(pWnd);
	}else if(pWnd->IsClass(SComboBox::GetClassName()))
	{
		return new SAccProxyCombobox(pWnd);
	}else if(pWnd->IsClass(SRichEdit::GetClassName()))
	{
		return new SAccProxyEdit(pWnd);
	}else if(pWnd->IsClass(SCheckBox::GetClassName()))
	{
		return new SAccProxyCheckButton(pWnd);
	}else if(pWnd->IsClass(SRadioBox::GetClassName()))
	{
		return new SAccProxyRadioButton(pWnd);
	}

	return new SAccProxyWindow(pWnd);
#else
	return NULL;
#endif//SOUI_ENABLE_ACC
}

IAccessible * SApplication::CreateAccessible(SWindow *pWnd) const
{
#ifdef SOUI_ENABLE_ACC
	return new SAccessible(pWnd);
#else
	return NULL;
#endif//SOUI_ENABLE_ACC
}


void * SApplication::GetInnerSingleton(int nType)
{
	if (nType < 0 || nType >= SINGLETON_COUNT)
		return NULL;
	return m_pSingletons[nType];
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


SStringW SApplication::tr(const SStringW & strSrc,const SStringW & strCtx) const
{
	int nRet = m_translator->tr(strSrc,strCtx,NULL,0);
	if(nRet == 0) return strSrc;
	SStringW strRet;
	wchar_t *pBuf = strRet.GetBufferSetLength(nRet-1);
	m_translator->tr(strSrc,strCtx,pBuf,nRet);
	strRet.ReleaseBuffer();
	return strRet;
}

SWindow * SApplication::CreateWindowByName(LPCWSTR pszWndClass) const
{//支持使用类似button.ok这样的控件名来创建控件，对于这种格式自动应用button.ok为class属性.
	SStringW strClsName = pszWndClass;
	int nPos = strClsName.ReverseFind(L'.');
	if (nPos != -1) strClsName = strClsName.Left(nPos);
	SWindow *pRet = (SWindow*)CreateObject(SObjectInfo(strClsName, Window));
	if (pRet && nPos != -1)
	{
		pRet->SetAttribute(L"class", pszWndClass, TRUE);
	}
	return pRet;
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



}//namespace SOUI
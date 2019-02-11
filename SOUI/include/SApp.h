/**
 * Copyright (C) 2014-2050 SOUI团队
 * All rights reserved.
 * 
 * @file       stabctrl.h
 * @brief      
 * @version    v1.0      
 * @author     soui      
 * @date       2014-07-06
 * 
 * Describe    SOUI应用程序入口 
 */

#pragma once
#include "core/ssingleton.h"
#include "unknown/obj-ref-impl.hpp"
#include "interface/render-i.h"
#include "interface/SScriptModule-i.h"
#include "interface/STranslator-i.h"
#include "interface/stooltip-i.h"
#include "interface/slog-i.h"
#include "interface/SAttrStorage-i.h"
#include "interface/sinterpolator-i.h"
#include "control/RealWndHandler-i.h"

#include "res.mgr/SResProviderMgr.h"
#include "res.mgr/SNamedValue.h"

#include "core/smsgloop.h"
#include "core/SObjectFactory.h"
#include <OleAcc.h>

#define GETRESPROVIDER      SOUI::SApplication::getSingletonPtr()
#define GETRENDERFACTORY    SOUI::SApplication::getSingleton().GetRenderFactory()
#define GETREALWNDHANDLER   SOUI::SApplication::getSingleton().GetRealWndHander()
#define GETTOOLTIPFACTORY   SOUI::SApplication::getSingleton().GetToolTipFactory()

#define LOADXML(p1,p2,p3)   SOUI::SApplication::getSingleton().LoadXmlDocment(p1,p2,p3)
#define LOADIMAGE(p1,p2)    SOUI::SApplication::getSingleton().LoadImage(p1,p2)
#define LOADIMAGE2(p1)      SOUI::SApplication::getSingleton().LoadImage2(p1)
#define LOADICON(p1,p2)     SOUI::SApplication::getSingleton().LoadIcon(p1,p2,p2)
#define LOADICON2(p1)       SOUI::SApplication::getSingleton().LoadIcon2(p1)
#define TR(p1,p2)           SOUI::SApplication::getSingleton().tr(p1,p2)
#define STR2ID(p1)          SOUI::SApplication::getSingleton().Str2ID(p1)

#define GETCOLOR(x)         SOUI::SApplication::getSingleton().GetColor(x)
#define GETSTRING(x)        SOUI::SApplication::getSingleton().GetString(x)
#define GETLAYOUTSIZE(x)    SOUI::SApplication::getSingleton().GetLayoutSize(x)

#define CREATEINTERPOLATOR(x)  SOUI::SApplication::getSingleton().CreateInterpolatorByName(x)
#define RT_LAYOUT _T("LAYOUT")

namespace SOUI
{
struct IAccProxy;

interface IMsgLoopFactory : public IObjRef
{
    virtual SMessageLoop * CreateMsgLoop() = 0;
    virtual void DestoryMsgLoop(SMessageLoop * pMsgLoop) =0;
};

interface SOUI_EXP ISystemObjectRegister
{
	virtual void RegisterLayouts(SObjectFactoryMgr *objFactory)  {}
	virtual void RegisterSkins(SObjectFactoryMgr *objFactory)  {}
	virtual void RegisterWindows(SObjectFactoryMgr *objFactory)  {}
	virtual void RegisterInterpolator(SObjectFactoryMgr *objFactory) {}
};

class SOUI_EXP SObjectDefaultRegister : public ISystemObjectRegister
{
public:
	void RegisterWindows(SObjectFactoryMgr *objFactory);
	void RegisterSkins(SObjectFactoryMgr *objFactory);
	void RegisterLayouts(SObjectFactoryMgr *objFactory);
	void RegisterInterpolator(SObjectFactoryMgr *objFactory);
};


/** 
 * @class     SApplication
 * @brief     SOUI Application
 *
 * Describe   SOUI Application
 */
class SOUI_EXP SApplication :public SSingleton<SApplication>
                            ,public SResProviderMgr
	                        ,public SObjectFactoryMgr
{
public:
    /**
     * SApplication
     * @brief    构造函数
     * @param    IRenderFactory * pRendFactory --  渲染模块
     * @param    HINSTANCE hInst --  应用程序句柄
     * @param    LPCTSTR pszHostClassName --  使用SOUI创建窗口时默认的窗口类名
	 * @param    ISystemObjectRegister *pSysObjRegister -- 系统控件注册器
     *
     * Describe  
     */
    SApplication(IRenderFactory *pRendFactory,HINSTANCE hInst,LPCTSTR pszHostClassName = _T("SOUIHOST"), ISystemObjectRegister & sysObjRegister = SObjectDefaultRegister(), BOOL bImeApp = FALSE);

    ~SApplication(void);


    /**
     * GetInstance
     * @brief    获得应用程序句柄
     * @return   HINSTANCE 
     *
     * Describe  
     */
    HINSTANCE GetInstance();

    
    /**
     * Init
     * @brief    从数组里初始化命名ID列表
     * @param    SNamedID::NAMEDVALUE *pValue --  数组
     * @param    int nCount --  数组长度
     * @param    BOOL bSorted -- 数组关键字从小到大已经有序
     *
     * Describe  SNamedID::NAMEDVALUE应该是由uiresbuilder自动生成的列表数据，不要手动修改
     */
    void InitXmlNamedID(const SNamedID::NAMEDVALUE *pNamedValue,int nCount,BOOL bSorted);
    
    /**
     * LoadSystemNamedResource
     * @brief    加载SOUI系统默认的命名资源
     * @param    IResProvider * pResProvider --  
     * @return   UINT 
     *
     * Describe  
     */
    UINT LoadSystemNamedResource(IResProvider *pResProvider);
    
    /**
     * LoadXmlDocment
     * @brief    从资源中加载一个XML Document。
     * @param [out] pugi::xml_document & xmlDoc --  输出的xml_document对象
     * @param    LPCTSTR pszXmlName --  XML文件在资源中的name
     * @param    LPCTSTR pszType --  XML文件在资源中的type
     * @return   BOOL true-加载成功, false-加载失败
     *
     * Describe  
     */
    BOOL LoadXmlDocment(pugi::xml_document & xmlDoc,LPCTSTR pszXmlName ,LPCTSTR pszType);

    /**
     * LoadXmlDocment
     * @brief    从资源中加载一个XML Document。
     * @param [out] pugi::xml_document & xmlDoc --  输出的xml_document对象
     * @param    const SStringT & strXmlTypeName --  XML文件在资源中的type:name
     * @return   BOOL true-加载成功, false-加载失败
     *
     * Describe  
     */
    BOOL LoadXmlDocment(pugi::xml_document & xmlDoc,const SStringT & strXmlTypeName);

    /**
     * GetRenderFactory
     * @brief    获得当前的渲染模块
     * @return   IRenderFactory * 渲染模块指针
     *
     * Describe  
     */
    IRenderFactory * GetRenderFactory();
    
    /**
     * GetScriptModule
     * @brief    创建脚本模块对象
     * @param [out] IScriptModule **ppScriptModule -- 脚本模块对象
     * @return   HRESULT -- S_OK 创建成功
     *
     * Describe  
     */
    HRESULT CreateScriptModule(IScriptModule **ppScriptModule);

    /**
     * SetScriptModule
     * @brief    设置SOUI中使用的脚本模块类厂
     * @param    IScriptFactory *pScriptModule --  脚本模块类厂
     * @return   void 
     *
     * Describe  
     */
    void SetScriptFactory(IScriptFactory *pScriptModule);
    
        
    /**
     * GetTranslator
     * @brief    获取语言翻译模块
     * @return   ITranslator * 语言翻译模块指针
     *
     * Describe  
     */
    ITranslatorMgr * GetTranslator();
    
    /**
     * SetTranslator
     * @brief    设置语言翻译模块
     * @param    ITranslator * pTrans --  语言翻译模块指针
     * @return   void 
     *
     * Describe  
     */
    void SetTranslator(ITranslatorMgr * pTrans);
    
    /**
     * GetRealWndHander
     * @brief    获得RealWndHander
     * @return   IRealWndHandler * -- RealWndHander
     * Describe  
     */    
    IRealWndHandler * GetRealWndHander();

    /**
     * SetRealWndHandler
     * @brief    设置RealWnd处理接口
     * @param    IRealWndHandler * pRealHandler --  RealWnd处理接口
     * @return   void
     * Describe  
     */    
    void SetRealWndHandler(IRealWndHandler *pRealHandler);

    /**
     * GetToolTipFactory
     * @brief    获取ToolTip处理接口
     * @return   IToolTipFactory * -- ToolTip处理接口
     * Describe  
     */    
    IToolTipFactory * GetToolTipFactory();

    /**
     * SetToolTipFactory
     * @brief    设置ToolTip处理接口
     * @param    IToolTipFactory * pToolTipFac --  ToolTip处理接口
     * @return   void -- 
     * Describe  
     */    
    void SetToolTipFactory(IToolTipFactory* pToolTipFac);

    BOOL SetMsgLoopFactory(IMsgLoopFactory *pMsgLoopFac);

    IMsgLoopFactory * GetMsgLoopFactory();
    
    SMessageLoop * GetMsgLoop() { return m_pMsgLoop;}
    
    void SetLogManager(ILog4zManager * pLogMgr);
    
    ILog4zManager * GetLogManager();
    
	void SetAttrStorageFactory(IAttrStorageFactory * pAttrStorageFactory);
	IAttrStorageFactory * GetAttrStorageFactory();

    /**
     * Run
     * @brief    启动SOUI的主消息循环
     * @param    HWND hMainWnd --  应用程序主窗口句柄
     * @return   int 消息循环结束时的返回值
     *
     * Describe  
     */
    int Run(HWND hMainWnd);

    void SetAppDir(const SStringT & strAppDir);

    SStringT GetAppDir()const;
    
    HWND GetMainWnd();
    
    template<class T>
    bool RegisterWindowClass()
    {
		if (T::GetClassType() != Window) return false;
        return TplRegisterFactory<T>();
    }
    
    template<class T>
    bool UnregisterWindowClass()
    {
		if (T::GetClassType() != Window) return false;
        return TplUnregisterFactory<T>();
    }
    template<class T>
    bool RegisterSkinClass()
    {
		if (T::GetClassType() != Skin) return false;
		return TplRegisterFactory<T>();
	}
    
    template<class T>
    bool UnregisterSkinClass()
    {
		if (T::GetClassType() != Skin) return false;
		return TplUnregisterFactory<T>();
	}
    
    int Str2ID(const SStringW & str);

	SStringW tr(const SStringW & strSrc,const SStringW & strCtx) const;
	virtual SWindow * CreateWindowByName(LPCWSTR pszWndClass) const;
	virtual ISkinObj * CreateSkinByName(LPCWSTR pszSkinClass) const;
	virtual IInterpolator * CreateInterpolatorByName(LPCWSTR pszName) const;

	virtual IAccProxy * CreateAccProxy(SWindow* pWnd) const;
	virtual IAccessible * CreateAccessible(SWindow *pWnd) const;

	void * GetInnerSingleton(int nType);
protected:
	virtual void RegisterSystemObjects(){}

    void _CreateSingletons(HINSTANCE hInst, LPCTSTR pszHostClassName, BOOL bImeApp);
    void _DestroySingletons();
    BOOL _LoadXmlDocment(LPCTSTR pszXmlName ,LPCTSTR pszType ,pugi::xml_document & xmlDoc,IResProvider *pResProvider = NULL);
    
    CAutoRefPtr<IRealWndHandler>    m_pRealWndHandler;
    CAutoRefPtr<IScriptFactory>     m_pScriptFactory;
    CAutoRefPtr<IRenderFactory>     m_RenderFactory;
    CAutoRefPtr<ITranslatorMgr>     m_translator;
    CAutoRefPtr<IToolTipFactory>    m_tooltipFactory;
    CAutoRefPtr<IMsgLoopFactory>    m_msgLoopFactory;
    CAutoRefPtr<ILog4zManager>      m_logManager;   //log manager
	CAutoRefPtr<IAttrStorageFactory> m_pAttrStroageFactory;

    SNamedID                        m_namedID;
    
    SStringT    m_strAppDir;
    HINSTANCE   m_hInst;
    HWND        m_hMainWnd;
    SMessageLoop    * m_pMsgLoop;

	//一组单例指针
	void * m_pSingletons[SINGLETON_COUNT];
};



}//namespace SOUI
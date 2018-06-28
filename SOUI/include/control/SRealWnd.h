/**
 * Copyright (C) 2014-2050 SOUI团队
 * All rights reserved.
 * 
 * @file       SRealWnd.h
 * @brief      
 * @version    v1.0      
 * @author     soui      
 * @date       2014-07-06
 * 
 * Describe     
 */
#pragma  once
#include "core/Swnd.h"

//////////////////////////////////////////////////////////////////////////
// Real Window Control
// Binding a real window
//
// Usage: <realwnd id=xx wndclass="edit" wndname="name" style="00000001" exstyle="00000000"/>
//

namespace SOUI
{

/** 
 * @class     SRealWndParam
 * @brief     RealWnd窗口参数
 *
 * Describe   RealWnd窗口参数
 */
class SOUI_EXP SRealWndParam
{
public:
    /**
     * SRealWndParam::SRealWndParam
     * @brief    构造函数
     *
     * Describe  构造函数  
     */
    SRealWndParam();
    /**
     * SRealWndParam::~SRealWndParam
     * @brief    析构函数
     *
     * Describe  析构函数
     */
    ~SRealWndParam();

    SStringT m_strClassName;  /**< 类名 */
    SStringT m_strWindowName; /**< 窗口名 */
    DWORD     m_dwStyle;   /**< 窗口样式 */
    DWORD     m_dwExStyle; /**< 窗口扩展样式 */
    pugi::xml_document m_xmlParams;  /**< 文档 */
};

/** 
 * @class     SRealWnd
 * @brief     
 *
 * Describe   
 */
class SOUI_EXP SRealWnd : public SWindow
{
    SOUI_CLASS_NAME(SRealWnd, L"realwnd")
public:
    /**
     * SRealWnd::SRealWnd
     * @brief    构造函数
     *
     * Describe  构造函数  
     */
    SRealWnd();
    /**
     * SRealWnd::~SRealWnd
     * @brief    析构函数
     *
     * Describe  析构函数  
     */
    virtual ~SRealWnd();

    /**
     * SRealWnd::GetRealHwnd
     * @brief    获取窗口句柄
     * @param    BOOL bAutoCreate -- 自动创建
     * @return   返回HWND
     * 
     * Describe  获取窗口句柄
     */
    const HWND GetRealHwnd(BOOL bAutoCreate=TRUE);

    /**
     * SRealWnd::GetRealWndParam
     * @brief    获取窗口参数
     * @return   返回SRealWndParam
     *
     * Describe  获取窗口参数
     */
    const SRealWndParam & GetRealWndParam()
    {
        return m_realwndParam;
    }

    /**
     * SRealWnd::SetData
     * @brief    获取附加数据
     * @param    LPVOID lpData -- 附加数据
     *
     * Describe  获取附加数据
     */
    void SetData(LPVOID lpData)
    {
        m_lpData=lpData;
    }

    /**
     * SRealWnd::GetData
     * @brief    获取附加数据
     * @return   返回LPVOID
     *
     * Describe  获取附加数据
     */
    const LPVOID GetData()
    {
        return m_lpData;
    }

    SOUI_ATTRS_BEGIN()
        ATTR_STRINGT(L"wndclass", m_realwndParam.m_strClassName, FALSE)
        ATTR_STRINGT(L"wndname", m_realwndParam.m_strWindowName, FALSE)
        ATTR_HEX(L"style", m_realwndParam.m_dwStyle, FALSE)
        ATTR_HEX(L"exstyle", m_realwndParam.m_dwExStyle, FALSE)
        ATTR_INT(L"init",m_bInit,FALSE)
    SOUI_ATTRS_END()
protected:
    /**
     * SRealWnd::NeedRedrawWhenStateChange
     * @brief    状态修改后重绘
     * @return   返回BOOL
     *
     * Describe  状态修改后重绘
     */
    virtual BOOL NeedRedrawWhenStateChange();
    /**
     * SRealWnd::InitFromXml
     * @brief    初始化
     * @return   返回BOOL
     *
     * Describe  初始化
     */

    virtual BOOL InitFromXml(pugi::xml_node xmlNode);

	virtual BOOL OnRelayout(const CRect &rcWnd);


    /**
     * SRealWnd::OnShowWindow
     * @brief    窗口显示
     * @param    BOOL bShow -- 是否显示
     * @param    UINT nStatus -- 状态
     *
     * Describe  窗口显示
     */
    void OnShowWindow(BOOL bShow, UINT nStatus);

    /**
     * SRealWnd::OnDestroy
     * @brief    销毁
     *
     * Describe  销毁
     */
    void OnDestroy();

    /**
     * SRealWnd::OnPaint
     * @brief    绘制
     * @param    CDCHandle dc -- 绘制设备
     *
     * Describe  消息响应函数  
     */
    void OnPaint(IRenderTarget *pRT) {}

    /**
     * SRealWnd::ShowRealWindow
     * @brief    显示窗口
     *
     * Describe  显示窗口
     */
    void ShowRealWindow();

    /**
     * SRealWnd::InitRealWnd
     * @brief    初始化窗口
     *
     * Describe  初始化窗口
     */
    BOOL InitRealWnd();

    SOUI_MSG_MAP_BEGIN()
        MSG_WM_PAINT_EX(OnPaint)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_SHOWWINDOW(OnShowWindow)
    SOUI_MSG_MAP_END()

    SRealWndParam    m_realwndParam;  /**< 窗口参数 */
    BOOL    m_bInit;  /**< 是否初始化 */

    HWND     m_hRealWnd;  /**< 窗口句柄 */
    LPVOID    m_lpData;   /**< 附加参数 */
};


}//namespace SOUI
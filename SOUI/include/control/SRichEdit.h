/**
* Copyright (C) 2014-2050 SOUI团队
* All rights reserved.
* 
* @file       SRichEdit.h
* @brief      RichEdit控件
* @version    v1.0      
* @author     soui      
* @date       2014-07-13
* 
* Describe    RichEdit控件
*/
#pragma once

#include <Imm.h>
#include <Richedit.h>
#include <TextServ.h>
#include "core/SPanel.h"
#include "core/SSingleton2.h"

namespace SOUI
{
    enum{
        MENU_CUT =    1,
        MENU_COPY,
        MENU_PASTE,
        MENU_DEL,
        MENU_SELALL,
        //---------------
        MENU_USER =20,
    };

    class SRichEdit;
    /**
    * @class      STextServiceHelper
    * @brief      
    * 
    * Describe    
    */
    class SOUI_EXP STextServiceHelper: public SSingleton2<STextServiceHelper>
    {
		SINGLETON2_TYPE(SINGLETON_TEXTSERVICEHELPER)
    public:
        /**
        * STextServiceHelper::CreateTextServices
        * @brief    
        * @param  IUnknown *punkOuter
        * @param  ITextHost *pITextHost
        * @param  IUnknown **ppUnk
        * @return 返回HRESULT  
        *
        * Describe  
        */
        HRESULT CreateTextServices( IUnknown *punkOuter, ITextHost *pITextHost, IUnknown **ppUnk );

    protected:
        /**
        * STextServiceHelper::STextServiceHelper
        * @brief    构造函数 
        *
        * Describe  构造函数 
        */
        STextServiceHelper();
        /**
        * STextServiceHelper::~STextServiceHelper
        * @brief    析构函数 
        *
        * Describe  析构函数 
        */
        ~STextServiceHelper();

        HINSTANCE    m_rich20; /**< richedit module */
        PCreateTextServices    m_funCreateTextServices;  /**< 回调函数 */
    };

    /**
    * @class      SRicheditMenuDef
    * @brief      
    * 
    * Describe    
    */
    class SOUI_EXP SRicheditMenuDef : public SSingleton2<SRicheditMenuDef>
    {
		SINGLETON2_TYPE(SINGLETON_RICHEDITMENUDEF)
    public:
        /**
        * SRicheditMenuDef::SetMenuXml
        * @brief    加载xml文件
        *
        * Describe  加载xml文件 
        */
        void SetMenuXml(pugi::xml_node xmlMenu)
        {
            m_xmlMenu.reset();
            m_xmlMenu.append_copy(xmlMenu);
        }

        /**
        * SRicheditMenuDef::GetMenuXml
        * @brief    获取xml文件 
        *
        * Describe  获取xml文件 
        */
        pugi::xml_node GetMenuXml()
        {
            return m_xmlMenu.first_child();
        }
    protected:
        pugi::xml_document  m_xmlMenu;  /**< xml文件对象 */
    };

    /**
    * @class      STextHost
    * @brief      
    * 
    * Describe    
    */
    class SOUI_EXP STextHost : public ITextHost
    {
        friend class SRichEdit;
    public:
        /**
        * STextHost::STextHost
        * @brief    构造函数 
        *
        * Describe  构造函数 
        */
        STextHost(void);
        /**
        * STextHost::~STextHost
        * @brief    析构函数 
        *
        * Describe  析构函数 
        */
        ~STextHost(void);
        /**
        * STextHost::Init
        * @brief    初始化函数
        * @param    SRichEdit* pRichEdit -- SRichEdit对象
        *
        * Describe  初始化函数 
        */
        BOOL Init(SRichEdit* pRichEdit);
        /**
        * STextHost::GetTextService
        * @brief     
        *
        * Describe   
        */
        ITextServices * GetTextService()
        {
            return pserv;
        }

        /**
        * STextHost::GetCaretPos
        * @brief     获取坐标
        *
        * Describe   获取坐标   
        */
        POINT GetCaretPos(){return m_ptCaret;}
    protected:

        /**
        * STextHost::QueryInterface
        * @brief     
        * @param     REFIID riid
        * @param     void **ppvObject
        *
        * Describe      
        */
        virtual HRESULT _stdcall QueryInterface(REFIID riid, void **ppvObject);
        /**
        * STextHost::AddRef
        * @brief     
        * @brief     
        * @return    返回UNLONG
        *
        * Describe      
        */
        virtual ULONG _stdcall AddRef(void);
        /**
        * STextHost::Release
        * @brief     
        * @return    返回UNLONG
        *
        * Describe      
        */
        virtual ULONG _stdcall Release(void);

        /**
        * STextHost::TxGetDC
        * @brief     Get the DC for the host 
        * @return    返回HDC
        *
        * Describe   Get the DC for the host    
        */
        virtual HDC   TxGetDC();

        /**
        * STextHost::TxReleaseDC
        * @brief     Release the DC gotten from the host
        * @return    返回INT
        *
        * Describe   Release the DC gotten from the host   
        */
        virtual INT   TxReleaseDC(HDC hdc);

        /**
        * STextHost::TxShowScrollBar
        * @brief     Show the scroll bar
        * @param     INT fnBar -- 
        * @param     BOOL fShow -- 
        * @return    返回BOOL
        *
        * Describe   Show the scroll bar   
        */
        virtual BOOL  TxShowScrollBar(INT fnBar, BOOL fShow);

        /**
        * STextHost::TxEnableScrollBar
        * @brief     Enable the scroll bar
        * @param     INT fuSBFlags -- 
        * @param     INT fuArrowflags -- 
        * @return    返回BOOL
        *
        * Describe   Enable the scroll bar   
        */
        virtual BOOL  TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags);

        /**
        * STextHost::TxEnableScrollBar
        * @brief     Set the scroll range
        * @param     INT fnBar -- 
        * @param     LONG nMinPos -- 
        * @param     INT nMaxPos -- 
        * @param     BOOL fRedraw -- 
        * @return    返回BOOL
        *
        * Describe   Set the scroll range   
        */
        virtual BOOL  TxSetScrollRange(
            INT fnBar,
            LONG nMinPos,
            INT nMaxPos,
            BOOL fRedraw);

        /**
        * STextHost::TxSetScrollPos
        * @brief     Set the scroll position
        * @param     INT fnBar -- 
        * @param     INT nPos -- 
        * @param     BOOL fRedraw -- 
        * @return    返回BOOL
        *
        * Describe   Set the scroll position  
        */
        virtual BOOL  TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw);

        /**
        * STextHost::TxInvalidateRect
        * @brief     InvalidateRect
        * @param     LPCRECT prc -- 
        * @param     BOOL fMode -- 
        *
        * Describe   Set the scroll position  
        */
        virtual void  TxInvalidateRect(LPCRECT prc, BOOL fMode);

        /**
        * STextHost::TxViewChange
        * @brief     Send a WM_PAINT to the window
        * @param     BOOL fUpdate -- 
        *
        * Describe   Send a WM_PAINT to the window 
        */
        virtual void  TxViewChange(BOOL fUpdate);

        /**
        * STextHost::TxCreateCaret
        * @brief     Create the caret
        * @param     HBITMAP hbmp -- caret bitmap
        * @param     INT xWidth -- caret width
        * @param     INT yHeight -- caret height
        * @return    返回BOOL
        *
        * Describe   Create the caret
        */
        virtual BOOL  TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight);

        /**
        * STextHost::TxShowCaret
        * @brief     Show the caret
        * @param     BOOL fShow -- true to show the caret
        * @return    返回BOOL
        *
        * Describe   Show the caret
        */
        virtual BOOL  TxShowCaret(BOOL fShow);

        /**
        * STextHost::TxSetCaretPos
        * @brief     Set the caret position
        * @param     INT x -- caret position:x
        * @param     INT y -- caret position:y
        * @return    返回BOOL
        *
        * Describe   Set the caret position
        */
        virtual BOOL  TxSetCaretPos(INT x, INT y);

        /**
        * STextHost::TxSetTimer
        * @brief     Create a timer with the specified timeout
        * @param     UINT idTimer -- timer ID
        * @param     UINT uTimeout -- time interval
        * @return    返回BOOL
        *
        * Describe   Create a timer with the specified timeout
        */
        virtual BOOL  TxSetTimer(UINT idTimer, UINT uTimeout);

        /**
        * STextHost::TxSetTimer
        * @brief     Destroy a timer
        * @param     UINT idTimer -- timer id
        * @return    返回BOOL
        *
        * Describe   Destroy a timer
        */
        virtual void  TxKillTimer(UINT idTimer);

        /**
        * STextHost::TxScrollWindowEx
        * @brief     Scroll the content of the specified window's client area
        * @param     INT dx --
        * @param     INT dy --
        * @param     LPCRECT lprcScroll --
        * @param     LPCRECT lprcClip --
        * @param     HRGN hrgnUpdate --
        * @param     LPRECT lprcUpdate --
        * @param     UINT fuScroll -- 
        *
        * Describe   Scroll the content of the specified window's client area
        */
        virtual void  TxScrollWindowEx (
            INT dx,
            INT dy,
            LPCRECT lprcScroll,
            LPCRECT lprcClip,
            HRGN hrgnUpdate,
            LPRECT lprcUpdate,
            UINT fuScroll);

        /**
        * STextHost::TxSetCapture
        * @brief     Get mouse capture
        * @param     BOOL fCapture --
        *
        * Describe   Get mouse capture
        */
        virtual void  TxSetCapture(BOOL fCapture);

        /**
        * STextHost::TxSetFocus
        * @brief     Set the focus to the text window
        *
        * Describe   Set the focus to the text window
        */
        virtual void  TxSetFocus();

        /**
        * STextHost::TxSetCursor
        * @brief     Establish a new cursor shape
        * @param     HCURSOR hcur --
        * @param     BOOL fText --
        *
        * Describe   Establish a new cursor shape
        */
        virtual void  TxSetCursor(HCURSOR hcur, BOOL fText);

        /**
        * STextHost::TxScreenToClient
        * @brief     Converts screen coordinates of a specified point to the client coordinates
        * @param     LPPOINT lppt --
        * @return    返回BOOL
        *
        * Describe   Converts screen coordinates of a specified point to the client coordinates
        */
        virtual BOOL  TxScreenToClient (LPPOINT lppt);

        /**
        * STextHost::TxClientToScreen
        * @brief     Converts the client coordinates of a specified point to screen coordinates
        * @param     LPPOINT lppt --
        * @return    返回BOOL
        *
        * Describe   Converts the client coordinates of a specified point to screen coordinates
        */
        virtual BOOL  TxClientToScreen (LPPOINT lppt);

        /**
        * STextHost::TxActivate
        * @brief     Request host to activate text services
        * @param     LONG * plOldState --
        * @return    返回HRESULT
        *
        * Describe   Request host to activate text services
        */
        virtual HRESULT  TxActivate( LONG * plOldState );

        /**
        * STextHost::TxDeactivate
        * @brief     Request host to deactivate text services
        * @param     LONG lNewState --
        * @return    返回HRESULT
        *
        * Describe   Request host to deactivate text services
        */
        virtual HRESULT  TxDeactivate( LONG lNewState );

        /**
        * STextHost::TxGetClientRect
        * @brief     Retrieves the coordinates of a window's client area
        * @param     LPRECT prc --
        * @return    返回HRESULT
        *
        * Describe   Retrieves the coordinates of a window's client area
        */
        virtual HRESULT   TxGetClientRect(LPRECT prc);

        /**
        * STextHost::TxGetViewInset
        * @brief     Get the view rectangle relative to the inset
        * @param     LPRECT prc --
        * @return    返回HRESULT
        *
        * Describe   Get the view rectangle relative to the inset
        */
        virtual HRESULT  TxGetViewInset(LPRECT prc);

        /**
        * STextHost::TxGetCharFormat
        * @brief     Get the default character format for the text
        * @param     const CHARFORMATW **ppCF --
        * @return    返回HRESULT
        *
        * Describe   Get the default character format for the text
        */
        virtual HRESULT  TxGetCharFormat(const CHARFORMATW **ppCF );

        /**
        * STextHost::TxGetParaFormat
        * @brief     Get the default paragraph format for the text
        * @param     const PARAFORMAT **ppPF --
        * @return    返回HRESULT
        *
        * Describe   Get the default character format for the text
        */
        virtual HRESULT  TxGetParaFormat(const PARAFORMAT **ppPF);

        /**
        * STextHost::TxGetSysColor
        * @brief     Get the background color for the window
        * @param     int nIndex --
        * @return    返回COLORREF  
        *
        * Describe   Get the background color for the window
        */
        virtual COLORREF  TxGetSysColor(int nIndex);

        /**
        * STextHost::TxGetBackStyle
        * @brief     Get the background (either opaque or transparent)
        * @param     TXTBACKSTYLE *pstyle --
        * @return    返回HRESULT
        *
        * Describe   Get the background (either opaque or transparent)
        */
        virtual HRESULT  TxGetBackStyle(TXTBACKSTYLE *pstyle);

        /**
        * STextHost::TxGetMaxLength
        * @brief     Get the maximum length for the text
        * @param     DWORD *plength --
        * @return    返回HRESULT
        *
        * Describe   Get the maximum length for the text
        */
        virtual HRESULT  TxGetMaxLength(DWORD *plength);

        /**
        * STextHost::TxGetScrollBars
        * @brief     Get the bits representing requested scroll bars for the window
        * @param     DWORD *pdwScrollBar --
        * @return    返回HRESULT
        *
        * Describe   Get the bits representing requested scroll bars for the window
        */
        virtual HRESULT  TxGetScrollBars(DWORD *pdwScrollBar);

        /**
        * STextHost::TxGetPasswordChar
        * @brief     Get the character to display for password input
        * @param     TCHAR *pch --
        * @return    返回HRESULT
        *
        * Describe   Get the character to display for password input
        */
        virtual HRESULT  TxGetPasswordChar(TCHAR *pch);

        /**
        * STextHost::TxGetAcceleratorPos
        * @brief     Get the accelerator character
        * @param     LONG *pcp --
        * @return    返回HRESULT
        *
        * Describe   Get the accelerator character
        */
        virtual HRESULT  TxGetAcceleratorPos(LONG *pcp);

        /**
        * STextHost::TxGetExtent
        * @brief     Get the native size
        * @param     LPSIZEL lpExtent --
        * @return    返回HRESULT
        *
        * Describe   Get the native size
        */
        virtual HRESULT  TxGetExtent(LPSIZEL lpExtent);

        /**
        * STextHost::OnTxCharFormatChange
        * @brief     Notify host that default character format has changed
        * @param     const CHARFORMATW * pcf --
        * @return    返回HRESULT
        *
        * Describe   Notify host that default character format has changed
        */
        virtual HRESULT  OnTxCharFormatChange(const CHARFORMATW * pcf);

        /**
        * STextHost::OnTxParaFormatChange 
        * @brief     Notify host that default paragraph format has changed
        * @param     const PARAFORMAT * ppf --
        * @return    返回HRESULT
        *
        * Describe   Notify host that default paragraph format has changed
        */
        virtual HRESULT  OnTxParaFormatChange (const PARAFORMAT * ppf);

        /**
        * STextHost::TxGetPropertyBits
        * @brief     Bulk access to bit properties
        * @param     DWORD dwMask --
        * @param     DWORD *pdwBits --
        * @return    返回HRESULT
        *
        * Describe   Bulk access to bit properties
        */
        virtual HRESULT  TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits);

        /**
        * STextHost::TxNotify
        * @brief     Notify host of events
        * @param     DWORD iNotify  --
        * @param     void *pv --
        * @return    返回HRESULT
        *
        * Describe   Bulk access to bit properties
        */
        virtual HRESULT  TxNotify(DWORD iNotify, void *pv);

        // Far East Methods for getting the Input Context
        //#ifdef WIN95_IME
        /**
        * STextHost::TxImmGetContext
        * @brief     
        * @return     返回HIMC
        *
        * Describe  
        */
        virtual HIMC  TxImmGetContext();
        /**
        * STextHost::TxImmReleaseContext
        * @brief     
        * @param     HIMC himc  --
        *
        * Describe  
        */
        virtual void  TxImmReleaseContext( HIMC himc );
        //#endif

        /**
        * STextHost::TxGetSelectionBarWidth 
        * @brief     Returns HIMETRIC size of the control bar
        * @param     LONG *plSelBarWidth  --
        *
        * Describe   Returns HIMETRIC size of the control bar
        */
        virtual HRESULT  TxGetSelectionBarWidth (LONG *plSelBarWidth);
    protected:
        BOOL m_fUiActive; /**< Whether control is inplace active */

        ULONG            cRefs;          /**< Reference Count */
        ITextServices    *pserv;         /**< pointer to Text Services object */
        SRichEdit        *m_pRichEdit;   /**< swindow for text host */ 
        POINT            m_ptCaret;
    };

    /**
    * @class      SRichEdit
    * @brief      使用Windowless Richedit实现的edit控件
    * 
    * Describe    SRichEdit
    */
    class SOUI_EXP SRichEdit :public SPanel
    {
        friend class STextHost;
    public:
        SOUI_CLASS_NAME(SRichEdit, L"richedit")

        /**
        * SRichEdit::SRichEdit
        * @brief     构造函数
        *
        * Describe   构造函数
        */
        SRichEdit();

        /**
        * SRichEdit::~SRichEdit
        * @brief     析构函数
        *
        * Describe   析构函数
        */
        virtual ~SRichEdit() {}

        DWORD SaveRtf(LPCTSTR pszFileName);

        DWORD LoadRtf(LPCTSTR pszFileName);
    public://richedit interface
        /**
        * SRichEdit::GetWindowText
        * @brief     获取窗口标题
        * @return    返回SStringT
        *
        * Describe   获取窗口标题
        */
        SStringT GetWindowText(BOOL bRawText=FALSE);
        /**
        * SRichEdit::GetWindowTextLength
        * @brief     获取窗口标题长度
        * @return    返回int
        *
        * Describe   获取窗口标题长度
        */
        int GetWindowTextLength();
        /**
        * SRichEdit::SetWindowText
        * @brief     设置窗口标题
        * @param     LPCWSTR lpszText -- 窗口标题
        *
        * Describe   设置窗口标题
        */
        void SetWindowText(LPCTSTR lpszText);
        /**
        * SRichEdit::SetSel
        * @brief     设置选中
        * @param     DWORD dwSelection -- 
        * @param     BOOL bNoScroll -- 
        *
        * Describe   设置选中
        */
        void SetSel(DWORD dwSelection, BOOL bNoScroll = FALSE);
        /**
        * SRichEdit::ReplaceSel
        * @brief     替换选中项
        * @param     LPCWSTR pszText -- 
        * @param     BOOL bCanUndo -- 
        *
        * Describe   替换选中项
        */
        void ReplaceSel(LPCWSTR pszText,BOOL bCanUndo=TRUE);
        /**
        * SRichEdit::GetWordWrap
        * @brief    
        * @return   返回BOOL  
        *
        * Describe   
        */
        BOOL GetWordWrap(void);
        /**
        * SRichEdit::SetWordWrap
        * @brief     
        * @param     BOOL fWordWrap -- 
        *
        * Describe   
        */
        void SetWordWrap(BOOL fWordWrap);

        /**
        * SRichEdit::GetReadOnly
        * @brief    判断是否只读
        * @return   返回BOOL 
        *
        * Describe  判断是否只读 
        */
        BOOL GetReadOnly();

        /**
        * SRichEdit::SetReadOnly
        * @brief     设置只读
        * @param     BOOL bReadOnly -- 是否只读
        * @return    返回BOOL 
        *
        * Describe   设置只读   
        */
        BOOL SetReadOnly(BOOL bReadOnly);

        /**
        * SRichEdit::GetLimitText
        * @brief     获取最小文本长度
        * @return    返回BOOL 
        *
        * Describe   获取最小文本长度  
        */
        LONG GetLimitText();

        /**
        * SRichEdit::SetLimitText
        * @brief     设置最小文本长度  
        * @param     int nLength -- 长度
        * @return    返回BOOL
        *
        * Describe   设置最小文本长度    
        */
        BOOL SetLimitText(int nLength);

        /**
        * SRichEdit::GetDefaultAlign
        * @brief     获取对齐方式
        * @return    返回WORD
        *
        * Describe   设置最小文本长度    
        */
        WORD GetDefaultAlign();

        /**
        * SRichEdit::SetDefaultAlign
        * @brief     设置默认对齐方式 
        * @param     WORD wNewAlign -- 对齐方式
        *
        * Describe   设置默认对齐方式   
        */
        void SetDefaultAlign(WORD wNewAlign);

        /**
        * SRichEdit::GetRichTextFlag
        * @brief     获取标志
        * @return    返回BOOL
        *
        * Describe   获取标志
        */
        BOOL GetRichTextFlag();

        /**
        * SRichEdit::SetRichTextFlag
        * @brief     设置标志
        * @param     BOOL fRich -- 标志
        *
        * Describe   设置标志
        */
        void SetRichTextFlag(BOOL fRich);

        /**
        * SRichEdit::SetRichTextFlag
        * @brief     设置标志
        * @param     BOOL fRich -- 标志
        * @return    返回LONG
        *
        * Describe   设置标志
        */
        LONG GetDefaultLeftIndent();

        /**
        * SRichEdit::SetDefaultLeftIndent
        * @brief     设置缩进
        * @param     LONG lNewIndent -- 缩进字符数
        *
        * Describe   设置缩进
        */
        void SetDefaultLeftIndent(LONG lNewIndent);

        /**
        * SRichEdit::SetSaveSelection
        * @brief     
        * @param     BOOL fSaveSelection
        * @return    返回BOOL
        *
        * Describe   
        */
        BOOL SetSaveSelection(BOOL fSaveSelection);

        /**
        * SRichEdit::SetDefaultTextColor
        * @brief     默认文本颜色
        * @param     COLORREF cr -- 颜色
        * @return    返回COLORREF 
        *
        * Describe   设置默认文本颜色   
        */
        COLORREF SetDefaultTextColor(COLORREF cr);
    protected:
        /**
        * SRichEdit::OnCreate
        * @brief    创建
        * @return   LRESULT
        * 
        * Describe  此函数是消息响应函数
        */
        int OnCreate(LPVOID);
        /**
        * SRichEdit::OnDestroy
        * @brief    销毁窗口
        * 
        * Describe  消息响应函数
        */ 
        void OnDestroy();
        /**
        * SRichEdit::OnPaint
        * @brief    绘制消息
        * @param    IRenderTarget * pRT -- 绘画设备上下文
        * 
        * Describe  此函数是消息响应函数
        */
        void OnPaint(IRenderTarget * pRT);
        /**
        * SRichEdit::OnSetFocus
        * @brief    获得焦点
        * 
        * Describe  此函数是消息响应函数
        */
        void OnSetFocus(SWND wndOld);
        /**
        * SRichEdit::OnKillFocus
        * @brief    失去焦点
        * 
        * Describe  此函数是消息响应函数
        */
        void OnKillFocus(SWND wndFocus);
        /**
        * SRichEdit::OnTimer
        * @brief    定时器
        * @param    char idEvent idEvent 
        * 
        * Describe  定时器
        */
        void OnTimer(char idEvent);
        /**
        * SRichEdit::OnTimer2
        * @brief    定时器
        * @param    UINT_PTR idEvent 
        *
        * Describe  定时器
        */
        void OnTimer2(UINT_PTR idEvent);
        /**
        * SRichEdit::OnGetDlgCode
        * @brief    获取窗口消息码
        * @return   返回UINT 
        *
        * Describe  获取窗口消息码
        */
		virtual UINT OnGetDlgCode();
        
        /**
        * GetDesiredSize
        * @brief    当没有指定窗口大小时，通过如皮肤计算窗口的期望大小
        * @param    LPRECT pRcContainer --  容器位置
        * @return   CSize 
        *
        * Describe  注意需要增加inset的大小
        */
        virtual CSize GetDesiredSize(LPCRECT pRcContainer);

        /**
        * SRichEdit::OnScroll
        * @brief    滚动条事件
        * @param    BOOL bVertical -- 是否垂直滚动
        * @param    UINT uCode -- 消息码
        * @param    int nPos -- 位置
        * @return   返回BOOL
        *
        * Describe  滚动条事件
        */
        virtual BOOL OnScroll(BOOL bVertical,UINT uCode,int nPos);
        /**
        * SRichEdit::OnSetCursor
        * @brief    设置鼠标位置
        * @param    const CPoint &pt -- 坐标
        * @return   返回BOOL
        *
        * Describe  设置鼠标位置
        */
        virtual BOOL OnSetCursor(const CPoint &pt);
        /**
        * SRichEdit::SwndProc
        * @brief    窗口处理函数
        * @param    UINT uMsg --  消息码
        * @param    WPARAM wParam  -- 
        * @param    LPARAM lParam  --
        * @param    LRESULT &lResult -- 
        * @return   返回BOOL
        *
        * Describe  窗口处理函数
        */
        virtual BOOL SwndProc(UINT uMsg,WPARAM wParam,LPARAM lParam,LRESULT & lResult);

		virtual void OnScaleChanged(int nScale);

		void OnEnable(BOOL bEnable, UINT nStatus);
        /**
        * SRichEdit::InitDefaultCharFormat
        * @brief    
        * @param    CHARFORMAT2W* pcf --
        * @param    IFont *pFont -- 
        * @return   返回HRESULT 
        *
        * Describe  
        */
        HRESULT InitDefaultCharFormat(CHARFORMAT2W* pcf,IFont *pFont=NULL);
        /**
        * SRichEdit::InitDefaultParaFormat
        * @brief    
        * @param    PARAFORMAT2* ppf
        * @return   返回HRESULT
        *
        * Describe  
        */
        HRESULT InitDefaultParaFormat(PARAFORMAT2* ppf);
        /**
        * SRichEdit::OnTxNotify
        * @brief    
        * @param    DWORD iNotify --
        * @param    LPVOID pv  --  
        * @return   返回HRESULT
        *
        * Describe  
        */
        
        virtual HRESULT OnTxNotify(DWORD iNotify,LPVOID pv);
        /**
        * SRichEdit::DefAttributeProc
        * @brief    默认属性处理函数
        * @param    const SStringW & strAttribName -- 属性名
        * @param    const SStringW & strValue -- 值
        * @param    BOOL bLoading -- 是否加载
        *
        * Describe  默认属性处理函数
        */
        virtual HRESULT DefAttributeProc(const SStringW & strAttribName,const SStringW & strValue, BOOL bLoading);
        
        /**
        * SRichEdit::OnLButtonDown
        * @brief    左键按下事件
        * @param    UINT nFlags -- 标志
        * @param    CPoint point -- 鼠标坐标
        *
        * Describe  此函数是消息响应函数
        */
        void OnLButtonDown(UINT nFlags, CPoint point);
		void OnLButtonUp(UINT nFlags, CPoint point);
        
        /**
        * SRichEdit::OnRButtonDown
        * @brief    右键按下事件
        * @param    UINT nFlags -- 标志
        * @param    CPoint point -- 鼠标坐标
        *
        * Describe  此函数是消息响应函数
        */
        void OnRButtonDown(UINT nFlags, CPoint point);
        
        /**
        * SRichEdit::OnMouseMove
        * @brief    鼠标移动
        * @param    UINT nFlags -- 标志
        * @param    CPoint point -- 鼠标坐标
        *
        * Describe  此函数是消息响应函数
        */
        void OnMouseMove(UINT nFlags, CPoint point);

		BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
        
        /**
        * SRichEdit::OnKeyDown
        * @brief    键盘按下事件
        * @param    UINT nChar -- 按键对应的码值 
        * @param    UINT nRepCnt -- 重复次数
        * @param    UINT nFlags -- 标志
        * 
        * Describe  此函数是消息响应函数
        */ 
        void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
        
        /**
        * SRichEdit::OnButtonClick
        * @brief    通用鼠标事件
        * @param    UINT uMsg -- msg
        * @param    WPARAM wParam -- wParam
        * @param    LPARAM lParam -- lParam
        * @return   返回LRESULT  
        *
        * Describe  此函数是消息响应函数
        */
        LRESULT OnButtonClick(UINT uMsg,WPARAM wParam,LPARAM lParam);

        /**
        * SRichEdit::OnChar
        * @brief    字符事件
        * @param    UINT nChar -- 按键对应的码值 
        * @param    UINT nRepCnt -- 重复次数
        * @param    UINT nFlags -- 标志
        * 
        * Describe  此函数是消息响应函数
        */
        void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
        /**
        * SRichEdit::OnSetFont
        * @brief    设置字体
        * @param    IFont * font -- 字体
        * @param    BOOL bRedraw -- 是否重绘
        *
        * Describe  设置字体
        */
        void OnSetFont(IFont * font, BOOL bRedraw);
        /**
        * SRichEdit::OnSetText
        * @brief    设置文本
        * @param    UINT uMsg -- 消息码
        * @param    WPARAM wparam -- 
        * @param    LPARAM lparam -- 
        * @return   返回LRESULT  
        *
        * Describe  设置文本
        */
        LRESULT OnSetText(UINT uMsg,WPARAM wparam,LPARAM lparam);
        /**
        * SRichEdit::OnSetCharFormat
        * @brief    
        * @param    UINT uMsg -- 消息码
        * @param    WPARAM wparam -- 
        * @param    LPARAM lparam -- 
        * @return   返回LRESULT  
        *
        * Describe  
        */
        LRESULT OnSetCharFormat(UINT uMsg, WPARAM wParam, LPARAM lParam);
        /**
        * SRichEdit::OnSetParaFormat
        * @brief    
        * @param    UINT uMsg -- 消息码
        * @param    WPARAM wparam -- 
        * @param    LPARAM lparam -- 
        * @return   返回LRESULT  
        *
        * Describe  
        */
        LRESULT OnSetParaFormat(UINT uMsg, WPARAM wParam, LPARAM lParam);
        /**
        * SRichEdit::OnSetReadOnly
        * @brief    设置只读
        * @param    UINT uMsg -- 消息码
        * @param    WPARAM wparam -- 
        * @param    LPARAM lparam -- 
        * @return   返回LRESULT  
        *
        * Describe  设置只读
        */
        LRESULT OnSetReadOnly(UINT uMsg, WPARAM wParam, LPARAM lParam);
        /**
        * SRichEdit::OnSetLimitText
        * @brief    设置文本长度
        * @param    UINT uMsg -- 消息码
        * @param    WPARAM wparam -- 
        * @param    LPARAM lparam -- 
        * @return   返回LRESULT  
        *
        * Describe  设置文本长度
        */
        LRESULT OnSetLimitText(UINT uMsg, WPARAM wParam, LPARAM lParam);
        /**
        * SRichEdit::OnNcCalcSize
        * @brief    计算非客户大小
        * @param    BOOL bCalcValidRects -- 
        * @param    LPARAM lparam -- 
        * @return   返回LRESULT  
        *
        * Describe  消息响应函数
        */
        LRESULT OnNcCalcSize(BOOL bCalcValidRects, LPARAM lParam);
        /**
        * SRichEdit::OnEnableDragDrop
        * @brief    是否允许拖拽
        * @param    BOOL bEnable
        *
        * Describe  是否允许拖拽
        */
        void OnEnableDragDrop(BOOL bEnable);

        virtual pugi::xml_node GetMenuTemplate() const
        {
            return SRicheditMenuDef::getSingleton().GetMenuXml();
        }
        
        void OnShowWindow(BOOL bShow, UINT nStatus);

    protected:
        SOUI_MSG_MAP_BEGIN()
            MSG_WM_CREATE(OnCreate)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_PAINT_EX(OnPaint)
            MSG_WM_NCCALCSIZE(OnNcCalcSize)
            MSG_WM_SETFOCUS_EX(OnSetFocus)
            MSG_WM_KILLFOCUS_EX(OnKillFocus)
            MSG_WM_TIMER_EX(OnTimer)
            MSG_WM_TIMER2(OnTimer2)
            MSG_WM_LBUTTONDOWN(OnLButtonDown)
			MSG_WM_LBUTTONUP(OnLButtonUp)
            MSG_WM_RBUTTONDOWN(OnRButtonDown)
            MSG_WM_MOUSEMOVE(OnMouseMove)
			MSG_WM_MOUSEWHEEL(OnMouseWheel)
            MSG_WM_KEYDOWN(OnKeyDown)
            MSG_WM_CHAR(OnChar)
            MSG_WM_SETFONT_EX(OnSetFont)
            MSG_WM_SHOWWINDOW(OnShowWindow)
            MESSAGE_HANDLER_EX(WM_LBUTTONDBLCLK,OnButtonClick)
            MESSAGE_HANDLER_EX(WM_SETTEXT,OnSetText)
            MESSAGE_HANDLER_EX(EM_SETPARAFORMAT,OnSetParaFormat)
            MESSAGE_HANDLER_EX(EM_SETCHARFORMAT,OnSetCharFormat)
            MESSAGE_HANDLER_EX(EM_SETREADONLY,OnSetReadOnly)
            MESSAGE_HANDLER_EX(EM_EXLIMITTEXT,OnSetLimitText)
        SOUI_MSG_MAP_END()

        SOUI_ATTRS_BEGIN()
            ATTR_INT(L"style",m_dwStyle,FALSE)
            ATTR_INT(L"maxBuf",m_cchTextMost,FALSE)
            ATTR_INT(L"transparent",m_fTransparent,FALSE)
            ATTR_INT(L"rich",m_fRich,FALSE)
            ATTR_INT(L"vertical",m_fVertical,FALSE)
            ATTR_INT(L"wordWrap",m_fWordWrap,FALSE)
            ATTR_INT(L"allowBeep",m_fAllowBeep,FALSE)
            ATTR_INT(L"autoWordSel",m_fEnableAutoWordSel,FALSE)
            ATTR_INT(L"vcenter",m_fSingleLineVCenter,FALSE)
            ATTR_RECT(L"inset",m_rcInsetPixel,FALSE)
            ATTR_CUSTOM(L"colorText",OnAttrTextColor)
            ATTR_CUSTOM(L"rtf",OnAttrRTF)
            ATTR_CUSTOM(L"align",OnAttrAlign)
            ATTR_CUSTOM(L"notifyChange",OnAttrNotifyChange)
			ATTR_BOOL(L"wantTab",m_fWantTab,FALSE)
            //更多属性见SRichEdit::DefAttributeProc代码
            //包括：hscrollBar，vscrollBar，autoHscroll，autoVscroll，multiLines，readOnly，wantReturn，password
            //      number,passwordChar,enableDragdrop,autoSel
        SOUI_ATTRS_END()

    protected:
        /**
        * SRichEdit::OnAttrTextColor
        * @brief    设置文本颜色
        * @param    const SStringW &  strValue -- 字符串
        * @param    BOOL bLoading -- 是否加载
        * @return   返回HRESULT 
        *
        * Describe  设置文本颜色
        */
        HRESULT OnAttrTextColor(const SStringW &  strValue,BOOL bLoading);

        HRESULT OnAttrRTF(const SStringW &  strValue,BOOL bLoading);

        HRESULT OnAttrAlign(const SStringW &  strValue,BOOL bLoading);

        HRESULT OnAttrNotifyChange(const SStringW &  strValue,BOOL bLoading);
        
        CHARFORMAT2W   m_cfDef;              /**< Default character format  */
        PARAFORMAT2    m_pfDef;              /**< Default paragraph format  */
        DWORD m_cchTextMost;                 /**< Maximize Characters       */
        TCHAR m_chPasswordChar;              /**< Password character        */
        LONG         m_lAccelPos;            /**< Accelerator position      */
        SIZEL        m_sizelExtent;          /**< Extent array              */
        CRect        m_rcInset;              /**< inset margin              */
        CRect        m_rcInsetPixel;         /**< inset margin in pixel     */
        int          m_nFontHeight;          /**< 单行文字输出高度          */
        DWORD        m_dwStyle;

        UINT    m_fEnableAutoWordSel    :1;    /**< enable Word style auto word selection?  */
        UINT    m_fWordWrap            :1;     /**< Whether control should word wrap */
        UINT    m_fRich                :1;     /**< Whether control is rich text */
        UINT    m_fSaveSelection        :1;    /**< Whether to save the selection when inactive */
        UINT    m_fTransparent        :1;      /**< Whether control is transparent */
        UINT    m_fVertical            :1;     /**< Whether control is layout following vertical */
        UINT    m_fAllowBeep        :1;        /**< Whether message beep is allowed in the control */
        UINT    m_fWantTab            :1;      /**< Whether control will deal with tab input */
        UINT    m_fSingleLineVCenter:1;        /**< Whether control that is single line will be vertical centered */
        UINT    m_fScrollPending    :1;        /**< Whether scroll is activated by richedit or by panelex */
        UINT    m_fEnableDragDrop    :1;       /**< 允许在该控件中使用拖放 */
        UINT    m_fAutoSel              :1;    /**< 有焦点时自动全选 */
        UINT    m_fNotifyChange         :1;    /**< receive re_notify when data changed */
        
        BYTE    m_byDbcsLeadByte; /**< DBCS输入时的中文头字节*/
        SStringW m_strRtfSrc;     /**< 在XML中指定的RTF数据源*/
        STextHost    *m_pTxtHost; /**< Host of Richedit*/
		HIMC m_hCurIMC;			// 当前输入法 
    };

    /**
    * @class      SEdit 
    * @brief      简单edit控件
    * 
    * Describe    
    */
    class SOUI_EXP SEdit : public SRichEdit
    {
        SOUI_CLASS_NAME(SEdit, L"edit")
    public:
        /**
        * SEdit::SEdit
        * @brief    构造函数 
        *
        * Describe  构造函数
        */
        SEdit();
        
        SStringT GetCueText(BOOL bRawText=FALSE) const;
        
        SOUI_ATTRS_BEGIN()
            ATTR_COLOR(L"cueColor",m_crCue,TRUE)
            ATTR_I18NSTRT(L"cueText",m_strCue,TRUE)
        SOUI_ATTRS_END()

    protected:
    
		virtual HRESULT OnLanguageChanged();

        /**
        * SEdit::OnPaint
        * @brief    绘制消息
        * @param    IRenderTarget * pRT -- 绘画设备上下文
        * 
        * Describe  此函数是消息响应函数
        */
        void OnPaint(IRenderTarget * pRT);
        /**
        * SEdit::OnSetFocus
        * @brief    获得焦点
        * 
        * Describe  此函数是消息响应函数
        */
        void OnSetFocus(SWND wndOld);
        /**
        * SEdit::OnKillFocus
        * @brief    失去焦点
        * 
        * Describe  此函数是消息响应函数
        */
        void OnKillFocus(SWND wndFocus);

        SOUI_MSG_MAP_BEGIN()
            MSG_WM_PAINT_EX(OnPaint)
            MSG_WM_SETFOCUS_EX(OnSetFocus)
            MSG_WM_KILLFOCUS_EX(OnKillFocus)
        SOUI_MSG_MAP_END()

        COLORREF    m_crCue;
        STrText    m_strCue;
    };
}//namespace SOUI

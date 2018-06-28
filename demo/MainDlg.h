/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       MainDlg.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/15
* 
* Describe    主窗口实现
*/

#pragma once

using namespace SOUI;

#include "magnet/MagnetFrame.h"
#include "ThreadObject.h"
#include "skin/SDemoSkin.h"
#include "../../controls.extend/SMcListViewEx/STabCtrlHeaderBinder.h"

extern UINT g_dwSkinChangeMessage;
//演示使用SNotifyCenter的异步事件
class EventThread : public TplEventArgs<EventThread>
{
	SOUI_CLASS_NAME(EventThread,L"on_event_thread")
public:
	EventThread(SObject *pSender):TplEventArgs<EventThread>(pSender){}
	enum{EventID=EVT_EXTERNAL_BEGIN+30000};
	
	int nData;
};

//演示使用SNotifyCenter的同步事件
class EventThreadStart : public TplEventArgs<EventThreadStart>
{
	SOUI_CLASS_NAME(EventThreadStart,L"on_event_thread_start")
public:
	EventThreadStart(SObject *pSender):TplEventArgs<EventThreadStart>(pSender){}
	enum{EventID=EVT_EXTERNAL_BEGIN+30001};
};

//演示使用SNotifyCenter的同步事件
class EventThreadStop : public TplEventArgs<EventThreadStop>
{
	SOUI_CLASS_NAME(EventThreadStop,L"on_event_thread_stop")
public:
	EventThreadStop(SObject *pSender):TplEventArgs<EventThreadStop>(pSender){}
	enum{EventID=EVT_EXTERNAL_BEGIN+30002};
};


/**
* @class      CMainDlg
* @brief      主窗口实现
* 
* Describe    非模式窗口从SHostWnd派生，模式窗口从SHostDialog派生
*/
class CMainDlg : public SHostWnd
			   , public CMagnetFrame	//磁力吸附
			   //, public ISetSkinHandler	//皮肤处理
			   , public CThreadObject	//线程对象
			   , public TAutoEventMapReg<CMainDlg>//通知中心自动注册
			   , public ISetOrLoadSkinHandler
{
public:

    /**
     * CMainDlg
     * @brief    构造函数
     * Describe  使用uires.idx中定义的maindlg对应的xml布局创建UI
     */    
    CMainDlg() : SHostWnd(UIRES.LAYOUT.maindlg),m_bLayoutInited(FALSE)
    {
    } 

protected:
    //////////////////////////////////////////////////////////////////////////
    //  Window消息响应函数
    LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
    void OnDestory();

	void OnClose()
	{
		AnimateHostWindow(200,AW_CENTER|AW_HIDE);
        DestroyWindow();
	}
	void OnMaximize()
	{
		GetNative()->SendMessage(WM_SYSCOMMAND,SC_MAXIMIZE);
	}
	void OnRestore()
	{
		GetNative()->SendMessage(WM_SYSCOMMAND,SC_RESTORE);
	}
	void OnMinimize()
	{
		GetNative()->SendMessage(WM_SYSCOMMAND,SC_MINIMIZE);
	}

	void OnSize(UINT nType, CSize size)
	{
		SetMsgHandled(FALSE);   //这一行很重要，保证消息继续传递给SHostWnd处理，当然也可以用SHostWnd::OnSize(nType,size);代替，但是这里使用的方法更简单，通用
		if(!m_bLayoutInited) return;
		if(nType==SIZE_MAXIMIZED)
		{
			FindChildByID(3)->SetVisible(TRUE);
			FindChildByID(2)->SetVisible(FALSE);
		}else if(nType==SIZE_RESTORED)
		{
			FindChildByID(3)->SetVisible(FALSE);
			FindChildByID(2)->SetVisible(TRUE);
		}
	}
    
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
    
    //演示如何在应用层使用定时器
	void OnTimer(UINT_PTR idEvent);

    //DUI菜单响应函数
    void OnCommand(UINT uNotifyCode, int nID, HWND wndCtl);
        

protected:
    //virtual void OnSetSkin(int iSkin);

    //////////////////////////////////////////////////////////////////////////
    // SOUI事件处理函数
	//演示屏蔽指定edit控件的右键菜单
	BOOL OnEditMenu(CPoint pt)
	{
		return TRUE;
	}

    //按钮控件的响应
    void OnBtnSelectGIF();
    void OnBtnMenu();

    void OnBtnInsertGif2RE();
    void OnBtnAppendMsg();
    void OnBtnRtfSave();
    void OnBtnRtfOpen();
    
    void OnBtnHideTest();
    void OnBtnMsgBox();

    void OnBtnWebkitGo();
    void OnBtnWebkitBackward();
    void OnBtnWebkitForeward();
    void OnBtnWebkitRefresh();

	void OnBtnLRC();
    
	bool LoadSkin();

	//演示如何使用subscribeEvent来不使用事件映射表实现事件响应
    bool OnListHeaderClick(EventArgs *pEvt);

        
    void OnChromeTabNew(EventArgs *pEvt);

    void OnTabPageRadioSwitch(EventArgs *pEvt);
    
    void OnBtnFileWnd();

    void OnUrlReNotify(EventArgs *pEvt);
    
    void OnMclvCtxMenu(EventArgs *pEvt);
    
    //处理模拟菜单中控件的事件
    void OnMenuSliderPos(EventArgs *pEvt);
    
    void OnMatrixWindowReNotify(EventArgs *pEvt);

    void OnBtnCreateChildren();
    void OnBtnClock();
    void OnBtnSkin();
	void OnInitListBox();

	void OnBtnTip();

	virtual UINT Run();

	//Event Sender ID
	enum {SENDER_ID = 30000};
	virtual int GetID() const {return SENDER_ID;}

	void OnBtnStartNotifyThread();
	void OnBtnStopNotifyThread();

	bool OnEventThreadStart(EventArgs *e);
	bool OnEventThreadStop(EventArgs *e);
	bool OnEventThread(EventArgs *e);
	void OnBtnOpenWrapContent();
	
	void OnCbxInterpolotorChange(EventArgs *e);

	void OnEventPath(EventArgs *e);

	HRESULT OnSkinChangeMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL bHandled);
    //UI控件的事件及响应函数映射表
	EVENT_MAP_BEGIN()
		EVENT_HANDLER(EventPath::EventID,OnEventPath)
		EVENT_ID_HANDLER(R.id.cbx_interpolator,EventCBSelChange::EventID,OnCbxInterpolotorChange)
		EVENT_ID_COMMAND(1, OnClose)
		EVENT_ID_COMMAND(2, OnMaximize)
		EVENT_ID_COMMAND(3, OnRestore)
		EVENT_ID_COMMAND(5, OnMinimize)
		EVENT_ID_COMMAND(R.id.btn_tip,OnBtnTip)
		EVENT_NAME_CONTEXTMENU(L"edit_1140",OnEditMenu)
		EVENT_NAME_COMMAND(L"btn_msgbox",OnBtnMsgBox)
		
		//<--在新版本的uiresbuilder生成的resource.h中定义了R.id, R.name两个对象，可以使用如下方式来关联变量。
		EVENT_ID_COMMAND(R.id.btnSelectGif,OnBtnSelectGIF)
        EVENT_ID_RANGE_HANDLER(R.id.radio2_1,R.id.radio2_6,EventSwndStateChanged::EventID,OnTabPageRadioSwitch)    //10000-10005是XML中定义的radio2的ID
        EVENT_NAME_COMMAND(R.name.btn_menu,OnBtnMenu)
        EVENT_NAME_COMMAND(R.name.btn_webkit_go,OnBtnWebkitGo)
        EVENT_ID_COMMAND(R.id.btn_createchildren,OnBtnCreateChildren)
        EVENT_ID_COMMAND(R.id.btn_clock,OnBtnClock)
		EVENT_ID_COMMAND(R.id.btn_init_listbox,OnInitListBox)
		EVENT_ID_COMMAND(R.id.btn_skin,OnBtnSkin)
		EVENT_ID_COMMAND(R.id.btn_start_notify_thread,OnBtnStartNotifyThread)
		EVENT_ID_COMMAND(R.id.btn_stop_notify_thread,OnBtnStopNotifyThread)
		EVENT_ID_COMMAND(R.id.btn_open_wrap_content,OnBtnOpenWrapContent)
        //-->
		//<--通知中心事件
		EVENT_ID_HANDLER(SENDER_ID,EventThreadStart::EventID,OnEventThreadStart)
		EVENT_ID_HANDLER(SENDER_ID,EventThreadStop::EventID,OnEventThreadStop)
		EVENT_ID_HANDLER(SENDER_ID,EventThread::EventID,OnEventThread)
		//-->

        EVENT_NAME_COMMAND(L"btn_webkit_back",OnBtnWebkitBackward)
        EVENT_NAME_COMMAND(L"btn_webkit_fore",OnBtnWebkitForeward)
        EVENT_NAME_COMMAND(L"btn_webkit_refresh",OnBtnWebkitRefresh)
        EVENT_NAME_COMMAND(L"btn_hidetst",OnBtnHideTest)
        EVENT_NAME_COMMAND(L"btn_insert_gif",OnBtnInsertGif2RE)
        EVENT_NAME_COMMAND(L"btn_append_msg",OnBtnAppendMsg)
        EVENT_NAME_COMMAND(L"btn_richedit_save",OnBtnRtfSave)
        EVENT_NAME_COMMAND(L"btn_richedit_open",OnBtnRtfOpen)
		EVENT_NAME_COMMAND(L"btn_lrc",OnBtnLRC)
        EVENT_NAME_HANDLER(L"chromeTab",EVT_CHROMETAB_NEW,OnChromeTabNew)
        EVENT_NAME_COMMAND(L"btn_filewnd",OnBtnFileWnd)
        EVENT_NAME_HANDLER(L"edit_url",EVT_RE_NOTIFY,OnUrlReNotify)
        EVENT_NAME_HANDLER(L"mclv_test",EVT_CTXMENU,OnMclvCtxMenu)
        EVENT_NAME_HANDLER(L"edit_rotate",EVT_RE_NOTIFY,OnMatrixWindowReNotify)
        EVENT_NAME_HANDLER(L"edit_scale",EVT_RE_NOTIFY,OnMatrixWindowReNotify)
        EVENT_NAME_HANDLER(L"edit_skew",EVT_RE_NOTIFY,OnMatrixWindowReNotify)
        EVENT_NAME_HANDLER(L"edit_translate",EVT_RE_NOTIFY,OnMatrixWindowReNotify)
        
        EVENT_NAME_HANDLER(L"menu_slider",EventSliderPos::EventID,OnMenuSliderPos)
	EVENT_MAP_END()	

    //HOST消息及响应函数映射表
	BEGIN_MSG_MAP_EX(CMainDlg)
		MSG_WM_CREATE(OnCreate)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_DESTROY(OnDestory)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_SIZE(OnSize)
		MSG_WM_COMMAND(OnCommand)
		MSG_WM_TIMER(OnTimer)
		MESSAGE_HANDLER(g_dwSkinChangeMessage, OnSkinChangeMessage)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()

protected:
    //////////////////////////////////////////////////////////////////////////
    //  辅助函数
    void InitListCtrl();

	virtual bool SaveSkin(SkinType skinType, SkinSaveInf & skinSaveInf);

private:
	BOOL			m_bLayoutInited;/**<UI完成布局标志 */
	HWND			m_hSetSkinWnd;
	STabCtrlHeaderBinder* m_pTabBinder;
	STabCtrlHeaderBinder* m_pTabBinder2;
};

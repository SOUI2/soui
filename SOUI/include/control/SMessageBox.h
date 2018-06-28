/**
* Copyright (C) 2014-2050 SOUI团队
* All rights reserved.
* 
* @file       SMessageBox.h
* @brief      
* @version    v1.0      
* @author     soui      
* @date       2014-07-06
* 
* Describe     
*/
#pragma once
#include "core/shostdialog.h"

namespace SOUI
{
    //下面是几个在msgbox模板中必须指定的ID。
    #define NAME_MSGBOX_BTNSWITCH       L"btnSwitch"    //一个包含3个页面的tabctrl
    #define NAME_MSGBOX_TEXT            L"msgtext"      //文本控件，只需要指定两个坐标
    #define NAME_MSGBOX_TITLE           L"msgtitle"     //标题ID
    #define NAME_MSGBOX_ICON            L"msgicon"      //图标显示控件
    #define NAME_MSGBOX_BTN1            L"button1st"    //第1个按钮ID，按钮ID在显示时会自动修改为如IDOK,IDCANCEL这样的ID。
    #define NAME_MSGBOX_BTN2            L"button2nd"    //第2个按钮ID
    #define NAME_MSGBOX_BTN3            L"button3rd"    //第3个按钮ID
    
    BOOL SOUI_EXP SetMsgTemplate(pugi::xml_node xmlNode);

    pugi::xml_node SOUI_EXP GetMsgTemplate();

    INT_PTR SOUI_EXP SMessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);


    /** 
    * @class     SMessageBoxImpl
    * @brief     消息窗口模板
    *
    * Describe   msgbox的消息处理对象，如果需要更加个性化的msgbox，可以派生该类。
    */    
    class SOUI_EXP SMessageBoxImpl:public SHostDialog
    {
    public:
        /**
        * SMessageBoxImpl::SMessageBoxImpl
        * @brief    构造函数
        *
        * Describe  构造函数  
        */
        SMessageBoxImpl();
        /**
        * SMessageBoxImpl::MessageBox
        * @brief    提示框
        * @param    HWND hWnd -- 窗口句柄
        * @param    LPCTSTR lpText -- 消息内容
        * @param    LPCTSTR lpCaption -- 窗口标题
        * @param    UINT uType -- 窗口类型
        *
        * Describe  提示框
        */        
        INT_PTR MessageBox( HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType );
    protected:
        /**
         * OnInitDialog
         * @brief    MsgBox初始化
         * @param    HWND wnd -- host窗口句柄 
         * @param    LPARAM lInitParam --  不使用
         * @return   BOOL 不使用
         *
         * Describe  
         */
        BOOL OnInitDialog(HWND wnd, LPARAM lInitParam);
        
        /**
        * SMessageBoxImpl::OnSetIcon
        * @brief    设置图标
        * @param    UINT uType -- 类型
        * @return   返回BOOL
        *
        * Describe  设置图标 可以重载该方法来显示自定义的图标
        */ 
        virtual BOOL OnSetIcon(UINT uType);
        
        virtual SStringT OnGetButtonText(int nBtnID) const;
        
        /**
        * SMessageBoxImpl::OnBtnClick
        * @brief    按钮响应函数
        * @param    int uID -- 控件ID
        *
        * Describe  消息响应函数
        */ 
        void OnBtnClick(int uID)
        {
            EndDialog(uID);
        }


        EVENT_MAP_BEGIN()
            EVENT_ID_COMMAND_RANGE(IDOK,IDNO, OnBtnClick)
        EVENT_MAP_END()

        BEGIN_MSG_MAP_EX(SMessageBoxImpl)
            MSG_WM_INITDIALOG(OnInitDialog)
            CHAIN_MSG_MAP(SHostDialog)
            REFLECT_NOTIFICATIONS_EX()
        END_MSG_MAP()
    };

}//end of namespace 


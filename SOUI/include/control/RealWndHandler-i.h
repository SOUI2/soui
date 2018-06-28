/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       RealWndHandler-i.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/14
* 
* Describe     提供RealWnd的创建及销毁管理
*/
#pragma  once
#include <unknown/obj-ref-i.h>

namespace SOUI
{
    class SRealWnd;
    /** 
    * @struct     IRealWndHandler
    * @brief     
    *
    * Describe   
    */
    struct IRealWndHandler : public IObjRef
    {
        /**
        * SRealWnd::OnRealWndCreate
        * @brief    窗口创建
        * @param    SRealWnd *pRealWnd -- 窗口指针
        *
        * Describe  窗口创建
        */    
        virtual HWND OnRealWndCreate(SRealWnd *pRealWnd)=NULL;

        /**
        * SRealWnd::OnRealWndDestroy
        * @brief    销毁窗口
        * @param    SRealWnd *pRealWnd -- 窗口指针
        *
        * Describe  销毁窗口
        */
        virtual void OnRealWndDestroy(SRealWnd *pRealWnd)=NULL;

        /**
        * SRealWnd::OnRealWndInit
        * @brief    初始化窗口
        * @param    SRealWnd *pRealWnd -- 窗口指针
        * @return   BOOL -- FALSE:交由系统处理，TRUE:用户处理
        *
        * Describe  初始化窗口
        */
        virtual BOOL OnRealWndInit(SRealWnd *pRealWnd){return FALSE;}

        /**
        * SRealWnd::OnRealWndSize
        * @brief    调整窗口大小
        * @param    SRealWnd *pRealWnd -- 窗口指针
        * @return   BOOL -- FALSE：交由SOUI处理; TRUE:用户管理窗口的移动
        *
        * Describe  调整窗口大小,该方法由OnRealWndPosition代替
        */
        virtual BOOL OnRealWndSize(SRealWnd *pRealWnd){return FALSE;}

		/**
		* SRealWnd::OnRealWndPosition
		* @brief    调整窗口位置
		* @param    SRealWnd *pRealWnd -- 窗口指针
		* @return   BOOL -- FALSE：交由SOUI处理; TRUE:用户管理窗口的移动
		*
		* Describe  调整窗口大小
		*/
		virtual BOOL OnRealWndPosition(SRealWnd *pRealWnd, const CRect &rcWnd) { return FALSE; }
    };

}//namespace SOUI
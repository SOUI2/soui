#pragma once

#include <unknown/obj-ref-impl.hpp>

namespace SOUI
{
    class SRealWndHandler_Scintilla :public TObjRefImpl2<IRealWndHandler,SRealWndHandler_Scintilla>
    {
    public:
        SRealWndHandler_Scintilla(void);
        ~SRealWndHandler_Scintilla(void);

        /**
         * SRealWnd::OnRealWndCreate
         * @brief    创建真窗口
         * @param    SRealWnd * pRealWnd --  窗口指针
         * @return   HWND -- 创建出来的真窗口句柄
         * Describe  
         */    
        virtual HWND OnRealWndCreate(SRealWnd *pRealWnd);

        /**
        * SRealWnd::OnRealWndDestroy
        * @brief    销毁窗口
        * @param    SRealWnd *pRealWnd -- 窗口指针
        *
        * Describe  销毁窗口
        */
        virtual void OnRealWndDestroy(SRealWnd *pRealWnd);

        /**
        * SRealWnd::OnRealWndInit
        * @brief    初始化窗口
        * @param    SRealWnd *pRealWnd -- 窗口指针
        *
        * Describe  初始化窗口
        */
        virtual BOOL OnRealWndInit(SRealWnd *pRealWnd);

        /**
        * SRealWnd::OnRealWndSize
        * @brief    调整窗口大小
        * @param    SRealWnd *pRealWnd -- 窗口指针
        * @return   BOOL -- TRUE:用户管理窗口的移动；FALSE：交由SOUI自己管理。
        * Describe  调整窗口大小, 从pRealWnd中获得窗口位置。
        */
        virtual BOOL OnRealWndSize(SRealWnd *pRealWnd);

		virtual BOOL OnRealWndPosition(SRealWnd *pRealWnd, const CRect &rcWnd);

	};

}

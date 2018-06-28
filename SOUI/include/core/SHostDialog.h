/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       SHostDialog.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/02
* 
* Describe    SOUI的Dialog模块
*/

#pragma once

#include "shostwnd.h"
#include "smsgloop.h"

namespace SOUI
{
    class SOUI_EXP SHostDialog : public SHostWnd
    {
        SOUI_CLASS_NAME(SHostDialog,L"hostdlg")
    public:
        SHostDialog(LPCTSTR pszXmlName);
        ~SHostDialog(void);
        
        virtual INT_PTR DoModal(HWND hParent=NULL);
        
        virtual void EndDialog(INT_PTR nResult);

    protected:
        void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
        void OnOK();
        void OnCancel();
        virtual SMessageLoop * GetMsgLoop(){return m_MsgLoop;}
        
        EVENT_MAP_BEGIN()
            EVENT_ID_COMMAND(IDOK,OnOK)
            EVENT_ID_COMMAND(IDCANCEL,OnCancel)
        EVENT_MAP_END()

        BEGIN_MSG_MAP_EX(SHostDialog)
            MSG_WM_CLOSE(OnCancel)
            MSG_WM_KEYDOWN(OnKeyDown)
            CHAIN_MSG_MAP(SHostWnd)
            REFLECT_NOTIFICATIONS_EX()
        END_MSG_MAP()

        INT_PTR m_nRetCode;
        
        SMessageLoop * m_MsgLoop;
    };

}

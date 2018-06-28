#pragma once

#define SOUI_MSG_MAP_BEGIN()                                       \
protected:                                                          \
    virtual BOOL ProcessSwndMessage(                              \
    UINT uMsg, WPARAM wParam,                        \
    LPARAM lParam, LRESULT& lResult)                            \
    {

#define SOUI_MSG_MAP_END()                                        \
    if (!IsMsgHandled())                                        \
    return __super::ProcessSwndMessage(                   \
    uMsg, wParam, lParam, lResult);                     \
    return TRUE;                                                \
    }

#define WND_MSG_MAP_END_BASE()                                    \
    return SwndProc(uMsg,wParam,lParam,lResult);               \
    }


// BOOL OnEraseBkgnd(SOUI::IRenderTarget * pRT)
#define MSG_WM_ERASEBKGND_EX(func) \
    if (uMsg == WM_ERASEBKGND) \
    { \
    SetMsgHandled(TRUE); \
    lResult = (LRESULT)func((SOUI::IRenderTarget *)wParam); \
    if(IsMsgHandled()) \
    return TRUE; \
    }

// void OnPaint(SOUI::IRenderTarget * pRT)
#define MSG_WM_PAINT_EX(func) \
    if (uMsg == WM_PAINT) \
    { \
    SetMsgHandled(TRUE); \
    func((SOUI::IRenderTarget *)wParam); \
    lResult = 0; \
    if(IsMsgHandled()) \
    return TRUE; \
    }

// void OnNcPaint(SOUI::IRenderTarget * pRT)
#define MSG_WM_NCPAINT_EX(func) \
    if (uMsg == WM_NCPAINT) \
{ \
    SetMsgHandled(TRUE); \
    func((SOUI::IRenderTarget *)wParam); \
    lResult = 0; \
    if(IsMsgHandled()) \
    return TRUE; \
}

// void OnSetFont(IFont *pFont, BOOL bRedraw)
#define MSG_WM_SETFONT_EX(func) \
    if (uMsg == WM_SETFONT) \
    { \
    SetMsgHandled(TRUE); \
    func((IFont*)wParam, (BOOL)LOWORD(lParam)); \
    lResult = 0; \
    if(IsMsgHandled()) \
    return TRUE; \
    }

// void OnSetFocus(SWND wndOld)
#define MSG_WM_SETFOCUS_EX(func) \
    if (uMsg == WM_SETFOCUS) \
{ \
    SetMsgHandled(TRUE); \
    func((SWND)wParam); \
    lResult = 0; \
    if(IsMsgHandled()) \
    return TRUE; \
}

// void OnSetFocus(SWND wndOld,CFocusManager::FocusChangeReason)
#define MSG_WM_SETFOCUS_EX2(func) \
	if (uMsg == WM_SETFOCUS) \
{ \
	SetMsgHandled(TRUE); \
	func((SWND)wParam,(CFocusManager::FocusChangeReason)lParam); \
	lResult = 0; \
	if(IsMsgHandled()) \
	return TRUE; \
}


// void OnKillFocus(SWND wndFocus)
#define MSG_WM_KILLFOCUS_EX(func) \
    if (uMsg == WM_KILLFOCUS) \
{ \
    SetMsgHandled(TRUE); \
    func((SWND)wParam); \
    lResult = 0; \
    if(IsMsgHandled()) \
    return TRUE; \
}

// void OnNcMouseHover(int nFlag,CPoint pt)
#define MSG_WM_NCMOUSEHOVER(func) \
    if(uMsg==WM_NCMOUSEHOVER)\
{\
    SetMsgHandled(TRUE); \
    func(wParam,CPoint(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam))); \
    lResult = 0; \
    if(IsMsgHandled()) \
    return TRUE; \
}

// void OnNcMouseLeave()
#define MSG_WM_NCMOUSELEAVE(func) \
    if(uMsg==WM_NCMOUSELEAVE)\
{\
    SetMsgHandled(TRUE); \
    func(); \
    lResult = 0; \
    if(IsMsgHandled()) \
    return TRUE; \
}


// void OnTimer(char cTimerID)
#define MSG_WM_TIMER_EX(func) \
    if (uMsg == WM_TIMER) \
{ \
    SetMsgHandled(TRUE); \
    func((char)wParam); \
    lResult = 0; \
    if(IsMsgHandled()) \
    return TRUE; \
}

#define WM_TIMER2    (WM_USER+5432)    //定义一个与HWND定时器兼容的SOUI定时器

#define MSG_WM_TIMER2(func) \
    if (uMsg == WM_TIMER2) \
{ \
    SetMsgHandled(TRUE); \
    func(wParam); \
    lResult = 0; \
    if(IsMsgHandled()) \
    return TRUE; \
}

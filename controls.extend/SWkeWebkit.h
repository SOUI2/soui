#pragma once
#include <wke.h>

namespace SOUI
{   
    class SWkeLoader
    {
    public:
        typedef void (*FunWkeInit)();
        typedef void (*FunWkeShutdown)();
        typedef wkeWebView (*FunWkeCreateWebView)();
        typedef void (*FunWkeDestroyWebView)(wkeWebView);
    public:
        SWkeLoader();

        ~SWkeLoader();

        BOOL Init(LPCTSTR pszDll);

        static SWkeLoader* GetInstance();
    public:
        FunWkeCreateWebView m_funWkeCreateWebView;
        FunWkeDestroyWebView m_funWkeDestroyWebView;
    protected:
        HMODULE     m_hModWke;
        FunWkeInit  m_funWkeInit;
        FunWkeShutdown m_funWkeShutdown;
        
        static SWkeLoader * s_pInst;
    };

    const char TM_TICKER = 1;
    class SWkeWebkit : public SWindow, protected wkeBufHandler , protected IIdleHandler
    {
        SOUI_CLASS_NAME(SWkeWebkit, L"wkeWebkit")
    public:
        SWkeWebkit(void);
        ~SWkeWebkit(void);

        wkeWebView	GetWebView(){return m_pWebView;}
    protected:
        virtual void onBufUpdated (const HDC hdc,int x, int y, int cx, int cy);
        virtual BOOL OnIdle();
    protected:
        int OnCreate(void *);
        void OnDestroy();
        void OnPaint(IRenderTarget *pRT);
        void OnSize(UINT nType, CSize size);
        LRESULT OnMouseEvent(UINT uMsg, WPARAM wParam,LPARAM lParam); 
        LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam,LPARAM lParam); 
        LRESULT OnKeyDown(UINT uMsg, WPARAM wParam,LPARAM lParam);
        LRESULT OnKeyUp(UINT uMsg, WPARAM wParam,LPARAM lParam);
        LRESULT OnChar(UINT uMsg, WPARAM wParam,LPARAM lParam);
        LRESULT OnImeStartComposition(UINT uMsg, WPARAM wParam,LPARAM lParam);
        void OnSetFocus(SWND wndOld);
        void OnKillFocus(SWND wndFocus);
        void OnTimer(char cTimerID);

        virtual BOOL OnSetCursor(const CPoint &pt);
        virtual UINT OnGetDlgCode(){return SC_WANTALLKEYS;}
        BOOL OnAttrUrl(SStringW strValue, BOOL bLoading);
        SOUI_ATTRS_BEGIN()
            ATTR_CUSTOM(L"url",OnAttrUrl)
        SOUI_ATTRS_END()

        SOUI_MSG_MAP_BEGIN()
            MSG_WM_PAINT_EX(OnPaint)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_SIZE(OnSize)
            MSG_WM_TIMER_EX(OnTimer)
            MSG_WM_SETFOCUS_EX(OnSetFocus)
            MSG_WM_KILLFOCUS_EX(OnKillFocus)
            MESSAGE_RANGE_HANDLER_EX(WM_MOUSEFIRST,0x209,OnMouseEvent)
            MESSAGE_HANDLER_EX(WM_MOUSEWHEEL,OnMouseWheel)
            MESSAGE_HANDLER_EX(WM_KEYDOWN,OnKeyDown)
            MESSAGE_HANDLER_EX(WM_KEYUP,OnKeyUp)
            MESSAGE_HANDLER_EX(WM_CHAR,OnChar)
            MESSAGE_HANDLER_EX(WM_IME_STARTCOMPOSITION,OnImeStartComposition)
        SOUI_MSG_MAP_END()

    protected:
        wkeWebView m_pWebView;
        SStringW m_strUrl;
    };
}

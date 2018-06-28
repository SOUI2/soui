#pragma once

#include <core/Swnd.h>
#include "SAnimator.h"

namespace SOUI
{
    
#define EVT_CHROMETAB_BEGIN (EVT_EXTERNAL_BEGIN + 400)
#define EVT_CHROMETAB_NEW           (EVT_CHROMETAB_BEGIN + 0)
#define EVT_CHROMETAB_CLOSE         (EVT_CHROMETAB_BEGIN + 1)
#define EVT_CHROMETAB_SELCHANGED    (EVT_CHROMETAB_BEGIN + 2)
#define EVT_CHROMETAB_DBCLICK       (EVT_CHROMETAB_BEGIN + 3)

    class EventChromeTabNew : public TplEventArgs<EventChromeTabNew>
    {
        SOUI_CLASS_NAME(EventChromeTabNew,L"on_chrometab_new")
    public:
        EventChromeTabNew(SWindow *pSender):TplEventArgs<EventChromeTabNew>(pSender)
        {

        }
        enum{EventID=EVT_CHROMETAB_NEW};
        SWindow * pNewTab;

        int       iNewTab;
    };

    class EventChromeTabClose : public TplEventArgs<EventChromeTabClose>
    {
        SOUI_CLASS_NAME(EventChromeTabClose,L"on_chrometab_close")
    public:
        EventChromeTabClose(SWindow *pSender):TplEventArgs<EventChromeTabClose>(pSender)
        {

        }
        enum{EventID=EVT_CHROMETAB_CLOSE};

        SWindow * pCloseTab;

        int       iCloseTab;
    };

    class EventChromeTabSelChanged : public TplEventArgs<EventChromeTabSelChanged>
    {
        SOUI_CLASS_NAME(EventChromeTabSelChanged,L"on_chrometab_sel_changed")
    public:
        EventChromeTabSelChanged(SWindow *pSender):TplEventArgs<EventChromeTabSelChanged>(pSender)
        {

        }
        enum{EventID=EVT_CHROMETAB_SELCHANGED};

        int         iOldSel;
        int         iNewSel;
    };

    class EventChromeTabDbClick : public TplEventArgs<EventChromeTabDbClick>
    {
        SOUI_CLASS_NAME(EventChromeTabDbClick,L"on_chrometab_dbclick")
    public:
        EventChromeTabDbClick(SWindow *pSender):TplEventArgs<EventChromeTabDbClick>(pSender)
        {
        }
        enum{EventID=EVT_CHROMETAB_DBCLICK};

        int iTabIndex;
        int iOrder;        
    };

    class SChromeTabCtrl : public SWindow, public ITimelineHandler
    {
        SOUI_CLASS_NAME(SChromeTabCtrl,L"chromeTabCtrl")
        friend class SChromeTab;
    public:
        enum TABDIR{
            TDIR_HORZ,
            TDIR_VERT,
        };

        SChromeTabCtrl(void);
        ~SChromeTabCtrl(void);
        
        BOOL InsertTab(LPCTSTR pszTitle,int iPos = -1);
        
        BOOL RemoveTab(int iTab);
        BOOL RemoveTab(SChromeTab *pTab);
        
        void SetCurSel(int iTab,bool bSendNotify = true);
        
        int GetCurSel() const;

        int GetTabIndex(int iTab) const;
        int GetTabOrder(int iTabIndex) const;

        int GetTabCount() const;
        SWindow* GetTabWindow(int iPos);

    protected:
        int ChangeTabPos(SChromeTab* pCurMove,CPoint ptCur);

        virtual BOOL CreateChildren(pugi::xml_node xmlNode);
        virtual void UpdateChildrenPosition();
        
        virtual void OnNextFrame();

        bool OnBtnNewClick(EventArgs *pEvt);
        bool OnBtnCloseTabClick(EventArgs *pEvt);
        bool OnTabClick(EventArgs *pEvt);
    
        int OnCreate(LPVOID);
        void OnDestroy();

        SOUI_MSG_MAP_BEGIN()
            MSG_WM_CREATE(OnCreate)
            MSG_WM_DESTROY(OnDestroy)
        SOUI_MSG_MAP_END()

        SOUI_ATTRS_BEGIN()
            ATTR_INT(L"tabDesiredSize",m_nDesiredSize,FALSE)
            ATTR_INT(L"enableDrag",m_bEnableDrag,FALSE)
            ATTR_ENUM_BEGIN(L"tabAlign",TABDIR,FALSE)
                ATTR_ENUM_VALUE(L"vertical",TDIR_VERT)
                ATTR_ENUM_VALUE(L"horizontal",TDIR_HORZ)
            ATTR_ENUM_END(m_tabAlign)
        SOUI_ATTRS_END()

        int     m_nDesiredSize;
        TABDIR  m_tabAlign;
        BOOL    m_bEnableDrag;
        
        SArray<SChromeTab*> m_lstTabOrder;

        SChromeTab *        m_pBtnNew;
        SChromeTab *        m_pSelTab;
        
        pugi::xml_document  m_xmlStyle;
    };

}


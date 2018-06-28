/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       eventcrack.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/09/05
* 
* Describe    SOUI系统中使用的事件映射宏
*/


#define EVENT_MAP_BEGIN()                           \
public:                                          \
    virtual BOOL _HandleEvent(SOUI::EventArgs *pEvt)\
    {                                               \
        UINT      uCode = pEvt->GetID();            \


#define EVENT_MAP_DECLEAR()                         \
protected:                                          \
    virtual BOOL _HandleEvent(SOUI::EventArgs *pEvt);\
    

#define EVENT_MAP_BEGIN2(classname)                 \
    BOOL classname::_HandleEvent(SOUI::EventArgs *pEvt)\
    {                                               \
        UINT      uCode = pEvt->GetID();            \
 

#define EVENT_MAP_END()                             \
        return __super::_HandleEvent(pEvt);         \
    }                                               \

#define EVENT_MAP_BREAK()                           \
    return FALSE;                                   \
    }                                               \
 
#define CHAIN_EVENT_MAP(ChainClass)                 \
        if(ChainClass::_HandleEvent(pEvt))          \
            return TRUE;                            \
 
#define CHAIN_EVENT_MAP_MEMBER(theChainMember)      \
    {                                               \
    if(theChainMember._HandleEvent(pEvt))           \
        return TRUE;                                \
    }

#define EVENT_CHECK_SENDER_ROOT(pRoot)              \
    {                                               \
    SOUI::SWindow *pWnd = SOUI::sobj_cast<SOUI::SWindow>(pEvt->sender);\
    if(pWnd && pRoot != pWnd && !pWnd->IsDescendant(pRoot)) \
        return FALSE;                               \
    }

// void OnEvent(EventArgs *pEvt)
#define EVENT_HANDLER(cd, func)                     \
    if(cd == uCode)                                 \
    {                                               \
        func(pEvt); return TRUE;                    \
    } 


// void OnEvent(EventArgs *pEvt)
#define EVENT_ID_HANDLER(id, cd, func)              \
    if(cd == uCode && id == pEvt->idFrom)           \
    {                                               \
        func(pEvt); return TRUE;                    \
    }

// void OnEvent(EventArgs *pEvt)
#define EVENT_ID_RANGE_HANDLER(idMin, idMax , cd, func) \
    if(cd == uCode  && idMin <= pEvt->idFrom && idMax >= pEvt->idFrom )\
    {                                               \
    func(pEvt); return TRUE;                        \
    }


// void OnEvent(EventArgs *pEvt)
#define EVENT_NAME_HANDLER(name, cd, func)          \
    if(cd == uCode && pEvt->nameFrom!= NULL && wcscmp(pEvt->nameFrom,name)==0) \
    {                                               \
        func(pEvt); return TRUE;                    \
    }


// void OnCommand(EventArgs *pEvt)
#define EVENT_COMMAND(func)                                             \
    if (SOUI::EVT_CMD == uCode)                                         \
    {                                                                   \
        func(pEvt);                                                     \
        return TRUE;                                                    \
    }                                                                   \

// void OnCommand()
#define EVENT_ID_COMMAND(id, func)                                      \
    if (SOUI::EVT_CMD == uCode && id == pEvt->idFrom)                   \
    {                                                                   \
        func();                                                         \
        return TRUE;                                                    \
    }                                                                   \
 
// void OnCommand()
#define EVENT_ID_COMMAND_RANGE(idMin, idMax, func)                               \
    if (SOUI::EVT_CMD == uCode && idMin <= pEvt->idFrom && idMax >= pEvt->idFrom )  \
    {                                                                            \
        func(pEvt->idFrom);                                                      \
        return TRUE;                                                             \
    }                                                                            \

// void OnCommand()
#define EVENT_NAME_COMMAND(name, func)                                      \
    if (SOUI::EVT_CMD == uCode && pEvt->nameFrom!= NULL && wcscmp(pEvt->nameFrom,name)==0)  \
    {                                                                       \
        func();                                                             \
        return TRUE;                                                        \
    }                                                                       \

 
// BOOL OnContextMenu(CPoint pt)
#define EVENT_ID_CONTEXTMENU(id,func)                                      \
    if (SOUI::EVT_CTXMENU == uCode && pEvt->idFrom==id)                    \
{                                                                          \
    SOUI::EventCtxMenu* pEvtCtxMenu = (SOUI::EventCtxMenu*)pEvt;           \
    pEvtCtxMenu->bCancel=func(pEvtCtxMenu->pt);                            \
    return TRUE;                                                           \
}                                                                          \


// BOOL OnContextMenu(CPoint pt)
#define EVENT_NAME_CONTEXTMENU(name,func)                                       \
    if (SOUI::EVT_CTXMENU == uCode && pEvt->nameFrom!= NULL && wcscmp(pEvt->nameFrom,name)==0) \
{                                                                               \
    SOUI::EventCtxMenu* pEvtCtxMenu = (SOUI::EventCtxMenu*)pEvt;                \
    pEvtCtxMenu->bCancel=func(pEvtCtxMenu->pt);                                 \
    return TRUE;                                                                \
}                                                                               \


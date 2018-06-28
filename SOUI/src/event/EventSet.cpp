#include "souistd.h"
#include <event/EventSet.h>

namespace SOUI
{
    //////////////////////////////////////////////////////////////////////////
    // SEvent

    SEvent::SEvent(DWORD dwEventID,LPCWSTR pszEventName) :m_dwEventID(dwEventID),m_strEventName(pszEventName)
    {

    }

    SEvent::~SEvent()
    {
        for(UINT i=0;i<m_evtSlots.GetCount();i++)
        {
            delete m_evtSlots[i];
        }
        m_evtSlots.RemoveAll();
    }


    bool SEvent::subscribe( const ISlotFunctor& slot )
    {
        if(findSlotFunctor(slot) != -1) return false;
        m_evtSlots.Add(slot.Clone());
        return true;
    }

    bool SEvent::unsubscribe( const ISlotFunctor& slot )
    {
        int idx=findSlotFunctor(slot);
        if(idx==-1) return false;

        delete m_evtSlots[idx];
        m_evtSlots.RemoveAt(idx);
        return true;
    }

    int SEvent::findSlotFunctor( const ISlotFunctor& slot )
    {
        for(UINT i=0;i<m_evtSlots.GetCount();i++)
        {
            if(m_evtSlots[i]->Equal(slot))
            {
                return i;
            }
        }
        return -1;
    }


    void SEvent::operator()(EventArgs& args)
    {
        // execute all subscribers, updating the 'handled' state as we go
        for (int i=(int)m_evtSlots.GetCount()-1;i>=0; i--)
        {//the latest event handler handles the event first.
            BOOL bHandled = (*m_evtSlots[i])(&args);
            if(bHandled)
            {
                ++args.handled;
                if(!args.bubbleUp) break;
            }
        }
    }

    void SEvent::SetScriptHandler(const SStringA & strScriptHandler)
    {
        m_strScriptHandler = strScriptHandler;
    }

    SOUI::SStringA SEvent::GetScriptHandler() const
    {
        return m_strScriptHandler;
    }

    SOUI::SStringW SEvent::GetName() const
    {
        return m_strEventName;
    }

    DWORD SEvent::GetID()
    {
        return m_dwEventID;
    }

    //////////////////////////////////////////////////////////////////////////
    // SEventSet
    SEventSet::SEventSet(void):m_bMuted(FALSE)
    {
    }

    SEventSet::~SEventSet(void)
    {
        removeAllEvents();
    }

    SEvent * SEventSet::GetEventObject(const DWORD dwEventID )
    {
        for(UINT i=0;i<m_evtArr.GetCount();i++)
        {
            if(m_evtArr[i]->GetID()==dwEventID) return m_evtArr[i];
        }
        return NULL;
    }

    void SEventSet::FireEvent(EventArgs& args )
    {
        // find event object
        SEvent* ev = GetEventObject(args.GetID());

        // fire the event if present and set is not muted
        if ((ev != 0) && !m_bMuted)
        {
            (*ev)(args);
        }
    }

    void SEventSet::addEvent( const DWORD dwEventID ,LPCWSTR pszEventHandlerName)
    {
        if(!isEventPresent(dwEventID))
        {
            m_evtArr.Add(new SEvent(dwEventID,pszEventHandlerName));
        }
    }

    void SEventSet::removeEvent( const DWORD dwEventID )
    {
        for(UINT i=0;i<m_evtArr.GetCount();i++)
        {
            if(m_evtArr[i]->GetID()==dwEventID)
            {
                delete m_evtArr[i];
                m_evtArr.RemoveAt(i);
                return;
            }
        }
    }

    bool SEventSet::isEventPresent( const DWORD dwEventID )
    {
        return GetEventObject(dwEventID)!=NULL;
    }

    void SEventSet::removeAllEvents( void )
    {
        for(UINT i=0;i<m_evtArr.GetCount();i++)
        {
            delete m_evtArr[i];
        }
        m_evtArr.RemoveAll();
    }

    bool SEventSet::subscribeEvent( const DWORD dwEventID, const ISlotFunctor & subscriber )
    {
        if(!isEventPresent(dwEventID)) return false;
        return GetEventObject(dwEventID)->subscribe(subscriber);
    }

    bool SEventSet::unsubscribeEvent( const DWORD dwEventID, const ISlotFunctor & subscriber )
    {
        if(!isEventPresent(dwEventID)) return false;
        return GetEventObject(dwEventID)->unsubscribe(subscriber);
    }

    bool SEventSet::setEventScriptHandler( const SStringW & strEventName,const SStringA strScriptHandler )
    {
        for(UINT i=0;i<m_evtArr.GetCount();i++)
        {
            if(m_evtArr[i]->GetName() == strEventName)
            {
                m_evtArr[i]->SetScriptHandler(strScriptHandler);
                return true;
            }
        }
        return false;
    }

    SStringA SEventSet::getEventScriptHandler( const SStringW & strEventName ) const
    {
        for(UINT i=0;i<m_evtArr.GetCount();i++)
        {
            if(m_evtArr[i]->GetName() == strEventName)
            {
                return m_evtArr[i]->GetScriptHandler();
            }
        }
        return "";
    }
}//end of namespace
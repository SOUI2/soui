#pragma once

#ifndef WM_SYSTIMER
#define WM_SYSTIMER 0x0118   //(caret blink)
#endif//WM_SYSTIMER

namespace SOUI
{
    template<class T>
    BOOL RemoveElementFromArray(SArray<T> &arr, T ele)
    {
        for(size_t i=0;i<arr.GetCount();i++)
        {
            if(arr[i] == ele)
            {
                arr.RemoveAt(i);
                return TRUE;
            }
        }
        return FALSE;
    }
    
    struct IMessageFilter
    {
        virtual BOOL PreTranslateMessage(MSG* pMsg) = 0;
    };

    ///////////////////////////////////////////////////////////////////////////////
    // CIdleHandler - Interface for idle processing

    struct IIdleHandler
    {
        virtual BOOL OnIdle() = 0;
    };

    class SOUI_EXP SMessageLoop
    {
    public:
        SMessageLoop():m_bRunning(FALSE)
        {
        
        }
        virtual ~SMessageLoop(){}
        
        SArray<IMessageFilter*> m_aMsgFilter;
        SArray<IIdleHandler*> m_aIdleHandler;
        MSG m_msg;
                
        // Message filter operations
        BOOL AddMessageFilter(IMessageFilter* pMessageFilter);

        BOOL RemoveMessageFilter(IMessageFilter* pMessageFilter);

        // Idle handler operations
        BOOL AddIdleHandler(IIdleHandler* pIdleHandler);

        BOOL RemoveIdleHandler(IIdleHandler* pIdleHandler);

        static BOOL IsIdleMessage(MSG* pMsg);

        // Overrideables
        // Override to change message filtering
        virtual BOOL PreTranslateMessage(MSG* pMsg);

        // override to change idle processing
        virtual BOOL OnIdle(int /*nIdleCount*/);
        
        virtual int Run();
        
        virtual BOOL IsRunning() const{return m_bRunning;}
        
        virtual void OnMsg(LPMSG pMsg);
        
		virtual void Quit();
    protected:
        BOOL m_bRunning;
		BOOL m_bQuit;
    };


}//end of namespace SOUI

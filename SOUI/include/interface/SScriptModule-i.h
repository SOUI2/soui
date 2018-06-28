/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       SScriptModule-i.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/02
* 
* Describe    
*/

#pragma once

#include "event/EventSubscriber.h"
#include <unknown/obj-ref-i.h>

// Start of SOUI namespace section
namespace SOUI
{
    class SWindow;
/*!
\brief
    Abstract interface required for all scripting support modules to be used with
    the SOUI system.
*/
struct IScriptModule : public IObjRef
{
    /**
     * GetScriptEngine
     * @brief    获得脚本引擎的指针
     * @return   void * -- 脚本引擎的指针
     * Describe  
     */    
    virtual void * GetScriptEngine () = 0;

    /*************************************************************************
        Abstract interface
    *************************************************************************/
    /*!
    \brief
        Execute a script file.

    \param pszScriptFile
        String object holding the filename of the script file that is to be executed
        
    */
    virtual void    executeScriptFile(LPCSTR pszScriptFile)  = 0;

    /*!
    \brief
        Execute a script buffer.

    \param buff
        buffer of the script that is to be executed
        
    \param sz
        size of buffer
    */
    virtual	void	executeScriptBuffer(const char* buff, size_t sz)  = 0;
    /*!
    \brief
        Execute script code contained in the given String object.

    \param str
        String object holding the valid script code that should be executed.

    \return
        Nothing.
    */
    virtual void executeString(LPCSTR str) = 0;


    /*!
    \brief
        Execute a scripted global 'event handler' function.  The function should take some kind of EventArgs like parameter
        that the concrete implementation of this function can create from the passed EventArgs based object.  

    \param handler_name
        String object holding the name of the scripted handler function.

    \param EventArgs *pEvt
        SWindow based object that should be passed, by any appropriate means, to the scripted function.

    \return
        - true if the event was handled.
        - false if the event was not handled.
    */
    virtual    bool    executeScriptedEventHandler(LPCSTR handler_name, EventArgs *pEvt)=0;


    /*!
    \brief
        Return identification string for the ScriptModule.  If the internal id string has not been
        set by the ScriptModule creator, a generic string of "Unknown scripting module" will be returned.

    \return
        String object holding a string that identifies the ScriptModule in use.
    */
    virtual LPCSTR getIdentifierString() const = 0;

    /*!
    \brief
            Subscribes or unsubscribe the named Event to a scripted function

    \param target
            The target EventSet for the subscription.

    \param uEvent
            Event ID to subscribe to.

    \param subscriber_name
            String object containing the name of the script function that is to be subscribed to the Event.

    \return 
    */
    virtual bool subscribeEvent(SWindow* target, UINT uEvent, LPCSTR subscriber_name) = 0;

    /**
     * unsubscribeEvent
     * @brief    取消事件订阅
     * @param    SWindow * target --  目标窗口
     * @param    UINT uEvent --  目标事件
     * @param    LPCSTR subscriber_name --  脚本函数名
     * @return   bool -- true操作成功
     * Describe  
     */    
    virtual bool unsubscribeEvent(SWindow* target, UINT uEvent, LPCSTR subscriber_name ) = 0;

};

struct IScriptFactory : public IObjRef
{
    virtual HRESULT CreateScriptModule(IScriptModule ** ppScriptModule) = 0;
};

} // End of  SOUI namespace section


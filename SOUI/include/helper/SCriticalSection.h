/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       SCriticalSection.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/09/27
* 
* Describe    CriticalSection 包装
*/

#pragma once

namespace SOUI
{
    class SOUI_EXP SCriticalSection
    {
    public:
        SCriticalSection()
        {
            InitializeCriticalSection(&m_cs);
        }
        virtual ~SCriticalSection()
        {
            DeleteCriticalSection(&m_cs);
        }

        void Enter()
        {
            EnterCriticalSection(&m_cs);
        }
        void Leave()
        {
            LeaveCriticalSection(&m_cs);
        }
    protected:
        CRITICAL_SECTION    m_cs;
    };

    class SOUI_EXP SAutoLock
    {
    public:
        SAutoLock(SCriticalSection & cs):m_cs(cs)
        {
            m_cs.Enter();
        }

        ~SAutoLock()
        {
            m_cs.Leave();
        }
    protected:
        SCriticalSection & m_cs;
    };
}

/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       Accelerator.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/02
* 
* Describe    SOUI加速键管理模块
*/

#pragma once

namespace SOUI
{
    /**
    * @class      CAccelerator
    * @brief      加速键映射
    * 
    * Describe
    */
    class SOUI_EXP CAccelerator
    {
    public:

        /**
         * CAccelerator
         * @brief    构造函数
         * @param    DWORD dwAccel --  加速键值
         * Describe  
         */    
        CAccelerator(DWORD dwAccel);

        /**
         * CAccelerator
         * @brief    构造函数
         * @param    UINT vKey --  键值
         * @param    bool bCtrl --  与Ctrl组合
         * @param    bool bAlt --  与Alt组合
         * @param    bool bShift --  与Shilft组合
         * Describe  
         */    
        CAccelerator(UINT vKey=0,bool bCtrl=false,bool bAlt=false,bool bShift=false);

        /**
         * ~CAccelerator
         * @brief    析构函数
         * Describe  
         */    
        ~CAccelerator(void);

        /**
         * GetKeyName
         * @brief    将键盘值转换为对应的字符串
         * @param    WORD vk --  键盘值
         * @return   SOUI::SStringT -- 键名
         * Describe  
         */    
        SStringT GetKeyName(WORD vk);

        /**
         * FormatHotkey
         * @brief    获得当前加速键的字符串形式
         * @return   SOUI::SStringT -- 加速键的字符串形式
         * Describe  
         */    
        SStringT FormatHotkey();

        /**
         * GetModifier
         * @brief    获得加速键的修饰位
         * @return   WORD -- 加速键的修饰键
         * Describe  
         */    
        WORD GetModifier() const {return m_wModifier;}

        /**
         * GetKey
         * @brief    获得加速键的主键
         * @return   WORD -- 加速键的主键
         * Describe  
         */    
        WORD GetKey() const {return m_wVK;}

        /**
         * TranslateAccelKey
         * @brief    解析与字符串对应的加速键值
         * @param    LPCTSTR pszKeyName --  定义加速键的字符串
         * @return   DWORD -- 加速键值
         * Describe  
         */    
        static DWORD TranslateAccelKey(LPCTSTR pszKeyName);
    protected:
        WORD     m_wModifier;
        WORD    m_wVK;
    };

    template<>
    class  CElementTraits< CAccelerator > : public CElementTraitsBase< CAccelerator >
    {
    public:
        static ULONG Hash(INARGTYPE element ) throw()
        {
            return MAKELONG(element.GetModifier(),element.GetKey());
        }

        static bool CompareElements( INARGTYPE element1, INARGTYPE element2 )
        {
            return Hash(element1)==Hash(element2);
        }

        static int CompareElementsOrdered( INARGTYPE element1, INARGTYPE element2 )
        {
            return Hash(element1)-Hash(element2);
        }
    };

    /**
    * @struct     IAcceleratorTarget
    * @brief      加速键按下的处理接口
    * 
    * Describe 想要注册键盘加速键的类需要实现本接口
    */
    struct IAcceleratorTarget
    {
        /**
         * OnAcceleratorPressed
         * @brief    
         * @param    const CAccelerator & accelerator --  按下的加速键
         * @return   bool -- 加速键被处理返回true
         * Describe  
         */    
        virtual bool OnAcceleratorPressed(const CAccelerator& accelerator) = 0;
    };

    /**
    * @struct     IAcceleratorMgr
    * @brief      加速键管理接口
    * 
    * Describe
    */
    struct IAcceleratorMgr
    {
        // Register a keyboard accelerator for the specified target. If multiple
        // targets are registered for an accelerator, a target registered later has
        // higher priority.
        // Note that we are currently limited to accelerators that are either:
        // - a key combination including Ctrl or Alt
        // - the escape key
        // - the enter key
        // - any F key (F1, F2, F3 ...)
        // - any browser specific keys (as available on special keyboards)
        virtual void RegisterAccelerator(const CAccelerator& accelerator,
            IAcceleratorTarget* target)=NULL;

        // Unregister the specified keyboard accelerator for the specified target.
        virtual void UnregisterAccelerator(const CAccelerator& accelerator,
            IAcceleratorTarget* target)=NULL;

        // Unregister all keyboard accelerator for the specified target.
        virtual void UnregisterAccelerators(IAcceleratorTarget* target)=NULL;
    };
}//end of namespace SOUI

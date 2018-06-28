/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       SSkinobj-i.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/01
* 
* Describe    interface of SSkinObj
*/
#pragma once

#include <sobject/Sobject.hpp>
#include <unknown/obj-ref-i.h>

namespace SOUI
{
    struct IRenderTarget;
    
    /**
    * @struct     ISkinObj
    * @brief      Skin 对象
    * 
    * Describe
    */
    class SOUI_EXP ISkinObj : public IObjRef, public IObject
    {
    public:
        virtual ~ISkinObj()
        {
        }

        /**
        * Draw
        * @brief    将this绘制到RenderTarget上去
        * @param    IRenderTarget * pRT --  绘制用的RenderTarget
        * @param    LPCRECT rcDraw --  绘制位置
        * @param    DWORD dwState --  绘制状态
        * @param    BYTE byAlpha --  绘制透明度
        * @return   void
        * Describe  
        */    
        virtual void Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState,BYTE byAlpha)=0;

        /**
        * Draw
        * @brief    将this绘制到RenderTarget上去
        * @param    IRenderTarget * pRT --  绘制用的RenderTarget
        * @param    LPCRECT rcDraw --  绘制位置
        * @param    DWORD dwState --  绘制状态
        * @return   void
        * Describe  使用SkinObj的GetAlpha属性绘制
        */    
        virtual void Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState)=0;

        /**
        * GetSkinSize
        * @brief    获得Skin的默认大小
        * @return   SIZE -- Skin的默认大小
        * Describe  派生类应该根据skin的特点实现该接口
        */    
        virtual SIZE GetSkinSize()=0;
        /**
        * IgnoreState
        * @brief    查询skin是否有状态信息
        * @return   BOOL -- true有状态信息
        * Describe  
        */    
        virtual BOOL IgnoreState()=0;

        /**
        * GetStates
        * @brief    获得skin对象包含的状态数量
        * @return   int -- 状态数量
        * Describe  默认为1
        */    
        virtual int GetStates()=0;

        /**
        * GetAlpha
        * @brief    获得skin对象包含透明度
        * @return   BYTE -- 透明度
        * Describe  [0-255]
        */    
        virtual BYTE GetAlpha() const=0;

        /**
        * SetAlpha
        * @brief    设定skin对象包含透明度
        * @param    BYTE byAlpha-- 透明度
        * @return   void
        * Describe  
        */    
        virtual void SetAlpha(BYTE byAlpha) =0;
        
        /**
        * OnColorize
        * @brief    调整皮肤对象色调
        * @param    COLORREF cr -- 参考颜色
        * @return   void
        * Describe  默认不处理
        */ 
        virtual void OnColorize(COLORREF cr) {}

		virtual int GetScale() const = 0;

		virtual ISkinObj * Scale(int nScale) = 0;

    };
}//namespace SOUI

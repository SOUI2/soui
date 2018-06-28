/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       SResProvider-i.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/02
* 
* Describe    
*/

#ifndef _SRESPROVIDERBASE_
#define _SRESPROVIDERBASE_
#pragma once

#define OR_API SOUI_EXP

#include <unknown/obj-ref-i.h>
#include <string/tstring.h>
#include <souicoll.h>

#include "render-i.h"

#define UIRES_INDEX    _T("uires.idx")        //文件夹资源的文件映射表索引表文件名

namespace SOUI
{
    enum BUILTIN_RESTYPE
    {
        RES_PE=0,
        RES_FILE,
    };

    /**
    * @struct     IResProvider
    * @brief      ResProvider对象
    * 
    * Describe  实现各种资源的加载
    */
    struct IResProvider : public IObjRef
    {
        /**
         * Init
         * @brief    资源初始化函数
         * @param    WPARAM wParam --  param 1 
         * @param    LPARAM lParam --  param 2
         * @return   BOOL -- true:succeed
         *
         * Describe  every Resprovider must implement this interface.
         */
        virtual BOOL Init(WPARAM wParam,LPARAM lParam) =0;
        
        /**
         * HasResource
         * @brief    查询一个资源是否存在
         * @param    LPCTSTR strType --  资源类型
         * @param    LPCTSTR pszResName --  资源名称
         * @return   BOOL -- true存在，false不存在
         * Describe  
         */    
        virtual BOOL HasResource(LPCTSTR pszType,LPCTSTR pszResName)=0;

        /**
         * LoadIcon
         * @brief    从资源中加载ICON
         * @param    LPCTSTR pszResName --  ICON名称
         * @param    int cx --  ICON宽度
         * @param    int cy --  ICON高度
         * @return   HICON -- 成功返回ICON的句柄，失败返回0
         * Describe  
         */    
        virtual HICON LoadIcon(LPCTSTR pszResName,int cx=0,int cy=0)=0;

        /**
         * LoadBitmap
         * @brief    从资源中加载HBITMAP
         * @param    LPCTSTR pszResName --  BITMAP名称
         * @return   HBITMAP -- 成功返回BITMAP的句柄，失败返回0
         * Describe  
         */    
        virtual HBITMAP LoadBitmap(LPCTSTR pszResName)=0;

        /**
         * LoadCursor
         * @brief    从资源中加载光标
         * @param    LPCTSTR pszResName --  光标名
         * @return   HCURSOR -- 成功返回光标的句柄，失败返回0
         * Describe  支持动画光标
         */    
        virtual HCURSOR LoadCursor(LPCTSTR pszResName)=0;

        /**
         * LoadImage
         * @brief    从资源加载一个IBitmap对象
         * @param    LPCTSTR strType --  图片类型
         * @param    LPCTSTR pszResName --  图片名
         * @return   IBitmap * -- 成功返回一个IBitmap对象，失败返回0
         * Describe  如果没有定义strType，则根据name使用FindImageType自动查找匹配的类型
         */    
        virtual IBitmap * LoadImage(LPCTSTR pszType,LPCTSTR pszResName)=0;

        /**
         * LoadImgX
         * @brief    从资源中创建一个IImgX对象
         * @param    LPCTSTR strType --  图片类型
         * @param    LPCTSTR pszResName --  图片名
         * @return   IImgX   * -- 成功返回一个IImgX对象，失败返回0
         * Describe  
         */    
        virtual IImgX   * LoadImgX(LPCTSTR pszType,LPCTSTR pszResName)=0;

        /**
         * GetRawBufferSize
         * @brief    获得资源数据大小
         * @param    LPCTSTR strType --  资源类型
         * @param    LPCTSTR pszResName --  资源名
         * @return   size_t -- 资源大小（byte)，失败返回0
         * Describe  
         */    
        virtual size_t GetRawBufferSize(LPCTSTR pszType,LPCTSTR pszResName)=0;

        /**
         * GetRawBuffer
         * @brief    获得资源内存块
         * @param    LPCTSTR strType --  资源类型
         * @param    LPCTSTR pszResName --  资源名
         * @param    LPVOID pBuf --  输出内存块
         * @param    size_t size --  内存大小
         * @return   BOOL -- true成功
         * Describe  应该先用GetRawBufferSize查询资源大小再分配足够空间
         */    
        virtual BOOL GetRawBuffer(LPCTSTR pszType,LPCTSTR pszResName,LPVOID pBuf,size_t size)=0;
        
        
        /**
         * CheckResUsage
         * @brief    获得资源内存块
         * @param    const SMap<SStringT,int> & mapResUsage --  资源使用计数MAP
         * @return   void
         * Describe  debug版中检查资源的使用情况
         */    
        #ifdef _DEBUG
        virtual void CheckResUsage(const SMap<SStringT,int> & mapResUsage){}
        #endif
    };

}//namespace SOUI
#endif//_SRESPROVIDERBASE_

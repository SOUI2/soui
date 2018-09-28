/********************************************************************
created:	2012/12/27
created:	27:12:2012   14:55
filename: 	DuiSkinGif.h
file base:	DuiSkinGif
file ext:	h
author:		huangjianxiong

purpose:	自定义皮肤对象
*********************************************************************/
#pragma once
#include <interface/SSkinobj-i.h>
#include <unknown/obj-ref-impl.hpp>

#include "SAniImgFrame.h"

namespace Gdiplus
{
class Bitmap;
}

namespace SOUI
{

    /**
    * @class     SSkinGif
    * @brief     GIF图片加载及显示对象
    * 
    * Describe
    */
    class SSkinGif : public SSkinAni
    {
        SOUI_CLASS_NAME(SSkinGif, L"gif")
    public:
        SSkinGif()
        {

        }
        
        //初始化GDI+环境，由于这里需要使用GDI+来解码GIF文件格式
        static BOOL Gdiplus_Startup();
        //退出GDI+环境
        static void Gdiplus_Shutdown();

        /**
         * LoadFromFile
         * @brief    从文件加载GIF
         * @param    LPCTSTR pszFileName --  文件名
         * @return   int -- GIF帧数，0-失败
         * Describe  
         */    
        int LoadFromFile(LPCTSTR pszFileName);

        /**
         * LoadFromMemory
         * @brief    从内存加载GIF
         * @param    LPVOID pBits --  内存地址
         * @param    size_t szData --  内存数据长度
         * @return   int -- GIF帧数，0-失败
         * Describe  
         */    
        int LoadFromMemory(LPVOID pBits,size_t szData);

        SOUI_ATTRS_BEGIN()
            ATTR_CUSTOM(L"src",OnAttrSrc)   //XML文件中指定的图片资源名,(type:name)
        SOUI_ATTRS_END()
    protected:
        HRESULT OnAttrSrc(const SStringW &strValue,BOOL bLoading);
        int LoadFromGdipImage(Gdiplus::Bitmap * pImg);
    };
}//end of name space SOUI

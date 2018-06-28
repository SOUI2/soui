/********************************************************************
created:	2014/10/01
created:	01:10:2014   16:32
filename: 	DuiSkinAPNG.h
file base:	DuiSkinAPNG
file ext:	h
author:		huangjianxiong

purpose:	动画PNG（APNG）扩展支持，依赖于当前使用的图片解码器。
            目前只有imgdecoder-png支持APNG解码
*********************************************************************/
#pragma once
#include <interface/SSkinobj-i.h>
#include <unknown/obj-ref-impl.hpp>

#include "SAniImgFrame.h"

namespace SOUI
{

    /**
    * @class     SSkinAPNG
    * @brief     APNG图片加载及显示对象
    * 
    * Describe
    */
    class SSkinAPNG : public SSkinAni
    {
        SOUI_CLASS_NAME(SSkinAPNG, L"apng")
    public:
        SSkinAPNG()
        {

        }

        
        /**
         * LoadFromFile
         * @brief    从文件加载APNG
         * @param    LPCTSTR pszFileName --  文件名
         * @return   int -- APNG帧数，0-失败
         * Describe  
         */    
        int LoadFromFile(LPCTSTR pszFileName);

        /**
         * LoadFromMemory
         * @brief    从内存加载APNG
         * @param    LPVOID pBits --  内存地址
         * @param    size_t szData --  内存数据长度
         * @return   int -- APNG帧数，0-失败
         * Describe  
         */    
        int LoadFromMemory(LPVOID pBits,size_t szData);

        SOUI_ATTRS_BEGIN()
            ATTR_CUSTOM(L"src",OnAttrSrc)   //XML文件中指定的图片资源名,(type:name)
        SOUI_ATTRS_END()
    protected:
        LRESULT OnAttrSrc(const SStringW &strValue,BOOL bLoading);
        
        int _InitImgFrame(IImgX *pImgX);

    };
}//end of name space SOUI

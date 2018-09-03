/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       STranslator-i.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/02
* 
* Describe    
*/

#pragma once

#include <unknown/obj-ref-i.h>
#include <string/tstring.h>

namespace SOUI
{
#define TR_MAX_NAME_LEN	64
    /** 
     * @struct     ITranslator
     * @brief      语言翻译接口
     *
     * Describe
     */
    struct ITranslator : public IObjRef
    {
        /**
         * Load
         * @brief    从资源中加载语言翻译数据
         * @param    LPVOID pData --  资源指针，具体含义由接口的实现来解释
         * @param    UINT uType --  资源类型，具体含义由接口的实现来解释
         * @return   BOOL true-加载成功, false-加载失败
         *
         * Describe  
         */
        virtual BOOL Load(LPVOID pData,UINT uType)=0;
        /**
         * name
         * @brief    获取翻译资源的name
         * @return   SOUI::SStringW 翻译资源的name
         *
         * Describe  
         */
        virtual void GetName(wchar_t szBuf[TR_MAX_NAME_LEN])=0;

		virtual bool NameEqual(LPCWSTR pszName) = 0;
        /**
         * guid
         * @brief    获取翻译资源的ID
         * @return   GUID 翻译资源的ID
         *
         * Describe  
         */
        virtual GUID     guid()=0;
        /**
         * tr
         * @brief    执行翻译的接口
         * @param    const SStringW & strSrc --  原字符串
         * @param    const SStringW & strCtx --  翻译上下文
         * @param    SStringW & strRet --  翻译后的字符串
         * @return   int - buflen; 0 - no translation; -1 - buf size not enough;  
         *
         * Describe  
         */
        virtual int tr(const SStringW & strSrc,const SStringW & strCtx,wchar_t *pszOut,int nLen) const =0;

		/**
         * updateLogfont
         * @brief    更新字体信息
		 * @param    const SStringW & strName -- 布局中定义的一种字体的name属性
         * @param    LOGFONT * pfont --  字体信息
         * @return   true-修改完成，false-不需要修改
         *
         * Describe  多语言翻译的时候可能需要对字体信息进行更新，包含字符集，字体名等，这些信息可以在语言包中定义。
		 *           启程软件，2018年4月27日
         */
		virtual BOOL updateLogfont(const SStringW & strName,LOGFONT *pFont){return FALSE;}

		virtual BYTE charsetFromString(const SStringW & strCharset) const {return GB2312_CHARSET;}
    };


    /** 
     * @struct     ITranslatorMgr
     * @brief      语言翻译接口管理器
     *
     * Describe
     */
    struct ITranslatorMgr : public IObjRef
    {
		/**
		* SetLanguage
		* @brief    设置翻译模块当前接受的语言
		* @param [in] const SStringW & strLang --  翻译语言
		*
		* Describe 自动清除语言和目标语言不同的模块
		*/
		virtual void SetLanguage(const SStringW & strLang) = 0;

		/**
		* GetLanguage
		* @brief    获取翻译模块当前接受的语言
		* @return SStringW  --  翻译语言
		*
		* Describe 
		*/
		virtual void GetLanguage(wchar_t szOut[TR_MAX_NAME_LEN]) const = 0;

        /**
         * CreateTranslator
         * @brief    创建一个语言翻译对象
         * @param [out] ITranslator * * ppTranslator --  接收语言翻译对象的指针
         * @return   BOOL true-成功，false-失败
         *
         * Describe  
         */
        virtual BOOL CreateTranslator(ITranslator ** ppTranslator)=0;
        /**
         * InstallTranslator
         * @brief    向管理器中安装一个语言翻译对象
         * @param    ITranslator * ppTranslator -- 语言翻译对象
         * @return   BOOL true-成功，false-失败
         *
         * Describe  
         */

        virtual BOOL InstallTranslator(ITranslator * ppTranslator) =0;
        /**
         * UninstallTranslator
         * @brief    从管理器中卸载一个语言翻译对象
         * @param    REFGUID id --  语言翻译对象的ID
         * @return   BOOL true-成功，false-失败
         *
         * Describe  
         */
        virtual BOOL UninstallTranslator(REFGUID id) =0;
        
        /**
         * tr
         * @brief    翻译字符串
         * @param    const SStringW & strSrc --  原字符串
         * @param    const SStringW & strCtx --  翻译上下文
         * @return   SOUI::SStringW 翻译后的字符串
         *
         * Describe  调用ITranslator的tr接口执行具体翻译过程
         */
        virtual int tr(const SStringW & strSrc,const SStringW & strCtx,wchar_t *pszOut,int nLen) const =0;

		/**
         * updateLogfont
         * @brief    更新字体信息
		 * @param    const SStringW & strName -- 布局中定义的一种字体的name属性
         * @param    LOGFONT * pfont --  字体信息
         * @return   true-修改完成，false-不需要修改
         *
         * Describe  多语言翻译的时候可能需要对字体信息进行更新，包含字符集，字体名等，这些信息可以在语言包中定义。
		 *           启程软件，2018年4月27日
         */
		virtual BOOL updateLogfont(const SStringW & strName,LOGFONT * pfont){
			return FALSE;
		}

    };

}
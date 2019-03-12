/**
* Copyright (C) 2014-2050 SOUI团队
* All rights reserved.
* 
* @file       SStylePool.h
* @brief      SkinObj Pool
* @version    v1.0      
* @author     soui      
* @date       2014-05-28
* 
* Describe    管理Style
*/

#pragma once
#include "core/SSingletonMap.h"
#include <unknown/obj-ref-i.h>
#include <unknown/obj-ref-impl.hpp>

//获得StylePoolMgr指针
#define GETSTYLEPOOLMGR SStylePoolMgr::getSingletonPtr()
//通过NAME获得对应的Style
//p1: name of style
//p2: output of style
#define GETSTYLE(p) SStylePoolMgr::getSingleton().GetStyle(p)

namespace SOUI
{
    /**
    * @class      SStylePool
    * @brief      name和SwndStyle的映射表
    * 
    * Describe    
    */
    class SOUI_EXP SStylePool :public SCmnMap<pugi::xml_node,SStringW> , public TObjRefImpl2<IObjRef,SStylePool>
    {
    public:
        /**
         * GetStyle
         * @brief    Get style object from pool by class name
         * @param    LPCWSTR lpszName --  name of style
         * @param [out]  SwndStyle & style --  style
         * @return   BOOL -- TRUE: success; FALSE: not exist
         * Describe  
         */    
        pugi::xml_node GetStyle(LPCWSTR lpszName);

        /**
         * Init
         * @brief    Load Style table from xml node
         * @param    pugi::xml_node xmlNode --  xml node that describe style list
         * @return   BOOL -- TRUE: loaded; FALSE:failed;
         * Describe  
         */    
        BOOL Init(pugi::xml_node xmlNode);
        
    protected:
        pugi::xml_document  m_xmlDoc;
    };

    /**
    * @class      SStylePoolMgr
    * @brief      SwndStyle的映射表的列表
    * 
    * Describe    
    */
    class SOUI_EXP SStylePoolMgr : public SSingleton2<SStylePoolMgr>
    {
		SINGLETON2_TYPE(SINGLETON_STYLEPOOLMGR)
    public:
        ~SStylePoolMgr();
        
        /**
        * GetStyle
        * @brief    Get style object from pool by class name
        * @param    LPCWSTR lpszName --  name of style
        * @param [out]  SwndStyle & style --  style
        * @return   BOOL -- TRUE: success; FALSE: not exist
        * Describe  
        */    
        pugi::xml_node GetStyle(LPCWSTR lpszName);
        
        /**
         * PushStylePool
         * @brief    push a new SStylePool to the tail of style pool list
         * @param    SStylePool * pStylePool --  the target style pool
         * @return   void
         * Describe  
         */    
        void PushStylePool(SStylePool *pStylePool);
        
        /**
         * PopStylePool
         * @brief    remove the target style pool from list
         * @param    SStylePool * pStylePool --  the target style pool
         * @return   SStylePool * -- the removed style pool
         * Describe  if pStylePool is null, it remove the last style pool from the list
         */    
        SStylePool * PopStylePool(SStylePool *pStylePool);
    protected:
        SList<SStylePool *> m_lstStylePools;
    };

	class SOUI_EXP STemplatePool :public SCmnMap<SStringW, SStringW>, public TObjRefImpl2<IObjRef, STemplatePool>
	{
	public:
		/**
		* Init
		* @brief    Load temlpate table from xml node
		* @param    pugi::xml_node xmlNode --  xml node that describe temlpate list
		* @return   BOOL -- TRUE: loaded; FALSE:failed;
		* Describe
		*/
		BOOL Init(pugi::xml_node xmlNode);

		SStringW GetTemplateString(const SStringW &strName) const;
	};


	class SOUI_EXP STemplatePoolMgr : public SSingleton2<STemplatePoolMgr>
	{
		SINGLETON2_TYPE(SINGLETON_TEMPLATEPOOLMGR)
	public:
		~STemplatePoolMgr();

		/**
		* GetTemplateString
		* @brief    Get style object from pool by class name
		* @param    const SStringW & strName --  name of template
		* @param [out]  SwndStyle & style --  style
		* @return   BOOL -- TRUE: success; FALSE: not exist
		* Describe
		*/
		SStringW GetTemplateString(const SStringW & strName);

		/**
		* PushTemplatePool
		* @brief    push a new STemplatePool to the tail of style pool list
		* @param    STemplatePool * pTemplatePool --  the target style pool
		* @return   void
		* Describe
		*/
		void PushTemplatePool(STemplatePool *pTemplatePool);

		/**
		* PopTemplatePool
		* @brief    remove the target template pool from list
		* @param    STemplatePool * pTemplatePool --  the target template pool
		* @return   STemplatePool * -- the removed template pool
		* Describe  if STemplatePool is null, it remove the last template pool from the list
		*/
		STemplatePool * PopTemplatePool(STemplatePool *pTemplatePool);
	protected:
		SList<STemplatePool *> m_lstTemplatePools;
	};
}//end of namespace SOUI

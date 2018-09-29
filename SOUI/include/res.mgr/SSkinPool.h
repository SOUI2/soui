/**
* Copyright (C) 2014-2050 SOUI团队
* All rights reserved.
* 
* @file       SSkinPool.h
* @brief      SkinObj Pool
* @version    v1.0      
* @author     soui      
* @date       2014-05-28
* 
* Describe    管理Skin Object
*/

#pragma once
#include "core/SSingletonMap.h"
#include "interface/Sskinobj-i.h"
#include <unknown/obj-ref-impl.hpp>

#define GETSKIN(p1,scale) SOUI::SSkinPoolMgr::getSingleton().GetSkin(p1,scale)
#define GETBUILTINSKIN(p1) SOUI::SSkinPoolMgr::getSingleton().GetBuiltinSkin(p1,100)
#define GETBUILTINSKIN2(p1,scale) SOUI::SSkinPoolMgr::getSingleton().GetBuiltinSkin(p1,scale)
#define GETSKINPOOLMGR SOUI::SSkinPoolMgr::getSingletonPtr()

namespace SOUI
{
    //系统内置皮肤名称
    enum SYS_SKIN
    {
        SKIN_SYS_CHECKBOX=0,            //L"_skin.sys.checkbox",
        SKIN_SYS_RADIO,                 //L"_skin.sys.radio",
        SKIN_SYS_FOCUSCHECKBOX,         //L"_skin.sys.focuscheckbox",
        SKIN_SYS_FOCUSRADIO,            //L"_skin.sys.focusradio",
        SKIN_SYS_BTN_NORMAL,            //L"_skin.sys.btn.normal",
        SKIN_SYS_SCROLLBAR,             //L"_skin.sys.scrollbar",
        SKIN_SYS_BORDER,                //L"_skin.sys.border",
        SKIN_SYS_DROPBTN,               //L"_skin.sys.dropbtn",
        SKIN_SYS_TREE_TOGGLE,           //L"_skin.sys.tree.toggle",
        SKIN_SYS_TREE_CHECKBOX,         //L"_skin.sys.tree.checkbox"
        SKIN_SYS_TAB_PAGE,              //L"_skin.sys.tab.page",
        SKIN_SYS_HEADER,                //L"_skin.sys.header"
        SKIN_SYS_SPLIT_VERT,            //L"_skin.sys.split.vert",
        SKIN_SYS_SPLIT_HORZ,            //L"_skin.sys.split.horz",
        SKIN_SYS_PROG_BKGND,            //L"_skin.sys.prog.bkgnd",
        SKIN_SYS_PROG_BAR,              //L"_skin.sys.prog.bar",
        SKIN_SYS_VERT_PROG_BKGND,       //L"_skin.sys.vert.prog.bkgnd",
        SKIN_SYS_VERT_PROG_BAR,         //L"_skin.sys.vert.prog.bar",
        SKIN_SYS_SLIDER_THUMB,          //L"_skin.sys.slider.thumb",

        SKIN_SYS_BTN_CLOSE,             //L"_skin.sys.btn.close",
        SKIN_SYS_BTN_MINIMIZE,          //L"_skin.sys.btn.minimize",
        SKIN_SYS_BTN_MAXMIZE,           //L"_skin.sys.btn.maxmize",
        SKIN_SYS_BTN_RESTORE,           //L"_skin.sys.btn.restore",

        SKIN_SYS_MENU_CHECK,            //L"_skin.sys.menu.check",
        SKIN_SYS_MENU_SEP,              //L"_skin.sys.menu.sep",
        SKIN_SYS_MENU_ARROW,            //L"_skin.sys.menu.arrow",
        SKIN_SYS_MENU_BORDER,           //L"_skin.sys.menu.border",
        SKIN_SYS_MENU_SKIN,             //L"_skin.sys.menu.skin",
        SKIN_SYS_ICONS,                 //L"_skin.sys.icons",
        SKIN_SYS_WND_BKGND,             //L"_skin.sys.wnd.bkgnd",
        
		SKIN_SYS_BTN_PREV,
		SKIN_SYS_BTN_NEXT,
		SKIN_SYS_SPIN_DOWN,
		SKIN_SYS_SPIN_UP,

        SKIN_SYS_COUNT,
    };

typedef ISkinObj * SSkinPtr;


class SkinKey
{
public:
	SStringW strName;
	int		 scale;
};

template<>
class CElementTraits<SkinKey > :
	public CElementTraitsBase<SkinKey >
{
public:
	static ULONG Hash(INARGTYPE skinKey)
	{
		ULONG nHash = CElementTraits<SStringW>::Hash(skinKey.strName);

		nHash <<= 5;
		nHash += skinKey.scale;
		return(nHash);
	}

	static bool CompareElements(INARGTYPE element1, INARGTYPE element2)
	{
		return element1.strName == element2.strName
			&& element1.scale == element2.scale;
	}

	static int CompareElementsOrdered(INARGTYPE element1, INARGTYPE element2)
	{
		int nRet = element1.strName.CompareNoCase(element2.strName);
		if (nRet == 0) nRet = element1.scale -  element2.scale;
		return nRet;
	}
};

/**
* @class      SSkinPool
* @brief      name和ISkinObj的映射表
* 
* Describe    
*/
class SOUI_EXP SSkinPool :public SCmnMap<SSkinPtr,SkinKey>, public TObjRefImpl2<IObjRef,SSkinPool>
{
public:
    SSkinPool();

    virtual ~SSkinPool();

    /**
     * GetSkin
     * @brief    获得与指定name匹配的SkinObj
     * @param    LPCWSTR strSkinName --    Name of Skin Object     
     * @return   ISkinObj*  -- 找到的Skin Object
     * Describe  
     */    
    ISkinObj* GetSkin(const SStringW & strSkinName,int nScale);

    /**
     * LoadSkins
     * @brief    从XML中加载Skin列表
     * @param    pugi::xml_node xmlNode --  描述SkinObj的XML表     
     * @return   int -- 成功加载的SkinObj数量
     * Describe  
     */    
    int LoadSkins(pugi::xml_node xmlNode);
protected:
    static void OnKeyRemoved(const SSkinPtr & obj);
    
#ifdef _DEBUG
    SMap<SkinKey,int> m_mapSkinUseCount;   //皮肤使用计数
#endif
};

/**
* @class      SSkinPoolMgr
* @brief      管理一个name和ISkinObj的映射表的列表
* 
* Describe    
*/
class SOUI_EXP SSkinPoolMgr : public SSingleton2<SSkinPoolMgr> 
{
	SINGLETON2_TYPE(SINGLETON_SKINPOOLMGR)
public:
    SSkinPoolMgr();
    ~SSkinPoolMgr();

    /**
    * GetSkin
    * @brief    获得与指定name匹配的SkinObj
    * @param    LPCWSTR strSkinName --    Name of Skin Object     
    * @return   ISkinObj*  -- 找到的Skin Object
    * Describe  
    */    
    ISkinObj* GetSkin(const SStringW & strSkinName,int nScale);
    
    /**
     * PushSkinPool
     * @brief    向列表中增加一个新的SSkinPool对象
     * @param    SSkinPool * pSkinPool --    SSkinPool对象   
     * @return   void
     * Describe  
     */    
    void PushSkinPool(SSkinPool *pSkinPool);

    /**
     * PopSkinPool
     * @brief    弹出一个SSkinPool对象
     * @param    SSkinPool * pSkinPool --   准备弹出的SSkinPool对象
     * @return   SSkinPool *    在列表中找到后弹出的SSkinPool对象
     * Describe  内建SkinPool不用调用PopSkinPool
     */    
    SSkinPool * PopSkinPool(SSkinPool *pSkinPool);

    /**
     * GetBuiltinSkin
     * @brief    获得SOUI系统内建的命名SkinObj
     * @param    SYS_SKIN uID --  内建SKIN的ID
     * @return   ISkinObj * 与SKINID对应的ISkinObj
     * Describe  可能返回失败
     */    
    ISkinObj * GetBuiltinSkin(SYS_SKIN uID,int nScale);
    
    /**
     * GetBuiltinSkinPool
     * @brief    获得管理内建SkinPool对象
     * @return   SSkinPool * -- 内建SkinPool指针
     * Describe  用户在代码中创建的SkinObj可以交给内建SkinPool管理
     */    
    SSkinPool * GetBuiltinSkinPool(){return m_bulitinSkinPool;}

	void SetBuiltinSkinPool(SSkinPool *pSkinPool){
		m_bulitinSkinPool = pSkinPool;
	}
protected:
    SList<SSkinPool *> m_lstSkinPools;
    CAutoRefPtr<SSkinPool> m_bulitinSkinPool;

};


}//namespace SOUI
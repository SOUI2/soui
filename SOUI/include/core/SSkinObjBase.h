#pragma once

#include "interface/Sskinobj-i.h"
#include <unknown/obj-ref-impl.hpp>

namespace SOUI
{

    class SOUI_EXP SSkinObjBase : public TObjRefImpl<SObjectImpl<ISkinObj>>
    {
		SOUI_CLASS_NAME_EX(SSkinObjBase, L"skinObjBase", Skin)
    public:
        SSkinObjBase():m_byAlpha(0xFF),m_bEnableColorize(TRUE),m_crColorize(0),m_nScale(100){}

        /**
        * GetAlpha
        * @brief    获得skin对象包含透明度
        * @return   BYTE -- 透明度
        * Describe  [0-255]
        */    
        BYTE GetAlpha() const
        {
            return m_byAlpha;
        }

        /**
        * SetAlpha
        * @brief    设定skin对象包含透明度
        * @param    BYTE byAlpha-- 透明度
        * @return   void
        * Describe  
        */    
        virtual void SetAlpha(BYTE byAlpha)
        {
            m_byAlpha = byAlpha;
        }

        virtual void Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState,BYTE byAlpha)
        {
            _Draw(pRT,rcDraw,dwState,byAlpha);
        }

        virtual void Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState)
        {
            Draw(pRT,rcDraw,dwState,GetAlpha());
        }

        virtual SIZE GetSkinSize()
        {
            SIZE ret = {0, 0};

            return ret;
        }

        virtual BOOL IgnoreState()
        {
            return TRUE;
        }

        virtual int GetStates()
        {
            return 1;
        }

		virtual int GetScale() const{
			return m_nScale;
		}

		virtual ISkinObj * Scale(int nScale)
		{
			ISkinObj * skinObj = SApplication::getSingleton().CreateSkinByName(GetObjectClass());
			if(!skinObj) return NULL;
			_Scale(skinObj,nScale);
			return skinObj;
		}

		virtual LPCWSTR GetName() const
		{
			return m_strName;
		}

        SOUI_ATTRS_BEGIN()
            ATTR_INT(L"alpha",m_byAlpha,TRUE)   //皮肤透明度
            ATTR_INT(L"enableColorize",m_bEnableColorize,TRUE)
			ATTR_INT(L"scale",m_nScale,FALSE)
			ATTR_STRINGW(L"name",m_strName,FALSE)
        SOUI_ATTRS_END()

    protected:

		virtual void _Scale(ISkinObj *pObj, int nScale)
		{
			SSkinObjBase * pSkinObj = sobj_cast<SSkinObjBase>(pObj);
			pSkinObj->m_nScale = nScale;
			pSkinObj->m_byAlpha = m_byAlpha;
			pSkinObj->m_bEnableColorize = m_bEnableColorize;
			pSkinObj->m_crColorize = m_crColorize;
			pSkinObj->m_strName = m_strName;
		}

        /**
        * _Draw
        * @brief    Draw函数的实现
        * @param    IRenderTarget * pRT --  渲染目标
        * @param    LPCRECT rcDraw --  渲染位置
        * @param    DWORD dwState --  渲染状态
        * @param    BYTE byAlpha --  透明度
        * @return   void
        * Describe  每一个skin需要实现一个_Draw方法
        */    
        virtual void _Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState,BYTE byAlpha)=0;

        BYTE		m_byAlpha;
        COLORREF    m_crColorize;
        BOOL        m_bEnableColorize;
		int			m_nScale;

		SStringW	m_strName;

    };

}//namespace SOUI
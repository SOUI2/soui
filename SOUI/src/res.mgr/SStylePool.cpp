#include "souistd.h"
#include "res.mgr/SStylePool.h"

namespace SOUI
{

    //////////////////////////////////////////////////////////////////////////
    //SStylePool
    
    // Get style object from pool by class name
    pugi::xml_node SStylePool::GetStyle(LPCWSTR lpszName)
    {
        if(!HasKey(lpszName)) return pugi::xml_node();
        return GetKeyObject(lpszName);
    }

    // Load style-pool from xml tree
    BOOL SStylePool::Init(pugi::xml_node xmlStyleRoot)
    {
        if(!xmlStyleRoot) return FALSE;
        
        xmlStyleRoot = m_xmlDoc.append_copy(xmlStyleRoot);

		for (pugi::xml_node xmlChild = xmlStyleRoot.first_child(); xmlChild; xmlChild = xmlChild.next_sibling())
		{
			SStringW strClsName = xmlChild.name();
			if (strClsName.CompareNoCase(L"class") == 0)
			{
				strClsName = xmlChild.attribute(L"name").value();
				if (strClsName.IsEmpty()) continue;
				xmlChild.remove_attribute(L"name");//删除name属性，防止该属性被处理
			}
			SASSERT(!xmlChild.attribute(L"name"));
			AddKeyObject(strClsName, xmlChild);
		}

        return TRUE;
    }

    //////////////////////////////////////////////////////////////////////////
    // SStylePoolMgr
    pugi::xml_node SStylePoolMgr::GetStyle( LPCWSTR lpszName)
    {
        SPOSITION pos=m_lstStylePools.GetTailPosition();
        while(pos)
        {
            SStylePool *pStylePool=m_lstStylePools.GetPrev(pos);
            pugi::xml_node style = pStylePool->GetStyle(lpszName);
            if(style) return style; 
        }
        return pugi::xml_node();
    }

    void SStylePoolMgr::PushStylePool( SStylePool *pStylePool )
    {
        m_lstStylePools.AddTail(pStylePool);
        pStylePool->AddRef();
    }

    SStylePool * SStylePoolMgr::PopStylePool(SStylePool *pStylePool)
    {
        SStylePool * pRet=NULL;
        if(pStylePool)
        {
            SPOSITION pos=m_lstStylePools.Find(pStylePool);
            if(pos)
            {
                pRet=m_lstStylePools.GetAt(pos);
                m_lstStylePools.RemoveAt(pos);
            }
        }else
        {
            pRet = m_lstStylePools.RemoveTail();
        }
        if(pRet) pRet->Release();
        return pRet;
    }

    SStylePoolMgr::~SStylePoolMgr()
    {
        SPOSITION pos=m_lstStylePools.GetHeadPosition();
        while(pos)
        {
            SStylePool *p = m_lstStylePools.GetNext(pos);
            p->Release();
        }
        m_lstStylePools.RemoveAll();
    }

}//end of namespace SOUI

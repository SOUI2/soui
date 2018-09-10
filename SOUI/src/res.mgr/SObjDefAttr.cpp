#include "souistd.h"
#include "res.mgr/SObjDefAttr.h"
#include "SApp.h"
#include "helper/mybuffer.h"

namespace SOUI
{

BOOL SObjDefAttr::Init( pugi::xml_node xmlNode )
{
    if(!xmlNode) return FALSE;

    m_xmlRoot.append_copy(xmlNode);

    pugi::xml_node xmlObjAttr=m_xmlRoot.child(L"objattr").first_child();
    while(xmlObjAttr)
    {
        AddKeyObject(xmlObjAttr.name(),xmlObjAttr);
        xmlObjAttr=xmlObjAttr.next_sibling();
    }

    SPOSITION pos=m_mapNamedObj->GetStartPosition();
    while(pos)
    {
        SMap<SStringW,pugi::xml_node>::CPair *p=m_mapNamedObj->GetNext(pos);
        BuildClassAttribute(p->m_value,p->m_key);
    }

    return TRUE;
}

void SObjDefAttr::BuildClassAttribute( pugi::xml_node & xmlNode, LPCWSTR pszClassName)
{
    SObjectInfo baseClassInfo =SApplication::getSingleton().BaseObjectInfoFromObjectInfo(SObjectInfo(pszClassName,Window));
    if(!baseClassInfo.IsValid()) return;

    if(HasKey(baseClassInfo.mName))
    {
        pugi::xml_node xmlNodeAttrs = GetKeyObject(baseClassInfo.mName);
        pugi::xml_attribute attr=xmlNodeAttrs.first_attribute();
        while(attr)
        {
            if(!xmlNode.attribute(attr.name()))
                xmlNode.append_attribute(attr.name()).set_value(attr.value());
            attr=attr.next_attribute();
        }
    }
    BuildClassAttribute(xmlNode, baseClassInfo.mName);
}

pugi::xml_node SObjDefAttr::GetDefAttribute(LPCWSTR pszClassName )
{
    SASSERT(pszClassName);
	if (!HasKey(pszClassName))
	{
		SObjectInfo baseClassInfo = SApplication::getSingleton().BaseObjectInfoFromObjectInfo(SObjectInfo(pszClassName, Window));
		if (!baseClassInfo.IsValid()) return pugi::xml_node();

		return GetDefAttribute(baseClassInfo.mName);
	}
	else
	{
		return GetKeyObject(pszClassName);
	}
}

}//namespace SOUI


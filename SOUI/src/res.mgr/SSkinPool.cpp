#include "souistd.h"
#include "res.mgr/SSkinPool.h"
#include "core/Sskin.h"
#include "SApp.h"
#include "helper/mybuffer.h"

namespace SOUI
{

//////////////////////////////////////////////////////////////////////////
// SSkinPool

SSkinPool::SSkinPool()
{
    m_pFunOnKeyRemoved=OnKeyRemoved;
}

SSkinPool::~SSkinPool()
{
#ifdef _DEBUG
    //查询哪些皮肤运行过程中没有使用过,将结果用输出到Output
    STRACEW(L"####Detecting Defined Skin Usage BEGIN");    
    SPOSITION pos = m_mapNamedObj->GetStartPosition();
    while(pos)
    {
        SkinKey skinKey = m_mapNamedObj->GetNextKey(pos);
        if(!m_mapSkinUseCount.Lookup(skinKey))
        {
            STRACEW(L"skin of [%s.%d] was not used.",skinKey.strName,skinKey.scale);
        }
    }
    STRACEW(L"!!!!Detecting Defined Skin Usage END");    
#endif
}

int SSkinPool::LoadSkins(pugi::xml_node xmlNode)
{
    if(!xmlNode) return 0;
    
    int nLoaded=0;
    SStringW strSkinName, strTypeName;

	//loadSkins前把this加入到poolmgr,便于在skin中引用其它skin
	SSkinPoolMgr::getSingleton().PushSkinPool(this);

    pugi::xml_node xmlSkin=xmlNode.first_child();
    while(xmlSkin)
    {
        strTypeName = xmlSkin.name();
        strSkinName = xmlSkin.attribute(L"name").value();

        if (strSkinName.IsEmpty() || strTypeName.IsEmpty())
        {
            xmlSkin=xmlSkin.next_sibling();
            continue;
        }
            
        
        ISkinObj *pSkin=SApplication::getSingleton().CreateSkinByName(strTypeName);
        if(pSkin)
        {
            pSkin->InitFromXml(xmlSkin);
			SkinKey key = {strSkinName,pSkin->GetScale()};
			SASSERT(!HasKey(key));
            AddKeyObject(key,pSkin);
            nLoaded++;
        }
        else
        {
            SASSERT_FMTW(FALSE,L"load skin error,type=%s,name=%s",strTypeName,strSkinName);
        }
        xmlSkin=xmlSkin.next_sibling();
    }

	//由于push时直接把this加入tail，为了防止重复调用，这里传NULL,直接从tail删除。
	SSkinPoolMgr::getSingleton().PopSkinPool(NULL);

    return nLoaded;
}

const int KBuiltinScales [] =
{
	100,125,150,200,250,300
};

/*标准化放大比例, 选择比自己指定比例小一号的比例*/
int NormalizeScale(int nScale)
{
	for (int i = 1; i < ARRAYSIZE(KBuiltinScales); i++)
	{
		if (nScale < KBuiltinScales[i])
		{
			return KBuiltinScales[i-1];
		}
	}
	return KBuiltinScales[ARRAYSIZE(KBuiltinScales) - 1];
}

ISkinObj* SSkinPool::GetSkin(const SStringW & strSkinName,int nScale)
{
	SkinKey key ={strSkinName,nScale};

    if(!HasKey(key))
    {
		nScale = NormalizeScale(nScale);
		key.scale = nScale;
		if (!HasKey(key))
		{
			bool bFind = false;
			for (int i = 0; i < ARRAYSIZE(KBuiltinScales); i++)
			{
				key.scale = KBuiltinScales[i];
				bFind = HasKey(key);
				if (bFind) break;
			}
			if (!bFind)
				return NULL;

			ISkinObj * pSkinSrc = GetKeyObject(key);
			ISkinObj * pSkin = pSkinSrc->Scale(nScale);
			if (pSkin)
			{
				key.scale = nScale;
				AddKeyObject(key, pSkin);
			}
		}
    }
#ifdef _DEBUG
    m_mapSkinUseCount[key]++;
#endif
    return GetKeyObject(key);
}

void SSkinPool::OnKeyRemoved(const SSkinPtr & obj )
{
    obj->Release();
}

//////////////////////////////////////////////////////////////////////////

SSkinPoolMgr::SSkinPoolMgr()
{
    m_bulitinSkinPool.Attach(new SSkinPool);
    PushSkinPool(m_bulitinSkinPool);
}

SSkinPoolMgr::~SSkinPoolMgr()
{
    SPOSITION pos=m_lstSkinPools.GetHeadPosition();
    while(pos)
    {
        SSkinPool *p = m_lstSkinPools.GetNext(pos);
        p->Release();
    }
    m_lstSkinPools.RemoveAll();

}

ISkinObj* SSkinPoolMgr::GetSkin( const SStringW & strSkinName, int nScale)
{
    SPOSITION pos=m_lstSkinPools.GetTailPosition();
    while(pos)
    {
        SSkinPool *pSkinPool=m_lstSkinPools.GetPrev(pos);
        if(ISkinObj* pSkin=pSkinPool->GetSkin(strSkinName,nScale))
        {
            return pSkin;
        }
    }

    if(wcscmp(strSkinName,L"")!=0)
    {
        SASSERT_FMTW(FALSE,L"GetSkin[%s] Failed!",strSkinName);
    }
    return NULL;
}

const wchar_t * BUILDIN_SKIN_NAMES[]=
{
    L"_skin.sys.checkbox",
    L"_skin.sys.radio",
    L"_skin.sys.focuscheckbox",
    L"_skin.sys.focusradio",
    L"_skin.sys.btn.normal",
    L"_skin.sys.scrollbar",
    L"_skin.sys.border",
    L"_skin.sys.dropbtn",
    L"_skin.sys.tree.toggle",
    L"_skin.sys.tree.checkbox",
    L"_skin.sys.tab.page",
    L"_skin.sys.header",
    L"_skin.sys.split.vert",
    L"_skin.sys.split.horz",
    L"_skin.sys.prog.bkgnd",
    L"_skin.sys.prog.bar",
    L"_skin.sys.vert.prog.bkgnd",
    L"_skin.sys.vert.prog.bar",
    L"_skin.sys.slider.thumb",
    L"_skin.sys.btn.close",
    L"_skin.sys.btn.minimize",
    L"_skin.sys.btn.maxmize",
    L"_skin.sys.btn.restore",
    L"_skin.sys.menu.check",
    L"_skin.sys.menu.sep",
    L"_skin.sys.menu.arrow",
    L"_skin.sys.menu.border",
    L"_skin.sys.menu.skin",
    L"_skin.sys.icons",
    L"_skin.sys.wnd.bkgnd",
	L"_skin.sys.btn.prev",
	L"_skin.sys.btn.next",
	L"_skin.sys.spin.down",
	L"_skin.sys.spin.up",
};


ISkinObj * SSkinPoolMgr::GetBuiltinSkin( SYS_SKIN uID ,int nScale)
{
    return GetBuiltinSkinPool()->GetSkin(BUILDIN_SKIN_NAMES[uID], nScale);
}

void SSkinPoolMgr::PushSkinPool( SSkinPool *pSkinPool )
{
    m_lstSkinPools.AddTail(pSkinPool);
    pSkinPool->AddRef();
}

SSkinPool * SSkinPoolMgr::PopSkinPool(SSkinPool *pSkinPool)
{
    SSkinPool * pRet=NULL;
    if(pSkinPool)
    {
        if(pSkinPool == m_bulitinSkinPool) return NULL;

        SPOSITION pos=m_lstSkinPools.Find(pSkinPool);
        if(pos)
        {
            pRet=m_lstSkinPools.GetAt(pos);
            m_lstSkinPools.RemoveAt(pos);
        }
    }else
    {
        pRet = m_lstSkinPools.RemoveTail();
    }
    if(pRet) pRet->Release();
    return pRet;
}

}//namespace SOUI
#include "stdafx.h"
#include "SetSkinWnd2.h"
#include "SDemoSkin.h"
#include <winuser.h>
#include "SSkinLoader.h"
#include <io.h>

#define SKIN_CHANGE_MSG _T("{D17D208B-25FD-412C-8071-68816D4B1F9B}")
//注册皮肤改变消息
UINT g_dwSkinChangeMessage = RegisterWindowMessage(SKIN_CHANGE_MSG);

HRESULT CSetSkinWnd::OnSkinChangeMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL bHandled)
{
	FindChildByID(9527)->Invalidate();
	return S_OK;
}

void CSetSkinWnd::OnColor(EventArgs * e)
{
	SWindow *sender = (SWindow*) e->sender;
	SDemoSkin *skin = (SDemoSkin *) GETSKIN(L"demoskinbk",GetScale());
	if (skin)
	{
		skin->SetColor(sender->GetStyle().m_crBg);
		NotifUpdataWindow();
	}
}

long CSetSkinWnd::NotifUpdataWindow()
{
	WPARAM   wParam = MagicNumber;
	LPARAM   lParam = MagicNumber;
	DWORD   dwRecipients = BSM_APPLICATIONS;
	return ::BroadcastSystemMessage(BSF_POSTMESSAGE, &dwRecipients, g_dwSkinChangeMessage, wParam, lParam);
}

void CSetSkinWnd::LoadSkinConfigFormXml()
	{
		SStringT strSkinConfigPath = SApplication::getSingleton().GetAppDir() + _T("\\themes\\themes_config.xml");

		pugi::xml_document docLoad;
		pugi::xml_parse_result result = docLoad.load_file(strSkinConfigPath);
		if (result)
		{
			pugi::xml_node skinInf = docLoad.child(L"DEMO_SKIN_CONFIG").child(L"skinInf");
			while (skinInf)
			{
				SKIN_CONFIG_INF inf;
				inf.id = (SkinType)skinInf.attribute(L"id").as_int();
				int v1 = 0, v2 = 0, v3 = 0, v4 = 0;
				swscanf(skinInf.attribute(L"skin_margin").as_string(), L"%d,%d,%d,%d", &v1, &v2, &v3, &v4);
				inf.margin.left = v1;
				inf.margin.top = v2;
				inf.margin.right = v3;
				inf.margin.bottom = v4;
				m_skinConfigInf.AddTail(inf);
				skinInf=skinInf.next_sibling();
			}
		}		
	}
CSetSkinWnd::CSetSkinWnd() :SHostWnd(_T("LAYOUT:dlg_set_skin"))
{	
	LoadSkinConfigFormXml();
}

CSetSkinWnd::~CSetSkinWnd()
{
}

void CSetSkinWnd::OnActivate(UINT nState, BOOL bMinimized, HWND wndOther)
{
	if (nState == WA_INACTIVE)
		DestroyWindow();
	else
		SHostWnd::OnActivate(nState, bMinimized, wndOther);
}

BOOL CSetSkinWnd::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	return 0;
}

CRect CSetSkinWnd::GetMargin(int id)
{
	SPOSITION headPos= m_skinConfigInf.GetHeadPosition();
	while (headPos)
	{
		SKIN_CONFIG_INF inf= m_skinConfigInf.GetNext(headPos);
		if (inf.id == id)
		{
			return inf.margin;
		}
	}
	return CRect();
}

void CSetSkinWnd::OnSetSkin(EventArgs * e)
{
	SWindow *sender = (SWindow*) e->sender;
	int nIndex = sender->GetID();
	SDemoSkin *skin = (SDemoSkin *) GETSKIN(L"demoskinbk",GetScale());
	SStringT strSkinFile;
	SStringT strSkinPath = SApplication::getSingleton().GetAppDir() + _T("\\themes\\");
	strSkinFile.Format(_T("%s%d.png"), strSkinPath, nIndex - 9);
	SStringT strLoadSkin;
	strLoadSkin.Format(_T("themes\\skin%d"), ((nIndex - 9)%3)+1);
	SSkinLoader::getSingleton().LoadSkin(strLoadSkin);

	if (_taccess(strSkinFile, 0) != 0){
		SMessageBox(NULL, _T("无法设置当前主题,找不到系统主题文件。复制demos\\demo\\themes\\文件夹到soui根目录!"), _T("警告"), NULL);
		return;
	}
	if (skin)
	{
		skin->SetImage(S_CT2W(strSkinFile));
		skin->SetMargin(GetMargin(nIndex-9));
		NotifUpdataWindow();
	}
}
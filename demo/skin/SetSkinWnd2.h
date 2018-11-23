#pragma once

#define MagicNumber 9527
extern UINT g_dwSkinChangeMessage;
struct SKIN_CONFIG_INF
{
	int id;
	CRect margin;
};
class CSetSkinWnd :
	public SHostWnd
{
	//TODO:消息映射
	void OnClose()
	{
		CSimpleWnd::DestroyWindow();
	}
	HRESULT OnSkinChangeMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL bHandled);
	
	void OnColor(EventArgs * e);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
protected:
	virtual void OnFinalMessage(HWND hWnd) {
		__super::OnFinalMessage(hWnd);
		delete this;
	}
	long NotifUpdataWindow();
	void LoadSkinConfigFormXml();
	CRect GetMargin(int id);	
public:
	CSetSkinWnd();
	~CSetSkinWnd();
	void OnSetSkin(EventArgs *e);
	void OnActivate(UINT nState, BOOL bMinimized, HWND wndOther);
	void OnBuiltinSkin();

	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_close", OnClose)
		EVENT_ID_RANGE_HANDLER(10, 27, EVT_CMD, OnSetSkin)
		EVENT_ID_RANGE_HANDLER(30,48,EVT_CMD, OnColor)		
		EVENT_ID_COMMAND(51,OnBuiltinSkin)
	EVENT_MAP_END()
		BEGIN_MSG_MAP_EX(CSetSkinWnd)
		MSG_WM_INITDIALOG(OnInitDialog)
		MESSAGE_HANDLER(g_dwSkinChangeMessage, OnSkinChangeMessage)
		//MSG_WM_ACTIVATE(OnActivate)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
		END_MSG_MAP()
private:
	SList<SKIN_CONFIG_INF> m_skinConfigInf;
};


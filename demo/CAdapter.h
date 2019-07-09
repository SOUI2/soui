#pragma once
#include <helper/SAdapterBase.h>
class CTestAdapterFix : public SAdapterBase
{
	int * m_pCbxSel;
public:

	CTestAdapterFix()
	{
		m_pCbxSel = new int[getCount()];
		memset(m_pCbxSel, 0, sizeof(int)*getCount());
	}

	~CTestAdapterFix()
	{
		delete[]m_pCbxSel;
	}

	virtual int getCount()
	{
		return 50000;
	}

	virtual void getView(int position, SWindow * pItem, pugi::xml_node xmlTemplate)
	{
		if (pItem->GetChildrenCount() == 0)
		{
			pItem->InitFromXml(xmlTemplate);
		}

		SButton *pBtn = pItem->FindChildByName2<SButton>(L"btn_test");
		pBtn->SetWindowText(SStringT().Format(_T("button %d"), position));
		pBtn->GetRoot()->SetUserData(position);

		//由网友“从未来过” 修改的新事件订阅方式,采用模板函数从响应函数中自动提取事件类型，2016.12.13
		pBtn->GetEventSet()->subscribeEvent(&CTestAdapterFix::OnButtonClick, this);

		SComboBox * pCbx = pItem->FindChildByName2<SComboBox>(L"cbx_in_lv");
		if (pCbx)
		{
			pCbx->SetCurSel(m_pCbxSel[position]);
			pCbx->GetEventSet()->subscribeEvent(&CTestAdapterFix::OnCbxSelChange, this);
		}
	}

	bool OnCbxSelChange(EventCBSelChange *pEvt)
	{
		SComboBox *pCbx = sobj_cast<SComboBox>(pEvt->sender);
		int iItem = pCbx->GetRoot()->GetUserData();
		m_pCbxSel[iItem] = pCbx->GetCurSel();
		return true;
	}

	bool OnButtonClick(EventCmd *pEvt)
	{
		SButton *pBtn = sobj_cast<SButton>(pEvt->sender);
		int iItem = pBtn->GetRoot()->GetUserData();
		SMessageBox(NULL, SStringT().Format(_T("button of %d item was clicked"), iItem), _T("haha"), MB_OK);
		return true;
	}
};


const wchar_t * KAttrName_Height[] = {
	L"oddHeight",
	L"evenHeight",
	L"evenSelHeight"
};

const wchar_t* KNodeName_Item[] = {
	L"itemOdd",
	L"itemEven",
	L"itemEvenHover"
};
class CTestAdapterFlex : public SAdapterBase
{
public:
	int m_nItemHeight[3];


	CTestAdapterFlex()
	{

	}

	virtual void InitByTemplate(pugi::xml_node xmlTemplate)
	{
		m_nItemHeight[0] = xmlTemplate.attribute(KAttrName_Height[0]).as_int(50);
		m_nItemHeight[1] = xmlTemplate.attribute(KAttrName_Height[1]).as_int(60);
		m_nItemHeight[2] = xmlTemplate.attribute(KAttrName_Height[2]).as_int(70);
	}

	virtual int getCount()
	{
		return 12340;
	}

	virtual int getViewTypeCount() { return 3; }

	virtual int getItemViewType(int position, DWORD dwState)
	{
		if (position % 2 == 0)
			return 0;//1,3,5,... odd lines
		else if (dwState & WndState_Hover)
			return 2;//even lines with check state
		else
			return 1;//even lines 
	}

	virtual SIZE getViewDesiredSize(int position, SWindow *pItem, LPCRECT prcContainer)
	{
		DWORD dwState = pItem->GetState();
		int viewType = getItemViewType(position, dwState);
		return CSize(0, m_nItemHeight[viewType]);//cx在listview，mclistview中没有使用，不需要计算
	}

	virtual void getView(int position, SWindow * pItem, pugi::xml_node xmlTemplate)
	{
		if (pItem->GetChildrenCount() == 0)
		{
			int nViewType = getItemViewType(position, pItem->GetState());
			pItem->InitFromXml(xmlTemplate.child(KNodeName_Item[nViewType]));
		}
		pItem->GetEventSet()->subscribeEvent(EventSwndStateChanged::EventID, Subscriber(&CTestAdapterFlex::OnItemStateChanged, this));

		SButton *pBtn = pItem->FindChildByName2<SButton>(L"btn_test");
		pBtn->SetWindowText(SStringT().Format(_T("button %d"), position));
		pBtn->SetUserData(position);
		pBtn->GetEventSet()->subscribeEvent(EVT_CMD, Subscriber(&CTestAdapterFlex::OnButtonClick, this));
	}

	bool OnItemStateChanged(EventArgs *e)
	{
		EventSwndStateChanged *e2 = sobj_cast<EventSwndStateChanged>(e);
		if (!e2->CheckState(WndState_Hover)) return false;
		//通知界面重绘
		notifyDataSetInvalidated();
		return true;
	}

	bool OnButtonClick(EventArgs *pEvt)
	{
		SButton *pBtn = sobj_cast<SButton>(pEvt->sender);
		int iItem = pBtn->GetUserData();
		SMessageBox(NULL, SStringT().Format(_T("button of %d item was clicked"), iItem), _T("haha"), MB_OK);
		return true;
	}

};

class CTestMcAdapterFix : public SMcAdapterBase
{
#define NUMSCALE 100000
public:
	struct SOFTINFO
	{
		const wchar_t * pszSkinName;
		const wchar_t * pszName;
		const wchar_t * pszDesc;
		float     fScore;
		DWORD     dwSize;
		const wchar_t * pszInstallTime;
		const wchar_t * pszUseTime;
	};

	SArray<SOFTINFO> m_softInfo;

public:
	CTestMcAdapterFix()
	{
		SOFTINFO info[] =
		{
			{
				L"skin_icon1",
				L"鲁大师",
				L"鲁大师是一款专业的硬件检测，驱动安装工具",
				5.4f,
				15 * (1 << 20),
				L"2015-8-5",
				L"今天"
			},
			{
				L"skin_icon2",
				L"PhotoShop",
				L"强大的图片处理工具",
				9.0f,
				150 * (1 << 20),
				L"2015-8-5",
				L"今天"
			},
			{
				L"skin_icon3",
				L"QQ7.0",
				L"腾讯公司出品的即时聊天工具",
				8.0f,
				40 * (1 << 20),
				L"2015-8-5",
				L"今天"
			},
			{
				L"skin_icon4",
				L"Visual Studio 2008",
				L"Microsoft公司的程序开发套件",
				9.0f,
				40 * (1 << 20),
				L"2015-8-5",
				L"今天"
			},
			{
				L"skin_icon5",
				L"YY8",
				L"YY语音",
				9.0f,
				20 * (1 << 20),
				L"2015-8-5",
				L"今天"
			},
			{
				L"skin_icon6",
				L"火狐浏览器",
				L"速度最快的浏览器",
				8.5f,
				35 * (1 << 20),
				L"2015-8-5",
				L"今天"
			},
			{
				L"skin_icon7",
				L"迅雷",
				L"迅雷下载软件",
				7.3f,
				17 * (1 << 20),
				L"2015-8-5",
				L"今天"
			}
		};


		for (int i = 0; i < ARRAYSIZE(info); i++)
		{
			m_softInfo.Add(info[i]);
		}
	}

	virtual int getCount()
	{
		return m_softInfo.GetCount()*NUMSCALE;
	}

	SStringT getSizeText(DWORD dwSize)
	{
		int num1 = dwSize / (1 << 20);
		dwSize -= num1 *(1 << 20);
		int num2 = dwSize * 100 / (1 << 20);
		return SStringT().Format(_T("%d.%02dM"), num1, num2);
	}

	virtual void getView(int position, SWindow * pItem, pugi::xml_node xmlTemplate)
	{
		if (pItem->GetChildrenCount() == 0)
		{
			pItem->InitFromXml(xmlTemplate);
		}

		SOFTINFO *psi = m_softInfo.GetData() + position%m_softInfo.GetCount();
		pItem->FindChildByName(L"img_icon")->SetAttribute(L"skin", psi->pszSkinName);
		pItem->FindChildByName(L"txt_name")->SetWindowText(S_CW2T(psi->pszName));
		pItem->FindChildByName(L"txt_desc")->SetWindowText(S_CW2T(psi->pszDesc));
		pItem->FindChildByName(L"txt_score")->SetWindowText(SStringT().Format(_T("%1.2f 分"), psi->fScore));
		pItem->FindChildByName(L"txt_installtime")->SetWindowText(S_CW2T(psi->pszInstallTime));
		pItem->FindChildByName(L"txt_usetime")->SetWindowText(S_CW2T(psi->pszUseTime));
		pItem->FindChildByName(L"txt_size")->SetWindowText(getSizeText(psi->dwSize));
		pItem->FindChildByName2<SRatingBar>(L"rating_score")->SetValue(psi->fScore / 2);
		pItem->FindChildByName(L"txt_index")->SetWindowText(SStringT().Format(_T("第%d行"), position + 1));

		SButton *pBtnUninstall = pItem->FindChildByName2<SButton>(L"btn_uninstall");
		pBtnUninstall->SetUserData(position);
		pBtnUninstall->GetEventSet()->subscribeEvent(EVT_CMD, Subscriber(&CTestMcAdapterFix::OnButtonClick, this));
	}

	bool OnButtonClick(EventArgs *pEvt)
	{
		SButton *pBtn = sobj_cast<SButton>(pEvt->sender);
		int iItem = pBtn->GetUserData();
		//添加一个菜单弹出示例
		if(iItem%2==0)
		{
			CRect rc = pBtn->GetWindowRect();
			SItemPanel* pItemPanel = (SItemPanel*)pBtn->GetTopLevelParent();
			CRect rc2 = pItemPanel->GetItemRect();
			rc += rc2.TopLeft();
			POINT pt = { rc.left,rc.bottom };
			ClientToScreen(pItemPanel->GetContainer()->GetHostHwnd(), &pt);
			SMenu menu;
			menu.LoadMenu(_T("menu_test"), _T("SMENU"));
			menu.TrackPopupMenu(0, pt.x, pt.y + 2, pItemPanel->GetContainer()->GetHostHwnd());
		}
		else
		{
			if (SMessageBox(NULL, SStringT().Format(_T("Are you sure to uninstall the selected [%d] software?"), iItem), _T("uninstall"), MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
			{//删除一条记录
				DeleteItem(iItem);
			}
		}
		return true;
	}

	//删除一行，提供外部调用。
	void DeleteItem(int iPosition)
	{
		if (iPosition >= 0 && iPosition < getCount())
		{
			int iItem = iPosition % m_softInfo.GetCount();
			m_softInfo.RemoveAt(iItem);
			notifyDataSetChanged();
		}
	}

	SStringW GetColumnName(int iCol) const {
		return SStringW().Format(L"col%d", iCol + 1);
	}

	struct SORTCTX
	{
		int iCol;
		SHDSORTFLAG stFlag;
	};

	bool OnSort(int iCol, SHDSORTFLAG * stFlags, int nCols)
	{
		if (iCol == 5) //最后一列“操作”不支持排序
			return false;

		SHDSORTFLAG stFlag = stFlags[iCol];
		switch (stFlag)
		{
		case ST_NULL:stFlag = ST_UP; break;
		case ST_DOWN:stFlag = ST_UP; break;
		case ST_UP:stFlag = ST_DOWN; break;
		}
		for (int i = 0; i < nCols; i++)
		{
			stFlags[i] = ST_NULL;
		}
		stFlags[iCol] = stFlag;

		SORTCTX ctx = { iCol,stFlag };
		qsort_s(m_softInfo.GetData(), m_softInfo.GetCount(), sizeof(SOFTINFO), SortCmp, &ctx);
		return true;
	}

	static int __cdecl SortCmp(void *context, const void * p1, const void * p2)
	{
		SORTCTX *pctx = (SORTCTX*)context;
		const SOFTINFO *pSI1 = (const SOFTINFO*)p1;
		const SOFTINFO *pSI2 = (const SOFTINFO*)p2;
		int nRet = 0;
		switch (pctx->iCol)
		{
		case 0://name
			nRet = wcscmp(pSI1->pszName, pSI2->pszName);
			break;
		case 1://score
		{
			float fCmp = (pSI1->fScore - pSI2->fScore);
			if (fabs(fCmp) < 0.0000001) nRet = 0;
			else if (fCmp > 0.0f) nRet = 1;
			else nRet = -1;
		}
		break;
		case 2://size
			nRet = (int)(pSI1->dwSize - pSI2->dwSize);
			break;
		case 3://install time
			nRet = wcscmp(pSI1->pszInstallTime, pSI2->pszInstallTime);
			break;
		case 4://user time
			nRet = wcscmp(pSI1->pszUseTime, pSI2->pszUseTime);
			break;

		}
		if (pctx->stFlag == ST_UP)
			nRet = -nRet;
		return nRet;
	}
};



SStringW skins[5] = {
	L"skin_icon1",
	L"skin_icon2",
	L"skin_icon3",
	L"skin_icon4",
	L"skin_icon5"
};

class CTestTileAdapter : public SAdapterBase
{
public:
	CTestTileAdapter()
	{

	}
	virtual int getCount()
	{
		return 50000;
	}

	virtual void getView(int position, SWindow *pItem, pugi::xml_node xmlTemplate)
	{
		if (pItem->GetChildrenCount() == 0)
		{
			pItem->InitFromXml(xmlTemplate);
		}
		SImageWnd *pImg = pItem->FindChildByName2<SImageWnd>(L"img_file_icon");
		pImg->SetSkin(GETSKIN(skins[position % 5], pImg->GetScale()));
		SButton *pBtn = pItem->FindChildByName2<SButton>(L"btn_test");
		pBtn->SetWindowText(SStringT().Format(_T("btn %d"), position));
		pBtn->GetRoot()->SetUserData(position);
		pBtn->GetEventSet()->subscribeEvent(EVT_CMD, Subscriber(&CTestTileAdapter::OnButtonClick, this));
	}

	bool OnButtonClick(EventArgs *pEvt)
	{
		SButton *pBtn = sobj_cast<SButton>(pEvt->sender);
		int iItem = pBtn->GetRoot()->GetUserData();
		SMessageBox(NULL, SStringT().Format(_T("button of %d item was clicked"), iItem), _T("haha"), MB_OK);
		return true;
	}

};


struct TreeItemData
{
	SStringW strName;
	int      nAge;
	SStringT strTstLong;
};

class CTreeViewAdapter :public STreeAdapterBase<TreeItemData>
{
public:

	CTreeViewAdapter() {
		TreeItemData data;
		data.strName = L"name root";
		data.nAge = 100;

		HSTREEITEM hRoot = InsertItem(data);
		SetItemExpanded(hRoot, FALSE);
		for (int i = 0; i < 100; i++)
		{
			data.strName.Format(L"branch_%d", i);
			data.nAge++;
			data.strTstLong = _T("red text");
			if (i == 50) data.strTstLong = _T("Long Text Test. When this item is shown, the treeview size should be extended automatically.");
			InsertItem(data, hRoot);
		}
		//ExpandItem(hRoot,ITvAdapter::TV_TOGGLE);
	}

	~CTreeViewAdapter() {}

	virtual void getView(SOUI::HTREEITEM loc, SWindow * pItem, pugi::xml_node xmlTemplate) {
		if (pItem->GetChildrenCount() == 0)
		{
			pItem->InitFromXml(xmlTemplate);
		}
		ItemInfo & ii = m_tree.GetItemRef((HSTREEITEM)loc);
		SWindow * pWnd = pItem->FindChildByID(R.id.btn_test);
		SASSERT(pWnd);
		pWnd->SetWindowText(S_CW2T(ii.data.strName));
		SWindow *pTxtRed = pItem->FindChildByID(R.id.txt_red);
		SASSERT(pTxtRed);
		pTxtRed->SetWindowText(ii.data.strTstLong);

		SToggle *pSwitch = pItem->FindChildByID2<SToggle>(R.id.tgl_switch);
		SASSERT(pSwitch);
		pSwitch->SetVisible(HasChildren(loc));
		pSwitch->SetToggle(IsItemExpanded(loc));
		pSwitch->GetEventSet()->subscribeEvent(EVT_CMD, Subscriber(&CTreeViewAdapter::OnSwitchClick, this));
	}

	bool OnSwitchClick(EventArgs *pEvt)
	{
		SToggle *pToggle = sobj_cast<SToggle>(pEvt->sender);
		SASSERT(pToggle);
		SItemPanel *pItem = sobj_cast<SItemPanel>(pToggle->GetRoot());
		SASSERT(pItem);
		SOUI::HTREEITEM loc = (SOUI::HTREEITEM)pItem->GetItemIndex();
		ExpandItem(loc, ITvAdapter::TVC_TOGGLE);
		return true;
	}
};

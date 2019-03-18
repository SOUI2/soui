#include "souistd.h"
#include "core/SWnd.h"
#include "helper/color.h"
#include "helper/SplitString.h"
#include "layout/SouiLayout.h"
#include "interface/sacchelper-i.h"
namespace SOUI
{

	//////////////////////////////////////////////////////////////////////////
	// STextTr
	//////////////////////////////////////////////////////////////////////////
	STrText::STrText(ITrCtxProvider *pProvider /*= NULL*/):pTrCtxProvider(pProvider)
	{

	}

	void STrText::SetCtxProvider(ITrCtxProvider *pProvider)
	{
		pTrCtxProvider = pProvider;
	}

	SStringT STrText::GetText(BOOL bRawText) const
	{
		return bRawText?strRaw:strTr;
	}

	void STrText::SetText(const SStringT& strText)
	{
		strRaw = strText;
		TranslateText();
	}

	void STrText::TranslateText()
	{
		if(pTrCtxProvider == NULL) return;
		strTr = S_CW2T(TR(S_CT2W(strRaw),pTrCtxProvider->GetTrCtx()));
	}

	//////////////////////////////////////////////////////////////////////////
	// SWindow Implement
	//////////////////////////////////////////////////////////////////////////

	SWindow::SWindow()
		: m_swnd(SWindowMgr::NewWindow(this))
		, m_nID(0)
		, m_pContainer(NULL)
		, m_pParent(NULL),m_pFirstChild(NULL),m_pLastChild(NULL),m_pNextSibling(NULL),m_pPrevSibling(NULL)
		, m_nChildrenCount(0)
		, m_uZorder(0)
		, m_dwState(WndState_Normal)
		, m_bMsgTransparent(FALSE)
		, m_bVisible(TRUE)
		, m_bDisplay(TRUE)
		, m_bDisable(FALSE)
		, m_bUpdateLocked(FALSE)
		, m_bClipClient(FALSE)
		, m_bFocusable(FALSE)
		, m_bDrawFocusRect(TRUE)
		, m_bCacheDraw(FALSE)
		, m_bCacheDirty(TRUE)
		, m_bLayeredWindow(FALSE)
		, m_layoutDirty(dirty_self)
		, m_uData(0)
		, m_pOwner(NULL)
		, m_pCurMsg(NULL)
		, m_pBgSkin(NULL)
		, m_pNcSkin(NULL)
		, m_pGetRTData(NULL)
		, m_bFloat(FALSE)
		, m_crColorize(0)
		, m_strText(this)
		, m_strToolTipText(this)
#ifdef _DEBUG
		, m_nMainThreadId( ::GetCurrentThreadId() ) // 初始化对象的线程不一定是主线程
#endif
	{
		m_nMaxWidth.setWrapContent();

		m_pLayout.Attach(new SouiLayout());
		m_pLayoutParam.Attach(new SouiLayoutParam());
		m_pLayoutParam->SetMatchParent(Both);

		m_evtSet.addEvent(EVENTID(EventSwndCreate));
		m_evtSet.addEvent(EVENTID(EventSwndDestroy));
		m_evtSet.addEvent(EVENTID(EventSwndSize));
		m_evtSet.addEvent(EVENTID(EventSwndMouseHover));
		m_evtSet.addEvent(EVENTID(EventSwndMouseLeave));
		m_evtSet.addEvent(EVENTID(EventSwndStateChanged));
		m_evtSet.addEvent(EVENTID(EventSwndVisibleChanged));
		m_evtSet.addEvent(EVENTID(EventSwndUpdateTooltip));
		m_evtSet.addEvent(EVENTID(EventLButtonDown));
		m_evtSet.addEvent(EVENTID(EventLButtonUp));

		m_evtSet.addEvent(EVENTID(EventCmd));
		m_evtSet.addEvent(EVENTID(EventCtxMenu));
		m_evtSet.addEvent(EVENTID(EventSetFocus));
		m_evtSet.addEvent(EVENTID(EventKillFocus));

	}

	SWindow::~SWindow()
	{
#ifdef SOUI_ENABLE_ACC
		if(m_pAcc)
		{
			SComPtr<IAccHelper> accHelper;
			if(m_pAcc->QueryInterface(__uuidof(IAccHelper),(void**)&accHelper) == S_OK)
			{
				SASSERT(accHelper->GetOwner()==NULL);
			}
		}
#endif
		SWindowMgr::DestroyWindow(m_swnd);
	}


	// Get align
	UINT SWindow::GetTextAlign()
	{
		return m_style.GetTextAlign() ;
	}


	void SWindow::GetWindowRect(LPRECT prect)
	{
		SASSERT(prect);
		memcpy(prect,&m_rcWindow,sizeof(RECT));
	}

	void SWindow::GetClientRect(LPRECT pRect) const
	{
		SASSERT(pRect);
		CRect rc = m_rcWindow;
		rc.DeflateRect(m_style.GetMargin());
		*pRect=rc;
	}

	CRect SWindow::GetClientRect() const
	{
		CRect rc;
		SWindow::GetClientRect(&rc);
		return rc;
	}

	SStringT SWindow::GetWindowText(BOOL bRawText/*=FALSE*/)
	{
		return m_strText.GetText(bRawText);
	}

	BOOL SWindow::OnUpdateToolTip(CPoint pt, SwndToolTipInfo &tipInfo)
	{
		tipInfo.swnd = m_swnd;
		tipInfo.dwCookie = 0;
		tipInfo.rcTarget = m_rcWindow;

		EventSwndUpdateTooltip evt(this);
		evt.bUpdated = FALSE;
		FireEvent(evt);

		if (evt.bUpdated)
		{
			tipInfo.strTip = evt.strToolTip;
			return !tipInfo.strTip.IsEmpty();
		}
		else
		{
			if (m_strToolTipText.GetText(FALSE).IsEmpty())
				return FALSE;
			tipInfo.strTip = m_strToolTipText.GetText(FALSE);
		}
		return TRUE;
	}

	void SWindow::SetWindowText(LPCTSTR lpszText)
	{
		m_strText.SetText(lpszText);
		accNotifyEvent(EVENT_OBJECT_NAMECHANGE);
		OnContentChanged();
	}


	void SWindow::OnContentChanged()
	{
		if(IsVisible(TRUE)) Invalidate();
		if (GetLayoutParam()->IsWrapContent(Any))
		{
			RequestRelayout();
			if(IsVisible(TRUE)) Invalidate();
		}else if(GetLayoutParam()->IsMatchParent(Any) && GetParent())
		{
			GetParent()->OnContentChanged();
		}
	}

	void SWindow::TestMainThread()
	{
#ifdef _DEBUG
		if (IsBadWritePtr(this, sizeof(SWindow)))
		{
			SASSERT_FMT(FALSE, _T("this is null!!!"));
		}
		else
		{
			// 当你看到这个东西的时候，我不幸的告诉你，你的其他线程在刷界面
			// 这是一件很危险的事情
			DWORD dwCurThreadID = GetCurrentThreadId();
			DWORD dwProcID=GetCurrentProcessId();
			SASSERT_FMT(m_nMainThreadId == dwCurThreadID, _T("ProcessID:%d,请准备好红包再到群里提问"), dwProcID);
		}
#endif
	}


	// Send a message to SWindow
	LRESULT SWindow::SSendMessage(UINT Msg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/,BOOL *pbMsgHandled/*=NULL*/)
	{
		LRESULT lResult = 0;

		if ( Msg < WM_USER
			&& Msg != WM_DESTROY
			&& Msg != WM_CLOSE
			)
		{
			TestMainThread();
		}
		AddRef();
		SWNDMSG msgCur= {Msg,wParam,lParam};
		SWNDMSG *pOldMsg=m_pCurMsg;
		m_pCurMsg=&msgCur;

		BOOL bOldMsgHandle=IsMsgHandled();//备分上一个消息的处理状态

		SetMsgHandled(FALSE);

		ProcessSwndMessage(Msg, wParam, lParam, lResult);

		if(pbMsgHandled) *pbMsgHandled=IsMsgHandled();

		SetMsgHandled(bOldMsgHandle);//恢复上一个消息的处理状态

		m_pCurMsg=pOldMsg;
		Release();

		return lResult;
	}

	void SWindow::SDispatchMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		SSendMessage(uMsg,wParam,lParam);
		SWindow *pChild = GetWindow(GSW_FIRSTCHILD);
		while(pChild)
		{
			pChild->SDispatchMessage(uMsg,wParam,lParam);
			pChild = pChild->GetWindow(GSW_NEXTSIBLING);
		}
	}

	void SWindow::Move(LPCRECT prect)
	{
		TestMainThread();

		if(prect)
		{
			m_bFloat = TRUE;//使用Move后，程序不再自动计算窗口坐标
			OnRelayout(prect);
		}else if(GetParent())
		{
			//恢复自动计算位置
			m_bFloat = FALSE;
			//重新计算自己及兄弟窗口的坐标
			RequestRelayout();
		}
	}

	void SWindow::Move(int x,int y, int cx/*=-1*/,int cy/*=-1*/)
	{
		CRect rcWnd = GetWindowRect();
		if(cx==-1) cx=rcWnd.Width();
		if(cy==-1) cy=rcWnd.Height();
		CRect rcNew(x,y,x+cx,y+cy);
		Move(&rcNew);
	}

	// Set current cursor, when hover
	BOOL SWindow::OnSetCursor(const CPoint &pt)
	{
		HCURSOR hCursor=GETRESPROVIDER->LoadCursor(m_style.m_strCursor);
		::SetCursor(hCursor);
		return TRUE;
	}

	// Get SWindow state
	DWORD SWindow::GetState(void)
	{
		return m_dwState;
	}

	// Modify SWindow state
	DWORD SWindow::ModifyState(DWORD dwStateAdd, DWORD dwStateRemove,BOOL bUpdate/*=FALSE*/)
	{
		TestMainThread();

		DWORD dwOldState = m_dwState;

		DWORD dwNewState = m_dwState;
		dwNewState &= ~dwStateRemove;
		dwNewState |= dwStateAdd;

		OnStateChanging(dwOldState,dwNewState);

		m_dwState = dwNewState;

		OnStateChanged(dwOldState,dwNewState);
		if(bUpdate && NeedRedrawWhenStateChange()) InvalidateRect(m_rcWindow);
		return dwOldState;
	}

	ULONG_PTR SWindow::GetUserData()
	{
		return m_uData;
	}

	ULONG_PTR SWindow::SetUserData(ULONG_PTR uData)
	{
		ULONG_PTR uOld=m_uData;
		m_uData=uData;
		return uOld;
	}

	BOOL SWindow::SetTimer(char id,UINT uElapse)
	{
		STimerID timerID(m_swnd,id);
		return (BOOL)::SetTimer(GetContainer()->GetHostHwnd(),DWORD(timerID),uElapse,NULL);
	}

	void SWindow::KillTimer(char id)
	{
		STimerID timerID(m_swnd,id);
		::KillTimer(GetContainer()->GetHostHwnd(),DWORD(timerID));
	}


	BOOL SWindow::SetTimer2( UINT_PTR id,UINT uElapse )
	{
		return STimer2::SetTimer(m_swnd,id,uElapse);
	}

	void SWindow::KillTimer2( UINT_PTR id )
	{
		STimer2::KillTimer(m_swnd,id);
	}

	SWND SWindow::GetSwnd() const
	{
		return m_swnd;
	}


	SWindow *SWindow::GetParent() const
	{
		return m_pParent;
	}


	SWindow * SWindow::GetTopLevelParent() const
	{
		SWindow *pParent=(SWindow*)this;
		while(pParent->GetParent()) pParent=pParent->GetParent();
		return pParent;
	}


	BOOL SWindow::IsDescendant( const SWindow *pWnd ) const
	{
		if(!pWnd) return FALSE;
		SWindow *pParent=GetParent();
		while(pParent)
		{
			if(pParent == pWnd) return TRUE;
			pParent=pParent->GetParent();
		}
		return FALSE;
	}


	BOOL SWindow::DestroyWindow()
	{
		TestMainThread();
		if(!GetParent()) 
		{
			SSendMessage(WM_DESTROY);
			Release();
		}
		else 
		{
			GetParent()->DestroyChild(this);
		}
		return TRUE;
	}

	BOOL SWindow::DestroyChild(SWindow *pChild)
	{
		TestMainThread();
		if(this != pChild->GetParent()) return FALSE;
		pChild->InvalidateRect(NULL);
		pChild->SSendMessage(WM_DESTROY);
		RemoveChild(pChild);
		pChild->Release();
		return TRUE;
	}

	UINT SWindow::GetChildrenCount() const
	{
		return m_nChildrenCount;
	}

	void SWindow::InsertChild(SWindow *pNewChild,SWindow *pInsertAfter/*=ICWND_LAST*/)
	{
		TestMainThread();
		if(pNewChild->m_pParent == this) 
			return;

		pNewChild->SetContainer(GetContainer());
		pNewChild->m_pParent=this;
		pNewChild->m_pPrevSibling=pNewChild->m_pNextSibling=NULL;

		if(pInsertAfter==m_pLastChild) pInsertAfter=ICWND_LAST;

		if(pInsertAfter==ICWND_LAST)
		{
			//insert window at head
			pNewChild->m_pPrevSibling=m_pLastChild;
			if(m_pLastChild) m_pLastChild->m_pNextSibling=pNewChild;
			else m_pFirstChild=pNewChild;
			m_pLastChild=pNewChild;
		}
		else if(pInsertAfter==ICWND_FIRST)
		{
			//insert window at tail
			pNewChild->m_pNextSibling=m_pFirstChild;
			if(m_pFirstChild) m_pFirstChild->m_pPrevSibling=pNewChild;
			else m_pLastChild=pNewChild;
			m_pFirstChild=pNewChild;
		}
		else
		{
			//insert window at middle
			SASSERT(pInsertAfter->m_pParent == this);
			SASSERT(m_pFirstChild && m_pLastChild);
			SWindow *pNext=pInsertAfter->m_pNextSibling;
			SASSERT(pNext);
			pInsertAfter->m_pNextSibling=pNewChild;
			pNewChild->m_pPrevSibling=pInsertAfter;
			pNewChild->m_pNextSibling=pNext;
			pNext->m_pPrevSibling=pNewChild;
		}
		m_nChildrenCount++;

		m_layoutDirty = dirty_self;

		if(!GetLayout()->IsParamAcceptable(pNewChild->GetLayoutParam()))
		{//检查子窗口原有的布局属性是不是和当前窗口的布局类型是否匹配
			ILayoutParam *pLayoutParam = GetLayout()->CreateLayoutParam();
			pNewChild->SetLayoutParam(pLayoutParam);
			pLayoutParam->Release();
		}

		//继承父窗口的disable状态
		pNewChild->OnEnable(!IsDisabled(TRUE),ParentEnable);

		//只在插入新控件时需要标记zorder失效,删除控件不需要标记
		GetContainer()->MarkWndTreeZorderDirty();
	}

	BOOL SWindow::RemoveChild(SWindow *pChild)
	{
		TestMainThread();
		if(this != pChild->GetParent()) 
			return FALSE;

		SWindow *pPrevSib=pChild->m_pPrevSibling;
		SWindow *pNextSib=pChild->m_pNextSibling;

		if(pPrevSib) 
			pPrevSib->m_pNextSibling=pNextSib;
		else 
			m_pFirstChild=pNextSib;

		if(pNextSib) 
			pNextSib->m_pPrevSibling=pPrevSib;
		else 
			m_pLastChild=pPrevSib;

		pChild->m_pParent=NULL;
		pChild->m_pNextSibling = NULL;
		pChild->m_pPrevSibling = NULL;
		m_nChildrenCount--;

		return TRUE;
	}

	BOOL SWindow::IsChecked()
	{
		return WndState_Check == (m_dwState & WndState_Check);
	}

	BOOL SWindow::IsDisabled(BOOL bCheckParent /*= FALSE*/)
	{
		if(bCheckParent) return m_dwState & WndState_Disable;
		else return m_bDisable;
	}

	BOOL SWindow::IsVisible(BOOL bCheckParent /*= FALSE*/)
	{
		if(bCheckParent) return (0 == (m_dwState & WndState_Invisible));
		else return m_bVisible;
	}

	//因为NotifyInvalidateRect只有窗口可见时再通知刷新，这里在窗口可见状态改变前后都执行一次通知。
	void SWindow::SetVisible(BOOL bVisible,BOOL bUpdate/*=FALSE*/)
	{
		if(bUpdate) InvalidateRect(m_rcWindow);
		SSendMessage(WM_SHOWWINDOW,bVisible);
		if(bUpdate) InvalidateRect(m_rcWindow);
	}

	void SWindow::EnableWindow( BOOL bEnable,BOOL bUpdate)
	{
		SSendMessage(WM_ENABLE,bEnable);
		if(bUpdate) InvalidateRect(m_rcWindow);
	}

	void SWindow::SetCheck(BOOL bCheck)
	{
		if (bCheck)
			ModifyState(WndState_Check, 0,TRUE);
		else
			ModifyState(0, WndState_Check,TRUE);
	}

	ISwndContainer *SWindow::GetContainer()
	{
		return m_pContainer;
	}

	const ISwndContainer* SWindow::GetContainer() const
	{
		return m_pContainer;
	}

	void SWindow::SetContainer(ISwndContainer *pContainer)
	{
		TestMainThread();
		m_pContainer=pContainer;

		SWindow *pChild=GetWindow(GSW_FIRSTCHILD);
		while(pChild)
		{
			pChild->SetContainer(m_pContainer);
			pChild=pChild->GetWindow(GSW_NEXTSIBLING);
		}
	}

	void SWindow::SetOwner(SWindow *pOwner)
	{
		m_pOwner=pOwner;
	}

	SWindow *SWindow::GetOwner()
	{
		return m_pOwner;
	}

	BOOL SWindow::IsMsgTransparent()
	{
		return m_bMsgTransparent;
	}

	// add by dummyz@126.com
	SwndStyle& SWindow::GetStyle()
	{
		return m_style;
	}

	//改用广度优先算法搜索控件,便于逐级查找 2014年12月8日
	SWindow* SWindow::FindChildByID(int id, int nDeep/* =-1*/)
	{
		if(id == 0 || nDeep ==0) return NULL;


		SWindow *pChild = GetWindow(GSW_FIRSTCHILD);
		while(pChild)
		{
			if (pChild->GetID() == id)
				return pChild;
			pChild = pChild->GetWindow(GSW_NEXTSIBLING);
		}

		if(nDeep>0) nDeep--;
		if(nDeep==0) return NULL;

		pChild = GetWindow(GSW_FIRSTCHILD);
		while(pChild)
		{
			SWindow *pChildFind=pChild->FindChildByID(id,nDeep);
			if(pChildFind) return pChildFind;
			pChild = pChild->GetWindow(GSW_NEXTSIBLING);
		}

		return NULL;
	}

	SWindow* SWindow::FindChildByName( LPCWSTR pszName , int nDeep)
	{
		if(!pszName || nDeep ==0) return NULL;

		SWindow *pChild = GetWindow(GSW_FIRSTCHILD);
		while(pChild)
		{
			if (pChild->m_strName == pszName)
				return pChild;
			pChild = pChild->GetWindow(GSW_NEXTSIBLING);
		}

		if(nDeep>0) nDeep--;
		if(nDeep==0) return NULL;

		pChild = GetWindow(GSW_FIRSTCHILD);
		while(pChild)
		{
			SWindow *pChildFind=pChild->FindChildByName(pszName,nDeep);
			if(pChildFind) return pChildFind;
			pChild = pChild->GetWindow(GSW_NEXTSIBLING);
		}

		return NULL;
	}

	const static wchar_t KLabelInclude[] = L"include";	//文件包含的标签
	const static wchar_t KLabelTemplate[] = L"template";//模板标签
	const static wchar_t KTempNamespace[] = L"t:";//模板识别ＮＳ
	const static wchar_t KTempData[] = L"data";//模板参数
	const static wchar_t KTempParamFmt[] = L"{{%s}}";//模板数据替换格式

	BOOL SWindow::CreateChildren(pugi::xml_node xmlNode)
	{
		TestMainThread();
		for (pugi::xml_node xmlChild=xmlNode.first_child(); xmlChild; xmlChild=xmlChild.next_sibling())
		{
			if(xmlChild.type() != pugi::node_element) continue;

			if(_wcsicmp(xmlChild.name(),KLabelInclude)==0)
			{//在窗口布局中支持include标签
				SStringT strSrc = S_CW2T(xmlChild.attribute(L"src").value());
				pugi::xml_document xmlDoc;
				SStringTList strLst;

				if(2 == ParseResID(strSrc,strLst))
				{
					LOADXML(xmlDoc,strLst[1],strLst[0]);
				}else
				{
					LOADXML(xmlDoc,strLst[0],RT_LAYOUT);
				}
				if(xmlDoc)
				{
					CreateChildren(xmlDoc.child(KLabelInclude));
				}else
				{
					SASSERT(FALSE);
				}
			}
			else if(!xmlChild.get_userdata())//通过userdata来标记一个节点是否可以忽略
			{
				SStringW strName = xmlChild.name();
				if (strName.StartsWith(KTempNamespace))
				{
					strName = strName.Right(strName.GetLength() - 2);
					SStringW strXml = GETTEMPLATEPOOLMR->GetTemplateString(strName);
					SASSERT(!strXml.IsEmpty());
					if (!strXml.IsEmpty())
					{//create children by template.
						pugi::xml_node xmlData = xmlChild.child(KTempData);
						for (pugi::xml_attribute param = xmlData.first_attribute(); param; param = param.next_attribute())
						{
							SStringW strParam = SStringW().Format(KTempParamFmt, param.name());
							strXml.Replace(strParam, param.value());//replace params to value.
						}
						pugi::xml_document xmlDoc;
						if (xmlDoc.load_buffer_inplace(strXml.GetBuffer(strXml.GetLength()), strXml.GetLength() * sizeof(WCHAR), 116, pugi::encoding_utf16))
						{
							pugi::xml_node xmlTemp = xmlDoc.first_child();
							SASSERT(xmlTemp);
							//merger properties.
							for (pugi::xml_attribute attr = xmlChild.first_attribute(); attr; attr = attr.next_attribute())
							{
								if (!xmlTemp.attribute(attr.name()))
								{
									xmlTemp.append_attribute(attr.name()).set_value(attr.value());
								}else
								{
									xmlTemp.attribute(attr.name()).set_value(attr.value());
								}
							}
							//create child.
							SWindow *pChild = SApplication::getSingleton().CreateWindowByName(xmlTemp.name());
							if (pChild)
							{
								InsertChild(pChild);
								pChild->InitFromXml(xmlTemp);
							}
						}
						strXml.ReleaseBuffer();
					}
				}
				else
				{
					SWindow *pChild = SApplication::getSingleton().CreateWindowByName(xmlChild.name());
					if (pChild)
					{
						InsertChild(pChild);
						pChild->InitFromXml(xmlChild);
					}
				}
			}
		}
		return TRUE;
	}


	SStringW SWindow::tr( const SStringW &strSrc )
	{
		return TR(strSrc,GetTrCtx());
	}

	// Create SWindow from xml element
	BOOL SWindow::InitFromXml(pugi::xml_node xmlNode)
	{
		TestMainThread();
		SASSERT(m_pContainer);
		if (xmlNode)
		{

			if(m_pLayoutParam) m_pLayoutParam->Clear();

			//优先处理"layout"属性
			pugi::xml_attribute attrLayout=xmlNode.attribute(L"layout");
			if(attrLayout)
			{
				IObject::MarkAttributeHandled(attrLayout,true);
				SetAttribute(attrLayout.name(), attrLayout.value(), TRUE);
			}

			//优先处理"class"属性
			pugi::xml_attribute attrClass=xmlNode.attribute(L"class");
			if(attrClass)
			{
				IObject::MarkAttributeHandled(attrClass,true);
				SetAttribute(attrClass.name(), attrClass.value(), TRUE);
			}

			SObject::InitFromXml(xmlNode);

			IObject::MarkAttributeHandled(attrClass,false);
			IObject::MarkAttributeHandled(attrLayout,false);

			SStringW strText = xmlNode.text().get();
			strText.TrimBlank();
			if (!strText.IsEmpty())
			{
				m_strText.SetText(S_CW2T(GETSTRING(strText)));   //使用语言包翻译。
			}else if(m_strText.GetText(TRUE).IsEmpty())
			{//try to apply cdata as text
				SStringW strCData = xmlNode.child_value();
				strCData.TrimBlank();
				if(!strCData.IsEmpty())
				{
					m_strText.SetText(S_CW2T(GETSTRING(strCData)));
				}
			}
		}

		//发送WM_CREATE消息
		if(0!=SSendMessage(WM_CREATE))
		{
			if(m_pParent)    m_pParent->DestroyChild(this);
			return FALSE;
		}

		//给this发一个WM_SHOWWINDOW消息，一些控件需要在WM_SHOWWINDOW中处理状态
		//初始化的WM_SHOWWINDOW只影响this,子窗口的SHOW由子窗口发出。
		//不改变窗口的m_bVisible状态，需要使用ParentShow标志
		if(m_pParent)
		{//从父窗口更新状态
			if(!m_pParent->IsVisible(TRUE))
				m_dwState |= WndState_Invisible;
			if(m_pParent->IsDisabled(TRUE))
				m_dwState |= WndState_Disable;
		}
		SSendMessage(WM_SHOWWINDOW,IsVisible(TRUE),ParentShow);

		//创建子窗口
		CreateChildren(xmlNode);

		//请求根窗口重新布局。由于布局涉及到父子窗口同步进行，同步执行布局操作可能导致布局过程重复执行。
		RequestRelayout();

		return TRUE;
	}

	SWindow * SWindow::CreateChildren(LPCWSTR pszXml)
	{
		pugi::xml_document xmlDoc;
		if(!xmlDoc.load_buffer(pszXml,wcslen(pszXml)*sizeof(wchar_t),pugi::parse_default,pugi::encoding_utf16)) return NULL;
		BOOL bLoaded=CreateChildren(xmlDoc);
		if(!bLoaded) return NULL;
		else return m_pLastChild;
	}

	// Hittest children
	SWND SWindow::SwndFromPoint(CPoint ptHitTest, BOOL bOnlyText)
	{
		if(!IsContainPoint(ptHitTest,FALSE))
			return NULL;

		if(!IsContainPoint(ptHitTest,TRUE))
			return m_swnd;//只在鼠标位于客户区时，才继续搜索子窗口

		SWND swndChild = NULL;

		SWindow *pChild=GetWindow(GSW_LASTCHILD);
		while(pChild)
		{
			if (pChild->IsVisible(TRUE) && !pChild->IsMsgTransparent())
			{
				swndChild = pChild->SwndFromPoint(ptHitTest, bOnlyText);

				if (swndChild) return swndChild;
			}

			pChild=pChild->GetWindow(GSW_PREVSIBLING);
		}

		return m_swnd;
	}

	BOOL SWindow::NeedRedrawWhenStateChange()
	{
		if (m_pBgSkin && !m_pBgSkin->IgnoreState())
		{
			return TRUE;
		}
		return m_style.GetStates()>1;
	}

	//如果当前窗口有绘制缓存，它可能是由cache属性定义的，也可能是由于定义了alpha
	void SWindow::_PaintClient(IRenderTarget *pRT)
	{
		if(IsDrawToCache())
		{
			IRenderTarget *pRTCache=m_cachedRT;
			if(pRTCache)
			{//在窗口正在创建的时候进来pRTCache可能为NULL
				CRect rcWnd=m_rcWindow;
				if(IsCacheDirty())
				{
					pRTCache->ClearRect(&rcWnd,0);

					CAutoRefPtr<IFont> oldFont;
					COLORREF crOld=pRT->GetTextColor();
					pRTCache->SelectObject(pRT->GetCurrentObject(OT_FONT),(IRenderObj**)&oldFont);
					pRTCache->SetTextColor(crOld);

					SSendMessage(WM_ERASEBKGND, (WPARAM)pRTCache);
					SSendMessage(WM_PAINT, (WPARAM)pRTCache);

					pRTCache->SelectObject(oldFont);
					pRTCache->SetTextColor(crOld);

					MarkCacheDirty(false);
				}
				CRect rcClip , rcInter;
				pRT->GetClipBox(&rcClip);
				rcInter.IntersectRect(rcClip,rcWnd);
				if(!rcInter.IsRectEmpty())
				{
					pRT->AlphaBlend(&rcInter,pRTCache,&rcInter,IsLayeredWindow()?0xFF:m_style.m_byAlpha);
				}
			}
		}else
		{
			SSendMessage(WM_ERASEBKGND, (WPARAM)pRT);
			SSendMessage(WM_PAINT, (WPARAM)pRT);
		}
	}

	void SWindow::_PaintNonClient( IRenderTarget *pRT )
	{
		CRect rcWnd = GetWindowRect();
		CRect rcClient = GetClientRect();
		if(rcWnd==rcClient) return;

		if(IsDrawToCache())
		{
			IRenderTarget *pRTCache=m_cachedRT;
			if(pRTCache)
			{
				SSendMessage(WM_NCPAINT, (WPARAM)pRTCache);
				int nSave =-1;
				pRT->SaveClip(&nSave);
				pRT->PushClipRect(&rcClient,RGN_DIFF);
				if(m_rgnWnd)
				{
					m_rgnWnd->Offset(rcWnd.TopLeft());
					pRT->PushClipRegion(m_rgnWnd);
					m_rgnWnd->Offset(-rcWnd.TopLeft());
				}
				pRT->AlphaBlend(&rcWnd,pRTCache,&rcWnd,IsLayeredWindow()?0xFF:m_style.m_byAlpha);
				pRT->RestoreClip(nSave);
			}
		}else
		{
			if(m_rgnWnd)
			{
				m_rgnWnd->Offset(rcWnd.TopLeft());
				pRT->PushClipRegion(m_rgnWnd);
				m_rgnWnd->Offset(-rcWnd.TopLeft());
			}
			SSendMessage(WM_NCPAINT, (WPARAM)pRT);
			if(m_rgnWnd)
			{
				pRT->PopClip();
			}
		}
	}

	void SWindow::_RedrawNonClient()
	{
		CAutoRefPtr<IRegion> rgn;
		GETRENDERFACTORY->CreateRegion(&rgn);
		CRect rcWnd = GetWindowRect();
		CRect rcClient = SWindow::GetClientRect();
		rgn->CombineRect(&rcWnd,RGN_COPY);
		rgn->CombineRect(&rcClient,RGN_DIFF);
		if(m_rgnWnd)
		{
			m_rgnWnd->Offset(m_rcWindow.TopLeft());
			rgn->CombineRgn(m_rgnWnd,RGN_AND);
			m_rgnWnd->Offset(-m_rcWindow.TopLeft());
		}
		IRenderTarget *pRT=GetRenderTarget(OLEDC_OFFSCREEN,rgn);//不自动画背景

		PaintBackground(pRT,&rcWnd);
		SSendMessage(WM_NCPAINT, (WPARAM)pRT);
		PaintForeground(pRT,&rcWnd);

		ReleaseRenderTarget(pRT);
	}

	//paint zorder in [iZorderBegin,iZorderEnd) widnows
	void SWindow::_PaintRegion2( IRenderTarget *pRT, IRegion *pRgn,UINT iZorderBegin,UINT iZorderEnd )
	{
		if(!IsVisible(TRUE))  //只在自己完全可见的情况下才绘制
			return;

		CRect rcWnd,rcClient;
		GetWindowRect(&rcWnd);
		GetClientRect(&rcClient);

		CRect rcRgn = rcWnd;
		if(pRgn && !pRgn->IsEmpty())
		{
			pRgn->GetRgnBox(&rcRgn);
		}
		CRect rcRgnUnionClient;
		rcRgnUnionClient.UnionRect(rcClient,rcRgn);
		BOOL bRgnInClient = rcRgnUnionClient == rcClient;

		IRenderTarget *pRTBack = NULL;//backup current RT

		if(IsLayeredWindow() && pRT != GetLayerRenderTarget())
		{//获得当前LayeredWindow RT来绘制内容
			pRTBack = pRT;
			pRT = GetLayerRenderTarget();

			//绘制到窗口的缓存上,需要继承原RT的绘图属性
			CAutoRefPtr<IFont> curFont;
			HRESULT hr = pRTBack->SelectDefaultObject(OT_FONT,(IRenderObj**)&curFont);
			COLORREF crTxt = pRTBack->GetTextColor();
			if(S_OK == hr) pRT->SelectObject(curFont);
			pRT->SetTextColor(crTxt);

			if(pRgn && !pRgn->IsEmpty()) pRT->PushClipRegion(pRgn,RGN_COPY);
			pRT->ClearRect(&rcWnd,0);
		}

		if(m_rgnWnd)
		{
			m_rgnWnd->Offset(m_rcWindow.TopLeft());
			pRT->PushClipRegion(m_rgnWnd);
			m_rgnWnd->Offset(-m_rcWindow.TopLeft());
		}
		if(IsClipClient())
		{
			pRT->PushClipRect(rcClient);
		}
		if(m_uZorder >= iZorderBegin
			&& m_uZorder < iZorderEnd 
			&& (!pRgn || pRgn->IsEmpty() || pRgn->RectInRegion(&rcClient)))
		{//paint client
			_PaintClient(pRT);
		}

		SPainter painter;
		BeforePaint(pRT,painter);

		SWindow *pChild = GetWindow(GSW_FIRSTCHILD);
		while(pChild)
		{
			if(pChild->m_uZorder >= iZorderEnd) break;
			if(pChild->m_uZorder< iZorderBegin)
			{//看整个分枝的zorder是不是在绘制范围内
				SWindow *pNextChild = pChild->GetWindow(GSW_NEXTSIBLING);
				if(pNextChild)
				{
					if(pNextChild->m_uZorder<=iZorderBegin)
					{
						pChild = pNextChild;
						continue;
					}
				}else
				{//最后一个节点时查看最后子窗口的zorder
					SWindow *pLastChild = pChild;
					while(pLastChild->GetChildrenCount())
					{
						pLastChild = pLastChild->GetWindow(GSW_LASTCHILD);
					}
					if(pLastChild->m_uZorder < iZorderBegin)
					{
						break;
					}
				}
			}
			pChild->_PaintRegion2(pRT,pRgn,iZorderBegin,iZorderEnd);
			pChild = pChild->GetWindow(GSW_NEXTSIBLING);
		}
		AfterPaint(pRT,painter);

		if(IsClipClient())
		{
			pRT->PopClip();
		}
		if(m_rgnWnd)
		{
			pRT->PopClip();
		}
		if(m_uZorder >= iZorderBegin
			&& m_uZorder < iZorderEnd 
			&& !bRgnInClient
			&& (!pRgn ||  pRgn->IsEmpty() ||pRgn->RectInRegion(&rcWnd)) 
			)
		{//paint nonclient
			_PaintNonClient(pRT);
		}

		if(pRTBack)
		{//将绘制到窗口的缓存上的图像返回到上一级RT
			if(pRgn  && !pRgn->IsEmpty()) pRT->PopClip();
			pRTBack->AlphaBlend(&m_rcWindow,pRT,&m_rcWindow,m_style.m_byAlpha);

			CAutoRefPtr<IFont> curFont;
			HRESULT hr = pRT->SelectDefaultObject(OT_FONT,(IRenderObj**)&curFont);

			pRT = pRTBack;
			if(S_OK == hr) pRT->SelectObject(curFont);
		}

	}

	//当前函数中的参数包含zorder,为了保证传递进来的zorder是正确的,必须在外面调用zorder重建.
	void SWindow::_PaintRegion(IRenderTarget *pRT, IRegion *pRgn,UINT iZorderBegin,UINT iZorderEnd)
	{
		TestMainThread();
		BeforePaintEx(pRT);
		_PaintRegion2(pRT,pRgn,iZorderBegin,iZorderEnd);
	}

	void SWindow::RedrawRegion(IRenderTarget *pRT, IRegion *pRgn)
	{
		TestMainThread();
		_PaintRegion2(pRT, pRgn, (UINT)ZORDER_MIN, (UINT)ZORDER_MAX);
	}


	void SWindow::UpdateWindow()
	{
		GetContainer()->UpdateWindow();
	}

	void SWindow::Invalidate()
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		InvalidateRect(rcClient);
	}

	void SWindow::InvalidateRect(LPCRECT lprect)
	{
		if (lprect)
		{
			CRect rect = *lprect;
			InvalidateRect(rect);
		}else
		{
			InvalidateRect(m_rcWindow);
		}
	}

	void SWindow::InvalidateRect(const CRect& rect,BOOL bFromThis/*=TRUE*/)
	{
		TestMainThread();
		if(bFromThis) MarkCacheDirty(true);
		if(!IsVisible(TRUE) || IsUpdateLocked()) return ;
		//只能更新窗口有效区域
		CRect rcIntersect = rect & m_rcWindow;
		if(rcIntersect.IsRectEmpty()) return;

		if(!m_style.m_bBkgndBlend)
		{//非背景混合窗口，直接发消息支宿主窗口来启动刷新
			if(!m_invalidRegion)
			{
				GETRENDERFACTORY->CreateRegion(&m_invalidRegion);
			}
			m_invalidRegion->CombineRect(rcIntersect,RGN_OR);
			::SendMessage(GetContainer()->GetHostHwnd(),UM_UPDATESWND,(WPARAM)m_swnd,0);//请求刷新窗口
		}else
		{
			if(GetParent())
			{
				GetParent()->InvalidateRect(rcIntersect,FALSE);
			}else
			{
				GetContainer()->OnRedraw(rcIntersect);
			}
		}
	}

	void SWindow::LockUpdate()
	{
		m_bUpdateLocked=TRUE;
	}

	void SWindow::UnlockUpdate()
	{
		m_bUpdateLocked=FALSE;
	}

	BOOL SWindow::IsUpdateLocked()
	{
		return m_bUpdateLocked;
	}

	void SWindow::BringWindowToTop()
	{
		TestMainThread();
		SWindow *pParent=GetParent();
		if(!pParent) return;
		pParent->RemoveChild(this);
		pParent->InsertChild(this);
	}

	BOOL SWindow::FireEvent(EventArgs &evt)
	{
		TestMainThread();
		if(m_evtSet.isMuted()) return FALSE;

		//调用事件订阅的处理方法
		m_evtSet.FireEvent(evt);
		if(!evt.bubbleUp) return evt.handled>0;

		//调用脚本事件处理方法
		if(GetScriptModule())
		{
			SStringW strEvtName = evt.GetName();
			if(!strEvtName.IsEmpty())
			{
				SStringA strScriptHandler = m_evtSet.getEventScriptHandler(strEvtName);
				if(!strScriptHandler.IsEmpty())
				{
					GetScriptModule()->executeScriptedEventHandler(strScriptHandler,&evt);
					if(!evt.bubbleUp) return evt.handled>0;
				}
			}
		}
		if(GetOwner()) return GetOwner()->FireEvent(evt);
		return GetContainer()->OnFireEvent(evt);
	}

	BOOL SWindow::OnRelayout(const CRect &rcWnd)
	{
		if (rcWnd.EqualRect(m_rcWindow) && m_layoutDirty == dirty_clean)
			return FALSE;
		if(!rcWnd.EqualRect(m_rcWindow))
		{
			m_layoutDirty = dirty_self;

			InvalidateRect(m_rcWindow);
			m_rcWindow = rcWnd;

			if(m_rcWindow.left>m_rcWindow.right) 
				m_rcWindow.right = m_rcWindow.left;
			if(m_rcWindow.top>m_rcWindow.bottom) 
				m_rcWindow.bottom = m_rcWindow.top;

			InvalidateRect(m_rcWindow);

			SSendMessage(WM_NCCALCSIZE);//计算非客户区大小
		}


		UpdateChildrenPosition();   //更新子窗口位置

		CRect rcClient;
		GetClientRect(&rcClient);
		SSendMessage(WM_SIZE,0,MAKELPARAM(rcClient.Width(),rcClient.Height()));
		m_layoutDirty = dirty_clean;
		return TRUE;
	}

	int SWindow::OnCreate( LPVOID )
	{
		if(m_style.m_bTrackMouseEvent)
			GetContainer()->RegisterTrackMouseEvent(m_swnd);
		else
			GetContainer()->UnregisterTrackMouseEvent(m_swnd);

		m_style.SetScale(GetScale());

		EventSwndCreate evt(this);
		FireEvent(evt);
		accNotifyEvent(EVENT_OBJECT_CREATE);
		return 0;
	}

	void SWindow::OnDestroy()
	{
		EventSwndDestroy evt(this);
		FireEvent(evt);
		accNotifyEvent(EVENT_OBJECT_DESTROY);

#ifdef SOUI_ENABLE_ACC
		if(m_pAcc)
		{
			SComPtr<IAccHelper> accHelper;
			if(m_pAcc->QueryInterface(__uuidof(IAccHelper),(void**)&accHelper) == S_OK)
			{
				accHelper->SetOwner(NULL);
			}
		}
#endif
		//destroy children windows
		SWindow *pChild=m_pFirstChild;
		while (pChild)
		{
			SWindow *pNextChild=pChild->m_pNextSibling;
			pChild->SSendMessage(WM_DESTROY);
			pChild->Release();

			pChild=pNextChild;
		}
		m_pFirstChild=m_pLastChild=NULL;
		m_nChildrenCount=0;
	}

	// Draw background default
	BOOL SWindow::OnEraseBkgnd(IRenderTarget *pRT)
	{
		CRect rcClient=GetClientRect();
		if (!m_pBgSkin)
		{
			COLORREF crBg = m_style.m_crBg;

			if (CR_INVALID != crBg)
			{
				pRT->FillSolidRect(&rcClient,crBg);
			}
		}
		else
		{
			int nState=0;

			if(GetState()&WndState_Disable)
			{
				nState=3;
			}
			else if(GetState()&WndState_Check || GetState()&WndState_PushDown)
			{
				nState=2;
			}else if(GetState()&WndState_Hover)
			{
				nState=1;
			}
			if(nState>=m_pBgSkin->GetStates()) nState=0;
			m_pBgSkin->Draw(pRT, rcClient, nState); 
		}
		return TRUE;
	}

	void SWindow::BeforePaint(IRenderTarget *pRT, SPainter &painter)
	{
		IFontPtr pFont = m_style.GetTextFont(IIF_STATE4(m_dwState,0,1,2,3));
		if(pFont) 
			pRT->SelectObject(pFont,(IRenderObj**)&painter.oldFont);

		COLORREF crTxt = m_style.GetTextColor(IIF_STATE4(m_dwState,0,1,2,3));
		if(crTxt != CR_INVALID)
			painter.oldTextColor = pRT->SetTextColor(crTxt);
	}

	void SWindow::BeforePaintEx(IRenderTarget *pRT)
	{
		SWindow *pParent=GetParent();
		if(pParent) pParent->BeforePaintEx(pRT);
		SPainter painter;
		BeforePaint(pRT,painter);
	}

	void SWindow::AfterPaint(IRenderTarget *pRT, SPainter &painter)
	{
		if(painter.oldFont) pRT->SelectObject(painter.oldFont);
		if(painter.oldTextColor!=CR_INVALID) pRT->SetTextColor(painter.oldTextColor);
	}

	// Draw inner text default and focus rect
	void SWindow::OnPaint(IRenderTarget *pRT)
	{
		SPainter painter;

		BeforePaint(pRT, painter);

		CRect rcText;
		GetTextRect(rcText);
		SStringT strText = GetWindowText(FALSE);
		DrawText(pRT, strText, strText.GetLength(), rcText, GetTextAlign());

		//draw focus rect
		if(IsFocused())
		{
			DrawFocus(pRT);
		}

		AfterPaint(pRT, painter);
	}

	void SWindow::OnNcPaint(IRenderTarget *pRT)
	{
		if(!IsVisible(TRUE)) return;
		if(!m_style.GetMargin().IsRectNull())
		{
			SASSERT(pRT);
			CRect rcClient = SWindow::GetClientRect();

			pRT->PushClipRect(&rcClient,RGN_DIFF);

			int nState=0;
			if(WndState_Hover & m_dwState) nState=1;
			if(m_pNcSkin)
			{
				if(nState>=m_pNcSkin->GetStates()) nState=0;
				m_pNcSkin->Draw(pRT,m_rcWindow,nState);
			}
			else
			{
				COLORREF crBg = m_style.m_crBorder;
				if (CR_INVALID != crBg)
				{
					pRT->FillSolidRect(&m_rcWindow,crBg);
				}
			}
			pRT->PopClip();
		}
	}

	const int KWnd_MaxSize  = 0x7fffff;

	CSize SWindow::GetDesiredSize(LPCRECT pRcContainer)
	{
		CRect rcContainer;
		if(pRcContainer)
		{
			rcContainer = *pRcContainer;
			rcContainer.MoveToXY(0,0);
		}else
		{
			rcContainer.SetRect(0,0,KWnd_MaxSize,KWnd_MaxSize);
		}

		CSize szRet(KWnd_MaxSize,KWnd_MaxSize);
		if(GetLayoutParam()->IsSpecifiedSize(Horz))
		{//检查设置大小
			szRet.cx = GetLayoutParam()->GetSpecifiedSize(Horz).toPixelSize(GetScale());
		}else if(GetLayoutParam()->IsMatchParent(Horz))
		{
			szRet.cx = rcContainer.Width();
		}

		if(GetLayoutParam()->IsSpecifiedSize(Vert))
		{//检查设置大小
			szRet.cy = GetLayoutParam()->GetSpecifiedSize(Vert).toPixelSize(GetScale());
		}else if(GetLayoutParam()->IsMatchParent(Vert))
		{
			szRet.cy = rcContainer.Height();
		}

		if(szRet.cx !=KWnd_MaxSize && szRet.cy != KWnd_MaxSize) 
			return szRet;

		int nTestDrawMode = GetTextAlign() & ~(DT_CENTER | DT_RIGHT | DT_VCENTER | DT_BOTTOM);

		CRect rcPadding = GetStyle().GetPadding();
		//计算文本大小
		CRect rcTest4Text (0,0,szRet.cx,szRet.cy);
		int nMaxWid = GetLayoutParam()->IsWrapContent(Horz)?m_nMaxWidth.toPixelSize(GetScale()):szRet.cx;
		if(nMaxWid == SIZE_WRAP_CONTENT) 
		{
			nMaxWid = KWnd_MaxSize;
		}
		else //if(nMaxWid >= SIZE_SPEC)
		{
			nMaxWid -= rcPadding.left + rcPadding.right;
			nTestDrawMode|=DT_WORDBREAK;
		}
		rcTest4Text.right = smax(nMaxWid,10);

		CAutoRefPtr<IRenderTarget> pRT;
		GETRENDERFACTORY->CreateRenderTarget(&pRT,0,0);
		BeforePaintEx(pRT);

		SStringT strText = GetWindowText(FALSE);
		DrawText(pRT, strText, strText.GetLength(), rcTest4Text, nTestDrawMode | DT_CALCRECT);

		CRect rcMargin = m_style.GetMargin();
		//计算子窗口大小
		if(rcContainer.Width()>0)
		{
			rcContainer.left += rcMargin.left + rcPadding.left;
			rcContainer.right-=rcMargin.right + rcPadding.right;
			if(rcContainer.Width()<0) rcContainer.right=rcContainer.left;
		}
		if(rcContainer.Height()>0)
		{
			rcContainer.top += rcMargin.top + rcPadding.top;
			rcContainer.bottom -= rcMargin.bottom + rcPadding.bottom;
			if(rcContainer.Height()<0) rcContainer.bottom = rcContainer.top;
		}
		CSize szChilds = GetLayout()->MeasureChildren(this,rcContainer.Width(),rcContainer.Height());

		CRect rcTest(0,0, smax(szChilds.cx,rcTest4Text.right),smax(szChilds.cy,rcTest4Text.bottom));

		rcTest.InflateRect(m_style.GetMargin());
		rcTest.InflateRect(rcPadding);

		if(GetLayoutParam()->IsWrapContent(Horz)) 
			szRet.cx = rcTest.Width();
		if(GetLayoutParam()->IsWrapContent(Vert)) 
			szRet.cy = rcTest.Height();

		return szRet;
	}


	CSize SWindow::GetDesiredSize(int nParentWid , int nParentHei )
	{
		bool isParentHorzWrapContent = nParentWid<0;
		bool isParentVertWrapContent = nParentHei<0;

		ILayoutParam * pLayoutParam = GetLayoutParam();
		bool bSaveHorz = isParentHorzWrapContent && pLayoutParam->IsMatchParent(Horz);
		bool bSaveVert = isParentVertWrapContent && pLayoutParam->IsMatchParent(Vert);
		if(bSaveHorz)
			pLayoutParam->SetWrapContent(Horz);

		if(bSaveVert)
			pLayoutParam->SetWrapContent(Vert);

		CRect rcContainer(0,0,nParentWid,nParentHei);
		CSize szRet = GetDesiredSize(rcContainer);

		if(bSaveHorz) pLayoutParam->SetMatchParent(Horz);
		if(bSaveVert) pLayoutParam->SetMatchParent(Vert);

		return szRet;
	}

	void SWindow::GetTextRect( LPRECT pRect )
	{
		CRect rcClient = GetClientRect();
		rcClient.DeflateRect(GetStyle().GetPadding());
		*pRect = rcClient;
	}

	void SWindow::DrawText(IRenderTarget *pRT,LPCTSTR pszBuf,int cchText,LPRECT pRect,UINT uFormat)
	{
		pRT->DrawText(pszBuf,cchText,pRect,uFormat);
	}

	void SWindow::DrawFocus(IRenderTarget *pRT)
	{
		CRect rcFocus;
		GetTextRect(&rcFocus);
		if(IsFocusable() && m_bDrawFocusRect)    DrawDefFocusRect(pRT,rcFocus);
	}


	void SWindow::DrawDefFocusRect(IRenderTarget *pRT,CRect rcFocus )
	{
		rcFocus.DeflateRect(2,2);
		CAutoRefPtr<IPen> pPen,oldPen;
		pRT->CreatePen(PS_DOT,RGBA(88,88,88,0xFF),1,&pPen);
		pRT->SelectObject(pPen,(IRenderObj**)&oldPen);
		pRT->DrawRectangle(&rcFocus);    
		pRT->SelectObject(oldPen);
	}

	UINT SWindow::OnGetDlgCode()
	{
		return 0;
	}

	BOOL SWindow::IsFocusable()
	{
		return m_bFocusable;
	}

	void SWindow::OnShowWindow(BOOL bShow, UINT nStatus)
	{
		if(nStatus == ParentShow)
		{
			if(bShow && !IsVisible(FALSE)) bShow=FALSE;
		}
		else
		{
			m_bVisible=bShow;
		}
		if(bShow && m_pParent)
		{
			bShow=m_pParent->IsVisible(TRUE);
		}

		if (bShow)
		{
			ModifyState(0, WndState_Invisible);
			accNotifyEvent(EVENT_OBJECT_SHOW);
		}
		else
		{
			ModifyState(WndState_Invisible, 0);
			accNotifyEvent(EVENT_OBJECT_HIDE);
		}

		SWindow *pChild=m_pFirstChild;
		while(pChild)
		{
			pChild->AddRef();
			pChild->SSendMessage(WM_SHOWWINDOW,bShow,ParentShow);
			SWindow *pNextChild=pChild->GetWindow(GSW_NEXTSIBLING);;
			pChild->Release();
			pChild=pNextChild;
		}
		if(!IsVisible(TRUE))
		{
			if(IsFocused()) GetContainer()->OnSetSwndFocus(NULL);   //窗口隐藏时自动失去焦点
			if(GetCapture() == m_swnd) ReleaseCapture();//窗口隐藏时自动失去Capture
		}

		if(!m_bDisplay)
		{
			RequestRelayout();
		}

		EventSwndVisibleChanged evtShow(this);
		FireEvent(evtShow);
	}


	void SWindow::OnEnable( BOOL bEnable,UINT nStatus )
	{
		if(nStatus == ParentEnable)
		{
			if(bEnable && IsDisabled(FALSE)) bEnable=FALSE;
		}
		else
		{
			m_bDisable=!bEnable;
		}
		if(bEnable && m_pParent)
		{
			bEnable=!m_pParent->IsDisabled(TRUE);
		}

		if (bEnable)
			ModifyState(0, WndState_Disable);
		else
			ModifyState(WndState_Disable, WndState_Hover);

		SWindow *pChild=m_pFirstChild;
		while(pChild)
		{
			pChild->SSendMessage(WM_ENABLE,bEnable,ParentEnable);
			pChild=pChild->GetWindow(GSW_NEXTSIBLING);
		}
		if(IsDisabled(TRUE) && IsFocused())
		{
			GetContainer()->OnSetSwndFocus(NULL);
		}
	}

	void SWindow::OnLButtonDown(UINT nFlags,CPoint pt)
	{
		if(m_bFocusable) SetFocus();
		SetCapture();
		ModifyState(WndState_PushDown, 0,TRUE);

		EventLButtonDown evtLButtonDown(this);
		evtLButtonDown.pt = pt;
		FireEvent(evtLButtonDown);
	}

	void SWindow::OnLButtonUp(UINT nFlags,CPoint pt)
	{
		ReleaseCapture();

		if(!(GetState()&WndState_PushDown)) return;

		ModifyState(0, WndState_PushDown,TRUE);
		if(!m_rcWindow.PtInRect(pt)) return;

		EventLButtonUp evtLButtonUp(this);
		evtLButtonUp.pt = pt;
		FireEvent(evtLButtonUp);

		if (GetID() || GetName())
		{
			FireCommand();
		}
	}

	void SWindow::OnRButtonDown( UINT nFlags, CPoint point )
	{
		FireCtxMenu(point);
	}

	void SWindow::OnRButtonUp(UINT nFlags, CPoint point)
	{
	}

	void SWindow::OnMouseHover(WPARAM wParam, CPoint ptPos)
	{
		if(GetCapture()==m_swnd)
			ModifyState(WndState_PushDown,0,FALSE);
		ModifyState(WndState_Hover, 0,TRUE);
		_RedrawNonClient();
		EventSwndMouseHover evtHover(this);
		FireEvent(evtHover);
	}

	void SWindow::OnMouseLeave()
	{
		if(GetCapture()==m_swnd)
			ModifyState(0,WndState_PushDown,FALSE);
		ModifyState(0,WndState_Hover,TRUE);
		_RedrawNonClient();
		EventSwndMouseLeave evtLeave(this);
		FireEvent(evtLeave);
	}

	BOOL SWindow::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
	{
		BOOL bRet=FALSE;
		if(m_pParent) bRet=(BOOL)m_pParent->SSendMessage(WM_MOUSEWHEEL,MAKEWPARAM(nFlags,zDelta),MAKELPARAM(pt.x,pt.y));
		return bRet;
	}

	CRect SWindow::GetChildrenLayoutRect()
	{
		CRect rcRet;
		GetClientRect(rcRet);
		rcRet.DeflateRect(GetStyle().GetPadding());
		return rcRet;
	}

	void SWindow::UpdateChildrenPosition()
	{
		if(m_layoutDirty == dirty_self)
		{//当前窗口所有子窗口全部重新布局
			GetLayout()->LayoutChildren(this);

			SWindow *pChild=GetWindow(GSW_FIRSTCHILD);
			while(pChild)
			{
				if(pChild->m_bFloat)
				{
					pChild->OnUpdateFloatPosition(GetChildrenLayoutRect());
					if(pChild->m_layoutDirty != dirty_clean)
					{
						pChild->UpdateChildrenPosition();
					}
				}
				pChild=pChild->GetWindow(GSW_NEXTSIBLING);
			}
		}else if(m_layoutDirty == dirty_child)
		{//只有个别子窗口需要重新布局
			SWindow *pChild = GetNextLayoutChild(NULL);
			while(pChild)
			{
				if(pChild->m_layoutDirty != dirty_clean)
				{
					pChild->UpdateChildrenPosition();
				}
				pChild = GetNextLayoutChild(pChild);
			}
		}
	}

	void SWindow::RequestRelayout()
	{
		RequestRelayout(this,TRUE);//此处bSourceResizable可以为任意值
	}

	void SWindow::RequestRelayout(SWindow *pSource,BOOL bSourceResizable)
	{
		SASSERT(pSource);

		if(bSourceResizable)
		{//源窗口大小发生变化,当前窗口的所有子窗口全部重新布局
			m_layoutDirty = dirty_self;
		}

		if(m_layoutDirty != dirty_self)
		{//需要检测当前窗口是不是内容自适应
			m_layoutDirty = (pSource == this || GetLayoutParam()->IsWrapContent(Any)) ? dirty_self:dirty_child;
		}

		SWindow *pParent = GetParent();
		if(pParent) pParent->RequestRelayout(pSource,GetLayoutParam()->IsWrapContent(Any) || !IsDisplay());
	}

	void SWindow::UpdateLayout()
	{
		if(m_layoutDirty == dirty_clean) return;
		UpdateChildrenPosition();
		m_layoutDirty = dirty_clean;
	}


	SWindow * SWindow::GetNextLayoutChild(SWindow *pCurChild) const
	{
		SWindow *pRet = NULL;
		if(pCurChild == NULL)
		{
			pRet = GetWindow(GSW_FIRSTCHILD);
		}else
		{
			pRet = pCurChild->GetWindow(GSW_NEXTSIBLING);
		}

		if(pRet && (pRet->IsFloat() || (!pRet->IsDisplay() && !pRet->IsVisible(FALSE))))
			return GetNextLayoutChild(pRet);
		return pRet;
	}

	void SWindow::OnSetFocus(SWND wndOld)
	{
		EventSetFocus evt(this);
		FireEvent(evt);
		InvalidateRect(m_rcWindow);
		accNotifyEvent(EVENT_OBJECT_FOCUS);
	}

	void SWindow::OnKillFocus(SWND wndFocus)
	{
		EventKillFocus evt(this);
		FireEvent(evt);
		InvalidateRect(m_rcWindow);
	}

	LRESULT SWindow::OnSetScale(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		int nScale = (int)wParam;
		OnScaleChanged(nScale);
		return 0;
	}


	LRESULT SWindow::OnSetLanguage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		HRESULT hr = OnLanguageChanged();
		if(hr == S_FALSE)
			Invalidate();
		else if(hr == S_OK)
			RequestRelayout(this,TRUE);
		return 0;
	}

	//当窗口有半透明属性并且透明度要需要应用于子窗口时，子窗口的图像渲染到this的缓存RT上。
	BOOL SWindow::IsLayeredWindow() const
	{
		return m_bLayeredWindow;
	}

	//查询当前窗口内容将被渲染到哪一个渲染层上，没有渲染层时返回NULL
	SWindow * SWindow::_GetCurrentLayeredWindow()
	{
		SWindow *pWnd = this;
		while(pWnd)
		{
			if(pWnd->IsLayeredWindow())
			{
				break;
			}
			pWnd = pWnd->GetParent();
		}

		return pWnd;
	}

	IRenderTarget * SWindow::GetRenderTarget(LPCRECT pRc,DWORD gdcFlags/*=OLEDC_NODRAW*/,BOOL bClientRT/*=TRUE*/)
	{
		CRect rcRT ;        
		if(bClientRT)
		{
			GetClientRect(&rcRT);
		}else
		{
			GetWindowRect(&rcRT);
		}
		if(pRc) rcRT.IntersectRect(pRc,&rcRT);


		CAutoRefPtr<IRegion> rgn;
		GETRENDERFACTORY->CreateRegion(&rgn);
		rgn->CombineRect(rcRT,RGN_COPY);

		return GetRenderTarget(gdcFlags,rgn);
	}

	IRenderTarget * SWindow::GetRenderTarget( DWORD gdcFlags,IRegion *pRgn )
	{
		CRect rcClip;
		pRgn->GetRgnBox(&rcClip);
		SWindow *pParent = GetParent();
		while(pParent)
		{
			rcClip.IntersectRect(rcClip,pParent->GetClientRect());
			pParent = pParent->GetParent();
		}

		pRgn->CombineRect(&rcClip,RGN_AND);
		pRgn->GetRgnBox(&rcClip);

		//获得最近的一个渲染层的RT
		IRenderTarget *pRT = _GetRenderTarget(rcClip,gdcFlags,pRgn);
		BeforePaintEx(pRT);
		return pRT;
	}

	void SWindow::ReleaseRenderTarget(IRenderTarget *pRT)
	{
		SASSERT(m_pGetRTData);
		_ReleaseRenderTarget(pRT);        
	}

	IRenderTarget * SWindow::_GetRenderTarget(CRect & rcGetRT,DWORD gdcFlags,IRegion *pRgn)
	{
		IRenderTarget *pRT = NULL;
		SWindow *pLayerWindow = _GetCurrentLayeredWindow();

		SASSERT(!m_pGetRTData);
		m_pGetRTData = new GETRTDATA;

		m_pGetRTData->gdcFlags = gdcFlags;
		m_pGetRTData->rcRT = rcGetRT;
		m_pGetRTData->rgn = pRgn;

		GetContainer()->BuildWndTreeZorder();

		if(pLayerWindow)
		{
			pRT = pLayerWindow->GetLayerRenderTarget();
		}else
		{
			pLayerWindow = GetRoot();
			pRT = GetContainer()->OnGetRenderTarget(rcGetRT,gdcFlags);
		}

		pRT->PushClipRegion(pRgn,RGN_COPY);

		if(gdcFlags == OLEDC_PAINTBKGND)
		{//重新绘制当前窗口的背景
			pRT->ClearRect(&rcGetRT,0);
			pLayerWindow->_PaintRegion(pRT,pRgn,ZORDER_MIN,m_uZorder);
		}
		return pRT;
	}


	void SWindow::_ReleaseRenderTarget(IRenderTarget *pRT)
	{
		SASSERT(m_pGetRTData);

		SWindow *pRoot = GetRoot();
		SWindow *pLayerWindow = _GetCurrentLayeredWindow();

		if(m_pGetRTData->gdcFlags == OLEDC_PAINTBKGND)
		{//从指定的窗口开始绘制前景
			SWindow * pLayer = pLayerWindow?pLayerWindow:pRoot;
			pLayer->_PaintRegion2(pRT,m_pGetRTData->rgn,(UINT)m_uZorder+1,(UINT)ZORDER_MAX);
		}
		pRT->PopClip();//对应_GetRenderTarget中调用的PushClipRegion

		if(pLayerWindow)
		{//存在一个渲染层
			SASSERT(m_pGetRTData);
			if(m_pGetRTData->gdcFlags != OLEDC_NODRAW)
			{
				UINT uFrgndZorderMin = (UINT)ZORDER_MAX;
				SWindow *pParent = pLayerWindow->GetParent();
				if(pParent)
				{
					//查找上一个渲染层的前景：向上层查找下一个兄弟，直到找到为止
					SWindow *pWnd = pLayerWindow;
					while(pWnd)
					{
						SWindow *pNextSibling = pWnd->GetWindow(GSW_NEXTSIBLING);
						if(pNextSibling)
						{
							uFrgndZorderMin = pNextSibling->m_uZorder;
							break;
						}else
						{
							pWnd = pWnd->GetParent();
						}
					}
				}

				IRenderTarget *pRTRoot = GetContainer()->OnGetRenderTarget(m_pGetRTData->rcRT,OLEDC_OFFSCREEN);
				pRTRoot->PushClipRegion(m_pGetRTData->rgn);
				pRTRoot->ClearRect(m_pGetRTData->rcRT,0);
				//从root开始绘制当前layer前的窗口背景
				pRoot->_PaintRegion2(pRTRoot,m_pGetRTData->rgn,ZORDER_MIN,pLayerWindow->m_uZorder);
				//将layer的渲染更新到root上
				pRTRoot->AlphaBlend(m_pGetRTData->rcRT,pRT,m_pGetRTData->rcRT,pLayerWindow->m_style.m_byAlpha);
				//绘制当前layer前的窗口前景
				if(uFrgndZorderMin!=ZORDER_MAX) 
					pRoot->_PaintRegion2(pRTRoot,m_pGetRTData->rgn,(UINT)uFrgndZorderMin,(UINT)ZORDER_MAX);
				pRTRoot->PopClip();
				GetContainer()->OnReleaseRenderTarget(pRTRoot,m_pGetRTData->rcRT,OLEDC_OFFSCREEN);
			}
		}else
		{//不在绘制层
			GetContainer()->OnReleaseRenderTarget(pRT,m_pGetRTData->rcRT,m_pGetRTData->gdcFlags);
		}
		delete m_pGetRTData;
		m_pGetRTData = NULL;
	}

	SWND SWindow::GetCapture()
	{
		return GetContainer()->OnGetSwndCapture();
	}

	SWND SWindow::SetCapture()
	{
		return GetContainer()->OnSetSwndCapture(m_swnd);
	}

	BOOL SWindow::ReleaseCapture()
	{
		return GetContainer()->OnReleaseSwndCapture();
	}

	void SWindow::SetFocus()
	{
		if(!IsVisible(TRUE) || IsDisabled(TRUE) && IsFocusable()) return;
		GetContainer()->OnSetSwndFocus(m_swnd);
	}

	void SWindow::KillFocus()
	{
		if(IsFocused())
		{
			GetContainer()->OnSetSwndFocus(NULL);
		}
	}

	BOOL SWindow::IsFocused()
	{
		return GetContainer()->GetFocus() == m_swnd;
	}

	BOOL SWindow::OnNcHitTest(CPoint pt)
	{
		CRect rcClient = GetClientRect();
		return !rcClient.PtInRect(pt);
	}

	SWindow * SWindow::GetWindow(int uCode) const
	{
		SWindow *pRet=NULL;
		switch(uCode)
		{
		case GSW_FIRSTCHILD:
			pRet=m_pFirstChild;
			break;
		case GSW_LASTCHILD:
			pRet=m_pLastChild;
			break;
		case GSW_PREVSIBLING:
			pRet=m_pPrevSibling;
			break;
		case GSW_NEXTSIBLING:
			pRet=m_pNextSibling;
			break;
		case GSW_OWNER:
			pRet=m_pOwner;
			break;
		case GSW_PARENT:
			pRet=m_pParent;
			break;
		}
		return pRet;
	}

	SWindow * SWindow::GetChild(int iChild)
	{
		if (iChild == CHILDID_SELF)
			return this;
		SWindow *pChild = GetWindow(GSW_FIRSTCHILD);
		for (int i = 0; i < iChild-1 && pChild; i++)
		{
			pChild = pChild->GetWindow(GSW_NEXTSIBLING);
			if (!pChild) return NULL;
		}

		return pChild;
	}


	void SWindow::PaintBackground(IRenderTarget *pRT,LPRECT pRc )
	{
		CRect rcDraw=m_rcWindow;
		if(pRc) rcDraw.IntersectRect(rcDraw,pRc);
		pRT->PushClipRect(&rcDraw,RGN_AND);

		SWindow *pTopWnd=GetRoot();
		CAutoRefPtr<IRegion> pRgn;
		GETRENDERFACTORY->CreateRegion(&pRgn);
		pRgn->CombineRect(&rcDraw,RGN_COPY);

		pRT->ClearRect(&rcDraw,0);//清除残留的alpha值

		GetContainer()->BuildWndTreeZorder();
		pTopWnd->_PaintRegion(pRT,pRgn,ZORDER_MIN,m_uZorder);

		pRT->PopClip();
	}

	void SWindow::PaintForeground( IRenderTarget *pRT,LPRECT pRc )
	{
		CRect rcDraw=m_rcWindow;
		if(pRc) rcDraw.IntersectRect(rcDraw,pRc);
		CAutoRefPtr<IRegion> pRgn;
		GETRENDERFACTORY->CreateRegion(&pRgn);
		pRgn->CombineRect(&rcDraw,RGN_COPY);
		pRT->PushClipRect(&rcDraw);

		GetContainer()->BuildWndTreeZorder();
		GetTopLevelParent()->_PaintRegion(pRT,pRgn,(UINT)m_uZorder+1,(UINT)ZORDER_MAX);

		pRT->PopClip();
	}
	void SWindow::PaintForeground2(IRenderTarget *pRT, LPRECT pRc)
	{
		CRect rcDraw = m_rcWindow;
		if (pRc) rcDraw.IntersectRect(rcDraw, pRc);
		CAutoRefPtr<IRegion> pRgn;
		GETRENDERFACTORY->CreateRegion(&pRgn);
		pRgn->CombineRect(&rcDraw, RGN_COPY);
		pRT->PushClipRect(&rcDraw);

		GetContainer()->BuildWndTreeZorder();
		//UINT MAX_ZORDER=GetParent()->GetWindow(GSW_LASTCHILD)->m_uZorder;
		GetParent()->_PaintRegion(pRT, pRgn, (UINT)m_uZorder + 1, (UINT)ZORDER_MAX);

		pRT->PopClip();
	}
	void SWindow::DrawAniStep( CRect rcFore,CRect rcBack,IRenderTarget * pRTFore,IRenderTarget * pRTBack,CPoint ptAnchor)
	{
		IRenderTarget * pRT=GetRenderTarget(rcBack,OLEDC_OFFSCREEN);
		pRT->BitBlt(&rcBack,pRTBack,rcBack.left,rcBack.top,SRCCOPY);
		pRT->BitBlt(&rcFore,pRTFore,ptAnchor.x,ptAnchor.y,SRCCOPY);
		PaintForeground(pRT,rcBack);//画前景
		ReleaseRenderTarget(pRT);
	}

	void SWindow::DrawAniStep( CRect rcWnd,IRenderTarget * pRTFore,IRenderTarget * pRTBack,BYTE byAlpha)
	{
		IRenderTarget * pRT=GetRenderTarget(rcWnd,OLEDC_OFFSCREEN);
		if(byAlpha>0 && byAlpha<255)
		{
			pRT->BitBlt(&rcWnd,pRTBack,rcWnd.left,rcWnd.top,SRCCOPY);
			pRT->AlphaBlend(&rcWnd,pRTFore,&rcWnd,byAlpha);
		}else if(byAlpha==0)
		{
			pRT->BitBlt(&rcWnd,pRTBack,rcWnd.left,rcWnd.top,SRCCOPY);
		}else if(byAlpha==255)
		{
			pRT->BitBlt(&rcWnd,pRTFore,rcWnd.left,rcWnd.top,SRCCOPY);
		}
		PaintForeground(pRT,rcWnd);//画前景
		ReleaseRenderTarget(pRT);
	}

	BOOL SWindow::AnimateWindow(DWORD dwTime,DWORD dwFlags )
	{
		if(dwFlags & AW_HIDE)
		{
			if(!IsVisible(TRUE))
				return FALSE;
		}else
		{//动画显示窗口时，不能是最顶层窗口，同时至少上一层窗口应该可见
			if(IsVisible(TRUE))
				return FALSE;
			SWindow *pParent=GetParent();
			if(!pParent) return FALSE;
			if(!pParent->IsVisible(TRUE)) return FALSE;
		}
		CRect rcWnd;
		GetWindowRect(&rcWnd);

		CAutoRefPtr<IRegion> rgn;
		GETRENDERFACTORY->CreateRegion(&rgn);
		rgn->CombineRect(&rcWnd,RGN_COPY);

		IRenderTarget *pRT=GetRenderTarget(rcWnd,OLEDC_NODRAW);
		CAutoRefPtr<IRenderTarget> pRTBefore;
		GETRENDERFACTORY->CreateRenderTarget(&pRTBefore,rcWnd.Width(),rcWnd.Height());
		pRTBefore->OffsetViewportOrg(-rcWnd.left,-rcWnd.top);

		//渲染窗口变化前状态
		PaintBackground(pRT,rcWnd);
		RedrawRegion(pRT,rgn);
		pRTBefore->BitBlt(&rcWnd,pRT,rcWnd.left,rcWnd.top,SRCCOPY);

		//更新窗口可见性
		SetVisible(!(dwFlags&AW_HIDE),FALSE);
		//窗口变化后
		CAutoRefPtr<IRenderTarget> pRTAfter;
		GETRENDERFACTORY->CreateRenderTarget(&pRTAfter,rcWnd.Width(),rcWnd.Height());
		pRTAfter->OffsetViewportOrg(-rcWnd.left,-rcWnd.top);

		PaintBackground(pRT,rcWnd);
		RedrawRegion(pRT,rgn);
		pRTAfter->BitBlt(&rcWnd,pRT,rcWnd.left,rcWnd.top,SRCCOPY);

		ReleaseRenderTarget(pRT);

		int nSteps=dwTime/10;
		if(dwFlags & AW_HIDE)
		{//hide
			if(dwFlags& AW_SLIDE)
			{
				CRect rcNewState(rcWnd);
				LONG  x1 = rcNewState.left;
				LONG  x2 = rcNewState.left;
				LONG  y1 = rcNewState.top;
				LONG  y2 = rcNewState.top;
				LONG * x =&rcNewState.left;
				LONG * y =&rcNewState.top;

				if(dwFlags & AW_HOR_POSITIVE)
				{//left->right:move left
					x1=rcNewState.left,x2=rcNewState.right;
					x=&rcNewState.left;
				}else if(dwFlags & AW_HOR_NEGATIVE)
				{//right->left:move right
					x1=rcNewState.right,x2=rcNewState.left;
					x=&rcNewState.right;
				}
				if(dwFlags & AW_VER_POSITIVE)
				{//top->bottom
					y1=rcNewState.top,y2=rcNewState.bottom;
					y=&rcNewState.top;
				}else if(dwFlags & AW_VER_NEGATIVE)
				{//bottom->top
					y1=rcNewState.bottom,y2=rcNewState.top;
					y=&rcNewState.bottom;
				}
				LONG xStepLen=(x2-x1)/nSteps;
				LONG yStepLen=(y2-y1)/nSteps;

				CPoint ptAnchor;
				for(int i=0;i<nSteps;i++)
				{
					*x+=xStepLen;
					*y+=yStepLen;
					ptAnchor=rcWnd.TopLeft();
					if(dwFlags & AW_HOR_NEGATIVE)
					{//right->left:move right
						ptAnchor.x=rcWnd.right-rcNewState.Width();
					}
					if(dwFlags & AW_VER_NEGATIVE)
					{
						ptAnchor.y=rcWnd.bottom-rcNewState.Height();
					}
					DrawAniStep(rcNewState,rcWnd,pRTBefore,pRTAfter,ptAnchor);
					Sleep(10);
				}
				DrawAniStep(CRect(),rcWnd,pRTBefore,pRTAfter,rcWnd.TopLeft());
				return TRUE;
			}else if(dwFlags&AW_CENTER)
			{
				CRect rcNewState(rcWnd);
				int xStep=rcNewState.Width()/(2*nSteps);
				int yStep=rcNewState.Height()/(2*nSteps);
				for(int i=0;i<nSteps;i++)
				{
					rcNewState.DeflateRect(xStep,yStep);
					DrawAniStep(rcNewState,rcWnd,pRTBefore,pRTAfter,rcNewState.TopLeft());
					Sleep(10);
				}
				DrawAniStep(CRect(),rcWnd,pRTBefore,pRTAfter,rcWnd.TopLeft());
				return TRUE;
			}else if(dwFlags&AW_BLEND)
			{
				BYTE byAlpha=255;
				BYTE byStepLen=255/nSteps;
				for(int i=0;i<nSteps;i++)
				{
					DrawAniStep(rcWnd,pRTBefore,pRTAfter,byAlpha);
					Sleep(10);
					byAlpha-=byStepLen;
				}
				DrawAniStep(rcWnd,pRTBefore,pRTAfter,0);
				return TRUE;
			}
			return FALSE;
		}else
		{//show
			if(dwFlags& AW_SLIDE)
			{
				CRect rcNewState(rcWnd);
				LONG  x1 = rcNewState.left;
				LONG  x2 = rcNewState.left;
				LONG  y1 = rcNewState.top;
				LONG  y2 = rcNewState.top;
				LONG * x =&rcNewState.left;
				LONG * y =&rcNewState.top;

				if(dwFlags & AW_HOR_POSITIVE)
				{//left->right:move right
					x1=rcNewState.left,x2=rcNewState.right;
					rcNewState.right=rcNewState.left,x=&rcNewState.right;
				}else if(dwFlags & AW_HOR_NEGATIVE)
				{//right->left:move left
					x1=rcNewState.right,x2=rcNewState.left;
					rcNewState.left=rcNewState.right,x=&rcNewState.left;
				}
				if(dwFlags & AW_VER_POSITIVE)
				{//top->bottom
					y1=rcNewState.top,y2=rcNewState.bottom;
					rcNewState.bottom=rcNewState.top,y=&rcNewState.bottom;
				}else if(dwFlags & AW_VER_NEGATIVE)
				{//bottom->top
					y1=rcNewState.bottom,y2=rcNewState.top;
					rcNewState.top=rcNewState.bottom,y=&rcNewState.top;
				}
				LONG xStepLen=(x2-x1)/nSteps;
				LONG yStepLen=(y2-y1)/nSteps;

				CPoint ptAnchor;
				for(int i=0;i<nSteps;i++)
				{
					*x+=xStepLen;
					*y+=yStepLen;
					ptAnchor=rcWnd.TopLeft();
					if(dwFlags & AW_HOR_POSITIVE)
					{//left->right:move left
						ptAnchor.x=rcWnd.right-rcNewState.Width();
					}
					if(dwFlags & AW_VER_POSITIVE)
					{
						ptAnchor.y=rcWnd.bottom-rcNewState.Height();
					}
					DrawAniStep(rcNewState,rcWnd,pRTAfter,pRTBefore,ptAnchor);
					Sleep(10);
				}
				DrawAniStep(rcWnd,rcWnd,pRTAfter,pRTBefore,rcWnd.TopLeft());
				return TRUE;
			}else if(dwFlags&AW_CENTER)
			{
				CRect rcNewState(rcWnd);
				int xStep=rcNewState.Width()/(2*nSteps);
				int yStep=rcNewState.Height()/(2*nSteps);
				rcNewState.left=rcNewState.right=(rcNewState.left+rcNewState.right)/2;
				rcNewState.top=rcNewState.bottom=(rcNewState.top+rcNewState.bottom)/2;
				for(int i=0;i<nSteps;i++)
				{
					rcNewState.InflateRect(xStep,yStep);
					DrawAniStep(rcNewState,rcWnd,pRTAfter,pRTBefore,rcNewState.TopLeft());
					Sleep(10);
				}
				DrawAniStep(rcWnd,rcWnd,pRTAfter,pRTBefore,rcWnd.TopLeft());
				return TRUE;
			}else if(dwFlags&AW_BLEND)
			{
				BYTE byAlpha=0;
				BYTE byStepLen=255/nSteps;
				for(int i=0;i<nSteps;i++)
				{
					DrawAniStep(rcWnd,pRTAfter,pRTBefore,byAlpha);
					Sleep(10);
					byAlpha+=byStepLen;
				}
				DrawAniStep(rcWnd,pRTAfter,pRTBefore,255);
				return TRUE;
			}
			return FALSE;
		}
	}

	BOOL SWindow::FireCommand()
	{
		EventCmd evt(this);
		return FireEvent(evt);
	}

	BOOL SWindow::FireCtxMenu( CPoint pt)
	{
		EventCtxMenu evt(this);
		evt.bCancel = FALSE;
		evt.pt=pt;
		FireEvent(evt);
		return evt.bCancel;
	}

	//////////////////////////////////////////////////////////////////////////

	HRESULT SWindow::OnAttrVisible( const SStringW& strValue, BOOL bLoading )
	{
		BOOL bVisible = STRINGASBOOL(strValue);
		if(!bLoading)   SetVisible(bVisible,TRUE);
		else m_bVisible=bVisible;
		return S_FALSE;
	}

	HRESULT SWindow::OnAttrEnable( const SStringW& strValue, BOOL bLoading )
	{
		BOOL bEnable = STRINGASBOOL(strValue);
		if(bLoading)
		{
			if (bEnable)
				ModifyState(0, WndState_Disable);
			else
				ModifyState(WndState_Disable, WndState_Hover);
		}
		else
		{
			EnableWindow(bEnable,TRUE);
		}
		return S_FALSE;
	}

	HRESULT SWindow::OnAttrDisplay( const SStringW& strValue, BOOL bLoading )
	{
		m_bDisplay = STRINGASBOOL(strValue);
		if(bLoading)
			return S_FALSE;

		RequestRelayout();
		return S_OK;
	}

	HRESULT SWindow::OnAttrSkin( const SStringW& strValue, BOOL bLoading )
	{
		m_pBgSkin = GETSKIN(strValue,GetScale());
		if(bLoading) return S_FALSE;

		if((GetLayoutParam()->IsWrapContent(Vert) || GetLayoutParam()->IsWrapContent(Horz)))
		{
			RequestRelayout();
		}
		return S_OK;
	}

	HRESULT SWindow::OnAttrClass( const SStringW& strValue, BOOL bLoading )
	{
		pugi::xml_node xmlStyle=GETSTYLE(strValue);
		if(xmlStyle)
		{
			//优先处理layout属性
			pugi::xml_attribute attrLayout = xmlStyle.attribute(L"layout");
			if(attrLayout)
			{
				SetAttribute(attrLayout.name(),attrLayout.value(),bLoading);
				IObject::MarkAttributeHandled(attrLayout,true);
			}
			for(pugi::xml_attribute attr=xmlStyle.first_attribute();attr;attr =attr.next_attribute())
			{//解析style中的属性
				if(wcsicmp(attr.name(),L"class")==0 || IObject::IsAttributeHandled(attr)) 
					continue;//防止class中包含有其它class属性,避免发生死循环
				SetAttribute(attr.name(),attr.value(),bLoading);
			}
			IObject::MarkAttributeHandled(attrLayout,false);
		}
		return S_FALSE;
	}

	HRESULT SWindow::OnAttrTrackMouseEvent( const SStringW& strValue, BOOL bLoading )
	{
		m_style.m_bTrackMouseEvent = STRINGASBOOL(strValue);
		if(!bLoading)
		{
			if(m_style.m_bTrackMouseEvent)
				GetContainer()->RegisterTrackMouseEvent(m_swnd);
			else
				GetContainer()->UnregisterTrackMouseEvent(m_swnd);
		}
		return S_FALSE;
	}

	void SWindow::OnSize( UINT nType, CSize size )
	{
		if(IsDrawToCache())
		{
			if(!m_cachedRT)
			{
				GETRENDERFACTORY->CreateRenderTarget(&m_cachedRT,m_rcWindow.Width(),m_rcWindow.Height());
			}else
			{
				m_cachedRT->Resize(m_rcWindow.Size());
			}
			m_cachedRT->SetViewportOrg(-m_rcWindow.TopLeft());

			MarkCacheDirty(true);
		}
		if(IsLayeredWindow())
		{
			if(!m_layeredRT)
			{
				GETRENDERFACTORY->CreateRenderTarget(&m_layeredRT,m_rcWindow.Width(),m_rcWindow.Height());
			}else
			{
				m_layeredRT->Resize(m_rcWindow.Size());
			}
			m_layeredRT->SetViewportOrg(-m_rcWindow.TopLeft());
		}

		EventSwndSize evt(this);
		evt.szWnd = size;
		FireEvent(evt);
	}

	void SWindow::UpdateCacheMode()
	{
		if(IsDrawToCache() && !m_cachedRT)
		{
			GETRENDERFACTORY->CreateRenderTarget(&m_cachedRT,m_rcWindow.Width(),m_rcWindow.Height());
			m_cachedRT->SetViewportOrg(-m_rcWindow.TopLeft());
			MarkCacheDirty(true);
		}
		if(!IsDrawToCache() && m_cachedRT)
		{
			m_cachedRT=NULL;
		}
	}

	HRESULT SWindow::OnAttrCache( const SStringW& strValue, BOOL bLoading )
	{
		m_bCacheDraw = STRINGASBOOL(strValue);

		if(!bLoading)
		{
			UpdateCacheMode();
			InvalidateRect(NULL);
		}
		return S_FALSE;
	}

	HRESULT SWindow::OnAttrAlpha( const SStringW& strValue, BOOL bLoading )
	{
		m_style.m_byAlpha = _wtoi(strValue);
		if(!bLoading)
		{
			if(!IsLayeredWindow()) UpdateCacheMode();
			InvalidateRect(NULL);
		}
		return bLoading?S_FALSE:S_OK;
	}

	void SWindow::UpdateLayeredWindowMode()
	{
		if(IsLayeredWindow() && !m_layeredRT)
		{
			GETRENDERFACTORY->CreateRenderTarget(&m_layeredRT,m_rcWindow.Width(),m_rcWindow.Height());
			m_layeredRT->SetViewportOrg(-m_rcWindow.TopLeft());
		}
		if(!IsLayeredWindow() && m_layeredRT)
		{
			m_layeredRT=NULL;
		}

	}

	HRESULT SWindow::OnAttrLayeredWindow( const SStringW& strValue, BOOL bLoading )
	{
		m_bLayeredWindow = STRINGASBOOL(strValue);
		if(!bLoading)
		{
			UpdateLayeredWindowMode();
		}
		return bLoading?S_FALSE:S_OK;
	}


	HRESULT SWindow::OnAttrID( const SStringW& strValue, BOOL bLoading )
	{
		static struct SystemID{
			int id;
			LPCWSTR pszName;
		} systemID[] = {
			IDOK,L"IDOK",
			IDCANCEL,L"IDCANCEL",
			IDCLOSE,L"IDCLOSE",
			IDHELP,L"IDHELP",
			IDABORT,L"IDABORT",
			IDYES,L"IDYES",
			IDNO,L"IDNO",
			IDRETRY,L"IDRETRY",
			IDIGNORE,L"IDIGNORE"
		};
		if(!strValue.IsEmpty())
		{
			if(strValue.Left(2).CompareNoCase(L"ID")==0)
			{
				for(int i=0;i<ARRAYSIZE(systemID);i++)
				{
					if(strValue.CompareNoCase(systemID[i].pszName)==0)
					{
						m_nID =  systemID[i].id;
						break;
					}
				}
			}else
			{
				m_nID = _wtoi(strValue);
			}
		}
		return S_FALSE;
	}

	HRESULT SWindow::OnAttrName( const SStringW& strValue, BOOL bLoading )
	{   
		m_strName = strValue;
		if(m_nID == 0)
		{
			m_nID = STR2ID(strValue);
		}
		return S_FALSE;
	}

	HRESULT SWindow::OnAttrTip(const SStringW& strValue, BOOL bLoading)
	{
		SetToolTipText(S_CW2T(GETSTRING(strValue)));
		return S_FALSE;
	}

	SWindow * SWindow::GetSelectedChildInGroup()
	{
		SWindow *pChild = GetWindow(GSW_FIRSTCHILD);
		while(pChild)
		{
			if(pChild->IsSiblingsAutoGroupped())
				break;
			pChild = pChild->GetWindow(GSW_NEXTSIBLING);
		}
		if(!pChild ) 
			return NULL;
		return pChild->GetSelectedSiblingInGroup();
	}

	void SWindow::_Update()
	{
		SASSERT(!m_style.m_bBkgndBlend);

		if(!m_style.m_bBkgndBlend && m_invalidRegion && !m_invalidRegion->IsEmpty()) 
		{
			if(m_invalidRegion)
			{
				//刷新非背景混合的窗口
				CRect rcDirty;
				m_invalidRegion->GetRgnBox(&rcDirty);
				CAutoRefPtr<IRegion> tmpRegin = m_invalidRegion;
				m_invalidRegion = NULL;

				if(IsVisible(TRUE))
				{//可能已经不可见了。
					IRenderTarget *pRT = GetRenderTarget(rcDirty,OLEDC_OFFSCREEN);

					pRT->PushClipRegion(tmpRegin,RGN_AND);
					SSendMessage(WM_ERASEBKGND, (WPARAM)pRT);
					SSendMessage(WM_PAINT, (WPARAM)pRT);
					PaintForeground(pRT,rcDirty);//画前景
					pRT->PopClip();

					ReleaseRenderTarget(pRT);
				}
			}
		}
	}


	bool SWindow::IsDrawToCache() const
	{
		return m_bCacheDraw || (!IsLayeredWindow() && m_style.m_byAlpha!=0xff);
	}

	IRenderTarget * SWindow::GetLayerRenderTarget()
	{
		SASSERT(IsLayeredWindow());
		if(!m_layeredRT)  GETRENDERFACTORY->CreateRenderTarget(&m_layeredRT,0,0);
		return m_layeredRT;
	}

	void SWindow::OnStateChanging( DWORD dwOldState,DWORD dwNewState )
	{

	}

	void SWindow::OnStateChanged( DWORD dwOldState,DWORD dwNewState )
	{
		EventSwndStateChanged evt(this);
		evt.dwOldState = dwOldState;
		evt.dwNewState = dwNewState;
		FireEvent(evt);
		accNotifyEvent(EVENT_OBJECT_STATECHANGE);
	}

	IScriptModule * SWindow::GetScriptModule()
	{
		if(!GetContainer()) return NULL;
		return GetContainer()->GetScriptModule();
	}

	HRESULT SWindow::DefAttributeProc( const SStringW & strAttribName,const SStringW & strValue, BOOL bLoading )
	{
		HRESULT hr = E_FAIL;
		if(GetScriptModule())
		{
			if(m_evtSet.setEventScriptHandler(strAttribName,S_CW2A(strValue,CP_UTF8)))
			{
				hr = S_FALSE;
			}
		}
		return hr;
	}

	//////////////////////////////////////////////////////////////////////////
	// caret functions

	BOOL SWindow::CreateCaret(HBITMAP pBmp,int nWid,int nHeight)
	{
		return GetContainer()->OnCreateCaret(m_swnd,pBmp,nWid,nHeight);
	}

	void SWindow::ShowCaret(BOOL bShow)
	{
		GetContainer()->OnShowCaret(bShow);
	}

	void SWindow::SetCaretPos(int x,int y)
	{
		GetContainer()->OnSetCaretPos(x,y);
	}

	BOOL SWindow::IsContainPoint(const POINT &pt,BOOL bClientOnly) const
	{
		BOOL bRet = FALSE;
		if(bClientOnly)
		{
			CRect rcClient = GetClientRect();
			bRet = rcClient.PtInRect(pt);
		}else
		{
			bRet = m_rcWindow.PtInRect(pt);
		}
		if(m_rgnWnd)
		{
			CPoint ptTmp = pt;
			ptTmp -= m_rcWindow.TopLeft();
			bRet = m_rgnWnd->PtInRegion(ptTmp);
		}
		return bRet;
	}

	void SWindow::SetWindowRgn(IRegion *pRgn,BOOL bRedraw/*=TRUE*/)
	{
		m_rgnWnd = NULL;
		if(pRgn)
		{
			GETRENDERFACTORY->CreateRegion(&m_rgnWnd);
			m_rgnWnd->CombineRgn(pRgn,RGN_COPY);
		}
		if(bRedraw) InvalidateRect(NULL);
	}

	BOOL SWindow::GetWindowRgn(IRegion *pRgn)
	{
		SASSERT(pRgn);
		if(!m_rgnWnd) return FALSE;
		pRgn->CombineRgn(m_rgnWnd,RGN_COPY);
		return TRUE;
	}

	void SWindow::DoColorize(COLORREF cr)
	{
		SDispatchMessage(UM_SETCOLORIZE,cr,0);
	}


	LRESULT SWindow::OnSetColorize(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		OnColorize((COLORREF)wParam);
		return 0;
	}

	void SWindow::OnColorize(COLORREF cr)
	{
		if(m_crColorize == cr) return;
		m_crColorize = cr;
		if(m_pNcSkin) m_pNcSkin->OnColorize(cr);
		if(m_pBgSkin) m_pBgSkin->OnColorize(cr);
		InvalidateRect(NULL);
	}

	COLORREF SWindow::GetColorizeColor() const
	{
		return m_crColorize;
	}


	HRESULT SWindow::OnAttrLayout(const SStringW& strValue, BOOL bLoading)
	{
		ILayout *pLayout = (ILayout*)SApplication::getSingleton().CreateObject(SObjectInfo(strValue, Layout));
		if (pLayout == NULL)
			return E_INVALIDARG;

		m_pLayout = pLayout;
		pLayout->Release();
		return S_OK;
	}

	HRESULT SWindow::AfterAttribute(const SStringW & strAttribName,const SStringW & strValue, BOOL bLoading,HRESULT hr)
	{
		if(!m_attrStorage)
		{
			IAttrStorageFactory * pFactory = SApplication::getSingleton().GetAttrStorageFactory();
			if(pFactory) pFactory->CreateAttrStorage(this,strAttribName,strValue,&m_attrStorage);
		}
		if(m_attrStorage)
		{
			m_attrStorage->OnSetAttribute(strAttribName,strValue);
		}
		if((hr&0x0000ffff) == S_OK && !bLoading)
		{
			HRESULT hFlag = hr & 0xFFFF0000;
			if((hFlag & HRET_FLAG_LAYOUT_PARAM) || (hFlag & HRET_FLAG_LAYOUT))
			{//修改了窗口的布局属性,请求父窗口重新布局
				if(GetParent())
				{
					GetParent()->RequestRelayout();
				}else
				{
					InvalidateRect(NULL);
				}
			}
			else if((hr & 0x0000ffff) == S_OK)
			{
				InvalidateRect(NULL);
			}
		}
		return hr;
	}

	SStringW SWindow::GetAttribute(const SStringW & strAttr) const
	{
		if(m_attrStorage)
		{
			return m_attrStorage->OnGetAttribute(strAttr);
		}else
		{
			return __super::GetAttribute(strAttr);
		}
	}

	HRESULT SWindow::OnLanguageChanged()
	{
		m_strText.TranslateText();
		m_strToolTipText.TranslateText();
		return GetLayoutParam()->IsWrapContent(Any)?S_OK:S_FALSE;
	}

	SStringT SWindow::GetToolTipText()
	{
		return m_strToolTipText.GetText(FALSE);
	}

	void SWindow::SetToolTipText(LPCTSTR pszText)
	{
		m_strToolTipText.SetText(pszText);
		if(GetContainer()->GetHover() == m_swnd)
		{//请求更新显示的tip
			GetContainer()->UpdateTooltip();
		}
	}

	const SStringW & SWindow::GetTrCtx() const
	{
		if (m_strTrCtx.IsEmpty())
			return GetContainer()->GetTranslatorContext();
		else
			return m_strTrCtx;
	}

	int SWindow::GetScale() const
	{
		return GetContainer()?GetContainer()->GetScale():100;
	}


	void SWindow::OnScaleChanged(int scale)
	{
		m_style.SetScale(scale);
		GetScaleSkin(m_pNcSkin,scale);
		GetScaleSkin(m_pBgSkin,scale);

		//标记布局脏
		m_layoutDirty = dirty_self;
	}

	void SWindow::GetScaleSkin(ISkinObj * &pSkin,int nScale)
	{
		if(!pSkin) return;
		SStringW strName=pSkin->GetName();
		if(!strName.IsEmpty())
		{
			ISkinObj * pNewSkin =   GETSKIN(strName,nScale);
			if(pNewSkin) pSkin = pNewSkin;
		}
	}

	IAccessible * SWindow::GetAccessible()
	{
#ifdef SOUI_ENABLE_ACC
		if (!m_pAcc) m_pAcc.Attach(SApplication::getSingleton().CreateAccessible(this));
		return m_pAcc;
#else
		return NULL;
#endif
	}

	IAccProxy * SWindow::GetAccProxy() {
#ifdef SOUI_ENABLE_ACC
		if (!m_pAccProxy)
		{
			m_pAccProxy.Attach(SApplication::getSingleton().CreateAccProxy(this));
		}
		return m_pAccProxy;
#else
		return NULL;
#endif
	}

	void SWindow::accNotifyEvent(DWORD dwEvt)
	{
#ifdef SOUI_ENABLE_ACC
		NotifyWinEvent(dwEvt, GetContainer()->GetHostHwnd(), GetSwnd(), CHILDID_SELF);
#endif
	}

	bool SWindow::SetLayoutParam(ILayoutParam * pLayoutParam)
	{
		SWindow *pParent = GetParent();
		if (!pParent->GetLayout()->IsParamAcceptable(pLayoutParam))
			return false;
		m_pLayoutParam = pLayoutParam;
		return true;
	}

}//namespace SOUI

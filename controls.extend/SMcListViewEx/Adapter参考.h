#pragma once

#include "SMCListViewEx.h"
#include "SLibZplay.h"

struct musicItem
{
	SStringT songname;
	SStringT artist;
	SStringT album;
	SStringT lasttime;
	SStringT songpath;
};

class SMusicListAdapter :public SMcAdapterBaseEx
{
	SArray<SStringT> m_colNames;
	SArray<musicItem> m_musicList;
	SArray<musicItem> m_musicListBackup;
	int m_id;
	int m_curren_play;
	HWND m_nofitywnd;
public:
	SMusicListAdapter(int id,HWND nofitywnd):m_id(id),m_nofitywnd(nofitywnd), m_curren_play(0)
	{
		
// 		musicItem abc = { _T("天向霜晨吉百"),_T("天天") ,_T("了了 了") ,_T("三百年前") };
// 		for (int i = 0;i < 10;i++)
// 		{
// 			abc.songname.Format(_T("第%d个卖炭的"), i);
// 			m_musicList.Add(abc);
// 		}
	}

	virtual int getCount()
	{
		SAutoLock autolock(updatalock);
		return m_musicList.GetCount();
	}

	virtual void getView(int position, SWindow * pItem, pugi::xml_node xmlTemplate)
	{
		SAutoLock autolock(updatalock);
		if (pItem->GetChildrenCount() == 0)
		{
			pItem->InitFromXml(xmlTemplate);
		}
		pItem->FindChildByName(_T("songname"))->SetWindowText(m_musicList[position].songname);
		pItem->FindChildByName(_T("artist"))->SetWindowText(m_musicList[position].artist);
		pItem->FindChildByName(_T("album"))->SetWindowText(m_musicList[position].album);
		if (IsSel(position))
		{
			pItem->ModifyState(WndState_Check, 0);
		}
		else
		{
			pItem->ModifyState(0, WndState_Check);
		}
		if ((pItem->GetState() & WndState_Check) || (pItem->GetState() & WndState_Hover))
		{
			SWindow* pChildItem = pItem->FindChildByName(_T("playbtn"));
			if (pChildItem)
			{
				pChildItem->SetVisible(TRUE, TRUE);
			}
		}
		pItem->GetEventSet()->subscribeEvent(EventSwndStateChanged::EventID, Subscriber(&SMusicListAdapter::OnItemStateChanged, this));	
		pItem->FindChildByName(L"btn_play")->GetEventSet()->subscribeEvent(EVT_CMD, Subscriber(&SMusicListAdapter::btn_play, this));
		pItem->FindChildByName(L"btn_play")->SetUserData(position);
	}
	
	bool btn_play(EventArgs *pEvt)
	{
		SAutoLock autolock(updatalock);
		SWindow *btn = sobj_cast<SWindow>(pEvt->sender);		
		m_curren_play = btn->GetUserData();
		return SLibZplay::getSingleton().Play(m_musicList[m_curren_play].songpath);		
	}
	bool play()
	{
		SAutoLock autolock(updatalock);	
		return SLibZplay::getSingleton().Play(m_musicList[m_curren_play].songpath);		
	}
	void play_next()
	{
		SAutoLock autolock(updatalock);
		if ((++m_curren_play < m_musicList.GetCount())&& m_curren_play>=0)
		{
			SLibZplay::getSingleton().Play(m_musicList[m_curren_play].songpath);
		}
		else
		{
			m_curren_play = 0;
			SLibZplay::getSingleton().Play(m_musicList[m_curren_play].songpath);
		}
	}
	void play_prev()
	{
		SAutoLock autolock(updatalock);
		if ((--m_curren_play >=0)&&m_curren_play<m_musicList.GetCount())
		{
			SLibZplay::getSingleton().Play(m_musicList[m_curren_play].songpath);
		}
		else
		{
			m_curren_play = 0;
			SLibZplay::getSingleton().Play(m_musicList[m_curren_play].songpath);
		}
	}
	ISlotFunctor *ctxFun;
	void SetCtxMenuFun(const ISlotFunctor &pFun)
	{
		if (ctxFun)
			ctxFun=NULL;
		ctxFun = pFun.Clone();
	}
	bool OnItemCtxMenu(EventArgs *pEvt)
	{
		if (ctxFun)
		{
			return (*ctxFun)(pEvt);
		}
		return false;
	}
	bool OnItemStateChanged(EventArgs *pEvt)
	{
		SWindow *pItem = sobj_cast<SWindow>(pEvt->sender);
		if ((pItem->GetState() & WndState_Check)|| (pItem->GetState() & WndState_Hover))
		{
			pItem = pItem->FindChildByName(_T("playbtn"));
			if (pItem)
			{
				pItem->SetVisible(TRUE, TRUE);
			}
		}
		else
		{
			pItem = pItem->FindChildByName(_T("playbtn"));
			if (pItem)
			{
				pItem->SetVisible(FALSE, TRUE);
			}
		}
		return true;
	}

	int GetColCount()
	{
		return m_colNames.GetCount();
	}

	virtual bool SwapItem(long pos, SList<long> &sellist) override
	{
		SAutoLock autolock(updatalock);
		SArray<musicItem> _temp_items;
		SArray<long> _remove_idxs;
		SPOSITION _pos = sellist.GetHeadPosition();
		size_t offsetpos = pos;
		while (_pos)
		{
			long posoffset=0;
			long idx = sellist.GetNext(_pos);
			for (size_t i=0;i<_remove_idxs.GetCount();++i)
			{
				if (_remove_idxs[i] < idx)
					--posoffset;
			}
			_remove_idxs.Add(idx);
			if (idx < pos)
				--offsetpos;
			idx += posoffset;
			_temp_items.Add(m_musicList[idx]);
			m_musicList.RemoveAt(idx);
		}
		m_musicList.InsertArrayAt(offsetpos, &_temp_items);
		//更新选中索引	
		_pos = sellist.GetHeadPosition();
		while (_pos)
		{
			sellist.SetAt(_pos, offsetpos++);
			sellist.GetNext(_pos);
		}		
		return true;
	}
	/*
	bool OnPanelMouseHover(EventArgs *pEvt)
	{
		SWindow *pItem = sobj_cast<SWindow>(pEvt->sender);
		pItem=pItem->FindChildByName(_T("playbtn"));
		if (pItem)
		{
			pItem->SetVisible(TRUE, TRUE);
		}
		return true;
	}

	bool OnPanelMouseLeave(EventArgs *pEvt)
	{
		SWindow *pItem = sobj_cast<SWindow>(pEvt->sender);
		pItem = pItem->FindChildByName(_T("playbtn"));
		if (pItem)
		{
			pItem->SetVisible(FALSE, TRUE);
		}
		return true;
	}*/

	SStringW GetColumnName(int iCol) const {
		return m_colNames[iCol];
	}

	void IniColNames(pugi::xml_node xmlTemplate)
	{
		for (xmlTemplate = xmlTemplate.first_child(); xmlTemplate; xmlTemplate = xmlTemplate.next_sibling())
		{
			m_colNames.Add(xmlTemplate.attribute(L"name").value());
		}
	}

	virtual bool OnSort(int iCol, SHDSORTFLAG * stFlags, int nCols) 
	{
		return false;
	}

	virtual void InitByTemplate(pugi::xml_node xmlTemplate)
	{
		IniColNames(xmlTemplate);
	}
	//同步添加方法
	void add(SStringT &filepath)
	{
		SAutoLock lock(updatalock);
		musicItem item;
		item.songname=item.songpath = filepath;
		libZPlay::TID3InfoExW id3_inf;
		if (SLibZplay::getSingleton().GetFileId3(filepath, id3_inf))
		{
			item.songname = id3_inf.Title;
		}
		m_musicList.Add(item);
		notifyDataSetChanged();
	}
	//异步添加，必须到主线程notifyDataSetChanged
	void addformthread(SStringT &filepath)
	{
		musicItem item;
		item.songpath = filepath;
		libZPlay::TID3InfoExW id3_inf;
		if (SLibZplay::getSingleton().GetFileId3(filepath, id3_inf))
		{
			item.songname = id3_inf.Title;
			item.artist = id3_inf.Artist;
			item.album = id3_inf.Album;
			updatalock.Enter();
			m_musicList.Add(item);
			updatalock.Leave();
		}		
		//此处不应该postmessage，因为getview里的异步锁会可能让界面假死,因为这个消息可能会很多，加载时会使WM_TIMER无法执行，所以动画可能会失效。
		SendMessage(m_nofitywnd, UPDATAMSG, m_id, NULL);
	}
	private:
	SCriticalSection updatalock;
};

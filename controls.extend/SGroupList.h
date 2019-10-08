#pragma once

namespace SOUI{
	namespace GroupListStuct{
		struct ItemInfo
		{
			SStringT strText;
			int      iIcon;
			SStringW strName;
			int      id;
		};

		struct GroupInfo : ItemInfo
		{
			bool     bCollapsed;
		};
	}

	SEVENT_BEGIN(EventGroupListInitGroup, EVT_EXTERNAL_BEGIN + 3000)
		SWindow *pItem;
	    GroupListStuct::GroupInfo *pGroupInfo;
	SEVENT_END()

	SEVENT_BEGIN(EventGroupListInitItem, EVT_EXTERNAL_BEGIN + 3001)
		SWindow *pItem;
		GroupListStuct::ItemInfo *pItemInfo;
	SEVENT_END()

	SEVENT_BEGIN(EventGroupListItemCheck, EVT_EXTERNAL_BEGIN + 3002)
		SWindow *pItem;
		GroupListStuct::ItemInfo *pItemInfo;
	SEVENT_END()

	SEVENT_BEGIN(EventGroupStateChanged, EVT_EXTERNAL_BEGIN + 3003)
		SWindow *pItem;
		GroupListStuct::GroupInfo *pGroupInfo;
	SEVENT_END()

	class SGroupList : public SScrollView
	{
		SOUI_CLASS_NAME(SGroupList,L"groupList")
	public:
		SGroupList();
		~SGroupList();

		bool SelectPage(int nID);
		SWindow * InsertGroup(int iGroup,const SStringW & name,const SStringT & text, int iIcon, int nID);
		SWindow * InsertItem(SWindow *pGroup,int iItem, const SStringW & name, const SStringT & text, int iIcon, int nID);
	protected:
		bool OnGroupClick(EventArgs *e);
		bool OnItemClick(EventArgs *e);

		virtual BOOL CreateChildren(pugi::xml_node xmlNode);

	protected:
		void OnDestroy();
		SOUI_MSG_MAP_BEGIN()
			MSG_WM_DESTROY(OnDestroy)
		SOUI_MSG_MAP_END()

	protected:
		pugi::xml_document m_xmlDoc;
		pugi::xml_node     m_groupTemplate;
		pugi::xml_node	   m_itemTemplate;

		SWindow *		   m_pSelectedItem;
	};
}


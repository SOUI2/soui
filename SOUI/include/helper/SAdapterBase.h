#pragma once

#include "interface/Adapter-i.h"
#include <souicoll.h>
#include "helper/copylist.hpp"
#include "control/stree.hpp"

namespace SOUI
{
    class SLvObserverMgr
    {
    public:
        SLvObserverMgr(){}
        ~SLvObserverMgr(){
            SPOSITION pos = m_lstObserver.GetHeadPosition();
            while(pos)
            {
                ILvDataSetObserver *pObserver = m_lstObserver.GetNext(pos);
                pObserver->Release();
            }
            m_lstObserver.RemoveAll();
        }

        void registerObserver(ILvDataSetObserver * observer) {
            SASSERT(observer);
            if(m_lstObserver.Find(observer)) return;
            m_lstObserver.AddTail(observer);
            observer->AddRef();
        }

        /**
        * Removes a previously registered observer. The observer must not be null and it
        * must already have been registered.
        * @param observer the observer to unregister
        * @throws IllegalArgumentException the observer is null
        * @throws IllegalStateException the observer is not yet registered
        */
        void unregisterObserver(ILvDataSetObserver * observer) {
            SASSERT(observer);
            SPOSITION pos = m_lstObserver.Find(observer);
            if(!pos) return;
            m_lstObserver.RemoveAt(pos);
            observer->Release();
        }
        
        void notifyChanged()
        {
            SPOSITION pos = m_lstObserver.GetHeadPosition();
            while(pos)
            {
                ILvDataSetObserver *pObserver = m_lstObserver.GetNext(pos);
                pObserver->onChanged();
            }
        }
        
        void notifyInvalidated()
        {
            SPOSITION pos = m_lstObserver.GetHeadPosition();
            while(pos)
            {
                ILvDataSetObserver *pObserver = m_lstObserver.GetNext(pos);
                pObserver->onInvalidated();
            }
        }

		void notifyItemChanged(int iItem)
		{
			SPOSITION pos = m_lstObserver.GetHeadPosition();
			while(pos)
			{
				ILvDataSetObserver *pObserver = m_lstObserver.GetNext(pos);
				pObserver->OnItemChanged(iItem);
			}
		}

    protected:
        SList<ILvDataSetObserver *> m_lstObserver;
    };
    
    template<class BaseClass>
    class LvAdatperImpl : public BaseClass
    {
    public:
        void notifyDataSetChanged() {
            m_obzMgr.notifyChanged();
        }

        /**
        * Notifies the attached observers that the underlying data is no longer valid
        * or available. Once invoked this adapter is no longer valid and should
        * not report further data set changes.
        */
        void notifyDataSetInvalidated() {
            m_obzMgr.notifyInvalidated();
        }

		void notifyItemDataChanged(int iItem){
			m_obzMgr.notifyItemChanged(iItem);
		}

        virtual void registerDataSetObserver(ILvDataSetObserver * observer)
        {
            m_obzMgr.registerObserver(observer);
        }

        virtual void unregisterDataSetObserver(ILvDataSetObserver * observer)
        {
            m_obzMgr.unregisterObserver(observer);
        }

        virtual int getCount() PURE;   

        virtual long getItemId(int position)
        {
            return position;
        }

        virtual int getItemViewType(int position)
        {
			(position);
            return 0;
        }
        
        //增加这个函数来自动转换不需要处理状态变化的View
        virtual int getItemViewType(int position,DWORD dwState)
        {
			(dwState);
            return getItemViewType(position);
        }

        virtual int getViewTypeCount()
        {
            return 1;
        }

        virtual bool isEmpty()
        {
            return getCount()>0;
        }

        virtual ULONG_PTR getItemData(int position){
			(position);
            return 0;
        }
        
        virtual SStringT getItemDesc(int position){
			(position);
            return SStringT();
        }
        
        virtual void InitByTemplate(pugi::xml_node xmlTemplate)
        {
			(xmlTemplate);
        }
        
        virtual SIZE getViewDesiredSize(int position,SWindow *pItem, LPCRECT prcContainer)
        {
			(position);
			(pItem);
            return pItem->GetDesiredSize(prcContainer);
        }
    protected:
        SLvObserverMgr    m_obzMgr;
    };
    
    class SAdapterBase : public TObjRefImpl<LvAdatperImpl<ILvAdapter>>
    {
    public:
        SAdapterBase()
        {
        }

        ~SAdapterBase()
        {
        }
    };
    
    class SMcAdapterBase : public TObjRefImpl<LvAdatperImpl<IMcAdapter>>
    {
    public:
        SMcAdapterBase()
        {
        }

        ~SMcAdapterBase()
        {
        }
        
        virtual bool OnSort(int iCol,SHDSORTFLAG * stFlags,int nCols){
			(iCol);
			(stFlags);
			(nCols);
			return false;
		}
    };


    class STvObserverMgr
    {
    public:
        STvObserverMgr(){}
        ~STvObserverMgr(){
            SPOSITION pos = m_lstObserver.GetHeadPosition();
            while(pos)
            {
                ITvDataSetObserver *pObserver = m_lstObserver.GetNext(pos);
                pObserver->Release();
            }
            m_lstObserver.RemoveAll();
        }

        void registerObserver(ITvDataSetObserver * observer) {
            SASSERT(observer);
            if(m_lstObserver.Find(observer)) return;
            m_lstObserver.AddTail(observer);
            observer->AddRef();
        }

        /**
        * Removes a previously registered observer. The observer must not be null and it
        * must already have been registered.
        * @param observer the observer to unregister
        * @throws IllegalArgumentException the observer is null
        * @throws IllegalStateException the observer is not yet registered
        */
        void unregisterObserver(ITvDataSetObserver * observer) {
            SASSERT(observer);
            SPOSITION pos = m_lstObserver.Find(observer);
            if(!pos) return;
            m_lstObserver.RemoveAt(pos);
            observer->Release();
        }
        
        void notifyChanged(HTREEITEM hBranch)
        {
            SPOSITION pos = m_lstObserver.GetHeadPosition();
            while(pos)
            {
                ITvDataSetObserver *pObserver = m_lstObserver.GetNext(pos);
                pObserver->onBranchChanged(hBranch);
            }
        }
        
        void notifyInvalidated(HTREEITEM hBranch)
        {
            SPOSITION pos = m_lstObserver.GetHeadPosition();
            while(pos)
            {
                ITvDataSetObserver *pObserver = m_lstObserver.GetNext(pos);
                pObserver->onBranchInvalidated(hBranch);
            }
        }
        
        void notifyExpandChanged(HTREEITEM hBranch,BOOL bExpandedOld,BOOL bExpandedNew)
        {
            SPOSITION pos = m_lstObserver.GetHeadPosition();
            while(pos)
            {
                ITvDataSetObserver *pObserver = m_lstObserver.GetNext(pos);
                pObserver->onBranchExpandedChanged(hBranch,bExpandedOld,bExpandedNew);
            }
        }
    protected:
        SList<ITvDataSetObserver *> m_lstObserver;
    };


    template<class BaseClass>
    class TvAdatperImpl : public BaseClass
    {
    public:
        void notifyBranchChanged(HTREEITEM hBranch) {
            m_obzMgr.notifyChanged(hBranch);
        }

        /**
        * Notifies the attached observers that the underlying data is no longer valid
        * or available. Once invoked this adapter is no longer valid and should
        * not report further data set changes.
        */
        void notifyBranchInvalidated(HTREEITEM hBranch) {
            m_obzMgr.notifyInvalidated(hBranch);
        }

        void notifyBranchExpandChanged(HTREEITEM hBranch,BOOL bExpandedOld,BOOL bExpandedNew) {
            m_obzMgr.notifyExpandChanged(hBranch,bExpandedOld,bExpandedNew);
        }

        virtual void registerDataSetObserver(ITvDataSetObserver * observer)
        {
            m_obzMgr.registerObserver(observer);
        }

        virtual void unregisterDataSetObserver(ITvDataSetObserver * observer)
        {
            m_obzMgr.unregisterObserver(observer);
        }

        virtual void InitByTemplate(pugi::xml_node xmlTemplate)
        {
			(xmlTemplate);
        }

    protected:
        STvObserverMgr    m_obzMgr;
    };
    
    template<typename T>
	class STreeAdapterBase: public TObjRefImpl<TvAdatperImpl<ITvAdapter>>
	{
	public:
		STreeAdapterBase() {
			memset(m_rootUserData, 0, sizeof(m_rootUserData));
		}
		~STreeAdapterBase(){}
		
		struct ItemInfo
		{
		ULONG_PTR userData[DATA_INDEX_NUMBER];
		T         data;
		};
		
		//获取hItem中的指定索引的数据
        virtual ULONG_PTR GetItemDataByIndex(HTREEITEM hItem,DATA_INDEX idx) const
        {
            if(hItem == ITvAdapter::ITEM_ROOT)
                return m_rootUserData[idx];
            ItemInfo & ii = m_tree.GetItemRef((HSTREEITEM)hItem);
            return ii.userData[idx];
        }
        
        //保存hItem指定索引的数据
        virtual void SetItemDataByIndex(HTREEITEM hItem,DATA_INDEX idx,ULONG_PTR data)
        {
            if(hItem == ITvAdapter::ITEM_ROOT)
                m_rootUserData[idx] = data;
            else
            {
                ItemInfo & ii = m_tree.GetItemRef((HSTREEITEM)hItem);
                ii.userData[idx] = data;
            }
        }
        
        virtual HTREEITEM GetParentItem(HTREEITEM hItem) const
        {
            if(hItem == ITEM_ROOT) return ITvAdapter::ITEM_NULL;
            HSTREEITEM hParent = m_tree.GetParentItem((HSTREEITEM)hItem);
            if(hParent == NULL) hParent = ITvAdapter::ITEM_ROOT;
            return (HTREEITEM)hParent;
        }
        
        virtual BOOL HasChildren(HTREEITEM hItem) const
        {
            return GetFirstChildItem(hItem)!=ITEM_NULL;
        }
        
        virtual HTREEITEM GetFirstChildItem(HTREEITEM hItem) const
        {
            SASSERT(hItem != ITvAdapter::ITEM_NULL);
            return (HTREEITEM)m_tree.GetChildItem((HSTREEITEM)hItem,TRUE);    
        }
        
        virtual HTREEITEM GetLastChildItem(HTREEITEM hItem) const 
        {
            SASSERT(hItem != ITvAdapter::ITEM_NULL);
            return (HTREEITEM)m_tree.GetChildItem((HSTREEITEM)hItem,FALSE);    
        }
        
        virtual HTREEITEM GetPrevSiblingItem(HTREEITEM hItem) const 
        {
            SASSERT(hItem != ITvAdapter::ITEM_NULL && hItem != ITvAdapter::ITEM_ROOT);
            return (HTREEITEM)m_tree.GetPrevSiblingItem((HSTREEITEM)hItem);
        }
        
        virtual HTREEITEM GetNextSiblingItem(HTREEITEM hItem) const
        {
            SASSERT(hItem != ITvAdapter::ITEM_NULL && hItem != ITvAdapter::ITEM_ROOT);
            return (HTREEITEM)m_tree.GetNextSiblingItem((HSTREEITEM)hItem);
        }
        
        virtual int getViewType(HTREEITEM hItem) const
        {
            return 0;
        }
        
		virtual int getViewTypeCount() const
		{
		    return 1;
		}
		
		virtual SIZE getViewDesiredSize(HTREEITEM hItem,SWindow * pItem, LPCRECT prcContainer)
		{
		    return pItem->GetDesiredSize(prcContainer);
		}
        
        virtual bool isViewWidthMatchParent() const
        {
            return false;
        }
        
        virtual void ExpandItem(HTREEITEM hItem,UINT uCode)
        {
            BOOL bExpandedOld = IsItemExpanded(hItem);
            BOOL bExpandedNew = bExpandedOld;
            switch(uCode)
            {
                case TVC_COLLAPSE: bExpandedNew = FALSE;break;
                case TVC_EXPAND: bExpandedNew = TRUE;break;
                case TVC_TOGGLE: bExpandedNew = !bExpandedOld;break;
            }
            if(bExpandedOld == bExpandedNew)
                return ;
            
            SetItemExpanded(hItem,bExpandedNew);
            notifyBranchExpandChanged(hItem,bExpandedOld,bExpandedNew);
        }

        virtual BOOL IsItemExpanded(HTREEITEM hItem) const
        {
			if (hItem == ITEM_ROOT) return TRUE;//虚拟根节点自动展开
            return (BOOL)GetItemDataByIndex(hItem,DATA_INDEX_ITEM_EXPANDED);
        }
        
        virtual void SetItemExpanded(HTREEITEM hItem,BOOL bExpanded)
        {
            SetItemDataByIndex(hItem,DATA_INDEX_ITEM_EXPANDED,bExpanded);
        }
        
        virtual BOOL IsItemVisible(HTREEITEM hItem) const
        {
            HTREEITEM hParent = GetParentItem(hItem);
            while(hParent != ITEM_NULL)
            {
                if(!IsItemExpanded(hParent)) return FALSE;
                hParent = GetParentItem(hParent);
            }
            return TRUE;
        }

        virtual HTREEITEM GetFirstVisibleItem() const
        {
            return GetFirstChildItem(ITEM_ROOT);
        }
        
        virtual HTREEITEM GetLastVisibleItem() const
        {
            HTREEITEM hItem = GetLastChildItem(ITEM_ROOT);
            if(hItem == ITEM_NULL) return hItem;
            for(;IsItemExpanded(hItem);)
            {
                HTREEITEM hChild = GetLastChildItem(hItem);
                if(hChild == ITEM_NULL) break;
                hItem = hChild;
            }
            return hItem;
        }

        virtual HTREEITEM GetPrevVisibleItem(HTREEITEM hItem) const
        {
            SASSERT(IsItemVisible(hItem));
            HSTREEITEM hRet = GetPrevSiblingItem(hItem);
            if(hRet == ITEM_NULL)
            {
                hRet = GetParentItem(hItem);
                if(hRet == ITEM_ROOT) hRet = ITEM_NULL;
            }
            return hRet;
        }

        virtual HTREEITEM GetNextVisibleItem(HTREEITEM hItem) const
        {
            SASSERT(IsItemVisible(hItem));
            if(IsItemExpanded(hItem))
            {
                HTREEITEM hChild = GetFirstChildItem(hItem);
                if(hChild!=ITEM_NULL) return hChild;
            }

            HTREEITEM hParent = hItem;
            while(hParent!=ITEM_NULL && hParent!=ITEM_ROOT)
            {
                HTREEITEM hRet=GetNextSiblingItem(hParent);
                if(hRet) return hRet;
                hParent=GetParentItem(hParent);
            }
            return ITEM_NULL;
        }

    public:
        HSTREEITEM InsertItem(const T & data,HSTREEITEM hParent = STVI_ROOT,HSTREEITEM hInsertAfter = STVI_LAST)
        {
            ItemInfo ii={0};
            ii.data = data;
            return m_tree.InsertItem(ii,hParent,hInsertAfter);
        }


		void DeleteItem(HTREEITEM hItem)
		{
			m_tree.DeleteItem(hItem);
		}

		BOOL DeleteItemEx(HTREEITEM hItem)
		{
			return m_tree.DeleteItemEx(hItem);
		}

    protected:
		CSTree<ItemInfo> m_tree;
		ULONG_PTR        m_rootUserData[DATA_INDEX_NUMBER];
	};
}

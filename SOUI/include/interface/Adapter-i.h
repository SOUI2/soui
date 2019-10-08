#pragma once

#include <unknown/obj-ref-i.h>
#include <string/tstring.h>


namespace SOUI
{
    interface  ILvDataSetObserver : public IObjRef{
        /**
        * This method is called when the entire data set has changed,
        * most likely through a call to {@link Cursor#requery()} on a {@link Cursor}.
        */
        virtual void onChanged()  PURE;

        /**
        * This method is called when the entire data becomes invalid,
        * most likely through a call to {@link Cursor#deactivate()} or {@link Cursor#close()} on a
        * {@link Cursor}.
        */
        virtual void onInvalidated()  PURE;

		virtual void OnItemChanged(int iItem) PURE;
    };
    
    interface ILvAdapter : public IObjRef{

        /**
        * Register an observer that is called when changes happen to the data used by this adapter.
        *
        * @param observer the object that gets notified when the data set changes.
        */
        virtual void registerDataSetObserver(ILvDataSetObserver * observer) PURE;

        /**
        * Unregister an observer that has previously been registered with this
        * adapter via {@link #registerDataSetObserver}.
        *
        * @param observer the object to unregister.
        */
        virtual void unregisterDataSetObserver(ILvDataSetObserver * observer) PURE;


        /**
        * How many items are in the data set represented by this Adapter.
        * 
        * @return Count of items.
        */
        virtual int getCount() PURE;   


        /**
        * Get a View that displays the data at the specified position in the data set. You can either
        * create a View manually or inflate it from an XML layout file. When the View is inflated, the
        * parent View (GridView, ListView...) will apply default layout parameters unless you use
        * {@link android.view.LayoutInflater#inflate(int, android.view.ViewGroup, boolean)}
        * to specify a root view and to prevent attachment to the root.
        * 
        * @param position The position of the item within the adapter's data set of the item whose view
        *        we want.
        * @param pItem The old view to reuse, if possible. Note: You should check that this view
        *        is non-null and of an appropriate type before using. If it is not possible to convert
        *        this view to display the correct data, this method can create a new view.
        *        Heterogeneous lists can specify their number of view types, so that this View is
        *        always of the right type (see {@link #getViewTypeCount()} and
        *        {@link #getItemViewType(int,DWORD)}).
        * @param xmlTemplate the xml template provided by its owner
        */
        virtual void getView(int position, SWindow * pItem, pugi::xml_node xmlTemplate) PURE;


        /**
        * Get the type of View that will be created by {@link #getView} for the specified item.
        * 
        * @param position The position of the item within the adapter's data set whose view type we
        *        want.
        * @param dwState the state of view.
        * @return An integer representing the type of View. Two views should share the same type if one
        *         can be converted to the other in {@link #getView}. Note: Integers must be in the
        *         range 0 to {@link #getViewTypeCount} - 1. {@link #IGNORE_ITEM_VIEW_TYPE} can
        *         also be returned.
        * @see #IGNORE_ITEM_VIEW_TYPE
        */
        virtual int getItemViewType(int position,DWORD dwState) PURE;

        /**
        * <p>
        * Returns the number of types of Views that will be created by
        * {@link #getView}. Each type represents a set of views that can be
        * converted in {@link #getView}. If the adapter always returns the same
        * type of View for all items, this method should return 1.
        * </p>
        * <p>
        * This method will only be called when when the adapter is set on the
        * the {@link AdapterView}.
        * </p>
        * 
        * @return The number of types of Views that will be created by this adapter
        */
        virtual int getViewTypeCount() PURE;

        virtual SIZE getViewDesiredSize(int position,SWindow *pItem, LPCRECT prcContainer) PURE;

        /**
        * @return true if this adapter doesn't contain any data.  This is used to determine
        * whether the empty view should be displayed.  A typical implementation will return
        * getCount() == 0 but since getCount() includes the headers and footers, specialized
        * adapters might want a different behavior.
        */
        virtual bool isEmpty() PURE;
        
        /**
        * @return a custom defined data that associate with the specified item.
        */
        virtual ULONG_PTR getItemData(int position) PURE;
        
        /**
        * @return a string object that associate with the specified item.
        */
        virtual SStringT getItemDesc(int positoin) PURE;
        
        /**
        * init adapter from the specified template xml data
        */
        virtual void InitByTemplate(pugi::xml_node xmlTemplate) PURE;
        
    };
 
    enum SHDSORTFLAG;
    interface IMcAdapter : public ILvAdapter
    {
        //获取一个列在模板中对应的窗口名称
		//int iCol: 列序号
        virtual SStringW GetColumnName(int iCol) const PURE;

		//由Adapter决定一列是否需要显示
		//int iCol: 列序号
		//remark:默认显示
		virtual bool IsColumnVisible(int iCol) const
		{
			return true;
		}

        //排序接口
        // int iCol:排序列
        // SHDSORTFLAG * stFlags [in, out]:当前列排序标志
        // int nCols:总列数,stFlags数组长度
        virtual bool OnSort(int iCol,SHDSORTFLAG * stFlags,int nCols) PURE;
    };

    typedef ULONG_PTR HTREEITEM;

    interface  ITvDataSetObserver : public IObjRef{
        /**
        * This method is called when the entire data set has changed,
        * most likely through a call to {@link Cursor#requery()} on a {@link Cursor}.
        */
        virtual void onBranchChanged(HTREEITEM hBranch)  PURE;

        /**
        * This method is called when the entire data becomes invalid,
        * most likely through a call to {@link Cursor#deactivate()} or {@link Cursor#close()} on a
        * {@link Cursor}.
        */
        virtual void onBranchInvalidated(HTREEITEM hBranch)  PURE;
        
        virtual void onBranchExpandedChanged(HTREEITEM hBranch,BOOL bExpandedOld,BOOL bExpandedNew) PURE;
    };
   
	interface ITvAdapter : public IObjRef{

        static const HTREEITEM ITEM_NULL=0;
        static const HTREEITEM ITEM_ROOT=0xFFFF0000;
        
        enum DATA_INDEX{
        DATA_INDEX_ITEM_HEIGHT=0,   //行高
        DATA_INDEX_ITEM_WIDTH,      //行宽度
        DATA_INDEX_BRANCH_HEIGHT,   //分枝高度
        DATA_INDEX_BRANCH_WIDTH,    //分枝宽度,不包含indent
        DATA_INDEX_ITEM_EXPANDED,   //子项展开状态
        DATA_INDEX_ITEM_OFFSET,     //当前节点在父节点中的Y方向偏移
        DATA_INDEX_ITEM_USER,       //自定义数据
        DATA_INDEX_NUMBER
        };    
    
        enum{
            TVC_COLLAPSE   =         0x0001,
            TVC_EXPAND     =         0x0002,
            TVC_TOGGLE     =         0x0003,
        };
        
        /**
        * Register an observer that is called when changes happen to the data used by this adapter.
        *
        * @param observer the object that gets notified when the data set changes.
        */
        virtual void registerDataSetObserver(ITvDataSetObserver * observer) PURE;

        /**
        * Unregister an observer that has previously been registered with this
        * adapter via {@link #registerDataSetObserver}.
        *
        * @param observer the object to unregister.
        */
        virtual void unregisterDataSetObserver(ITvDataSetObserver * observer) PURE;


        //获取hItem中的指定索引的数据
        virtual ULONG_PTR GetItemDataByIndex(HTREEITEM hItem,DATA_INDEX idx) const PURE;
        
        //保存hItem指定索引的数据
        virtual void SetItemDataByIndex(HTREEITEM hItem,DATA_INDEX idx,ULONG_PTR data) PURE;
        
        virtual HTREEITEM GetParentItem(HTREEITEM hItem) const PURE;
        
        virtual HTREEITEM GetFirstChildItem(HTREEITEM hItem) const PURE;
        virtual HTREEITEM GetLastChildItem(HTREEITEM hItem) const PURE;
        virtual HTREEITEM GetPrevSiblingItem(HTREEITEM hItem) const PURE;
        virtual HTREEITEM GetNextSiblingItem(HTREEITEM hItem) const PURE;
        virtual BOOL HasChildren(HTREEITEM hItem) const PURE;
        
        virtual BOOL IsItemVisible(HTREEITEM hItem) const PURE;
        
        virtual HTREEITEM GetFirstVisibleItem() const PURE;
        virtual HTREEITEM GetLastVisibleItem() const PURE;
        virtual HTREEITEM GetPrevVisibleItem(HTREEITEM hItem) const PURE;
        virtual HTREEITEM GetNextVisibleItem(HTREEITEM hItem) const PURE;
        
        virtual void ExpandItem(HTREEITEM hItem,UINT code) PURE;
        
        virtual BOOL IsItemExpanded(HTREEITEM hItem) const PURE;
        
        virtual void SetItemExpanded(HTREEITEM hItem,BOOL bExpanded) PURE;

       /**
        * Get a View that displays the data at the specified position in the data set. You can either
        * create a View manually or inflate it from an XML layout file. When the View is inflated, the
        * parent View (GridView, ListView...) will apply default layout parameters unless you use
        * {@link android.view.LayoutInflater#inflate(int, android.view.ViewGroup, boolean)}
        * to specify a root view and to prevent attachment to the root.
        * 
        * @param position The position of the item within the adapter's data set of the item whose view
        *        we want.
        * @param pItem The old view to reuse, if possible. Note: You should check that this view
        *        is non-null and of an appropriate type before using. If it is not possible to convert
        *        this view to display the correct data, this method can create a new view.
        *        Heterogeneous lists can specify their number of view types, so that this View is
        *        always of the right type (see {@link #getViewTypeCount()} and
        *        {@link #getItemViewType(int,DWORD)}).
        * @param xmlTemplate the xml template provided by its owner
        */
        virtual void getView( HTREEITEM hItem, SWindow * pContainer, pugi::xml_node xmlTemplate) PURE;
        
        virtual int getViewType(HTREEITEM hItem) const PURE;
        
        virtual int getViewTypeCount() const PURE;
        
        virtual SIZE getViewDesiredSize(HTREEITEM hItem,SWindow * pItem, LPCRECT prcContainer) PURE;
        
        //定义行宽度和treeview客户区宽度相同
        virtual bool isViewWidthMatchParent() const PURE;
        
        /**
        * init adapter from the specified template xml data
        */
        virtual void InitByTemplate(pugi::xml_node xmlTemplate) PURE;
    };
}
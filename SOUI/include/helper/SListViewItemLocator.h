#pragma once
#include "interface/LvItemLocator-i.h"
#include "control/stree.hpp"

namespace SOUI
{
    class SOUI_EXP SListViewItemLocatorFix : public TObjRefImpl<IListViewItemLocator>
    {
    public:
        SListViewItemLocatorFix(SLayoutSize nItemHei,SLayoutSize nDividerSize=SLayoutSize());

        virtual void SetAdapter(ILvAdapter *pAdapter);

        virtual void OnDataSetChanged(){}

        virtual bool IsFixHeight() const;

        virtual int GetItemHeight(int iItem) const ;

        virtual void SetItemHeight(int iItem,int nHeight);


        virtual int GetTotalHeight();
        virtual int Item2Position(int iItem);

        virtual int Position2Item(int position);

        virtual int GetScrollLineSize() const;

		virtual int GetDividerSize() const;

		virtual void SetScale(int nScale);
    protected:
		int GetFixItemHeight() const;

        SLayoutSize m_nItemHeight;
        SLayoutSize m_nDividerSize;
		int	m_nScale;
        CAutoRefPtr<ILvAdapter> m_adapter;
    };

    class SOUI_EXP SListViewItemLocatorFlex : public TObjRefImpl<IListViewItemLocator>
    {

    public:

        SListViewItemLocatorFlex(SLayoutSize nItemHei,SLayoutSize nDividerSize=SLayoutSize());
        ~SListViewItemLocatorFlex();


        virtual void SetAdapter(ILvAdapter *pAdapter);
        virtual void OnDataSetChanged();

        virtual bool IsFixHeight() const;

        virtual int GetItemHeight(int iItem) const;

        virtual void SetItemHeight(int iItem,int nHeight);


        virtual int GetTotalHeight();
        virtual int Item2Position(int iItem);

        virtual int Position2Item(int position);

        virtual int GetScrollLineSize() const;   

		virtual int GetDividerSize() const;

		virtual void SetScale(int nScale);
    protected:
        void InitIndex(HSTREEITEM hParent,int nItems,int nSubBranchSize);
		int GetFixItemHeight() const;
        int GetIndexDeep() const;
        void Clear();
        int Branch2Offset(HSTREEITEM hBranch) const;
        int Branch2Index(HSTREEITEM hBranch) const;
        HSTREEITEM Offset2Branch(HSTREEITEM hParent,int nOffset);

        SLayoutSize m_nItemHeight;  //默认表项高度
        SLayoutSize m_nDividerSize;
		int m_nScale;
        struct BranchInfo
        {
            int nBranchHei; //分枝高度
            int nBranchSize;//分枝中包含的节点数量
        };

        CSTree<BranchInfo>    m_itemPosIndex;//记录分枝高度
        class SegmentInfo
        {
        public:
            SegmentInfo(int nItems,HSTREEITEM hBranch):hItem(hBranch){
                this->nItems = nItems;
                pItemHeight = new int[nItems];
                memset(pItemHeight,0xff,nItems*sizeof(int));
            }
            ~SegmentInfo()
            {
                if(pItemHeight) delete[] pItemHeight;
            }

            HSTREEITEM hItem;
            int        nItems;
            int*       pItemHeight;//段中每一个表项的高度
        };

        SArray<SegmentInfo*>     m_segments;
        CAutoRefPtr<ILvAdapter>   m_adapter;
    };

}
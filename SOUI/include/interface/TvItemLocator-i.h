#pragma once

#include <unknown/obj-ref-i.h>
#include <unknown/obj-ref-impl.hpp>
#include "Adapter-i.h"

namespace SOUI
{
    interface ITreeViewItemLocator : public IObjRef
    {
        virtual void SetAdapter(ITvAdapter *pAdapter) PURE;
        virtual void OnBranchChanged(HTREEITEM hItem) PURE;
        virtual void OnBranchExpandedChanged(HTREEITEM hItem,BOOL bExpandedOld,BOOL bExpandedNew) PURE;

        virtual int GetTotalHeight() const PURE;
        virtual int GetTotalWidth() const PURE;
        virtual int Item2Position(HTREEITEM hItem) const PURE;
        virtual HTREEITEM Position2Item(int position) const PURE;
        virtual int GetScrollLineSize() const PURE;
        
        virtual void SetItemWidth(HTREEITEM hItem,int nWidth) PURE;
        virtual int GetItemWidth(HTREEITEM hItem) const PURE;
        virtual void SetItemHeight(HTREEITEM hItem,int nHeight) PURE;
        virtual int GetItemHeight(HTREEITEM hItem) const PURE;
        virtual int GetItemIndent(HTREEITEM hItem) const PURE;
    };
    
}
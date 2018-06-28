#pragma once

#include <unknown/obj-ref-i.h>
#include "Adapter-i.h"

namespace SOUI
{
    interface IListViewItemLocator : public IObjRef
    {
        virtual void SetAdapter(ILvAdapter *pAdapter) PURE;
        virtual void OnDataSetChanged() PURE;
        virtual bool IsFixHeight() const PURE;
        virtual int GetItemHeight(int iItem) const PURE;
        virtual void SetItemHeight(int iItem,int nHeight) PURE;
        virtual int GetTotalHeight() PURE;
        virtual int Item2Position(int iItem) PURE;
        virtual int Position2Item(int position) PURE;
        virtual int GetScrollLineSize() const PURE;
        virtual int GetDividerSize() const PURE;
		virtual void SetScale(int nScale) PURE;
    };
}
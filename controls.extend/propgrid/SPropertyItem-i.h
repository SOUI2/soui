#pragma once

namespace SOUI
{
    #define IC_FIRST    (IPropertyItem *)0
    #define IC_LAST     (IPropertyItem *)1

    class SPropertyGrid;
    struct IPropertyItem : public IObjRef
                         , public SObject
    {
        enum PROPITEMTYPE{
            GPI_PARENT,
            GPI_FIRSTCHILD,
            GPI_LASTCHILD,
            GPI_NEXTSIBLING,
            GPI_PREVSIBLINIG,
        };
        virtual ~IPropertyItem(){}
        virtual LPCWSTR GetItemClass() const =0;
        virtual BOOL IsGroup() const =0;
        virtual BOOL HasButton() const =0;
        virtual int  GetLevel() const =0;
        virtual BOOL IsExpand() const =0;
        virtual void Expand(BOOL bExpend) =0;

        virtual IPropertyItem * GetParent() const =0;
        virtual void SetParent(IPropertyItem * pParent) =0;
        virtual IPropertyItem * GetItem(PROPITEMTYPE  type) const =0;
        virtual SPropertyGrid * GetOwner() const =0;
        virtual BOOL InsertChild(IPropertyItem * pChild,IPropertyItem * pInsertAfter=IC_LAST)=0;
        virtual BOOL RemoveChild(IPropertyItem * pChild)=0;
        virtual int ChildrenCount() const =0;

        virtual SStringW GetName2() const =0;
        virtual void SetName(const SStringW & strName) =0;
        virtual void SetID(int nID) =0;
        virtual SStringT GetDescription() const =0;
        virtual void SetDescription(const SStringT & strDescription) =0;
        virtual SStringT GetString() const =0;
        virtual void SetString(const SStringT & strValue) =0;
        virtual const void* GetValue() const =0;
        virtual void SetValue(void *pValue)=0;
        virtual void DrawItem(IRenderTarget *pRT,CRect rc) =0;
        virtual void AdjustInplaceActiveWndRect(CRect & rc)=0;
        virtual bool IsInplaceActive()=0;
        virtual void OnInplaceActive(bool bActive)=0;
        virtual void OnButtonClick()=0;
        virtual void OnValueChanged()=0;
        virtual void OnChildValueChanged(IPropertyItem *pChild)=0;
    };

    struct IPropInplaceWnd{
        virtual IPropertyItem * GetOwner() =0;
        virtual void UpdateData() =0;
    };

}
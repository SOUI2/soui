//////////////////////////////////////////////////////////////////////////
//  Class Name: SItemBox
// Description: Items Container
//     Creator: huangjianxiong
//     Version: 2011.7.8 - 1.0 - Create
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sitembox.h"

namespace SOUI
{

SItemBox::SItemBox():m_nItemWid(100),m_nItemHei(100),m_nSepHei(5),m_nSepWid(5)
{

}

SWindow* SItemBox::InsertItem(LPCWSTR pszXml,int iItem/*=-1*/,BOOL bEnsureVisible/*=FALSE*/)
{
    pugi::xml_document xmlDoc;
    if(!xmlDoc.load_buffer(pszXml,wcslen(pszXml),pugi::parse_default,pugi::encoding_utf16)) return NULL;

    SWindow *pChild=m_pFirstChild,*pPrevChild=ICWND_FIRST;
    for(int iChild=0; iChild<iItem || iItem==-1; iChild++)
    {
        if(!pChild) break;
        pPrevChild=pChild;
        pChild=pChild->GetWindow(GSW_NEXTSIBLING);
    }

    SWindow *pPanel=new SWindow;
    InsertChild(pPanel, pPrevChild);

    pPanel->CreateChildren(xmlDoc.first_child());
    pPanel->SetVisible(TRUE);
    pPanel->GetLayout()->SetWidth(m_nItemWid);
    pPanel->GetLayout()->SetHeight(m_nItemHei);

    UpdateScroll();
    ReLayout();

    if(bEnsureVisible) EnsureVisible(pPanel);

    Invalidate();
    return pPanel;
}

SWindow* SItemBox::InsertItem(pugi::xml_node xmlNode,int iItem/*=-1*/,BOOL bEnsureVisible/*=FALSE*/)
{
    if (!xmlNode) return NULL;    

    SWindow *pChild=m_pFirstChild,*pPrevChild=ICWND_FIRST;
    for(int iChild=0; iChild<iItem || iItem==-1; iChild++)
    {
        if(!pChild) break;
        pPrevChild=pChild;
        pChild=pChild->GetWindow(GSW_NEXTSIBLING);
    }

    SWindow *pPanel=new SWindow;
    InsertChild(pPanel, pPrevChild);

    pPanel->CreateChildren(xmlNode);
    pPanel->SetVisible(TRUE);
    pPanel->GetLayout()->SetWidth(m_nItemWid);
    pPanel->GetLayout()->SetHeight(m_nItemHei);

    UpdateScroll();
    ReLayout();

    if(bEnsureVisible) EnsureVisible(pPanel);

    Invalidate();
    return pPanel;
}

BOOL SItemBox::RemoveItem(UINT iItem)
{
    if(iItem>=GetItemCount()) return FALSE;
    SWindow *pChild=m_pFirstChild;
    for(UINT iChild=0; iChild<iItem ; iChild++)
    {
        pChild=pChild->GetWindow(GSW_NEXTSIBLING);
    }
    DestroyChild(pChild);

    UpdateScroll();
    ReLayout();
    Invalidate();

    return TRUE;
}

BOOL SItemBox::RemoveItem(SWindow * pChild)
{
    if (DestroyChild(pChild))
    {
        UpdateScroll();
        ReLayout();
        Invalidate();
        return TRUE;
    }
    return FALSE;
}

void SItemBox::BringWindowAfter(SWindow * pChild, SWindow * pInsertAfter)
{
    RemoveChild(pChild);
    InsertChild(pChild, pInsertAfter);
}

BOOL SItemBox::SetNewPosition(SWindow * pChild, DWORD nPos, BOOL bEnsureVisible)
{
    if (pChild == NULL)
    {
        return FALSE;
    }

    SWindow * pCurChild = m_pFirstChild;
    DWORD nCurPos = 0;
    for (; pCurChild != NULL; pCurChild = pCurChild->GetWindow(GSW_NEXTSIBLING))
    {
        if (pCurChild == pChild)
        {
            break;
        }
        ++nCurPos;
    }

    if (pCurChild == NULL)
    {
        return FALSE;
    }

    if (nCurPos == nPos)
    {
        if (bEnsureVisible)
        {
            EnsureVisible(pChild);
        }

        Invalidate();
        return TRUE;
    }

    if (nPos == 0)
    {
        BringWindowAfter(pChild, ICWND_FIRST);
    }
    else
    {
        SWindow * pNewNext = m_pFirstChild;
        for (UINT i = 0; i < nPos && pNewNext != NULL; i++)
        {
            pNewNext = pNewNext->GetWindow(GSW_NEXTSIBLING);
        }

        BringWindowAfter(pChild, pNewNext);
    }

    UpdateScroll();
    ReLayout();
    if (bEnsureVisible)
    {
        EnsureVisible(pChild);
    }
    Invalidate();
    return TRUE;
}

int SItemBox::GetItemPos(SWindow * lpCurItem)
{
    if (lpCurItem == NULL)
    {
        return -1;
    }
    int nPos = 0;
    for (SWindow *pChild = m_pFirstChild; pChild != NULL; pChild = pChild->GetWindow(GSW_NEXTSIBLING), ++nPos)
    {
        if (pChild == lpCurItem)
        {
            return nPos;
        }
    }
    return -1;
}

void SItemBox::RemoveAllItems()
{
    SWindow::OnDestroy();
    UpdateScroll();
    Invalidate();
}

UINT SItemBox::GetItemCount()
{
    return GetChildrenCount();
}

void SItemBox::PageUp()
{
    OnScroll(TRUE,SB_PAGEUP,0);
}

void SItemBox::PageDown()
{
    OnScroll(TRUE,SB_PAGEDOWN,0);
}

void SItemBox::EnsureVisible(SWindow *pItem)
{
    if(!HasScrollBar(TRUE)) return;
    SASSERT(pItem);
    CRect rcItem;
    pItem->GetWindowRect(&rcItem);
    int yOffset=0;
    if(rcItem.bottom>GetWindowRect().bottom)
    {
        yOffset=rcItem.bottom-GetWindowRect().bottom;
    }
    else if(rcItem.top<GetWindowRect().top)
    {
        yOffset=rcItem.top-GetWindowRect().top;
    }
    SetScrollPos(TRUE,GetScrollPos(TRUE)+yOffset,TRUE);
}

void SItemBox::UpdateScroll()
{
    CRect rcClient=GetWindowRect();

    int nPageCols=(rcClient.Width()+m_nSepWid)/(m_nItemWid+m_nSepWid);
    int nPageRows=(rcClient.Height()+m_nSepHei)/(m_nItemHei+m_nSepHei);
    if(nPageCols<1) nPageCols=1;
    int nPageItems=nPageCols*nPageRows;
    if(nPageItems<(int)GetItemCount())
    {
        rcClient.right-=m_nSbWid;
        nPageCols=(rcClient.Width()+m_nSepWid)/(m_nItemWid+m_nSepWid);
        if(nPageCols<1) nPageCols=1;
        nPageItems=nPageCols*nPageRows;
    }


    SCROLLINFO si;
    si.fMask=SIF_RANGE|SIF_PAGE;
    si.nPage=(nPageItems<(int)GetItemCount())?rcClient.Height():0;
    si.nMin=0;
    si.nMax=(GetItemCount()+nPageCols-1)/nPageCols*(m_nItemHei+m_nSepHei)-m_nSepHei;
    SetScrollInfo(si,TRUE);

    SetViewSize(CSize(rcClient.Width(),si.nMax));
    if(si.nPage==0) SetViewOrigin(CPoint(0,0));
    else SetViewOrigin(CPoint(0,GetScrollPos(TRUE)));
}

CRect SItemBox::GetItemRect(int iItem)
{
    CRect rcClient=GetWindowRect();

    int nPageCols=(rcClient.Width()+m_nSepWid)/(m_nItemWid+m_nSepWid);
    int nPageRows=(rcClient.Height()+m_nSepHei)/(m_nItemHei+m_nSepHei);
    if(nPageCols<1) nPageCols=1;
    int nPageItems=nPageCols*nPageRows;
    if(nPageItems<(int)GetItemCount())
    {
        rcClient.right-=m_nSbWid;
        nPageCols=(rcClient.Width()+m_nSepWid)/(m_nItemWid+m_nSepWid);
        if(nPageCols<1) nPageCols=1;
        nPageItems=nPageCols*nPageRows;
    }

    int nMargin=(rcClient.Width()-((m_nItemWid+m_nSepWid)*nPageCols-m_nSepWid))/2;
    CRect rcItem(CPoint(nMargin,0),CSize(m_nItemWid,m_nItemHei));
    int iCol=iItem%nPageCols;
    int iRow=iItem/nPageCols;
    rcItem.OffsetRect(iCol*(m_nItemWid+m_nSepWid),iRow*(m_nItemHei+m_nSepHei));
    return rcItem;
}

void SItemBox::OnSize(UINT nType, CSize size)
{
    __super::OnSize(nType,size);
    if(GetWindowRect().IsRectEmpty()) return;

    m_ptOrigin=CPoint(0,0);
    m_siVer.nPos=0;
    UpdateScroll();
    ReLayout();
}

void SItemBox::ReLayout()
{
    CRect rcItem;
    int iItem=0;
    SWindow *pChild=m_pFirstChild;
    while(pChild)
    {
        rcItem=GetItemRect(iItem);
        rcItem.OffsetRect(GetWindowRect().TopLeft()-m_ptOrigin);
        pChild->Move(rcItem);
        pChild=pChild->GetWindow(GSW_NEXTSIBLING);
        iItem++;
    }
}

BOOL SItemBox::OnScroll(BOOL bVertical,UINT uCode,int nPos)
{
    if(!__super::OnScroll(bVertical,uCode,nPos)) return FALSE;

    ReLayout();

    return TRUE;
}

int SItemBox::GetScrollLineSize(BOOL bVertical)
{
    if(bVertical) return m_nItemHei+m_nSepHei;
    else return m_nItemWid+m_nSepWid;
}

BOOL SItemBox::CreateChildren(pugi::xml_node xmlNode)
{
    if(!xmlNode) return FALSE;
    RemoveAllItems();

    pugi::xml_node xmlParent=xmlNode.parent();
    pugi::xml_node xmlItem=xmlParent.child(L"item");

    while(xmlItem)
    {
        SWindow *pChild=new SWindow;

        InsertChild(pChild);

        pChild->InitFromXml(xmlItem);
        pChild->SetVisible(TRUE);
        pChild->GetLayout()->SetWidth(m_nItemWid);
        pChild->GetLayout()->SetHeight(m_nItemHei);


        xmlItem=xmlItem.next_sibling(L"item");
    }
    return TRUE;
}

}//namespace SOUI
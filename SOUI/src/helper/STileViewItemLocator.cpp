#include "souistd.h"
#include "helper/STileViewItemLocator.h"

namespace SOUI
{
//////////////////////////////////////////////////////////////////////////
// STileViewItemLocatorFix
STileViewItemLocator::STileViewItemLocator(int nItemHei, int nItemWid, int nMarginSize /*= 0*/) :
    m_nItemHeight((float)nItemHei, SLayoutSize::px),
    m_nItemWidth((float)nItemWid, SLayoutSize::px),
    m_nItemMargin((float)nMarginSize, SLayoutSize::px),
    m_nTileViewWidth(0.f, SLayoutSize::px),
    m_nCountInRow(1)
{

}

STileViewItemLocator::STileViewItemLocator(LPCWSTR szItemHei, LPCWSTR szItemWid, SLayoutSize marginSize) :
    m_nItemHeight(GETLAYOUTSIZE(szItemHei)),
    m_nItemWidth(GETLAYOUTSIZE(szItemWid)),
    m_nItemMargin(marginSize),
    m_nTileViewWidth(0, SLayoutSize::px),
    m_nCountInRow(1),
    m_scale(100)
{
}

int STileViewItemLocator::GetScrollLineSize() const
{
    return GetItemLineHeight();
}

int STileViewItemLocator::GetMarginSize() const
{
	return m_nItemMargin.toPixelSize(m_scale);
}

int STileViewItemLocator::GetCountInRow()const
{
	return m_nCountInRow;
}

int STileViewItemLocator::GetItemLineHeight() const
{
	return m_nItemHeight.toPixelSize(m_scale) + m_nItemMargin.toPixelSize(m_scale);
}

int STileViewItemLocator::GetItemWidth()const
{
	return m_nItemWidth.toPixelSize(m_scale) + m_nItemMargin.toPixelSize(m_scale);
}

int STileViewItemLocator::SetScale(int scale)
{
    m_scale = scale;
    return 0;
}

int STileViewItemLocator::Position2Item(int position)
{
    if(!m_adapter)
    {
        return -1;
    }
    int nRet = position / GetItemLineHeight() * m_nCountInRow;
    
    if(nRet < 0)
    {
        nRet = 0;
    }
    if(nRet > m_adapter->getCount())
    {
        nRet = m_adapter->getCount();
    }
    return nRet;
}

int STileViewItemLocator::Item2Position(int iItem)
{
    return (iItem / m_nCountInRow) * GetItemLineHeight();
}

int STileViewItemLocator::GetTotalHeight()
{
    if(!m_adapter || m_adapter->getCount() == 0)
    {
        return 0;
    }
    int row_count = (m_adapter->getCount() - 1) / m_nCountInRow + 1;
    return GetItemLineHeight() * row_count + m_nItemMargin.toPixelSize(m_scale);
}

void STileViewItemLocator::SetItemHeight(int iItem, int nHeight)
{
}

int STileViewItemLocator::GetItemHeight(int iItem) const
{
    return m_nItemHeight.toPixelSize(m_scale);
}

void STileViewItemLocator::SetAdapter(ILvAdapter *pAdapter)
{
    m_adapter = pAdapter;
}

CRect STileViewItemLocator::GetItemRect(int iItem)
{
    //返回相对于TileView内部的Rect
    int nRowIdx, nColIdx;
    GetItemRowAndColIndex(iItem, nRowIdx, nColIdx);
    
    CRect rect;
    rect.left = m_nItemMargin.toPixelSize(m_scale) + nColIdx * (m_nItemWidth.toPixelSize(m_scale) + m_nItemMargin.toPixelSize(m_scale));
    rect.top = m_nItemMargin.toPixelSize(m_scale) + nRowIdx * (m_nItemHeight.toPixelSize(m_scale) + m_nItemMargin.toPixelSize(m_scale));
    rect.right = rect.left + m_nItemWidth.toPixelSize(m_scale);
    rect.bottom = rect.top + m_nItemHeight.toPixelSize(m_scale);
    return rect;
}

void STileViewItemLocator::SetTileViewWidth(LPCWSTR width)
{
    m_nTileViewWidth = GETLAYOUTSIZE(width);
    m_nCountInRow = m_nTileViewWidth.toPixelSize(m_scale) / (m_nItemWidth.toPixelSize(m_scale) + m_nItemMargin.toPixelSize(m_scale));
    if(m_nCountInRow == 0) m_nCountInRow=1;
}

void STileViewItemLocator::SetTileViewWidth(int width)
{
    m_nTileViewWidth.setSize((float)width, SLayoutSize::px);
    m_nCountInRow = m_nTileViewWidth.toPixelSize(m_scale) / (m_nItemWidth.toPixelSize(m_scale) + m_nItemMargin.toPixelSize(m_scale));
    if(m_nCountInRow == 0) m_nCountInRow=1;
}

void STileViewItemLocator::GetItemRowAndColIndex(int iItem, int &row, int &col)
{
    row = (iItem + 1) / m_nCountInRow;
    if(row * m_nCountInRow - 1 >= iItem)
    {
        row--;
    }
    col = m_nCountInRow - ((row + 1) * m_nCountInRow - iItem);
}

BOOL STileViewItemLocator::IsLastInRow(int iItem)
{
    return iItem % m_nCountInRow == m_nCountInRow - 1;
}

int STileViewItemLocator::GetUpItem(int iItem)
{
    int up_idx = iItem - m_nCountInRow;
    //没有上一行了，返回原值
    return up_idx < 0 ? iItem : up_idx;
}

int STileViewItemLocator::GetDownItem(int iItem)
{
    int up_idx = iItem + m_nCountInRow;
    if(up_idx < m_adapter->getCount())
    {
        return up_idx;
    }
    else if((m_adapter->getCount() - 1) / m_nCountInRow == iItem / m_nCountInRow)
    {
        //没有下一行了，返回原值
        return iItem;
    }
    //有下一行，但同一列没有元素，返回下一行最后一个元素
    return m_adapter->getCount() - 1;
}

}

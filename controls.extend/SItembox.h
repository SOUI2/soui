/**
 * Copyright (C) 2014-2050 SOUI团队
 * All rights reserved.
 * 
 * @file       SItembox.h
 * @brief      
 * @version    v1.0      
 * @author     soui      
 * @date       2014-07-03
 * 
 * Describe     
 */
#pragma once
#include <core/SPanel.h>

namespace SOUI
{

/**
 * @class      SItemBox
 * @brief      ItemBox
 * 
 * Describe    ItemBox
 */
class SItemBox
    : public SScrollView
{
    SOUI_CLASS_NAME(SItemBox, L"itembox")
public:
    /**
     * SItemBox::SItemBox
     * @brief    构造函数
     *
     * Describe  构造函数  
     */
    SItemBox();

    /**
     * SItemBox::~SItemBox
     * @brief    析构函数
     *
     * Describe  析构函数  
     */
    virtual ~SItemBox() {}
    
    /**
     * SItemBox::InsertItem
     * @brief    插入新项
     * @param    LPCWSTR pszXml -- xml配置文件
     * @param    int iItem -- 索引
     * @param    BOOL bEnsureVisible -- 是否显示
     * @return   返回SWindow
     *
     * Describe  插入新项  
     */
    SWindow* InsertItem(LPCWSTR pszXml,int iItem=-1,BOOL bEnsureVisible=FALSE);

    /**
     * SItemBox::InsertItem
     * @brief    插入新项
     * @param    LPCWSTR pszXml -- xml配置文件
     * @param    int iItem -- 索引
     * @param    BOOL bEnsureVisible -- 是否显示
     * @return   返回SWindow
     *
     * Describe  插入新项  
     */
    SWindow* InsertItem(pugi::xml_node xmlNode, int iItem=-1,BOOL bEnsureVisible=FALSE);

    /**
     * SItemBox::RemoveItem
     * @brief    删除项
     * @param    UINT iItem -- 索引
     * @return   返回BOOL
     *     
     * Describe  删除项  
     */
    BOOL RemoveItem(UINT iItem);

    /**
     * SItemBox::RemoveItem
     * @brief    删除项
     * @param    SWindow * pChild -- 窗口节点
     * @return   返回BOOL
     *
     * Describe  删除项  
     */
    BOOL RemoveItem(SWindow * pChild);

    /**
     * SItemBox::SetNewPosition
     * @brief    析构函数
     * @param    SWindow * pChild -- 节点
     * @param    DWORD nPos -- 位置
     * @param    BOOL bEnsureVisible -- 是否显示
     * @return   返回BOOL
     *     
     * Describe  析构函数  
     */
    BOOL SetNewPosition(SWindow * pChild, DWORD nPos, BOOL bEnsureVisible = TRUE);

    /**
     * SItemBox::RemoveAllItems
     * @brief    删除所有
     *
     * Describe  删除所有  
     */
    void RemoveAllItems();

    /**
     * SItemBox::GetItemCount
     * @brief    获取项个数
     * @return   UINT
     *
     * Describe  获取项个数  
     */
    UINT GetItemCount();

    /**
     * SItemBox::PageUp
     * @brief    上一页
     *
     * Describe  上一页  
     */
    void PageUp();

    /**
     * SItemBox::PageDown
     * @brief    下一页
     *
     * Describe  下一页  
     */
    void PageDown();

    /**
     * SItemBox::EnsureVisible
     * @brief    设置显示
     * @param    SWindow *pItem  -- 某项指针
     *
     * Describe  设置显示  
     */
    void EnsureVisible(SWindow *pItem);

    /**
     * SItemBox::GetItemPos
     * @brief    获取某项得索引
     * @return   返回int
     *
     * Describe  获取某项得索引  
     */
    int GetItemPos(SWindow * lpCurItem);

protected:
    int m_nItemWid; /**< Item宽度 */
    int m_nItemHei; /**< Item高度*/
    int m_nSepWid;  /**< */
    int m_nSepHei;  /**< */

    /**
     * SItemBox::UpdateScroll
     * @brief    更新滚动条
     *
     * Describe  更新滚动条  
     */
    void UpdateScroll();

    /**
     * SItemBox::GetItemRect
     * @brief    获取某项位置
     * @param    int iItem -- 某项索引
     * @return   返回int
     *
     * Describe  获取某项得索引  
     */
    
    CRect GetItemRect(int iItem);

    /**
     * SItemBox::BringWindowAfter
     * @brief    插入新节点
     * @param    SWindow * pChild -- 新节点
     * @param    SWindow * pInsertAfter -- 位置节点
     *
     * Describe  在某个节点后插入新节点  
     */
    void BringWindowAfter(SWindow * pChild, SWindow * pInsertAfter);

    /**
     * SItemBox::OnSize
     * @brief    消息响应函数
     * @param    UINT nType --
     * @param    CSize size -- 
     *
     * Describe  获取某项得索引  
     */
    void OnSize(UINT nType, CSize size);

    /**
     * SItemBox::UpdateChildrenPosition
     * @brief    更新子节点位置
     *
     * Describe  更新子节点位置  
     */
    virtual void UpdateChildrenPosition(){}//leave it empty

    /**
     * SItemBox::ReLayout
     * @brief    重新布局
     *
     * Describe  重新布局  
     */
    void ReLayout();

    /**
     * SItemBox::OnScroll
     * @brief    滚动事件
     * @param    BOOL bVertical -- 是否是竖直
     * @param    UINT uCode -- 消息码
     * @param    int nPos -- 位置
     * @retur    返回int
     *
     * Describe  获取某项得索引  
     */
    virtual BOOL OnScroll(BOOL bVertical,UINT uCode,int nPos);

    /**
     * SItemBox::GetScrollLineSize
     * @brief    获取滚动条大小
     * @param    BOOL bVertical -- 是否是竖直方向
     * @retur    返回int
     *
     * Describe  获取滚动条大小  
     */
    virtual int GetScrollLineSize(BOOL bVertical);

    /**
     * SItemBox::CreateChildren
     * @brief    创建新项
     * @param    pugi::xml_node xmlNode
     * @return   返回BOOL
     *
     * Describe  获取某项得索引  
     */
    virtual BOOL CreateChildren(pugi::xml_node xmlNode);

    SOUI_ATTRS_BEGIN()
        ATTR_INT(L"itemWidth", m_nItemWid, TRUE)
        ATTR_INT(L"itemHeight", m_nItemHei, TRUE)
        ATTR_INT(L"sepWidth", m_nSepWid, TRUE)
        ATTR_INT(L"sepHeight", m_nSepHei, TRUE)
    SOUI_ATTRS_END()

    SOUI_MSG_MAP_BEGIN()
        MSG_WM_SIZE(OnSize)
    SOUI_MSG_MAP_END()

};

}//namespace SOUI

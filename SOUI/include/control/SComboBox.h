/**
 * Copyright (C) 2014-2050 SOUI团队
 * All rights reserved.
 * 
 * @file       SCmnCtrl.h
 * @brief      通用控件
 * @version    v1.0      
 * @author     soui      
 * @date       2014-06-26
 * 
 * Describe    ComboBox控件
 */
#pragma once
#include "SComboBase.h"
#include "Slistbox.h"
#include "SCmnCtrl.h"

namespace SOUI
{

/**
 * @class      SComboBox
 * @brief      可输入CommboBox
 * 
 * Describe    可输入下拉列表
 */
class SOUI_EXP SComboBox : public SComboBase
{
    SOUI_CLASS_NAME(SComboBox, L"combobox")
public:
    /**
     * SComboBox::SComboBox
     * @brief    构造函数
     *
     * Describe  构造函数
     */
    SComboBox();
      
    /**
     * SComboBox::~SComboBox
     * @brief    析构函数
     *
     * Describe  析构函数
     */
    virtual ~SComboBox();

    /**
     * SComboBox::SetCurSel
     * @brief    设置当前选中
     * @param    int iSel -- 选中索引
     * 
     * Describe  设置当前选中
     */ 
    BOOL SetCurSel(int iSel)
    {
        if(m_pListBox->SetCurSel(iSel))
        {
            OnSelChanged();
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

    /**
     * SComboBox::GetCurSel
     * @brief    获取选中索引
     * @return   返回int -- 选中索引
     * 
     * Describe  获取选中索引
     */ 
    int GetCurSel() const
    {
        return m_pListBox->GetCurSel();
    }

    /**
     * SComboBox::GetCount
     * @brief    获取下拉项个数
     * @return   返回int
     * 
     * Describe  获取下拉项个数
     */ 
    int  GetCount() const
    {
        return m_pListBox->GetCount();
    }

    /**
     * SComboBox::GetItemData
     * @brief    获取附加数据
     * @param    UINT iItem -- 选项值
     *
     * Describe  获取附加数据
     */
    LPARAM GetItemData(UINT iItem) const
    {
        return m_pListBox->GetItemData(iItem);
    }

    /**
     * SComboBox::SetItemData
     * @brief    设置附加数据
     * @param    UINT iItem -- 索引值
     * @param    LPARAM lParam -- 附加值
     *
     * Describe  设置附加数据
     */
    int SetItemData(UINT iItem, LPARAM lParam)
    {
        return m_pListBox->SetItemData(iItem,lParam);
    }

    /**
     * SComboBox::InsertItem
     * @brief    插入新项
     * @param    UINT iPos -- 位置
     * @param    LPCTSTR pszText -- 文本值
     * @param    int iIcon -- 图标
     * @param    LPARAM lParam -- 附加值
     *
     * Describe  插入新项
     */
    int InsertItem(UINT iPos,LPCTSTR pszText,int iIcon,LPARAM lParam)
    {
        return m_pListBox->InsertString(iPos,pszText,iIcon,lParam);
    }

    /**
     * SComboBox::DeleteString
     * @brief    删除某一项
     * @param    UINT iItem -- 索引值
     *
     * Describe  删除某一项
     */
    BOOL DeleteString(UINT iItem)
    {
        return m_pListBox->DeleteString(iItem);
    }

    /**
     * SComboBox::ResetContent
     * @brief    删除所有项
     *
     * Describe  设置附加数据
     */
    void ResetContent()
    {
        SetCurSel(-1);
        return m_pListBox->DeleteAll();
    }

    /**
     * SComboBox::GetLBText
     * @brief    获取文本
     * @param    int iItem -- 索引值
	 * @param    BOOL bRawText-- true:未经翻译的文字，false：翻译后的文字
     *
     * Describe  获取文本
     */
    SStringT GetLBText(int iItem,BOOL bRawText = FALSE)
    {
        return m_pListBox->GetText(iItem,bRawText);
    }
    /**
     * SComboBox::GetListBox
     * @brief    获取下拉列表指针
     * @param    返回SListBox * 
     *
     * Describe  获取下拉列表指针
     */
    SListBox * GetListBox(){return m_pListBox;}
    
protected:
    /**
     * SComboBox::FireEvent
     * @brief    通知消息
     * @param    EventArgs &evt -- 事件对象 
     * 
     * Describe  此函数是消息响应函数
     */ 
    virtual BOOL FireEvent(EventArgs &evt);

    virtual void OnScaleChanged(int nScale);

	virtual HRESULT OnLanguageChanged();

    /**
     * SComboBox::CreateListBox
     * @brief    创建下拉列表
     * @param    返回BOOL TRUE -- 成功 FALSE -- 失败
     *
     * Describe  创建下拉列表
     */
    virtual BOOL CreateListBox(pugi::xml_node xmlNode);
    
    /**
     * SComboBox::GetListBoxHeight
     * @brief    获取下拉列表高度
     * @param    返回int
     *
     * Describe  获取下拉列表高度
     */
    virtual int  GetListBoxHeight();

    /**
     * SComboBox::OnCreateDropDown
     * @brief    下拉列表创建事件
     * @param    SDropDownWnd *pDropDown -- 下拉列表指针
     *
     * Describe  下拉列表事件
     */
    virtual void OnCreateDropDown(SDropDownWnd *pDropDown);

    /**
     * SComboBox::OnDestroyDropDown
     * @brief    下拉列表销毁事件
     * @param    SDropDownWnd *pDropDown -- 下拉列表指针
     *
     * Describe  获取下拉列表指针
     */
    virtual void OnDestroyDropDown(SDropDownWnd *pDropDown);
    
    /**
     * SComboBox::OnSelChanged
     * @brief    下拉列表selected事件
     *
     * Describe  下拉列表selected事件
     */
    virtual void OnSelChanged();

protected:

    SListBox *m_pListBox;  /**< SListBox指针 */
};

}//namespace

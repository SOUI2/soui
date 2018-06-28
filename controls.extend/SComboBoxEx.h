#pragma once

#include "slistboxex.h"
namespace SOUI
{
    
    class EventOfComoboxExItem : public TplEventArgs<EventOfComoboxExItem>
    {
        SOUI_CLASS_NAME(EventOfComoboxExItem,L"on_event_of_comboxex_item")
    public:
        EventOfComoboxExItem(SObject *pSender,EventCmd *_pOrgEvt )
            :TplEventArgs<EventOfComoboxExItem>(pSender)
            ,pOrgEvt(_pOrgEvt)
            ,bCancel(FALSE)
        {

        }

        enum{EventID=EVT_EXTERNAL_BEGIN + 300};

        EventCmd    *  pOrgEvt;
        BOOL           bCancel;
    };

class  SComboBoxEx : public SComboBase
{
    SOUI_CLASS_NAME(SComboBoxEx, L"comboboxex")
public:
    /**
     * SComboBoxEx::SComboBoxEx
     * @brief    构造函数
     *
     * Describe  构造函数
     */
    SComboBoxEx();
    /**
     * SComboBoxEx::~SComboBoxEx
     * @brief    析构函数
     *
     * Describe  析构函数
     */    
    virtual ~SComboBoxEx();

    /**
     * SComboBoxEx::SetCurSel
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
        }else
        {
            return FALSE;
        }
    }

    /**
     * SComboBoxEx::GetCurSel
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
        return m_pListBox->GetItemCount();
    }
    

    /**
     * SComboBoxEx::GetItemData
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
     * SComboBoxEx::SetItemData
     * @brief    设置附加数据
     * @param    UINT iItem -- 索引值
     * @param    LPARAM lParam -- 附加值
     *
     * Describe  设置附加数据
     */
    void SetItemData(UINT iItem, LPARAM lParam)
    {
        m_pListBox->SetItemData(iItem,lParam);
    }
    
    /**
     * SComboBoxEx::InsertItem
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
        int iInserted= m_pListBox->InsertItem(iPos,NULL,lParam);
        if(iInserted!=-1)
        {
            SWindow *pWnd=m_pListBox->GetItemPanel(iInserted);
            if(m_uTxtID!=0)
            {
                SWindow *pText=pWnd->FindChildByID(m_uTxtID);
                if(pText) pText->SetWindowText(pszText);
            }
            if(m_uIconID!=0)
            {
                SImageWnd *pIcon=pWnd->FindChildByID2<SImageWnd>(m_uIconID);
                if(pIcon) pIcon->SetIcon(iIcon);
            }
        }
        return iInserted;
    }
    /**
     * SComboBoxEx::DeleteString
     * @brief    删除某一项
     * @param    UINT iItem -- 索引值
     *
     * Describe  删除某一项
     */
    void DeleteString(UINT iItem)
    {
        m_pListBox->DeleteItem(iItem);
    }

    /**
     * SComboBoxEx::ResetContent
     * @brief    删除所有项
     *
     * Describe  设置附加数据
     */
    void ResetContent()
    {
        return m_pListBox->DeleteAllItems();
    }
    
    /**
     * SComboBoxEx::GetLBText
     * @brief    获取文本
     * @param    int iItem -- 索引值
     *
     * Describe  获取文本
     */
    SStringT GetLBText(int iItem);

    SListBoxEx * GetListBox(){return m_pListBox;}

protected:
    
    /**
     * SComboBox::OnSelChanged
     * @brief    下拉列表selected事件
     *
     * Describe  下拉列表selected事件
     */
    virtual void OnSelChanged();
protected:
    /**
     * SComboBoxEx::FireEvent
     * @brief    通知消息
     * @param    EventArgs &evt -- 事件对象 
     * 
     * Describe  此函数是消息响应函数
     */ 
    virtual BOOL FireEvent(EventArgs &evt);

    /**
     * SComboBoxEx::CreateListBox
     * @brief    创建下拉列表
     * @param    返回BOOL TRUE -- 成功 FALSE -- 失败
     *
     * Describe  创建下拉列表
     */
    virtual BOOL CreateListBox(pugi::xml_node xmlNode);

    /**
     * SComboBoxEx::GetListBoxHeight
     * @brief    获取下拉列表高度
     * @param    返回int
     *
     * Describe  获取下拉列表高度
     */    
    virtual int  GetListBoxHeight();

    /**
     * SComboBoxEx::OnDropDown
     * @brief    下拉列表事件
     * @param    SDropDownWnd *pDropDown -- 下拉列表指针
     *
     * Describe  下拉列表事件
     */
    virtual void OnCreateDropDown(SDropDownWnd *pDropDown);

    /**
     * SComboBox::OnCloseUp
     * @brief    下拉列表关闭事件
     * @param    SDropDownWnd *pDropDown -- 下拉列表指针
     *
     * Describe  获取下拉列表指针
     */
    virtual void OnDestroyDropDown(SDropDownWnd *pDropDown);

protected:

    SOUI_ATTRS_BEGIN()
        ATTR_UINT(L"id_text", m_uTxtID, FALSE)
        ATTR_UINT(L"id_icon", m_uIconID, FALSE)
    SOUI_ATTRS_END()

    SListBoxEx *m_pListBox;  /**< SListBox指针 */
    UINT   m_uTxtID;  /**< 文本ID */
    UINT   m_uIconID; /**< 图标ID */
};


}
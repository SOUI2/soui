#pragma once

#include "SComboBase.h"
#include "SListView.h"

namespace SOUI
{
    class SOUI_EXP SComboView : public SComboBase
    {
        SOUI_CLASS_NAME(SComboView,L"comboview")
    public:
        SComboView(void);
        ~SComboView(void);

        BOOL SetCurSel(int iSel);

        /**
        * SComboView::GetCurSel
        * @brief    获取选中索引
        * @return   返回int -- 选中索引
        * 
        * Describe  获取选中索引
        */ 
        int GetCurSel() const;

        /**
        * SComboView::GetCount
        * @brief    获取下拉项个数
        * @return   返回int
        * 
        * Describe  获取下拉项个数
        */ 
        int  GetCount() const;

        /**
        * SComboView::GetLBText
        * @brief    获取文本
        * @param    int iItem -- 索引值
        *
        * Describe  获取文本
        */
        SStringT GetLBText(int iItem,BOOL bRawText=FALSE);
        
        /**
        * SComboView::GetListBox
        * @brief    获取下拉列表指针
        * @param    返回SListBox * 
        *
        * Describe  获取下拉列表指针
        */
        SListView * GetListView();

    protected:
        /**
        * SComboView::FireEvent
        * @brief    通知消息
        * @param    EventArgs &evt -- 事件对象 
        * 
        * Describe  此函数是消息响应函数
        */ 
        virtual BOOL FireEvent(EventArgs &evt);

        virtual void OnScaleChanged(int nScale);

        /**
        * SComboView::CreateListBox
        * @brief    创建下拉列表
        * @param    返回BOOL TRUE -- 成功 FALSE -- 失败
        *
        * Describe  创建下拉列表
        */
        virtual BOOL CreateListBox(pugi::xml_node xmlNode);

        /**
        * SComboView::GetListBoxHeight
        * @brief    获取下拉列表高度
        * @param    返回int
        *
        * Describe  获取下拉列表高度
        */
        virtual int  GetListBoxHeight();

        /**
        * SComboView::OnCreateDropDown
        * @brief    下拉列表事件
        * @param    SDropDownWnd *pDropDown -- 下拉列表指针
        *
        * Describe  下拉列表事件
        */
        virtual void OnCreateDropDown(SDropDownWnd *pDropDown);

        /**
        * SComboView::OnDestroyDropDown
        * @brief    下拉列表关闭事件
        * @param    SDropDownWnd *pDropDown -- 下拉列表指针
        * @param    UINT uCode -- 消息码
        *
        * Describe  获取下拉列表指针
        */
        virtual void OnDestroyDropDown(SDropDownWnd *pDropDown);

        /**
        * SComboView::OnSelChanged
        * @brief    下拉列表selected事件
        *
        * Describe  下拉列表selected事件
        */
        virtual void OnSelChanged();

    protected:

        SListView *m_pListBox;  /**< SListBox指针 */
    };

}

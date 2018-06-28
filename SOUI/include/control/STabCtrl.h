/**
* Copyright (C) 2014-2050 SOUI团队
* All rights reserved.
* 
* @file       stabctrl.h
* @brief      
* @version    v1.0      
* @author     soui      
* @date       2014-07-06
* 
* Describe    扩展列表框 
*/
#pragma once
#include "core/SWnd.h"
#include "helper/SplitString.h"

namespace SOUI
{
    /** 
    * @class     STabPage
    * @brief     tab标签页面
    *
    * Describe   tab标签页面
    */
    class SOUI_EXP STabPage : public SWindow
    {
        SOUI_CLASS_NAME(STabPage, L"page")
		friend class STabCtrl;
    public:
        /**
        * STabPage::STabPage
        * @brief    构造函数
        *
        * Describe  构造函数  
        */
        STabPage():m_iIcon(-1),m_strTitle(this)
        {
            m_bVisible=FALSE;
        }
        /**
        * STabPage::~STabPage
        * @brief    析构函数
        *
        * Describe  析构函数  
        */
        virtual ~STabPage()
        {
        }
        /**
        * STabPage::GetTitle
        * @brief    获取标题
        * @return   LPCTSTR --- 标题
        *
        * Describe  获取标题
        */
        LPCTSTR GetTitle()
        {
            return m_strTitle.GetText(FALSE);
        }
        /**
        * STabPage::SetTitle
        * @brief    设置标题
        * @param    LPCTSTR lpszTitle --- 标题
        *
        * Describe  设置标题 
        */
        void SetTitle(LPCTSTR lpszTitle)
        {
            m_strTitle.SetText(lpszTitle);
        }
        
        int GetIconIndex() const {return m_iIcon;}
        
        void SetIconIndex(int iIcon) {m_iIcon=iIcon;}
        
        
        /**
         * OnUpdateToolTip
         * @brief    处理tooltip
         * @param    const CPoint & pt --  测试点
         * @param [out]  SwndToolTipInfo & tipInfo -- tip信息 
         * @return   BOOL -- FALSE
         *
         * Describe  总是返回FALSE，禁止在page页面上显示tooltip
         */
        virtual BOOL OnUpdateToolTip(CPoint pt, SwndToolTipInfo &tipInfo){return FALSE;}

        SOUI_ATTRS_BEGIN()
            ATTR_I18NSTRT(L"title", m_strTitle, FALSE)
            ATTR_INT(L"iconIndex", m_iIcon,FALSE)
        SOUI_ATTRS_END()
    protected:

        STrText m_strTitle; /**< 标题 */
        int      m_iIcon;
    };

    /** 
    * @class     STabCtrl
    * @brief     tab控件
    *
    * Describe   tab控件
    */
    class SOUI_EXP STabCtrl : public SWindow
    {
		friend class STabSlider;
        SOUI_CLASS_NAME(STabCtrl, L"tabctrl")

    protected:
        int m_nHoverTabItem; /**< hover状态item */
        int m_nCurrentPage;  /**< 当前页码      */
        SLayoutSize m_nTabInterSize; /**< tab页面间距   */
        SLayoutSize m_szTab[2]; /**< tab SIZE   */
        SLayoutSize m_nTabPos;  /**< tab位置       */
        ISkinObj *m_pSkinTab; /**< ISkibObj对象 */
        ISkinObj *m_pSkinIcon; /**< ISkibObj对象  */
        ISkinObj *m_pSkinTabInter;  /**< ISkibObj对象  */
        ISkinObj *m_pSkinFrame;     /**< ISkibObj对象  */
        SLayoutSize m_ptIcon[2]; /**< 图标位置 */
        SLayoutSize m_ptText[2]; /**< 标题位置 */
        int m_nTabAlign;   /**< 排列方式 */

        SArray<STabPage*> m_lstPages;  /**< tab标签页面链表 */
		STabSlider *      m_tabSlider;        
        enum
        {
            AlignTop,
            AlignLeft,
            AlignBottom,
            AlignRight,
        };
        
        enum TEXTDIR
        {
            Text_Horz,
            Text_Vert,
        }m_txtDir;
        int    m_nAnimateSteps; /**< 动画次数 */
		int	m_nAniamteType;/*动画样式*/
		CAutoRefPtr<IInterpolator> m_aniInterpolator;
    public:
        /**
        * STabCtrl::STabCtrl
        * @brief    构造函数
        *
        * Describe  构造函数  
        */
        STabCtrl();

        /**
        * STabCtrl::~STabCtrl
        * @brief    析构函数
        *
        * Describe  析构函数  
        */
        virtual ~STabCtrl() {}

        /**
        * STabCtrl::GetCurSel
        * @brief    获取当前选中
        * @return   返回int
        *
        * Describe  获取当前选中 
        */
        int GetCurSel()
        {
            return m_nCurrentPage;
        }

        SWindow * GetPage(int iPage);
        SWindow * GetPage(LPCTSTR pszName,BOOL bTitle=TRUE);

        /**
        * STabCtrl::SetCurSel
        * @brief    设置当前选中
        * @param    int nIndex -- 索引
        * @return   返回BOOL
        *
        * Describe  获取当前选中 
        */
        BOOL SetCurSel(int nIndex);

        /**
        * STabCtrl::SetCurSel
        * @brief    设置当前选中
        * @param    LPCTSTR pszTitle -- 标题
        * @return   返回BOOL
        *
        * Describe  获取当前选中 
        */
        BOOL SetCurSel(LPCTSTR pszName,BOOL bTitle=TRUE);

        /**
        * STabCtrl::SetItemTitle
        * @brief    设置标题
        * @param    int nIndex  -- 索引
        * @param    LPCTSTR lpszTitle  -- 标题
        * @return   返回BOOL
        *
        * Describe  获取当前选中 
        */
        BOOL SetItemTitle(int nIndex, LPCTSTR lpszTitle);
        /**
        * STabCtrl::CreateChildren
        * @brief    创建tab页面
        * @param    pugi::xml_node xmlNode  -- xml文件
        * @return   返回BOOL
        *
        * Describe  创建tab页面
        */
        BOOL CreateChildren(pugi::xml_node xmlNode);

        /**
        * STabCtrl::InsertItem
        * @brief    插入tab页面
        * @param    LPCWSTR lpContent  -- XML描述的page信息
        * @param    int iInsert  -- 位置
        * @return   返回插入位置
        *
        * Describe  插入tab页面
        */
        virtual int InsertItem(LPCWSTR lpContent,int iInsert=-1);

        /**
        * STabCtrl::InsertItem
        * @brief    插入tab页面
        * @param    pugi::xml_node xmlNode  -- xml文件
        * @param    int iInsert  -- 位置
        * @param    BOOL bLoading -- 是否加载
        * @return   返回int
        *
        * Describe  插入tab页面
        */
        virtual int InsertItem(pugi::xml_node xmlNode,int iInsert=-1,BOOL bLoading=FALSE);

        /**
        * STabCtrl::GetItemCount
        * @brief    获取tab页面数
        * @return   返回int
        *
        * Describe  获取tab页面数
        */
        int GetItemCount()
        {
            return (int)m_lstPages.GetCount();
        }
        /**
        * STabCtrl::GetItem
        * @brief    获取指定tab页面
        * @param    int nIndex -- 索引
        * @return   返回int
        *
        * Describe  获取当前选中 
        */
        STabPage* GetItem(int nIndex);

        /**
        * STabCtrl::RemoveItem
        * @brief    删除指定tab页面
        * @param    int nIndex -- 索引
        * @param    int nSelPage -- 选中页面
        * @return   删除指定tab页面
        *
        * Describe  获取当前选中 
        */
        BOOL RemoveItem(int nIndex, int nSelPage=0);

        /**
        * STabCtrl::RemoveAllItems
        * @brief    删除所有页面
        *
        * Describe  删除所有页面 
        */
        void RemoveAllItems(void);
        
        /**
        * STabCtrl::GetPageIndex
        * @brief    获取指定页面的索引
        * @param    LPCTSTR pszName -- 查询字符串
        * @param    BOOL bTitle -- TRUE:pszName代表的是page的title属性,FALSE：pszName代表的是page的name属性
        * @return   找到的页面索引号
        *
        * Describe 
        */
        int GetPageIndex(LPCTSTR pszName,BOOL bTitle);

    protected:

		/**
        * OnItemInserted
        * @brief    插入page状态
        * @param    STabPage *pItem --  插入的PAGE
        * @return   void 
        *
        * Describe  
        */
		virtual void OnItemInserted(STabPage *pItem){}

		/**
        * OnItemRemoved
        * @brief    删除page状态
        * @param    STabPage *pItem --  删除的PAGE
        * @return   void 
        *
        * Describe  
        */
		virtual void OnItemRemoved(STabPage *pItem){}

        /**
        * BeforePaint
        * @brief    为RT准备好当前窗口的绘图环境
        * @param    IRenderTarget * pRT --  
        * @param    SPainter & painter --  
        * @return   void 
        *
        * Describe  和SWindow不同，STabCtrl中强制使用normal状态配置字体及颜色，其它状态给tab头使用
        */
        virtual void BeforePaint(IRenderTarget *pRT, SPainter &painter);

        /**
        * STabCtrl::GetChildrenLayoutRect
        * @brief    
        *
        * Describe  
        */
        virtual CRect GetChildrenLayoutRect();
        
        /**
         * GetTitleRect
         * @brief    获取tab头的矩形
         * @return   CRect 
         *
         * Describe  
         */
        virtual CRect GetTitleRect();
        
        /**
        * STabCtrl::GetItemRect
        * @brief    获取指定item位置
        * @param    int nIndex -- 索引
        * @param     CRect &rcItem -- 位置
        *
        * Describe  获取指定item位置 
        */
        virtual BOOL GetItemRect(int nIndex, CRect &rcItem);
        
        /**
        * STabCtrl::DrawItem
        * @brief    绘制item
        * @param    IRenderTarget *pRT -- 绘制设备
        * @param    const CRect &rcItem -- 绘制区域
        * @param    int iItem  -- 索引
        * @param    DWORD dwState  -- 状态
        *
        * Describe  绘制item
        */
        virtual void DrawItem(IRenderTarget *pRT,const CRect &rcItem,int iItem,DWORD dwState);

        virtual STabPage * CreatePageFromXml(pugi::xml_node xmlPage);
        
        /**
        * STabCtrl::OnGetDlgCode
        * @brief    获取窗口消息码
        * @return   返回UINT
        *
        * Describe  获取窗口消息码
        */
        virtual UINT OnGetDlgCode()
        {
            return SC_WANTARROWS;
        }

        virtual BOOL OnUpdateToolTip(CPoint pt, SwndToolTipInfo & tipInfo);
        
        /**
        * UpdateChildrenPosition
        * @brief    更新子窗口位置
        * @return   void 
        *
        * Describe  
        */
        virtual void UpdateChildrenPosition();

        virtual void OnInitFinished(pugi::xml_node xmlNode);
        
        virtual void OnColorize(COLORREF cr);
        virtual void OnScaleChanged(int nScale);

		virtual HRESULT OnLanguageChanged();
    protected:
        int HitTest(CPoint pt);
                
        void OnSliderFinish();

    protected:
        void TextOutV(IRenderTarget *pRT,int x,int y ,  const SStringT & strText);
        void DrawTextV(IRenderTarget *pRT, CRect rcText,  const SStringT & strText);
        SIZE MeasureTextV(IRenderTarget *pRT, const SStringT & strText);
    protected:
        /**
        * STabCtrl::OnPaint
        * @brief    绘画消息
        * @param    IRenderTarget *pRT -- 绘制设备句柄
        *
        * Describe  此函数是消息响应函数
        */
        void OnPaint(IRenderTarget *pRT);
        /**
        * STabCtrl::OnLButtonDown
        * @brief    鼠标左键按下事件
        * @param    UINT nFlags -- 标志
        * @param    CPoint point -- 鼠标坐标
        *
        * Describe  此函数是消息响应函数
        */
        void OnLButtonDown(UINT nFlags, CPoint point);
        /**
        * STabCtrl::OnMouseMove
        * @brief    鼠标移动事件
        * @param    UINT nFlags -- 标志
        * @param    CPoint point -- 鼠标坐标
        *
        * Describe  此函数是消息响应函数
        */
        void OnMouseMove(UINT nFlags, CPoint point);
        /**
        * STabCtrl::OnMouseLeave
        * @brief    鼠标离开事件
        *
        * Describe  此函数是消息响应函数
        */
        void OnMouseLeave()
        {
            OnMouseMove(0,CPoint(-1,-1));
        }
        /**
        * STabCtrl::OnKeyDown
        * @brief    键盘按下
        * @param    UINT nChar -- 键码
        * @param    UINT nRepCnt -- 重复次数
        * @param    UINT nFlags -- 标志
        *
        * Describe  此函数是消息响应函数
        */
        void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
        /**
        * STabCtrl::OnDestroy
        * @brief    销毁
        *
        * Describe  此函数是消息响应函数
        */
        void OnDestroy();

        SOUI_MSG_MAP_BEGIN()
            MSG_WM_PAINT_EX(OnPaint)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_LBUTTONDOWN(OnLButtonDown)
            MSG_WM_MOUSEMOVE(OnMouseMove)
            MSG_WM_MOUSELEAVE(OnMouseLeave)
            MSG_WM_KEYDOWN(OnKeyDown)
        SOUI_MSG_MAP_END()

        SOUI_ATTRS_BEGIN()
            ATTR_INT(L"curSel", m_nCurrentPage, FALSE)
            ATTR_LAYOUTSIZE2(L"tabSize",m_szTab,TRUE)
            ATTR_LAYOUTSIZE(L"tabWidth", m_szTab[0], FALSE)
            ATTR_LAYOUTSIZE(L"tabHeight", m_szTab[1], FALSE)
            ATTR_LAYOUTSIZE(L"tabPos", m_nTabPos, FALSE)
            ATTR_LAYOUTSIZE(L"tabInterSize", m_nTabInterSize, FALSE)
            ATTR_SKIN(L"tabInterSkin", m_pSkinTabInter, FALSE)
            ATTR_SKIN(L"tabSkin", m_pSkinTab, FALSE)
            ATTR_SKIN(L"iconSkin", m_pSkinIcon, FALSE)
            ATTR_SKIN(L"frameSkin", m_pSkinFrame, FALSE)
            ATTR_LAYOUTSIZE(L"icon-x", m_ptIcon[0], FALSE)
            ATTR_LAYOUTSIZE(L"icon-y", m_ptIcon[1], FALSE)
            ATTR_LAYOUTSIZE(L"text-x", m_ptText[0], FALSE)
            ATTR_LAYOUTSIZE(L"text-y", m_ptText[1], FALSE)
            ATTR_ENUM_BEGIN(L"tabAlign", int, TRUE)
                ATTR_ENUM_VALUE(L"top", AlignTop)
                ATTR_ENUM_VALUE(L"left", AlignLeft)
                ATTR_ENUM_VALUE(L"right", AlignRight)
                ATTR_ENUM_VALUE(L"bottom", AlignBottom)
            ATTR_ENUM_END(m_nTabAlign)
            ATTR_ENUM_BEGIN(L"textDir", TEXTDIR, TRUE)
                ATTR_ENUM_VALUE(L"hori", Text_Horz)
                ATTR_ENUM_VALUE(L"horizontal", Text_Horz)
                ATTR_ENUM_VALUE(L"vert", Text_Vert)
                ATTR_ENUM_VALUE(L"vertical", Text_Vert)
            ATTR_ENUM_END(m_txtDir)
            ATTR_INT(L"animateSteps",m_nAnimateSteps,FALSE)
			ATTR_INT(L"animateType", m_nAniamteType, FALSE)/*动画样式0：背景跟着动，1：背景不动*/
			ATTR_INTERPOLATOR(L"interpolator",m_aniInterpolator,FALSE)
			ATTR_CHAIN_PTR(m_aniInterpolator,0)//chain attributes to interpolator
        SOUI_ATTRS_END()
    };

}//namespace SOUI
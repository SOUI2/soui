/**
 * Copyright (C) 2014-2050 SOUI团队
 * All rights reserved.
 * 
 * @file       SHeaderCtrl.h
 * @brief      
 * @version    v1.0      
 * @author     soui      
 * @date       2014-07-02
 * 
 * Describe     
 */
#pragma once

#include "core/SWnd.h"
namespace SOUI
{
  #define SHDI_WIDTH               0x0001
  #define SHDI_TEXT                0x0002
  #define SHDI_SORTFLAG            0x0004
  #define SHDI_LPARAM              0x0008
  #define SHDI_ORDER               0x0010
  #define SHDI_VISIBLE             0x0020

  /**
   * @enum      _SHDSORTFLAG
   * @brief     排序标志 
   * 
   * Describe   排序标志
   */
  enum SHDSORTFLAG{
    ST_NULL=0,
    ST_UP,
    ST_DOWN,
  };

  /**
   * @struct    _SHDITEM
   * @brief     列表头项 
   * 
   * Describe   列表头项
   */
  typedef struct SHDITEM {
	  SHDITEM():mask(0),cx(0, SLayoutSize::px),stFlag(ST_NULL),lParam(0),state(0),iOrder(0),bVisible(true){
	  }
    UINT    mask; 
    SLayoutSize     cx;
	STrText strText;
    SHDSORTFLAG stFlag;
    LPARAM  lParam; 
    UINT   state;
    int     iOrder;
	bool    bVisible;
  }*LPSHDITEM;


  /**
   * @class     SHeaderCtrl
   * @brief     表头控件 
   * 
   * Describe   表头控件
   */
  class SOUI_EXP SHeaderCtrl: public SWindow
  {
      SOUI_CLASS_NAME(SHeaderCtrl, L"header")

  public:
      /**
       * SHeaderCtrl::SHeaderCtrl
       * @brief    构造函数
       *
       * Describe  构造函数  
       */
      SHeaderCtrl(void);
      /**
       * SHeaderCtrl::~SHeaderCtrl
       * @brief    析构函数
       *
       * Describe  析构函数  
       */      
      ~SHeaderCtrl(void);

      /**
       * SHeaderCtrl::InsertItem
       * @brief    插入新项
       * @param    int iItem --  新项索引
       * @param    LPCTSTR pszText  --  新项标题
       * @param    int nWidth  -- 宽度
       * @param    SHDSORTFLAG stFlag -- 排序标志
       * @param    LPARAM lParam -- 附加参数
       * @return   返回int 
       *
       * Describe  插入新项  
       */      
      int InsertItem(int iItem,LPCTSTR pszText,int nWidth, SHDSORTFLAG stFlag,LPARAM lParam );
      int InsertItem(int iItem,LPCTSTR pszText, int nWidth, SLayoutSize::Unit unit, SHDSORTFLAG stFlag,LPARAM lParam );
      /**
       * SHeaderCtrl::GetItem
       * @brief    获得新项
       * @param    int iItem  --  索引
       * @param    SHDITEM *pItem  -- 返回列表项结构
       * @return   返回BOOL 
       *
       * Describe  获得新项  
       */      
      BOOL GetItem(int iItem,SHDITEM *pItem);

      /**
       * SHeaderCtrl::GetItemCount
       * @brief    获取列表项个数
       * @return   返回int 
       *
       * Describe  获取列表项个数  
       */      
      size_t GetItemCount() const{return m_arrItems.GetCount();}
      /**
       * SHeaderCtrl::GetTotalWidth
       * @brief    获得所有宽度
       * @return   返回int 
       *
       * Describe  获得所有宽度  
       */      
      int GetTotalWidth();

      /**
       * SHeaderCtrl::GetItemWidth
       * @brief    获得指定项宽度
       * @param    int iItem  --  索引
       * @return   返回int 
       *
       * Describe  获得新项  
       */            
      int GetItemWidth(int iItem);
      /**
       * SHeaderCtrl::DeleteItem
       * @brief    删除指定项
       * @param    int iItem  --  索引
       * @return   返回BOOL 
       *
       * Describe  删除指定项  
       */            
      BOOL DeleteItem(int iItem);

      /**
       * SHeaderCtrl::DeleteAllItems
       * @brief    删除所有项
       *
       * Describe  获得新项  
       */            
      void DeleteAllItems();

      void SetItemSort(int iItem,SHDSORTFLAG stFlag);
      
	  void SetItemVisible(int iItem, bool visible);

	  bool IsItemVisible(int iItem) const;

      SOUI_ATTRS_BEGIN()
          ATTR_SKIN(L"itemSkin",m_pSkinItem,FALSE)
          ATTR_SKIN(L"sortSkin",m_pSkinSort,FALSE)
          ATTR_INT(L"fixWidth",m_bFixWidth,FALSE)
          ATTR_INT(L"itemSwapEnable",m_bItemSwapEnable,FALSE)
          ATTR_INT(L"sortHeader",m_bSortHeader,FALSE)
      SOUI_ATTRS_END()
  protected:
      /**
       * SHeaderCtrl::CreateChildren
       * @brief    创建新项
       * @param    pugi::xml_node xmlNode  -- xml配置文件
       *
       * Describe  创建新项  
       */              
      virtual BOOL CreateChildren(pugi::xml_node xmlNode);

      /**
       * SHeaderCtrl::OnSetCursor
       * @brief    设置选中
       * @param    const CPoint &pt  --  坐标点
       *
       * Describe  获得新项  
       */            
      virtual BOOL OnSetCursor(const CPoint &pt);
      /**
       * SHeaderCtrl::OnPaint
       * @brief    设置选中
       * @param    IRenderTarget * pRT  --  渲染目标
       *
       * Describe  消息响应函数  
       */            
      void OnPaint(IRenderTarget * pRT);

      /**
       * SHeaderCtrl::DrawItem
       * @brief    绘画
       * @param    IRenderTarget * pRT  --  渲染目标
       * @param    CRect rcItem  --  目标区域
       * @param    const LPSHDITEM pItem  --  列表头
       *
       * Describe  绘画  
       */            
      virtual void DrawItem(IRenderTarget * pRT,CRect rcItem,const LPSHDITEM pItem);
        
        
      virtual void OnColorize(COLORREF cr);
       
	  virtual HRESULT OnLanguageChanged();


      /**
       * SHeaderCtrl::GetItemRect
       * @brief    获取表头项得位置
       * @param    UINT iItem  --  索引
       *
       * Describe  获取表头项得位置  
       */            
      CRect GetItemRect(UINT iItem);

      /**
       * SHeaderCtrl::HitTest
       * @brief    
       * @param    CPoint pt  --  坐标
       *
       * Describe  鼠标位置分析，返回一个DWORD，LOWORD为前面的项，
       *           HIWORD为后面的项， 相同时代表在一个项的中间
       */
      DWORD  HitTest(CPoint pt);

      /**
       * SHeaderCtrl::RedrawItem
       * @brief    重绘
       * @param    CPoint pt  --  坐标
       *
       * Describe  重绘
       */
      void RedrawItem(int iItem);

      /**
       * SHeaderCtrl::DrawDraggingState
       * @brief    绘制拖拽状态
       * @param    DWORD dwDragTo  --  
       *
       * Describe  绘制拖拽状态
       */
      void DrawDraggingState(DWORD dwDragTo);

      /**
       * SHeaderCtrl::IsItemHover
       * @brief    判断是否Hover状态
       * @param    DWORD dwHitTest 
       * @return   返回BOOL
       *
       * Describe  判断是否Hover状态
       */
      BOOL IsItemHover(DWORD dwHitTest)
      {
          return dwHitTest!=-1 && LOWORD(dwHitTest)==HIWORD(dwHitTest);
      }

      /**
       * SHeaderCtrl::CreateDragImage
       * @brief    创建拖拽图片
       * @param    UINT iItem -- 列表项
       * @return   返回HBITMAP
       *
       * Describe  判断是否Hover状态
       */
      HBITMAP CreateDragImage(UINT iItem);
      /**
       * SHeaderCtrl::OnLButtonDown
       * @brief    左键按下事件
       * @param    UINT nFlags -- 标志
       * @param    CPoint point -- 鼠标坐标
       *
       * Describe  此函数是消息响应函数
       */
      void OnLButtonDown(UINT nFlags,CPoint pt);
      /**
       * SHeaderCtrl::OnLButtonUp
       * @brief    左键抬起事件
       * @param    UINT nFlags -- 标志
       * @param    CPoint point -- 鼠标坐标
       *
       * Describe  此函数是消息响应函数
       */      
      void OnLButtonUp(UINT nFlags,CPoint pt);
      /**
       * SHeaderCtrl::OnMouseMove
       * @brief    鼠标移动事件
       * @param    UINT nFlags -- 标志
       * @param    CPoint point -- 鼠标坐标
       *
       * Describe  此函数是消息响应函数
       */      
      void OnMouseMove(UINT nFlags,CPoint pt);

      /**
       * SHeaderCtrl::OnMouseLeave
       * @brief    鼠标离开事件
       *
       * Describe  此函数是消息响应函数
       */      
      void OnMouseLeave();

      /**
       * SHeaderCtrl::OnDestroy
       * @brief    销毁事件
       *
       * Describe  此函数是消息响应函数
       */            
      void OnDestroy();
      
      void OnActivateApp(BOOL bActive, DWORD dwThreadID);
      
      SOUI_MSG_MAP_BEGIN()
          MSG_WM_PAINT_EX(OnPaint)
          MSG_WM_LBUTTONDOWN(OnLButtonDown)
          MSG_WM_LBUTTONUP(OnLButtonUp)
          MSG_WM_MOUSEMOVE(OnMouseMove)
          MSG_WM_MOUSELEAVE(OnMouseLeave)
          MSG_WM_ACTIVATEAPP(OnActivateApp)
          MSG_WM_DESTROY(OnDestroy)
		  MESSAGE_HANDLER_EX(UM_SETSCALE, OnSetScale)
      SOUI_MSG_MAP_END()

      ISkinObj *    m_pSkinItem;  /**< 表头绘制Skin */
      ISkinObj *    m_pSkinSort;  /**< 排序标志Skin */
      BOOL          m_bSortHeader;      /**< 表头可以点击排序 */
      BOOL          m_bFixWidth;        /**< 表项宽度固定开关 */
      BOOL          m_bItemSwapEnable;  /**< 允许拖动调整位置开关 */

      BOOL            m_bDragging; /**< 正在拖动标志 */
      HBITMAP         m_hDragImg;  /**< 显示拖动窗口的临时位图 */
      CPoint          m_ptClick;   /**< 当前点击坐标 */
      DWORD           m_dwHitTest; /**< 鼠标位置 */
      DWORD           m_dwDragTo;  /**< 拖放目标 */    
      int             m_nAdjItemOldWidth;  /**< 保存被拖动项的原始宽度 */
      SArray<SHDITEM> m_arrItems; /**< 列表项集合 */
  };
}//end of namespace SOUI

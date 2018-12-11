/**
 * Copyright (C) 2014-2050 SOUI团队
 * All rights reserved.
 * 
 * @file       SScrollBar.h
 * @brief      
 * @version    v1.0      
 * @author     soui      
 * @date       2014-07-08
 * 
 * Describe     
 */
#pragma once
#include "core/SWnd.h"

namespace SOUI
{
/** 
 * @class     SScrollBar
 * @brief     滚动条
 *
 * Describe   滚动条
 */
class SOUI_EXP SScrollBar: public SWindow
{
// Construction
public:
    SOUI_CLASS_NAME(SScrollBar, L"scrollbar")
    /**
     * SScrollBar::SScrollBar
     * @brief    构造函数
     *
     * Describe  构造函数  
     */
    SScrollBar();
    /**
     * SScrollBar::SScrollBar
     * @brief    析构函数
     *
     * Describe  析构函数  
     */
    virtual ~SScrollBar();

public:
    
    /**
     * SScrollBar::IsVertical
     * @brief    是否是竖直
     * @return   返回BOOL 
     *
     * Describe  是否是竖直
     */
    BOOL IsVertical();

    /**
     * SScrollBar::HitTest
     * @brief    测试点
     *
     * Describe  测试某个点是否在滚动条上
     */
    UINT HitTest(CPoint pt);
    
    /**
     * SScrollBar::SetPos
     * @brief    设置位置
     *
     * Describe  设置位置
     */
    int SetPos(int nPos);
    
    /**
     * SScrollBar::GetPos
     * @brief    获取位置
     *
     * Describe  获取位置
     */
    int GetPos();


	/**
	 * SScrollBar::GetMax
	 * @brief    获取最大值
	 *
	 * Describe  获取最大值
	 */
	int GetMax();

	/**
	 * SScrollBar::GetMax
	 * @brief    获取最小值
	 *
	 * Describe  获取最小值
	 */
	int GetMin();
    
protected:    
    /**
     * SScrollBar::GetPartRect
     * @brief    
     * @param   UINT uSBCode -- 
     *
     * Describe  
     */
    CRect GetPartRect(UINT uSBCode);

    /**
     * SScrollBar::OnInitFinished
     * @brief    初始化
     * @param    pugi::xml_node xmlNode  -- 初始化
     *
     * Describe  初始化
     */
    virtual void OnInitFinished(pugi::xml_node xmlNode);

    /**
     * SScrollBar::OnPaint
     * @brief    绘制
     * @param    IRenderTarget * pRT  -- 绘画设备
     *
     * Describe  绘制
     */
    void OnPaint(IRenderTarget * pRT);

    /**
     * SScrollBar::OnLButtonUp
     * @brief    左键抬起
     * @param    UINT nFlags --  虚拟键标志
     * @param    CPoint point -- 坐标
     *
     * Describe  消息响应函数
     */
    void OnLButtonUp(UINT nFlags, CPoint point) ;

    /**
     * SScrollBar::OnLButtonDown
     * @brief    左键按下 
     * @param    UINT nFlags --  虚拟键标志
     * @param    CPoint point -- 坐标
     *
     * Describe  消息响应函数
     */
    void OnLButtonDown(UINT nFlags, CPoint point) ;

    /**
     * SScrollBar::OnMouseMove
     * @brief    鼠标移动
     * @param    UINT nFlags --  虚拟键标志
     * @param    CPoint point -- 坐标
     *
     * Describe  消息响应函数
     */
    void OnMouseMove(UINT nFlags, CPoint point) ;

    /**
     * SScrollBar::OnTimer
     * @brief    定时器 
     * @param    char nIDEvent  -- 定时器ID
     *
     * Describe  定时器ID
     */
    void OnTimer(char nIDEvent) ;

    /**
     * SScrollBar::OnMouseLeave
     * @brief    鼠标移动事件 
     *
     * Describe  消息响应函数 
     */
    void OnMouseLeave();

    /**
     * SScrollBar::OnSetScrollInfo
     * @brief    设置滚动条信息
     * @param    UINT uMsg  -- 消息码
     * @param    WPARAM wParam  
     * @param    LPARAM lParam  
     *
     * Describe  消息响应函数 
     */
    LRESULT OnSetScrollInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);

    /**
     * SScrollBar::OnGetScrollInfo
     * @brief    获取滚动条信息 
     * @param    UINT uMsg  -- 消息码
     * @param    WPARAM wParam  
     * @param    LPARAM lParam  
     *
     * Describe  自定义消息响应函数 
     */
    LRESULT OnGetScrollInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);

    /**
     * SScrollBar::NotifySbCode
     * @brief    通知 
     * @param    UINT uCode  -- 消息码
     * @param    int nPos  -- 坐标
     *
     * Describe  自定义消息响应函数 
     */
    LRESULT NotifySbCode(UINT uCode,int nPos);

protected:
    SOUI_ATTRS_BEGIN()
        ATTR_SKIN(L"skin", m_pSkin, FALSE)
        ATTR_UINT(L"arrowSize", m_uAllowSize, FALSE)
        ATTR_INT(L"min", m_si.nMin, FALSE)
        ATTR_INT(L"max", m_si.nMax, FALSE)
        ATTR_INT(L"value", m_si.nPos, FALSE)
        ATTR_INT(L"page", m_si.nPage, FALSE)
        ATTR_INT(L"vertical", m_bVertical, FALSE)
    SOUI_ATTRS_END()

    SOUI_MSG_MAP_BEGIN()
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_LBUTTONUP(OnLButtonUp)
        MSG_WM_MOUSEMOVE(OnMouseMove)
        MSG_WM_MOUSELEAVE(OnMouseLeave)
        MSG_WM_TIMER_EX(OnTimer)
        MSG_WM_PAINT_EX(OnPaint)
        MESSAGE_HANDLER_EX(SBM_SETSCROLLINFO,OnSetScrollInfo)
        MESSAGE_HANDLER_EX(SBM_GETSCROLLINFO,OnGetScrollInfo)
    SOUI_MSG_MAP_END()

protected:
    ISkinObj    *m_pSkin;        /**< 皮肤对象 */
    UINT        m_uAllowSize;    /**< 大小 */

    SCROLLINFO  m_si;         /**< 结构体 */
    BOOL        m_bDrag;      /**< 是否允许拖动 */
    CPoint      m_ptDrag;     /**< 拖动坐标 */
    int         m_nDragPos;   /**< 拖动位置 */
    UINT        m_uClicked;   /**<  */
    BOOL        m_bNotify;    /**<  */
    UINT        m_uHtPrev;    /**<  */
    BOOL        m_bVertical;  /**< 是否是垂直滚动条 */
    
};

}//namespace SOUI

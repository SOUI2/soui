/**
 * Copyright (C) 2014-2050 SOUI团队
 * All rights reserved.
 * 
 * @file       SCalendar.h
 * @brief      日历时间控件
 * @version    v1.0      
 * @author     soui      
 * @date       2014-05-25
 * 
 * Describe    此类完成日历控件
 */

#pragma once
#include "core/SWnd.h"

namespace SOUI
{
    /**
     * @class      SCalendarCore
     * @brief      日历核心类
     * 
     * Describe    此类是日历的核心类 大部分函数都是静态函数
     */
    class SOUI_EXP SCalendarCore
    {
    public:
        /**
         * SCalendarCore::IsLeapYear
         * @brief    判断闰年
         * @param    WORD wYear -- 待判断的公历年份
		 * @param	 BOOL &bLeapYear -- 【输出参数】TRUE：闰年，FALSE：平年
         * @return   TRUE -- 成功  FALSE -- 失败
         *
         * Describe  判断是否是闰年
					 注意：有效范围是（1600年到6999年）
         */        
        static BOOL IsLeapYear(WORD wYear,BOOL &bLeapYear);

		/**
         * SCalendarCore::GetDaysNumInYear
         * @brief    计算日期在年内的序数（序数从0开始，即0为元旦）
         * @param    WORD wYear -- 公历年
		 * @param	 WORD wMonth -- 公历月
		 * @param	 WORD wDay -- 公历日
		 * @param	 WORD &wDays -- 【输出参数】年内序数
         * @return   TRUE -- 成功  FALSE -- 失败
         *
         * Describe  输入日期，计算该日期在这一年内的序数，序数从0开始
					 注意：有效范围是（START_YEAR年到END_YEAR-1年）
         */ 
		static BOOL GetDaysNumInYear(WORD wYear, WORD wMonth, WORD wDay,WORD &wDays);

		/**
         * SCalendarCore::GetDateFromDays
         * @brief    从年内序数计算月、日
         * @param    WORD wYear -- 公历年
		 * @param	 WORD wDays -- 年内序数
		 * @param	 WORD &wMonth -- 【输出参数】公历月
		 * @param	 WORD &wDay -- 【输出参数】公历日
         * @return   TRUE -- 成功  FALSE -- 失败
         *
         * Describe  输入年份以及年内序数，计算出月和日
					 注意：有效范围是（START_YEAR年到END_YEAR-1年
         */ 
		static BOOL GetDateFromDays(WORD wYear, WORD wDays, WORD &wMonth, WORD &wDay);

        /**
         * SCalendarCore::GetDayOfWeek
         * @brief    返回星期几
         * @param    WORD wYear -- 公历年
         * @param    WORD wMonth -- 公历月
         * @param    WORD wDay -- 公历日
         * @return   返回:0,1,2,3,4,5,6分别对应日、一、二、三、四、五、六，-1表示日期错误或超出范围
         *
         * Describe  输入年月日返回星期几 
         *           注意:有效范围是(START_YEAR年1月1日 --- END_YEAR-1年12月31日)
         */
		static short GetDayOfWeek(WORD wYear,WORD wMonth,WORD wDay);

        /**
         * SCalendarCore::GetDaysOfMonth
         * @brief    返回指定月份的天数
         * @param    WORD wYear -- 公历年
         * @param    WORD wMonth -- 公历月
         * @return   返回该月的天数，返回0表示输入年月有误
         *
         * Describe  输入年月返回本月的天数 
         *           注意:有效范围是(START_YEAR年1月 --- END_YEAR-1年12月)
         */
        static WORD GetDaysOfMonth(WORD wYear,WORD wMonth);


		/**
         * SCalendarCore::DateCheck
         * @brief    检验年、月、日的合法性
         * @param    WORD wYear  -- 公历年 
         * @param    WORD wMonth  -- 公历月 
         * @param    WORD wDay  -- 公历日 
         * @return	 FALSE-失败，TRUE-成功
         *
         * Describe  输入公历日期，检查日期的合法性
         *           注意:有效范围是(START_YEAR年1月1日---END_YEAR-1年12月31日)
         */
		static BOOL DateCheck(WORD wYear,WORD wMonth,WORD wDay);

        /**
         * SCalendarCore::FormatYear
         * @brief    格式化年份显示型式
         * @param    WORD iYear -- 年
         *
         * Describe  指定阴历年返回采用干支纪年法
         */        
        static SStringT FormatYear(WORD  iYear);

        /**
         * SCalendarCore::FormatMonth
         * @brief    格式化月份显示型式
         * @param    WORD iMonth -- 月
         */        
        static SStringT FormatMonth(WORD iMonth);
        
        /**
         * SCalendarCore::FormatDay
         * @brief    格式化日期显示型式
         * @param    WORD iDay -- 日
         */
        static SStringT FormatDay(WORD wiDay);

    };

    /**
     * @class      SCalendar
     * @brief      日历类
     * 
     * Describe    此类是日历的核心类 大部分函数都是静态函数
     */
    class SOUI_EXP SCalendar : public SWindow
    {
    public:
        SOUI_CLASS_NAME(SCalendar, L"calendar")
        
        /**
         * SCalendarCore::SCalendar
         * @brief    构造函数
         * @param    WORD iYear -- 年
         * @param    WORD iMonth -- 月
         * @param    WORD iDay -- 日
         *
         * Describe  构造函数
         */
        SCalendar(WORD iYear, WORD iMonth, WORD iDay);

        /**
         * SCalendar::SCalendar
         * @brief    构造函数
         *
         * Describe  构造函数
         */
        SCalendar();

    public:
        /**
         * SCalendar::GetYear
         * @brief    获得年
         *
         * Describe  获得年
         */
        WORD GetYear(){return m_iYear;}
        
        /**
         * SCalendar::GetMonth
         * @brief    获得月
         *
         * Describe  获得月
         */
        WORD GetMonth(){return m_iMonth;}

        /**
         * SCalendar::GetDay
         * @brief    获得天
         *
         * Describe  获得天
         */
        WORD GetDay(){return m_iDay;}
        
        /**
         * SCalendar::GetDate
         * @brief    获得日期
         * @param    WORD iYear -- 年
         * @param    WORD iMonth -- 月
         * @param    WORD iDay -- 日         
         *
         * Describe  获得日期
         */
        void GetDate(WORD &iYear, WORD &iMonth, WORD &iDay);

        /**
         * SCalendar::SetDate
         * @brief    设置日期
         * @param    WORD iYear -- 年
         * @param    WORD iMonth -- 月
         * @param    WORD iDay -- 日         
         *
         * Describe  设置日期
         */        
        BOOL SetDate(WORD iYear, WORD iMonth, WORD iDay,BOOL bNotify=TRUE);

    protected:
      /**
       * SCalendar::Init
       * @brief    初始化函数
       *
       * Describe  初始化函数
       */            
        void Init();

		int OnCreate(LPVOID);

        /**
         * SCalendar::OnPaint
         * @brief    绘画消息
         * @param    IRenderTarget *pRT -- 绘制设备句柄
         *
         * Describe  此函数是消息响应函数
         */
        void OnPaint(IRenderTarget *pRT);
        
        /**
         * SCalendar::OnLButtonDown
         * @brief    鼠标左键抬起事件
         * @param    UINT nFlags -- 标志
         * @param    CPoint point -- 鼠标坐标
         *
         * Describe  此函数是消息响应函数
         */
        void OnLButtonDown(UINT nFlags, CPoint point);
        
        /**
         * SCalendar::OnMouseMove
         * @brief    鼠标移动事件
         * @param    UINT nFlags -- 标志
         * @param    CPoint point -- 鼠标坐标
         *
         * Describe  此函数是消息响应函数
         */
        void OnMouseMove(UINT nFlags,CPoint pt);
        
        /**
         * SCalendar::OnMouseLeave
         * @brief    鼠标离开事件
         *
         * Describe  此函数是消息响应函数
         */
        void OnMouseLeave();

    protected:
        /**
         * SCalendar::GetDayRect
         * @brief    获得日期的坐标
         * @param    WORD iDay  -- 日期         
         *
         * Describe  根据日期所在的周以及星期几，来计算坐标
         */    
        CRect GetDayRect(WORD iDay);
        WORD HitTest(CPoint  pt);

        /**
         * SCalendar::DrawTitle
         * @brief    绘制标题
         * @param    IRenderTarget *pRT -- 绘制设备句柄         
         *
         * Describe  返回只是该日期所在的坐标，采用CRect表示
         */    
        virtual void DrawTitle(IRenderTarget *pRT);
        
        /**
         * SCalendar::DrawDay
         * @brief    绘制日期--天
         * @param    IRenderTarget *pRT -- 绘制设备句柄         
         *
         * Describe  绘制日期--天
         */    
        virtual void DrawDay(IRenderTarget *pRT,CRect & rcDay,WORD iDay );

        /**
         * SCalendar::DrawDate
         * @brief    绘制日期
         * @param    IRenderTarget *pRT -- 绘制设备句柄         
         *
         * Describe  绘制日期
         */            
        void DrawDate(IRenderTarget *pRT);

        
        /**
         * SCalendar::RedrawDay
         * @brief    重新绘制日期--天
         * @param    CDCHandle dc -- 绘制设备句柄         
         *
         * Describe  重新绘制日期--天
         */    
        void RedrawDay(WORD iDay);

        SOUI_ATTRS_BEGIN()
            ATTR_LAYOUTSIZE(L"titleHeight", m_nTitleHei, FALSE)
            ATTR_LAYOUTSIZE(L"footerHeight", m_nFooterHei, FALSE)
            ATTR_COLOR(L"colorWeekend", m_crWeekend, FALSE)
            ATTR_COLOR(L"colorTitleBack", m_crTitleBack, FALSE)
            ATTR_COLOR(L"colorDay", m_crDay, FALSE)
            ATTR_SKIN(L"daySkin", m_pDaySkin, FALSE)
            ATTR_SKIN(L"titleSkin", m_pTitleSkin, FALSE)
            ATTR_I18NSTRT(L"title-1", m_strTitle[0], FALSE)
            ATTR_I18NSTRT(L"title-2", m_strTitle[1], FALSE)
            ATTR_I18NSTRT(L"title-3", m_strTitle[2], FALSE)
            ATTR_I18NSTRT(L"title-4", m_strTitle[3], FALSE)
            ATTR_I18NSTRT(L"title-5", m_strTitle[4], FALSE)
            ATTR_I18NSTRT(L"title-6", m_strTitle[5], FALSE)
            ATTR_I18NSTRT(L"title-7", m_strTitle[6], FALSE)
        SOUI_ATTRS_END()

        SOUI_MSG_MAP_BEGIN()
			MSG_WM_CREATE(OnCreate)
            MSG_WM_PAINT_EX(OnPaint)
            MSG_WM_LBUTTONDOWN(OnLButtonDown)
            MSG_WM_MOUSEMOVE(OnMouseMove)
            MSG_WM_MOUSELEAVE(OnMouseLeave)
        SOUI_MSG_MAP_END()
    protected:
        SLayoutSize     m_nTitleHei;    /**< 表头高度 */
        SLayoutSize     m_nFooterHei;   /**< 表尾高度 */
        ISkinObj    *m_pDaySkin;    /**< 日期项皮肤 */
        ISkinObj    *m_pTitleSkin;  /**< 表头皮肤 */

        COLORREF        m_crWeekend;    /**< 周末文字颜色 */
        COLORREF        m_crTitleBack;  /**< 表头背景色 */
        COLORREF        m_crDay;        /**< 选中日期颜色 */
        COLORREF        m_crDayBack;    /**< 选中日期背景颜色 */
        STrText			m_strTitle[7];  /**< 表头文本 */

        WORD    m_iYear, m_iMonth, m_iDay; /**< 年月日 */
        int        m_iHoverDay;

    };
}//end of namespace


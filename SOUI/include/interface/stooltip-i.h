#pragma once

#include <unknown/obj-ref-i.h>
#include "../core/smsgloop.h"

namespace SOUI
{
    /** 
     * @struct    TIPID
     * @brief     标识一个tooltip的ID
     *
     * Describe
     */
    struct TIPID
    {
        DWORD dwHi; //ID1, 用来保存一个SWND。
        DWORD dwLow;//ID2, 用来保存附加数据
    };

    /** 
     * @struct    IToolTip
     * @brief     tooltip对象接口
     *
     * Describe
     */
    struct IToolTip : public IMessageFilter
    {
        /**
         * UpdateTip
         * @brief    更新当前的tooltip
         * @param    const TIPID & id --  tooltip的ID
         * @param    CRect rc --  tooltip的感应区
         * @param    LPCTSTR pszTip --  tooltip字符串
         * @param    int nScale -- 当前的缩放比例
         * @return   void 
         *
         * Describe  
         */
        virtual void UpdateTip(const TIPID &id, CRect rc,LPCTSTR pszTip, int nScale) = 0;

        /**
         * ClearTip
         * @brief    清除当前的tooltip
         * @return   void 
         *
         * Describe  
         */
        virtual void ClearTip() = 0;

        /**
         * RelayEvent
         * @brief    鼠标消息的中继
         * @param    const MSG * pMsg --  鼠标消息
         * @return   void 
         *
         * Describe  
         */
        virtual void RelayEvent(const MSG *pMsg) = 0;
    };

    /** 
     * @struct    IToolTipFactory
     * @brief     tooltip类厂
     *
     * Describe
     */
    struct IToolTipFactory : IObjRef
    {
        /**
         * CreateToolTip
         * @brief    创建tooltip对象
         * @param    HWND hHost --  soui host
         * @return   IToolTip * 创建的tooltip对象
         *
         * Describe  不要将hHost做为tooltip的Owner，否则可能导致程序运行异常。
         */
        virtual IToolTip * CreateToolTip(HWND hHost) = 0;

        /**
         * DestroyToolTip
         * @brief    销毁tooltip对象
         * @param    IToolTip * pToolTip --  待销毁tooltip
         * @return   void 
         *
         * Describe  
         */
        virtual void DestroyToolTip(IToolTip *pToolTip) = 0;
    };
}
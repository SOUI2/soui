/**
 * Copyright (C) 2014-2050 SOUI团队
 * All rights reserved.
 * 
 * @file       SDateTimeEdit.h
 * @brief      通用控件
 * @version    v1.0      
 * @author     soui      
 * @date       2014-06-27
 * 
 * Describe    时间控件
 */

#pragma once

#include <control/SRichEdit.h>
#include <helper/STime.h>

namespace SOUI{

//===========================================================================
// Summary:
//     SMaskEdit is a CXTPEdit derived class. It allows text masking to be
//     applied to the control to format it for special editing restrictions.
//===========================================================================
/**
 * @class      SMaskEdit
 * @brief      隐码输入框
 * 
 * Describe    隐码输入框  主要用于固定输入格式
 */
class SMaskEdit : public SEdit
{
    SOUI_CLASS_NAME(SMaskEdit, L"maskedit")

public:
    /**
     * SMaskEdit::SMaskEdit
     * @brief    构造函数
     *
     * Describe  构造函数
     */
    SMaskEdit();
    /**
     * SMaskEdit::CanUseMask
     * @brief    是否启用mask功能
     * @return   返回BOOL
     * 
     * Describe  是否启用mask功能
     */
    BOOL CanUseMask() const;
    /**
     * SMaskEdit::SetUseMask
     * @brief    设置mask功能
     * @param    BOOL bUseMask  -- TRUE FALSE
     * 
     * Describe  设置mask功能
     */
    void SetUseMask(BOOL bUseMask);
    /**
     * SMaskEdit::CanOverType
     * @brief    是否启用改写功能
     * @param    返回BOOL
     * 
     * Describe  是否启用改写功能
     */
    BOOL CanOverType() const;

    /**
     * SMaskEdit::SetOverType
     * @brief    设置改写功能
     * @param    BOOL bOverType  -- TRUE FALSE     
     * 
     * Describe  设置改写功能
     */    
    void SetOverType(BOOL bOverType);

    /**
     * SMaskEdit::PosInRange
     * @brief    是否启用mask功能
     * @param    返回BOOL
     * 
     * Describe  是否启用mask功能
     */
    BOOL PosInRange(int nPos) const;

    /**
     * SMaskEdit::CanUseMask
     * @brief    是否启用mask功能
     * @param    返回BOOL
     * 
     * Describe  是否启用mask功能
     */
    TCHAR GetPromptChar() const;
    /**
     * SMaskEdit::CanUseMask
     * @brief    是否启用mask功能
     * @param    返回BOOL
     * 
     * Describe  是否启用mask功能
     */    
    SStringT GetPromptString(int nLength) const;
    /**
     * SMaskEdit::CanUseMask
     * @brief    是否启用mask功能
     * @param    返回BOOL
     * 
     * Describe  是否启用mask功能
     */  
    void  SetPromptChar(TCHAR ch, BOOL bAutoReplace = TRUE);


    BOOL            MaskCut();
    BOOL            MaskCopy();
    void            MaskReplaceSel(LPCTSTR lpszNewText);
    BOOL            MaskPaste();
    void            MaskDeleteSel();
    BOOL            MaskClear();
    void            MaskSelectAll();
    BOOL            IsModified() const;
    void            SetMaskedText(LPCTSTR lpszMaskedText, int nPos = 0, BOOL bUpdateWindow = TRUE);
    virtual BOOL    SetEditMask(LPCTSTR lpszMask, LPCTSTR lpszLiteral, LPCTSTR lpszDefault=NULL);
    TCHAR           ConvertUnicodeAlpha(TCHAR nChar, BOOL bUpperCase) const;
    virtual BOOL    CheckChar(TCHAR& nChar, int nPos);
    virtual BOOL    ProcessMask(TCHAR& nChar, int nEndPos);

    void            DeleteCharAt(int nPos);
    void            InsertCharAt(int nPos, TCHAR nChar);

    SStringT         GetMaskedText(int nStartPos = 0, int nEndPos = -1) const;

    void            GetMaskState(BOOL bCorrectSelection = TRUE);
    void            SetMaskState();

    void            MaskGetSel();

    BOOL            CopyToClipboard(const SStringT& strText);

    BOOL            IsPromptPos(int nPos) const;
    BOOL            IsPromptPos(const SStringT& strLiteral, int nPos) const;

    BOOL            CorrectPosition(int& nPos, BOOL bForward = TRUE);

    void            CorrectWindowText();

    virtual BOOL    IsPrintChar(TCHAR nChar);
    virtual BOOL    IsAlphaChar(TCHAR nChar);
    virtual void    NotifyPosNotInRange();
    virtual void    NotifyInvalidCharacter(TCHAR /*nChar*/, TCHAR /*chMask*/);
    void            ProcessChar(TCHAR nChar);

protected:
  
    /**
     * SMaskEdit::OnCreate
     * @brief    创建消息响应函数
     * @param    LPVOID
     * @return   返回值int
     *
     * Describe  此函数是消息响应函数
     */
    int  OnCreate(LPVOID);
  
    /**
     * SMaskEdit::OnChar
     * @brief    字符消息
     * @param    UINT nChar -- 按键对应的码值 
     * @param    UINT nRepCnt -- 重复次数
     * @param    UINT nFlags -- 标志
     * 
     * Describe  此函数是消息响应函数
     */ 
    void  OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
  
    /**
     * SMaskEdit::OnKeyDown
     * @brief    键盘按下事件
     * @param    UINT nChar -- 按键对应的码值 
     * @param    UINT nRepCnt -- 重复次数
     * @param    UINT nFlags -- 标志
     * 
     * Describe  此函数是消息响应函数
     */ 
    void  OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  
    /**
     * SMaskEdit::OnSetFocus
     * @brief    设置焦点
     * 
     * Describe  此函数是消息响应函数
     */
    void  OnSetFocus(SWND wndOld);
protected:
    int             m_nStartChar;       // Current position of the first character in the current selection.
    int             m_nEndChar;         // Current position of the first non-selected character past the end of the current selection.
    BOOL            m_bUseMask;         // TRUE to use the edit mask.
    BOOL            m_bOverType;        // TRUE to over type the text, set with VK_INSERT key press.
    BOOL            m_bModified;        // TRUE if mask edit has been modified.
    TCHAR           m_chPrompt;         // Prompt character used to identify the text entry.
    SStringT         m_strMask;          // Buffer that holds the actual edit mask value.
    SStringT         m_strDefault;       // Contains the edit controls default display text.
    SStringT         m_strWindowText;    // Buffer that holds the actual edit text.
    SStringT         m_strLiteral;       // Literal format that restricts where the user can enter text.

protected:
   SOUI_MSG_MAP_BEGIN()
        MSG_WM_CREATE(OnCreate)
        MSG_WM_CHAR(OnChar)
        MSG_WM_KEYDOWN(OnKeyDown)
        MSG_WM_SETFOCUS_EX(OnSetFocus)
   SOUI_MSG_MAP_END()
};

/**
 * @class      SDateEdit
 * @brief      日期编辑框
 * 
 * Describe    日期编辑框
 */
class  SDateEdit : public SMaskEdit
{
    SOUI_CLASS_NAME(SDateEdit, L"dateedit")

public:
      
    /**
     * SDateEdit::SDateEdit
     * @brief    构造函数
     *
     * Describe  构造函数
     */
    SDateEdit();
    /**
     * SDateEdit::SetDateTime
     * @brief    设置日期
     * @param    LPCTSTR strDate -- 日期字符串
     *
     * Describe  设置日期
     */
    virtual void SetDateTime(LPCTSTR strDate);
    /**
     * SDateEdit::SetDateTime
     * @brief    设置日期
     * @param    CTime tm -- 日期类对象
     *
     * Describe  设置日期
     */
    virtual void SetDateTime(CTime tm);
    
    /**
     * SDateEdit::GetWindowDateTime
     * @brief    获取系统时间
     * @return   SStringT 返回字符串    
     *
     * Describe  获取系统时间
     */
    virtual SStringT GetWindowDateTime();
       
    virtual BOOL    ProcessMask(TCHAR& nChar, int nEndPos);
protected:
    /**
     * SDateEdit::OnCreate
     * @brief    消息响应函数
     * @return   int    
     *
     * Describe  消息响应函数
     */
    int OnCreate(LPVOID);

protected:
   SOUI_MSG_MAP_BEGIN()
        MSG_WM_CREATE(OnCreate)
   SOUI_MSG_MAP_END()
};

//===========================================================================
//  CDxTimeEdit
/**
 * @class      STimeEdit
 * @brief      时间编辑框
 * 
 * Describe    时间编辑框
 */
class  STimeEdit : public SDateEdit
{
    SOUI_CLASS_NAME(STimeEdit, L"timeedit")

public:
    /**
     * STimeEdit::STimeEdit
     * @brief    构造函数
     *
     * Describe  构造函数
     */
    STimeEdit();

public:
    
    /**
     * SDateEdit::SetHours
     * @brief    设置小时
     *
     * Describe  设置小时
     */
    virtual void    SetHours(int nHours);
    /**
     * SDateEdit::SetMins
     * @brief    设置分钟
     *
     * Describe  设置分钟
     */
    virtual void    SetMins(int nMins);
    /**
     * SDateEdit::SetTime
     * @brief    设置时间
     * @param    int nHours -- 小时
     * @param    int nMins  -- 分钟
     *
     * Describe  设置时间
     */
    virtual void    SetTime(int nHours, int nMins);
    /**
     * SDateEdit::GetHours
     * @brief    获取小时
     * @return   返回int
     *
     * Describe  获取小时
     */

    int             GetHours() const;
    /**
     * SDateEdit::GetMins
     * @brief    获取分钟
     * @return   返回int
     *
     * Describe  设置分钟
     */

    int             GetMins() const;

    virtual BOOL    ProcessMask(TCHAR& nChar, int nEndPos);
    void            SetMilitary(BOOL bMilitary = TRUE);

protected:
    int             m_nHours;  /**< 小时 */
    int             m_nMins;   /**< 分钟 */
    BOOL            m_bMilitary; /**<  */

protected:
    /**
     * STimeEdit::OnCreate
     * @brief    消息响应函数
     * @return   int    
     *
     * Describe  消息响应函数
     */
    int             OnCreate(LPVOID);

protected:
   SOUI_MSG_MAP_BEGIN()
        MSG_WM_CREATE(OnCreate)
   SOUI_MSG_MAP_END()
};

//////////////////////////////////////////////////////////////////////
inline int STimeEdit::GetHours() const
{
    return m_nHours;
}

inline int STimeEdit::GetMins() const
{
    return m_nMins;
}

inline void STimeEdit::SetMilitary(BOOL bMilitary)
{
    m_bMilitary = bMilitary;

}


}//end of namespace

#include "souistd.h"
#include "core/Accelerator.h"

namespace SOUI
{

    CAccelerator::CAccelerator(UINT vKey,bool bCtrl,bool bAlt,bool bShift):m_wVK(vKey),m_wModifier(0)
    {
        if(bCtrl) m_wModifier|=MOD_CONTROL;
        if(bAlt) m_wModifier |=MOD_ALT;
        if(bShift) m_wModifier |= MOD_SHIFT;
    }

    CAccelerator::CAccelerator(DWORD dwAccel):m_wVK(LOWORD(dwAccel)),m_wModifier(HIWORD(dwAccel))
    {

    }

    CAccelerator::~CAccelerator(void)
    {
    }

    SStringT CAccelerator::FormatHotkey()
    {
        if(m_wModifier==0 && m_wVK==0) return _T("无");
        SStringT str;
        if(m_wModifier == MOD_ALT) str=_T("Alt+");
        else if(m_wModifier == MOD_CONTROL) str=_T("Ctrl+");
        else if(m_wModifier == MOD_SHIFT) str=_T("Shift+");
        else if(m_wModifier == (MOD_ALT|MOD_SHIFT)) str=_T("Shift+Alt+");
        else if(m_wModifier == (MOD_ALT|MOD_CONTROL)) str=_T("Ctrl+Alt+");
        else if(m_wModifier == (MOD_CONTROL|MOD_SHIFT)) str=_T("Ctrl+Shift+");
        else if(m_wModifier == (MOD_ALT|MOD_CONTROL|MOD_SHIFT)) str=_T("Ctrl+Shift+Alt+");
        str+=GetKeyName(m_wVK);
        return str;
    }

    SStringT CAccelerator::GetKeyName( WORD vk )
    {
        SStringT str;
        switch(vk)
        {
        case VK_ESCAPE:
            str=_T("ESC");
            break;
        case VK_RETURN:
            str=_T("Enter");
            break;
        case VK_UP:
            str=_T("Up");
            break;
        case VK_DOWN:
            str=_T("Down");
            break;
        case VK_LEFT:
            str=_T("Left");
            break;
        case VK_RIGHT:
            str=_T("Right");
            break;
        case VK_HOME:
            str=_T("Home");
            break;
        case VK_END:
            str=_T("End");
            break;
        case VK_PRIOR:
            str=_T("PageUp");
            break;
        case VK_NEXT:
            str=_T("PageDown");
            break;
        case VK_INSERT:
            str=_T("Insert");
            break;
        default:
            if((vk>='0' && vk<='9')||(vk>='A' && vk<='Z'))
                str=(TCHAR)vk;
            else if(vk>=VK_NUMPAD0 && vk<=VK_NUMPAD9)
                str.Format(_T("Num %d"),vk-VK_NUMPAD0);
            else if(vk==VK_MULTIPLY)
                str=_T("Num *");
            else if(vk==VK_ADD)
                str=_T("Num +");            
            else if(vk==VK_DECIMAL)
                str=_T("Num Del");            
            else if(vk>=VK_F1 && vk<=VK_F12)
                str.Format(_T("F%d"),vk-VK_F1+1);
            else
            {
                char c=MapVirtualKeyA(vk,2);
                switch(c)
                {
                case '-':
                case '=':
                case '[':
                case ']':
                case '\\':
                case ';':
                case '\'':
                case ',':
                case '.':
                case '/':
                case '`':
                    str+=TCHAR(c);
                    break;
                }
            }
            break;
        }
        return str;
    }

    //将字符串翻译为加速键
    DWORD CAccelerator::TranslateAccelKey( LPCTSTR pszAccelKey )
    {
        TCHAR szBuf[101]={0};//保证字符串结束有两个结束符
        WORD wModifier=0;
        WORD wKey=0;
        int nKeyLen=(int)_tcslen(pszAccelKey);
        if(_tcslen(pszAccelKey)>=100) return 0;
        _tcscpy(szBuf,pszAccelKey);
        CharLowerBuff(szBuf,nKeyLen);

        LPTSTR pszBuf=szBuf;
        LPTSTR pszKey=_tcstok(pszBuf,_T("+"));
        while(pszKey)
        {
            if(_tcscmp(pszKey,_T("ctrl"))==0)
            {
                wModifier|=MOD_CONTROL;
            }else if(_tcscmp(pszKey,_T("alt"))==0)
            {
                wModifier |=MOD_ALT;
            }else if(_tcscmp(pszKey,_T("shift"))==0)
            {
                wModifier |= MOD_SHIFT;
            }else
            {
                if(_tcscmp(pszKey,_T("esc"))==0)
                    wKey=VK_ESCAPE;
                else if(_tcscmp(pszKey,_T("enter"))==0)
                    wKey=VK_RETURN;
                else if(_tcscmp(pszKey,_T("up"))==0)
                    wKey=VK_UP;
                else if(_tcscmp(pszKey,_T("down"))==0)
                    wKey=VK_DOWN;
                else if(_tcscmp(pszKey,_T("left"))==0)
                    wKey=VK_LEFT;
                else if(_tcscmp(pszKey,_T("right"))==0)
                    wKey=VK_RIGHT;
                else if(_tcscmp(pszKey,_T("home"))==0)
                    wKey=VK_HOME;
                else if(_tcscmp(pszKey,_T("pageup"))==0)
                    wKey=VK_PRIOR;
                else if(_tcscmp(pszKey,_T("pagedown"))==0)
                    wKey=VK_NEXT;
                else if(_tcscmp(pszKey,_T("insert"))==0)
                    wKey=VK_INSERT;
                else if( _tcslen(pszKey)>1)
                {
                    if(pszKey[0]==_T('f'))
                    {//F1-F12
                        wKey=VK_F1+_tstoi(pszKey+1)-1;
                    }else if(_tcsncmp(pszKey,_T("num "),4)==0)
                    {//Num 0 - Num 9 || Num Del
                        if(_tcscmp(pszKey+4,_T("del"))==0)
                            wKey=VK_DECIMAL;
                        else if(_tcscmp(pszKey+4,_T("*"))==0)
                            wKey=VK_MULTIPLY;
                        else if(_tcscmp(pszKey+4,_T("+"))==0)
                            wKey=VK_ADD;
                        else
                            wKey=VK_NUMPAD0+_tstoi(pszKey+4);
                    }
                }else//if( _tcslen(pszKey)==1)
                {
                    if(pszKey[0]>=_T('a') && pszKey[0]<=_T('z'))//a-z
                        wKey=pszKey[0]-0x20;
                    else if(pszKey[0]>=_T('0') && pszKey[0]<=_T('9'))
                        wKey=pszKey[0];
                    else if(pszKey[0] == _T('-'))
                        wKey=VK_OEM_MINUS;
                    else if(pszKey[0] == _T('='))
                        wKey=VK_OEM_PLUS;
                    else if(pszKey[0] == _T(','))
                        wKey=VK_OEM_COMMA;
                    else if(pszKey[0] == _T('.'))
                        wKey=VK_OEM_PERIOD;
                    else if(pszKey[0] == _T(';'))
                        wKey=VK_OEM_1;
                    else if(pszKey[0] == _T('/'))
                        wKey=VK_OEM_2;
                    else if(pszKey[0] == _T('`'))
                        wKey=VK_OEM_3;
                    else if(pszKey[0] == _T('['))
                        wKey=VK_OEM_4;
                    else if(pszKey[0] == _T('\\'))
                        wKey=VK_OEM_5;
                    else if(pszKey[0] == _T(']'))
                        wKey=VK_OEM_6;
                    else if(pszKey[0] == _T('\''))
                        wKey=VK_OEM_7;
                    
                }
                break;
            }
            pszBuf+=_tcslen(pszKey)+1;
            pszKey=_tcstok(pszBuf,_T("+"));
        }
        return MAKELONG(wKey,wModifier);
    }

}//end of namespace SOUI

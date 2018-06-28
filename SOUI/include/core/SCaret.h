#pragma once

namespace SOUI{
    class SOUI_EXP SCaret : public TObjRefImpl<IObjRef>
    {
    public:
        SCaret(SWND swnd,HBITMAP hBmp,int nWid,int nHei);
        
        virtual ~SCaret(void);
        
        virtual RECT Draw(IRenderTarget *pRT,int x, int y, BOOL bErase);
        
    protected:
        CAutoRefPtr<IBitmap>    m_bmpCaret;         /**< 模拟插入符*/
        SWND                    m_owner;            /**< 光标宿主窗口*/
    };
}


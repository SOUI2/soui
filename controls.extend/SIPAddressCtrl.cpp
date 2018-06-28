#include "StdAfx.h"
#include "SIPAddressCtrl.h"

#define SEP_EDIT    2

namespace SOUI
{
    class SEditIP: public SEdit
    {
        SOUI_CLASS_NAME(SIPAddressCtrl,L"ipedit")
    public:
        SEditIP();
        ~SEditIP();
        inline const BYTE GetFiled(){
            SStringT str = GetWindowText();
            return (BYTE)_ttoi(str);
            }
        inline void SetField(BYTE nField){
            SStringT str;
            str.Format(_T("%u"),nField);
            SetWindowText(str);
            }
        inline void ClearEdit(){SetField(0);}
    protected:
        //重载OnFinalRelease来保证new和delete在同一个模块。这样就不需要向系统的窗口类厂注册SEditIP类了。
        virtual void OnFinalRelease()
        {
            delete this;
        }
        void OnChar(UINT nChar,UINT nRepCnt,UINT nFlags);
        void OnSetFocus(SWND wndOld);
        void OnKillFocus(SWND wndFocus);
        SOUI_MSG_MAP_BEGIN()
            MSG_WM_CHAR(OnChar)
            MSG_WM_SETFOCUS_EX(OnSetFocus)
            MSG_WM_KILLFOCUS_EX(OnKillFocus)
        SOUI_MSG_MAP_END()
    private:
        HIMC m_hImcCopy;
    };

	SEditIP::SEditIP():m_hImcCopy(NULL)
	{
	}
	
	SEditIP::~SEditIP()
	{
		
	}

	void SEditIP::OnChar(UINT nChar,UINT nRepCnt,UINT nFlags)
	{
		if(nChar == '.' || isdigit(nChar))
		{
			if (nChar != '.')
				__super::OnChar(nChar,nRepCnt,nFlags);

			if(GetWindowTextLength() > 2 || nChar == '.')
			{
				SWindow *pSwnd = GetWindow(GSW_NEXTSIBLING);
				if (NULL != pSwnd && pSwnd->IsClass(SEditIP::GetClassName()))
				{
					pSwnd->SetFocus();
				}
			}
		}
	}
	

    void SEditIP::OnSetFocus(SWND wndOld)
    {
        __super::OnSetFocus(wndOld);
        HWND hHost = GetContainer()->GetHostHwnd();
        m_hImcCopy = ImmGetContext(hHost);
        ImmAssociateContext(hHost,NULL);
    }

    void SEditIP::OnKillFocus(SWND wndFocus)
    {
        __super::OnKillFocus(wndFocus);
        HWND hHost = GetContainer()->GetHostHwnd();
        ImmAssociateContext(hHost,m_hImcCopy);
        m_hImcCopy = 0;
        
        SStringT strValue = GetWindowText();
        UINT uiValue = _ttoi(strValue);
        if (uiValue > 255)
        {
            SetField(255);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //
	SIPAddressCtrl::SIPAddressCtrl(void)
	{
	    memset(m_editFields,0,sizeof(m_editFields));
	}

	SIPAddressCtrl::~SIPAddressCtrl(void)
	{
	}

	void SIPAddressCtrl::OnSize( UINT nType, CSize size )
	{
		__super::OnSize(nType,size);
		
		if(!m_editFields[0]) return;
		
		CRect rcClient;
		GetClientRect(&rcClient);
		
		int nEditWid = (rcClient.Width() - SEP_EDIT*3)/4;
		CRect rcEdit = rcClient;
		rcEdit.right =rcEdit.left + nEditWid;
		for(int i=0;i<4;i++)
		{
		    m_editFields[i]->Move(&rcEdit);
		    rcEdit.OffsetRect(nEditWid+SEP_EDIT,0);
		}
	}

    LRESULT SIPAddressCtrl::OnCreate( LPVOID )
    {
        wchar_t szEditAttr[] = L"<ipedit margin=\"0\" number=\"1\" transparent=\"1\" align=\"center\" maxBuf=\"3\" mouseRelay=\"1\"/>";
        pugi::xml_document xmlDoc;
        xmlDoc.load_buffer(szEditAttr,sizeof(szEditAttr));
        for(int i=0;i<4;i++)
        {
            m_editFields[i] = new SEditIP;//直接new出来。
            InsertChild(m_editFields[i]);
            m_editFields[i]->InitFromXml(xmlDoc.first_child());
        }
        return 0;
    }

    void SIPAddressCtrl::OnPaint( IRenderTarget *pRT )
    {
        CRect rcClient;
        GetClientRect(&rcClient);
        int nEditWid = (rcClient.Width()-SEP_EDIT*3)/4;
        CRect rcSep =rcClient;
        rcSep.left += nEditWid;
        rcSep.right =rcSep.left + SEP_EDIT;
        for(int i=0;i<3;i++)
        {
            CRect rcDot = rcSep;
            rcDot.top += (rcSep.Height()-SEP_EDIT)/2;
            rcDot.bottom = rcDot.top+SEP_EDIT+1;
            rcDot.right += 1;
			pRT->FillSolidRect(rcDot,RGBA(153,153,153,255));
            rcSep.OffsetRect(nEditWid+SEP_EDIT,0);
        }
    }

	BOOL SIPAddressCtrl::IsBlank() const
	{
		if (m_editFields[0]->GetWindowText().GetLength() == 0 && m_editFields[1]->GetWindowText().GetLength() == 0 &&
			m_editFields[2]->GetWindowText().GetLength() == 0 && m_editFields[3]->GetWindowText().GetLength() == 0)
		{
			return TRUE;
		}
		return FALSE;
	}
	
	int SIPAddressCtrl::GetAddress(_Out_ BYTE& nField0,_Out_ BYTE& nField1,_Out_ BYTE& nField2,_Out_ BYTE& nField3) const
	{
		nField0 = m_editFields[0]->GetFiled();
		nField1 = m_editFields[1]->GetFiled();
		nField2 = m_editFields[2]->GetFiled();
		nField3 = m_editFields[3]->GetFiled();
		return 0;
	}
	
	int SIPAddressCtrl::GetAddress(_Out_ DWORD& dwAddress) const
	{
	    SASSERT(m_editFields[0]);
	    dwAddress = MAKELONG(MAKEWORD(m_editFields[0]->GetFiled(),m_editFields[1]->GetFiled()),
	                         MAKEWORD(m_editFields[2]->GetFiled(),m_editFields[3]->GetFiled()));
		return 0;
	}
	
	void SIPAddressCtrl::SetAddress(_In_ DWORD dwAddress)
	{
		in_addr inaddr;
		inaddr.s_addr = dwAddress;
		SetAddress(inaddr.S_un.S_un_b.s_b1,inaddr.S_un.S_un_b.s_b2,
					inaddr.S_un.S_un_b.s_b3,inaddr.S_un.S_un_b.s_b4);
	}
	
	void SIPAddressCtrl::SetAddress(_In_ BYTE nField0,_In_ BYTE nField1,_In_ BYTE nField2,_In_ BYTE nField3)
	{
		m_editFields[0]->SetField(nField0);
		m_editFields[1]->SetField(nField1);
		m_editFields[2]->SetField(nField2);
		m_editFields[3]->SetField(nField3);
	}
	
	void SIPAddressCtrl::ClearAddress()
	{
		m_editFields[0]->ClearEdit();
		m_editFields[1]->ClearEdit();
		m_editFields[2]->ClearEdit();
		m_editFields[3]->ClearEdit();
	}
}




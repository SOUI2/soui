/********************************************************************
	created:	2014/11/03
	created:	3:11:2014   16:13
	filename: 	SIPAddressCtrl.h
	author:		±ù
	
	purpose:	SOUI°æµÄIP¿Ø¼þ
*********************************************************************/
#pragma once

namespace SOUI
{
	
	class SEditIP;
	
	class SIPAddressCtrl :
		public SWindow
	{
		SOUI_CLASS_NAME(SIPAddressCtrl,L"ipctrl")
	public:
		SIPAddressCtrl(void);
		~SIPAddressCtrl(void);

		BOOL IsBlank() const;
		void ClearAddress();
		int GetAddress(_Out_ BYTE& nField0,_Out_  BYTE& nField1,_Out_  BYTE& nField2,_Out_  BYTE& nField3) const;
		int GetAddress(_Out_ DWORD& dwAddress) const;

		void SetAddress(_In_ DWORD dwAddress);
		void SetAddress(_In_ BYTE nField0,_In_ BYTE nField1,_In_ BYTE nField2,_In_ BYTE nField3);

		void SetFieldFocus(WORD nField);
		void SetFieldRange(int nField, BYTE nLower, BYTE nUpper);
	protected:
		void OnPaint(IRenderTarget *pRT);
		LRESULT OnCreate(LPVOID); 
		void OnSize( UINT nType, CSize size );
		
		SOUI_MSG_MAP_BEGIN()
		    MSG_WM_CREATE(OnCreate)
			MSG_WM_SIZE(OnSize)
			MSG_WM_PAINT_EX(OnPaint)
		SOUI_MSG_MAP_END()
	private:
		SEditIP * m_editFields[4];
	};
}

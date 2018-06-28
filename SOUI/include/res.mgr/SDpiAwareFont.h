#pragma once


namespace SOUI
{
	class SOUI_EXP SDpiAwareFont
	{
	public:
		SDpiAwareFont(void);
		~SDpiAwareFont(void);


		IFont * GetFontPtr();

		void SetFontDesc(const SStringW & strDesc, int nScale);

		void SetScale(int nScale);

	protected:
		IFont * OnGetFont(const SStringW & strDesc,int nScale);

		SStringW m_strDesc;
		CAutoRefPtr<IFont> m_object;
	};

}

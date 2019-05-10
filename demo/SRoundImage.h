#pragma once
namespace SOUI
{
	class SRoundImage : public SImageWnd
	{
		SOUI_CLASS_NAME(SRoundImage,L"roundImg")
	private:
		CAutoRefPtr<IBitmap> m_img;
	public:
		SRoundImage();
		~SRoundImage();

	protected:
		CAutoRefPtr<IBitmap> GetRoundImage() const;

		void OnPaint(IRenderTarget *pRT);
		void OnSize(UINT nType, CSize size);
		SOUI_MSG_MAP_BEGIN()
			MSG_WM_SIZE(OnSize)
			MSG_WM_PAINT_EX(OnPaint)
		SOUI_MSG_MAP_END()
	};

}


#include "souistd.h"
#include "res.mgr/SDpiAwareFont.h"

namespace SOUI
{
	SDpiAwareFont::SDpiAwareFont(void)
	{
	}

	SDpiAwareFont::~SDpiAwareFont(void)
	{
	}

	IFont * SDpiAwareFont::GetFontPtr()
	{
		return m_object;
	}

	void SDpiAwareFont::SetFontDesc(const SStringW & strDesc, int nScale)
	{
		m_strDesc = strDesc;
		m_object = OnGetFont(m_strDesc, nScale);
	}

	void SDpiAwareFont::SetScale(int nScale)
	{
		if (m_strDesc.IsEmpty()) return;
		m_object = OnGetFont(m_strDesc, nScale);
	}

	IFont * SDpiAwareFont::OnGetFont(const SStringW & strDesc,int nScale)
	{
		return SFontPool::getSingleton().GetFont(strDesc,nScale);
	}


}

#include "stdafx.h"
#include "SCheckBox2.h"

namespace SOUI
{
	SCheckBox2::SCheckBox2(void)
	{
	}

	SCheckBox2::~SCheckBox2(void)
	{
	}


	void SCheckBox2::OnLButtonUp(UINT nFlags, CPoint point)
	{
		if((GetState()&WndState_PushDown) && GetWindowRect().PtInRect(point))
		{
			if (!IsChecked())
				ModifyState(WndState_Check, 0,TRUE);
		}

		SWindow::OnLButtonUp(nFlags,point);
	}


	HRESULT SCheckBox2::OnAttrCheck(const SStringW& strValue, BOOL bLoading)
	{
		SetCheck(STRINGASBOOL(strValue));
		return bLoading?S_FALSE:S_OK;
	}

}

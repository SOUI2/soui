#pragma once

namespace SOUI
{
	class SWindow;

	struct __declspec(uuid("9FAF3067-6723-40DA-A56B-CDCB11B8902B")) 
	IAccHelper : public IUnknown
	{
		virtual void SetOwner(SWindow *pOwner) = 0;
		virtual SWindow *GetOwner() const = 0;
	};
}
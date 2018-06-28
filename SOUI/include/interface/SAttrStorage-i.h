#pragma once

namespace SOUI
{
	struct IAttrStorage : public IObjRef
	{
		virtual void OnSetAttribute(const SStringW & strName, const SStringW & strValue);
		virtual SStringW OnGetAttribute(const SStringW & strName) const;
	};

	struct IAttrStorageFactory : public IObjRef
	{
		virtual HRESULT CreateAttrStorage(SObject * owner, const SStringW & strName, const SStringW & strValue,IAttrStorage** ppAttrStorage) const; 
	};

}
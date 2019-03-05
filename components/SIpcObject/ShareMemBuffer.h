// MemMapFile.h: interface for the CShareMemBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SHAREMEMBUFFER_H__015AC2F8_C982_43A8_916D_EEA49B31428B__INCLUDED_)
#define _SHAREMEMBUFFER_H__015AC2F8_C982_43A8_916D_EEA49B31428B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <interface/sipcobj-i.h>

namespace SOUI
{

class CShareMemBuffer : public IShareBuffer
{
public:
	CShareMemBuffer();
	virtual ~CShareMemBuffer();
	BOOL OpenMemFile(LPCTSTR pszName,DWORD dwMaximumSize, void * pSecurityAttr= NULL);
	void Close();

	DWORD GetBufferSize() const { return *(DWORD*)m_pBuffer; }
	DWORD &GetBufferSizeRef() const { return *(DWORD*)m_pBuffer; }
	DWORD GetUsedSize() const { return *(DWORD*)(m_pBuffer+sizeof(DWORD)); }
	DWORD &GetUsedSizeRef() const { return *(DWORD*)(m_pBuffer + sizeof(DWORD)); }


	BYTE * GetBuffer() {return m_pBuffer+ sizeof(DWORD)*2;}
	const BYTE * GetBuffer() const{return m_pBuffer+ sizeof(DWORD)*2;}


public:
	void StartWrite() {
		DWORD & dwWritePos = GetUsedSizeRef();
		dwWritePos = 0;
	}

	void StartRead() {
		m_dwReadPos = 0;
	}

	int Write(const void * pBuf, UINT nLen);
	int Read(void *pBuf, UINT nLen);
protected:
	HANDLE m_hMap;
	
	DWORD  m_dwReadPos;
	BYTE  *m_pBuffer;

};

}

#endif // !defined(_SHAREMEMBUFFER_H__015AC2F8_C982_43A8_916D_EEA49B31428B__INCLUDED_)

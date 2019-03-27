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
#pragma pack(push,4)
	struct BufHeader {
		DWORD dwSize;	   //buf size.
		DWORD dwTailPos;   //data tail pos
		DWORD dwPos;	   //current pos for read and write.
	};
#pragma pack(pop)
public:
	CShareMemBuffer();
	virtual ~CShareMemBuffer();
	BOOL OpenMemFile(LPCTSTR pszName,DWORD dwMaximumSize, void * pSecurityAttr= NULL);
	void Close();

protected:
	BufHeader *GetHeader() { return m_pHeader; }
	BYTE * GetBuffer() { return m_pBuffer; }

	const BufHeader* GetHeader() const { return m_pHeader; }
	const BYTE * GetBuffer() const { return m_pBuffer; }
public:
	// Í¨¹ý IShareBuffer ¼Ì³Ð
	virtual int Write(const void * pBuf, UINT nLen) override;
	virtual int Read(void *pBuf, UINT nLen) override;
	virtual UINT Tell() const override;
	virtual UINT Seek(SEEK mode, int nOffset) override;
	virtual void SetTail(UINT uPos) override;
	virtual BOOL Lock() override;
	virtual void Unlock() override;
protected:
	HANDLE m_hMap;
	void * m_pMemBuf;
	BufHeader * m_pHeader;
	BYTE      * m_pBuffer;
	HANDLE	    m_hMutex;

};

}

#endif // !defined(_SHAREMEMBUFFER_H__015AC2F8_C982_43A8_916D_EEA49B31428B__INCLUDED_)

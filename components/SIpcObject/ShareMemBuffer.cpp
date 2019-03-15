// MemMapFile.cpp: implementation of the CMemMapFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ShareMemBuffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace SOUI
{

CShareMemBuffer::CShareMemBuffer()
{
	m_pBuffer=NULL;
	m_hMap=NULL;
}

CShareMemBuffer::~CShareMemBuffer()
{
	Close();
}

BOOL CShareMemBuffer::OpenMemFile(LPCTSTR pszName,DWORD dwMaximumSize , void * pSecurityAttr)
{
	if(m_hMap) return FALSE;
	if (dwMaximumSize == 0)
	{
		m_hMap = ::OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, pszName);
	}
	else
	{
		SECURITY_ATTRIBUTES *psa = (SECURITY_ATTRIBUTES*)pSecurityAttr;
		m_hMap = ::CreateFileMapping(INVALID_HANDLE_VALUE, psa, PAGE_READWRITE, 0, dwMaximumSize + sizeof(DWORD) * 2, pszName);
	}
	if (!m_hMap)	return FALSE;
	m_pBuffer=(LPBYTE)::MapViewOfFile(m_hMap, FILE_MAP_READ | FILE_MAP_WRITE,0,0,0);//map whole file
	if(!m_pBuffer)
	{
		::CloseHandle(m_hMap);
		m_hMap=0;
		return FALSE;
	}
	if (dwMaximumSize != 0)
	{
		GetBufferSizeRef() = dwMaximumSize;
		StartWrite();
	}
	return TRUE;

}

void CShareMemBuffer::Close(){
	if(m_pBuffer)
	{
		::UnmapViewOfFile(m_pBuffer);
		::CloseHandle(m_hMap);
		m_pBuffer = NULL;
		m_hMap=0;
	}
}

int CShareMemBuffer::Write(const void * pBuf, UINT nLen)
{
	assert(m_pBuffer);
	DWORD & dwWritePos = GetUsedSizeRef();
	UINT nRemain = GetBufferSize() - dwWritePos;
	if (nLen > nRemain) nLen = nRemain;
	memcpy(GetBuffer() + dwWritePos, pBuf, nLen);
	dwWritePos += nLen;
	return nLen;
}

int CShareMemBuffer::Read(void * pBuf, UINT nLen)
{
	DWORD & dwWritePos = GetUsedSizeRef();
	UINT nRemain = (dwWritePos - m_dwReadPos);
	if (nLen > nRemain) nLen = nRemain;
	memcpy(pBuf, GetBuffer() + m_dwReadPos, nLen);
	m_dwReadPos += nLen;
	return nLen;
}

}

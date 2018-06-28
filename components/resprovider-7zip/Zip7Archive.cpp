#include "Zip7Archive.h"
#include <assert.h>

#include "SevenZip/SevenZipExtractor.h"
#include "SevenZip/SevenZipExtractorMemory.h"
#include "SevenZip/SevenZipLister.h" 

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include <crtdbg.h>
#include <tchar.h>
#include <malloc.h>

static std::wstring StdStringtoWideString(const std::string &stdstring)
{
	const char* str = stdstring.c_str();
	size_t len = stdstring.length();
	unsigned int code_page = CP_ACP;

	std::wstring buf;
	int rc_chars = MultiByteToWideChar(code_page,
		0,
		str,
		len,
		NULL,
		0);

	if (len == -1)
		--rc_chars;
	if (rc_chars == 0)
		return buf;

	buf.resize(rc_chars);
	MultiByteToWideChar(code_page,
		0,
		str,
		len,
		const_cast<wchar_t*>(buf.c_str()),
		rc_chars);

	return buf;
}


static std::string WString2String(const std::wstring &wstr)
{
	int len = wstr.length();
	const wchar_t *pStr = wstr.c_str();
	std::string buf;
	 

	if (len < 0 && len != -1)
	{
		return buf;
	}

	// figure out how many narrow characters we are going to get
	int nChars = WideCharToMultiByte(CP_ACP, 0,	pStr, len, NULL, 0, NULL, NULL);
	if (len == -1)
		--nChars;
	if (nChars == 0)
		return "";

	// convert the wide string to a narrow string
	// nb: slightly naughty to write directly into the string like this
	buf.resize(nChars);
	WideCharToMultiByte(CP_ACP, 0, pStr, len, const_cast<char*>(buf.c_str()), nChars, NULL, NULL);

	return buf;
}


    CZipFile::CZipFile(DWORD dwSize/*=0*/)
		: m_dwPos(0)
	{

	}

	CZipFile::~CZipFile()
	{
		Close();
	}

	BlobBuffer &CZipFile::getBlob()
	{
		return m_blob;
	}

	BOOL CZipFile::Read(void* pBuffer, DWORD dwSize, LPDWORD pdwRead/* = NULL*/)
	{
		_ASSERTE(IsOpen());

		if (pdwRead != NULL)
			*pdwRead = 0;

		if (m_blob.GetBlobLength()==0)
			return FALSE;

		if (m_dwPos + dwSize > m_blob.GetBlobLength())
			dwSize = m_blob.GetBlobLength() - m_dwPos;

		::CopyMemory(pBuffer, m_blob.GetBlobPtr() + m_dwPos, dwSize);
		m_dwPos += dwSize;
		if (pdwRead != NULL)
			*pdwRead = dwSize;

		return TRUE;
	}
	BOOL CZipFile::Close()
	{
		m_blob.ClearContent();
		m_dwPos = 0;

		return TRUE;
	}
	BOOL CZipFile::IsOpen() const 
	{
		return (m_blob.GetBlobLength() > 0);
	}
	BYTE* CZipFile::GetData() 
	{
		_ASSERTE(IsOpen());
		return m_blob.GetBlobRealPtr();
	}
	DWORD CZipFile::GetSize() const
	{
		_ASSERTE(IsOpen());
		return m_blob.GetBlobLength();
	}
	DWORD CZipFile::GetPosition() const
	{
		_ASSERTE(IsOpen());
		return m_dwPos;
	}
	DWORD CZipFile::Seek(DWORD dwOffset, UINT nFrom)	//	return old pos
	{
		_ASSERTE(IsOpen());
		DWORD dwPos = m_dwPos;
		switch (nFrom)
		{
		case FILE_BEGIN:
			m_dwPos = dwOffset;
			break;
		case FILE_END:
			m_dwPos = m_blob.GetBlobLength() + dwOffset;
			break;
		case FILE_CURRENT:
			m_dwPos += dwOffset;
			break;
		}
		if (m_dwPos < 0)
			m_dwPos = 0;
		if (m_dwPos >= m_blob.GetBlobLength())
			m_dwPos = m_blob.GetBlobLength();
		return dwPos;
	} 

	BOOL CZipFile::Attach(LPBYTE pData, DWORD dwSize)
	{
		_ASSERTE(pData);
		_ASSERTE(!::IsBadReadPtr(pData,dwSize));

		m_blob.SetBlobContent(pData, dwSize);
		return TRUE;
	}

	void CZipFile::Detach()
	{ 
		m_blob.ClearContent();
		m_dwPos = 0;
	}


	//////////////////////////////////////////////////////////////////////////
	//CZipArchive
	//////////////////////////////////////////////////////////////////////////
	
	CZipArchive::CZipArchive()
	{
	}
	CZipArchive::~CZipArchive()
	{
		Close();
	}

	BOOL CZipArchive::OpenZip()
	{ 
		m_szPassword[0] = '\0';

		return TRUE;
	}
	void CZipArchive::Close()
	{
		CloseFile();
		 
	}
	BOOL CZipArchive::IsOpen() const
	{
		return TRUE;
	} 

	BOOL CZipArchive::SetPassword(LPCSTR pstrPassword)
	{
        if(!pstrPassword) return FALSE;

		if (::lstrlenA(pstrPassword) >= sizeof(m_szPassword)-1)
			return FALSE;

		::lstrcpyA(m_szPassword, pstrPassword);
		return TRUE;
	}

	// ZIP File API

	BOOL CZipArchive::GetFile(LPCTSTR pszFileName, CZipFile& file)
	{
		std::string fileName = WString2String(pszFileName);
		if (m_fileStreams.GetFile(fileName.c_str(),file.getBlob()))
			return TRUE;

		return FALSE;
	}
	 
	BOOL CZipArchive::Open(LPCTSTR pszFileName,LPCSTR pszPassword)
	{
		std::wstring s_pwd = StdStringtoWideString(pszPassword);
		SevenZip::SevenZipPassword pwd(true, s_pwd);
		CFileStream fileStreams;
		SevenZip::SevenZipExtractorMemory decompress;
		decompress.SetArchivePath(pszFileName);
		 
		return (S_OK == decompress.ExtractArchive(m_fileStreams, NULL, &pwd));
	}

	BOOL CZipArchive::Open(HMODULE hModule, LPCTSTR pszName, LPCTSTR pszPassword, LPCTSTR pszType)
	{
		HRSRC hResInfo = ::FindResource(hModule, pszName, pszType);
		if (hResInfo == NULL)
			return FALSE;

		DWORD dwLength = ::SizeofResource(hModule, hResInfo);
		if (dwLength == 0)
			return FALSE;

		HGLOBAL hResData = ::LoadResource(hModule, hResInfo);
		if (hResData == NULL)
			return FALSE;

		BYTE* pData = (BYTE*)::LockResource(hResData);
		if (pData == NULL)
			return FALSE;

		Close();

		m_fileRes.Attach(pData, dwLength);

		BOOL bOK=OpenZip();
		if(!bOK)
		{
			m_fileRes.Detach();
		}
		return bOK;
	}

	void CZipArchive::CloseFile()
	{ 
	}

	DWORD CZipArchive::ReadFile(void* pBuffer, DWORD dwBytes)
	{
		DWORD dwRead = 0;
		 

		return dwRead;
	}
 
	DWORD CZipArchive::GetFileSize( LPCTSTR pszFileName )
	{
		std::string fileName = WString2String(pszFileName);
		return m_fileStreams.GetFileSize(fileName.c_str());
	} 
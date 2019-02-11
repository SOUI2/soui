#include "stdafx.h"
#include "ZipArchive.h"
#include <assert.h>

#ifndef __cplusplus
#error ZipArchive requires C++ compilation (use a .cpp suffix)
#endif

#ifdef ZIP_WILDCARD
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#endif

// Include ZLIB stuff. ZLIB is maintained by Jean-loup Gailly and Mark Adler.
// It is a general GZIP and PKZIP compatible compression library.
#include <zconf.h>
#include <zlib.h>

#include <crtdbg.h>
#include <tchar.h>
#include <malloc.h>


CZipFile::CZipFile(DWORD dwSize/*=0*/)
		: m_pData(NULL),
		m_dwSize(0),
		m_dwPos(0)
	{
#ifdef ZLIB_DECRYPTION
		m_pCrcTable = NULL;
#endif
		if(dwSize!=0)
		{
			m_pData=new BYTE[dwSize];
			m_dwSize=dwSize;
		}
	}
	CZipFile::~CZipFile()
	{
		Close();
	}
	BOOL CZipFile::Read(void* pBuffer, DWORD dwSize, LPDWORD pdwRead/* = NULL*/)
	{
		_ASSERTE(IsOpen());

		if (pdwRead != NULL)
			*pdwRead = 0;
		if (m_pData == NULL)
			return FALSE;
		if (dwSize == 0 || m_dwPos >= m_dwSize)
			return FALSE;

		if (m_dwPos + dwSize > m_dwSize)
			dwSize = m_dwSize - m_dwPos;
		::CopyMemory(pBuffer, m_pData + m_dwPos, dwSize);
		m_dwPos += dwSize;
		if (pdwRead != NULL)
			*pdwRead = dwSize;

		return TRUE;
	}
	BOOL CZipFile::Close()
	{
		if (m_pData == NULL)
			return TRUE;

		delete[] m_pData;
		m_pData = NULL;
		m_dwSize = 0;
		m_dwPos = 0;

		return TRUE;
	}
	BOOL CZipFile::IsOpen() const 
	{
		return m_pData != NULL;
	}
	BYTE* CZipFile::GetData() const
	{
		_ASSERTE(IsOpen());
		return m_pData;
	}
	DWORD CZipFile::GetSize() const
	{
		_ASSERTE(IsOpen());
		return m_dwSize;
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
			m_dwPos = m_dwSize + dwOffset;
			break;
		case FILE_CURRENT:
			m_dwPos += dwOffset;
			break;
		}
		if (m_dwPos < 0)
			m_dwPos = 0;
		if (m_dwPos >= m_dwSize)
			m_dwPos = m_dwSize;
		return dwPos;
	}

#ifdef ZLIB_DECRYPTION
	BOOL CZipFile::_DecryptFile(LPCSTR pstrPassword, LPBYTE& pData, DWORD& dwSize, DWORD crc32)
	{
		_ASSERTE(pData);
		_ASSERTE(!::IsBadStringPtrA(pstrPassword, -1));

		if (!_InitKeys(pstrPassword))
			return FALSE;
		if (!_DecryptHeader(pData, dwSize, crc32))
			return FALSE;
		if (!_DecryptData(pData, dwSize))
			return FALSE;

		return TRUE;
	}

	BOOL CZipFile::_InitKeys(LPCSTR pstrPassword)
	{
		m_pCrcTable = get_crc_table();
		m_dwKey[0] = 305419896;
		m_dwKey[1] = 591751049;
		m_dwKey[2] = 878082192;

		int nLen = ::lstrlenA(pstrPassword);
		if (nLen == 0)
			return FALSE;

		for(int i = 0; i < nLen; i++)
			_UpdateKeys(pstrPassword[i]);

		return TRUE;
	}
	void CZipFile::_UpdateKeys(BYTE c)
	{      
		m_dwKey[0] = _crc32(m_dwKey[0], c);
		m_dwKey[1] = m_dwKey[1] + (m_dwKey[0] & 0x000000FF);
		m_dwKey[1] = m_dwKey[1] * 134775813 + 1;
		BYTE b = (BYTE) (m_dwKey[1] >> 24);
		m_dwKey[2] = _crc32(m_dwKey[2], b);
	}

    DWORD CZipFile::_crc32( DWORD c, BYTE b )
    {
        assert(m_pCrcTable);
        return m_pCrcTable[((DWORD) (c) ^ (b)) & 0xFF] ^ ((c) >> 8);
    }

	BOOL CZipFile::_DecryptHeader(LPBYTE pData, DWORD dwSize, DWORD crc32)
	{
		if (dwSize < 12)
			return FALSE;

		BYTE header[12];
		::CopyMemory(&header, pData, 12);
		for(int i = 0; i < 12; i++)
		{
			BYTE c = (BYTE)(header[i] ^ _DecryptByte());
			_UpdateKeys(c);
			header[i] = c;
		}

		// Password check
		return header[11] == (BYTE)(crc32 >> 24);
	}

    BYTE CZipFile::_DecryptByte() const
    {
        DWORD temp = (WORD) (m_dwKey[2] | 2);
        return (BYTE)((temp * (temp ^ 1)) >> 8);
    }

	BOOL CZipFile::_DecryptData(LPBYTE& pData, DWORD& dwSize)
	{
		LPBYTE pRawData;
		pRawData = new BYTE[dwSize - 12];
		_ASSERTE(pRawData);

		if (pRawData == NULL)
			return FALSE;

		LPBYTE p = pRawData;
		for(DWORD i = 12; i < dwSize; i++)
		{
			BYTE c = (BYTE) (pData[i] ^ _DecryptByte());
			_UpdateKeys(c);
			*p++ = c;
		}

		delete[] pData;
		pData = pRawData;
		dwSize -= 12;

		return TRUE;
	}
#endif	//	ZLIB_DECRYPTION

	BOOL CZipFile::Attach(LPBYTE pData, DWORD dwSize)
	{
		if(m_pData) return FALSE;
		_ASSERTE(pData);
		_ASSERTE(!::IsBadReadPtr(pData,dwSize));

		m_pData = pData;
		m_dwSize = dwSize;
		return TRUE;
	}

	void CZipFile::Detach()
	{
		m_pData = NULL;
		m_dwSize = 0;
		m_dwPos = 0;
	}


	//////////////////////////////////////////////////////////////////////////
	//CZipArchive
	//////////////////////////////////////////////////////////////////////////
	
	CZipArchive::CZipArchive()
		: m_hFile(INVALID_HANDLE_VALUE),
		m_Files(NULL),
		m_DirData(NULL)
	{
		memset(&m_Header, 0, sizeof(m_Header));
	}
	CZipArchive::~CZipArchive()
	{
		Close();
	}

	BOOL CZipArchive::OpenZip()
	{
		_ASSERTE(m_hFile);

		SeekFile(-(LONG)sizeof(m_Header), FILE_END);

		DWORD dwRead = ReadFile(&m_Header, sizeof(m_Header));
		_ASSERTE(dwRead == sizeof(m_Header));

		if (dwRead != sizeof(m_Header) || m_Header.sig != DIR_SIGNATURE)
		{
			Close();
			return FALSE;
		}

		_ASSERTE(m_Header.nDirEntries < 1000); // Sanity check

		m_DirData = (LPBYTE)malloc(m_Header.dirSize);
		_ASSERTE(m_DirData);

		m_Files = new ZipDirFileHeader*[m_Header.nDirEntries];
		_ASSERTE(m_Files);

		if (m_Files == NULL || m_DirData == NULL)
		{
			Close();
			return FALSE;
		}

		SeekFile(-(LONG)(sizeof(m_Header) + m_Header.dirSize), FILE_END);

		dwRead = ReadFile(m_DirData, m_Header.dirSize);
		_ASSERTE(dwRead == m_Header.dirSize);

		if (dwRead != m_Header.dirSize)
		{
			Close();
			return FALSE;
		}

		LPBYTE pData = m_DirData;
		for (int i = 0; i < m_Header.nDirEntries; i++)
		{
			// Set the header pointer in the m_Files array
			ZipDirFileHeader* fh = (ZipDirFileHeader*) pData;
			m_Files[i] = fh;
			if (fh->sig != FILE_SIGNATURE)
			{
				Close();
				return FALSE;
			}
			// Convert UNIX slash to Windows backslash in ANSI string
			LPSTR pszName = fh->GetName();
			for(int j = 0; j < fh->fnameLen; j++, pszName++)
				if (*pszName == '/')
					*pszName='\\';

			// Get next header
			pData += sizeof(ZipDirFileHeader) + fh->fnameLen + fh->xtraLen + fh->cmntLen;
		}

		m_szPassword[0] = '\0';

		return TRUE;
	}
	void CZipArchive::Close()
	{
		CloseFile();

		if (m_Files != NULL)
		{
			delete[] m_Files;
			m_Files = NULL;
		}
		if (m_DirData != NULL)
		{
			free(m_DirData);
			m_DirData = NULL;
		}
		memset(&m_Header, 0, sizeof(m_Header));
	}
	BOOL CZipArchive::IsOpen() const
	{
		return m_hFile != INVALID_HANDLE_VALUE;
	}
	int CZipArchive::GetEntries() const
	{
		return m_Header.nDirEntries;
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
		return GetFile(GetFileIndex(pszFileName),file);
	}

	BOOL CZipArchive::GetFile2(int iIndex, CZipFile& file)
	{
		_ASSERTE(IsOpen());
		_ASSERTE(iIndex >= 0 && iIndex < m_Header.nDirEntries);

		if (m_hFile == INVALID_HANDLE_VALUE)
			return FALSE;
		if (iIndex < 0 || iIndex >= m_Header.nDirEntries)
			return FALSE;

		ZipLocalHeader hdr;
		SeekFile(m_Files[iIndex]->hdrOffset, FILE_BEGIN);

		DWORD dwRead = ReadFile(&hdr, sizeof(hdr));
		if (dwRead != sizeof(hdr))
			return FALSE;

		if (hdr.sig != LOCAL_SIGNATURE)
			return FALSE;

		SeekFile(hdr.fnameLen + hdr.xtraLen, FILE_CURRENT);

		DWORD dwSize = hdr.cSize;
		if (hdr.flag & 1)
		{//这个接口不支持加密
			return FALSE;
		}

		switch (hdr.compression)
		{
		case LOCAL_COMP_STORE:
			_ASSERTE(hdr.cSize == hdr.ucSize);
			dwRead = ReadFile(file.GetData(), hdr.cSize);
			return dwRead==hdr.cSize;
		case LOCAL_COMP_DEFLAT: 
			{
				if(file.GetSize()<hdr.ucSize) return FALSE;
				// Read Source Data.
				LPBYTE pData=new BYTE[hdr.cSize];

				if (pData == NULL)
					return FALSE;

				dwRead = ReadFile(pData, hdr.cSize);
				if (dwRead != hdr.cSize)
				{
					delete[] pData;
					return FALSE;
				}

				LPBYTE pTarget=file.GetData();

				z_stream stream = { 0 };
				stream.next_in = (Bytef*) pData;
				stream.avail_in = (uInt) hdr.cSize;
				stream.next_out = (Bytef*) pTarget;
				stream.avail_out = hdr.ucSize;
				stream.zalloc = (alloc_func) NULL;
				stream.zfree = (free_func) NULL;
				// Perform inflation; wbits < 0 indicates no zlib header inside the data.
				int err = inflateInit2(&stream, -MAX_WBITS);
				if (err == Z_OK)
				{
					err = inflate(&stream, Z_OK);
					if (err != Z_OK && err != Z_STREAM_END)
						_ASSERTE(err == Z_OK);
					inflateEnd(&stream);
					if (err == Z_STREAM_END)
						err = Z_OK;
					inflateEnd(&stream);
				}

				delete[] pData;

				return err == Z_OK;
			}
		default:
			_ASSERTE(FALSE); // unsupported compression scheme
			return FALSE;
		}
	}


	BOOL CZipArchive::GetFile(int iIndex, CZipFile& file)
	{
		_ASSERTE(IsOpen());
		_ASSERTE(iIndex >= 0 && iIndex < m_Header.nDirEntries);

		if (m_hFile == INVALID_HANDLE_VALUE)
			return FALSE;
		if (iIndex < 0 || iIndex >= m_Header.nDirEntries)
			return FALSE;

		ZipLocalHeader hdr;
		SeekFile(m_Files[iIndex]->hdrOffset, FILE_BEGIN);

		DWORD dwRead = ReadFile(&hdr, sizeof(hdr));
		if (dwRead != sizeof(hdr))
			return FALSE;

		if (hdr.sig != LOCAL_SIGNATURE)
			return FALSE;

		SeekFile(hdr.fnameLen + hdr.xtraLen, FILE_CURRENT);

		// 		// Decompress file if needed.
		LPBYTE pData;
		pData = new BYTE[hdr.cSize];

		if (pData == NULL)
			return FALSE;

		dwRead = ReadFile(pData, hdr.cSize);
		if (dwRead != hdr.cSize)
		{
            delete[] pData;
			return FALSE;
		}

		DWORD dwSize = hdr.cSize;
		if (hdr.flag & 1)
		{
#ifdef ZLIB_DECRYPTION
			if (::lstrlenA(m_szPassword) == 0)
			{
				delete[] pData;
				return FALSE;
			}
			if (!file._DecryptFile(m_szPassword, pData, dwSize, hdr.crc32))
			{
                delete[] pData;
				return FALSE;
			}
#else
            delete[] pData;
			return FALSE;
#endif
		}

		switch (hdr.compression)
		{
		case LOCAL_COMP_STORE:
			//_ASSERTE(hdr.cSize == hdr.ucSize);//加密的时这两个值不一定相等
			break;
		case LOCAL_COMP_DEFLAT: 
			{
				LPBYTE pTarget;
				pTarget = new BYTE[hdr.ucSize];
				_ASSERTE(pTarget);

				if (pTarget == NULL)
					return FALSE;

				z_stream stream = { 0 };
				stream.next_in = (Bytef*) pData;
				stream.avail_in = (uInt) hdr.cSize;
				stream.next_out = (Bytef*) pTarget;
				stream.avail_out = hdr.ucSize;
				stream.zalloc = (alloc_func) NULL;
				stream.zfree = (free_func) NULL;
				// Perform inflation; wbits < 0 indicates no zlib header inside the data.
				int err = inflateInit2(&stream, -MAX_WBITS);
				if (err == Z_OK)
				{
					err = inflate(&stream, Z_OK);
					if (err != Z_OK && err != Z_STREAM_END)
						_ASSERTE(err == Z_OK);
					inflateEnd(&stream);
					if (err == Z_STREAM_END)
						err = Z_OK;
					inflateEnd(&stream);
				}

				delete[] pData;

				if (err != Z_OK)
				{
					delete[] pTarget;
					return FALSE;
				}
				pData = pTarget;
			}
			break;
		default:
			_ASSERTE(FALSE); // unsupported compression scheme
			return FALSE;
		}
		// The memory we allocated is passed to the file, which
		// takes ownership of it.
		file.Attach(pData, hdr.ucSize);

		return TRUE;
	}

	// FindFile API

	HANDLE CZipArchive::FindFirstFile(LPCTSTR pszFileName, LPZIP_FIND_DATA lpFindFileData) const
	{
		_ASSERTE(IsOpen());
		_ASSERTE(!::IsBadWritePtr(lpFindFileData, sizeof(ZIP_FIND_DATA)));

		::ZeroMemory(lpFindFileData, sizeof(ZIP_FIND_DATA));

		_ASSERTE(pszFileName != NULL);
		if (pszFileName == NULL || pszFileName[0] == '\0')
			return INVALID_HANDLE_VALUE;

		FindFileHandle* pFF;
		pFF = new FindFileHandle;
		if (pFF == NULL)
			return INVALID_HANDLE_VALUE;

		::lstrcpy(pFF->szSearch, pszFileName);
		pFF->nPos = 0;

		BOOL bRet = FindNextFile((HANDLE)pFF, lpFindFileData);
		if (!bRet)
		{
			delete pFF;
			return INVALID_HANDLE_VALUE;
		}
		return (HANDLE)pFF;
	}
	BOOL CZipArchive::FindNextFile(HANDLE hFindFile, LPZIP_FIND_DATA lpFindFileData) const
	{
		_ASSERTE(IsOpen());

		if (!IsOpen())
			return FALSE;
		if (hFindFile == INVALID_HANDLE_VALUE || hFindFile == NULL)
			return FALSE;

		FindFileHandle* pFF = reinterpret_cast<FindFileHandle*>(hFindFile);
		while (TRUE)
		{
			if (pFF->nPos >= m_Header.nDirEntries)
				return FALSE;

			// Extract filename and match with pattern
			ZipDirFileHeader* fh = m_Files[pFF->nPos];
			TCHAR szFile[MAX_PATH] = { 0 };
			::OemToCharBuff(fh->GetName(), szFile, fh->fnameLen);

#ifdef ZIP_WILDCARD
			if (::PathMatchSpec(szFile, pFF->szSearch) != NULL)
#else
			if (_tcsicmp(szFile, pFF->szSearch) == 0)
#endif
			{
				// Copy data to the ZIP_FIND_DATA structure
				::lstrcpy(lpFindFileData->szFileName, szFile);
				lpFindFileData->szComment[0] = _T('\0'); // unsupported right now
				lpFindFileData->nFileSizeCompressed = fh->cSize;
				lpFindFileData->nFileSizeUncompressed = fh->ucSize;
				::DosDateTimeToFileTime(fh->modDate, fh->modTime, &lpFindFileData->ftCreationDate);
				lpFindFileData->nIndex = pFF->nPos;

				// Figure out the file attributes
				DWORD& dwFlags = lpFindFileData->dwFileAttributes = 0;
				if (fh->flag & 1)
					dwFlags |= FILE_ATTRIBUTE_ENCRYPTED;
				if (fh->flag & (2|4))
					dwFlags |= FILE_ATTRIBUTE_COMPRESSED;
				if (fh->flag & (16|32|64))
					dwFlags |= FILE_ATTRIBUTE_OFFLINE; // unsupported compression used
				if (dwFlags == 0)
					dwFlags = (fh->compression == LOCAL_COMP_STORE) ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;

				// Ready for next entry...
				pFF->nPos++;
				return TRUE;
			}
			pFF->nPos++;
		}
		return FALSE;
	}
	BOOL CZipArchive::FindClose(HANDLE hFindFile) const
	{
		if (hFindFile == INVALID_HANDLE_VALUE || hFindFile == NULL)
			return FALSE;

		FindFileHandle* pFF = reinterpret_cast<FindFileHandle*>(hFindFile);
		delete pFF;
		return TRUE;
	}

	BOOL CZipArchive::Open(LPCTSTR pszFileName)
	{
		_ASSERTE(!::IsBadStringPtr(pszFileName, MAX_PATH));
		HANDLE hFile = ::CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		
		if(INVALID_HANDLE_VALUE==hFile) return FALSE;
		
		Close();
		m_hFile=hFile;
		BOOL bOK=OpenZip();
		if(!bOK)
		{
			CloseHandle(m_hFile);
			m_hFile=INVALID_HANDLE_VALUE;
		}
		return bOK;
	}

	BOOL CZipArchive::Open(LPBYTE pBytes, DWORD dwByteCount) 
	{
		Close();

		m_fileRes.Attach(pBytes, dwByteCount);

		// ?
		m_hFile = (HANDLE)pBytes;

		BOOL bOK=OpenZip();
		if(!bOK)
		{
			m_fileRes.Detach();
			m_hFile=INVALID_HANDLE_VALUE;
		}
		return bOK;
	}

	BOOL CZipArchive::Open( HMODULE hModule,LPCTSTR pszName,LPCTSTR pszType )
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

		m_hFile = (HANDLE)hResInfo;

		BOOL bOK=OpenZip();
		if(!bOK)
		{
			m_fileRes.Detach();
			m_hFile=INVALID_HANDLE_VALUE;
		}
		return bOK;
	}

	void CZipArchive::CloseFile()
	{
		if (m_hFile != INVALID_HANDLE_VALUE)
		{
			if (m_fileRes.IsOpen())
				m_fileRes.Detach();
			else
				::CloseHandle(m_hFile);
			m_hFile = INVALID_HANDLE_VALUE;
		}
	}

	DWORD CZipArchive::ReadFile(void* pBuffer, DWORD dwBytes)
	{
		_ASSERTE(m_hFile != INVALID_HANDLE_VALUE);

		DWORD dwRead = 0;

		if (m_fileRes.IsOpen())
			m_fileRes.Read(pBuffer, dwBytes, &dwRead);
		else
			::ReadFile(m_hFile, pBuffer, dwBytes, &dwRead, NULL);

		return dwRead;
	}
	DWORD CZipArchive::SeekFile(LONG lOffset, UINT nFrom)
	{
		_ASSERTE(m_hFile != INVALID_HANDLE_VALUE);

		if (m_fileRes.IsOpen())
		{
			m_fileRes.Seek(lOffset, nFrom);
			return m_fileRes.GetPosition();
		}

		return ::SetFilePointer(m_hFile, lOffset, NULL, nFrom);
	}

	DWORD CZipArchive::GetFileSize( LPCTSTR pszFileName )
	{
		return GetFileSize(GetFileIndex(pszFileName));
	}

	DWORD CZipArchive::GetFileSize( int iIndex )
	{
		if(iIndex==-1) return 0;

		if (m_hFile == INVALID_HANDLE_VALUE)
			return 0;
		if (iIndex < 0 || iIndex >= m_Header.nDirEntries)
			return 0;

		ZipLocalHeader hdr;
		SeekFile(m_Files[iIndex]->hdrOffset, FILE_BEGIN);

		DWORD dwRead = ReadFile(&hdr, sizeof(hdr));
		if (dwRead != sizeof(hdr))
			return 0;
		return hdr.ucSize;
	}

	int CZipArchive::GetFileIndex( LPCTSTR pszFileName )
	{
		_ASSERTE(IsOpen());
		_ASSERTE(!::IsBadStringPtr(pszFileName, MAX_PATH));

		ZIP_FIND_DATA fd;
		HANDLE hFindFile = FindFirstFile(pszFileName, &fd);
		if (hFindFile == INVALID_HANDLE_VALUE)
			return -1;
		FindClose(hFindFile);
		return fd.nIndex;
	}
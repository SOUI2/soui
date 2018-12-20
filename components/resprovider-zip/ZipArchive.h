#ifndef __ZIPARCHIVE_H__
#define __ZIPARCHIVE_H__

#pragma once

#define ZLIB_DECRYPTION
///////////////////////////////////////////////////////////////
// Structures and defines

typedef struct ZIP_FIND_DATA
{
	TCHAR		szFileName[MAX_PATH];
	TCHAR		szComment[64];
	DWORD		nFileSizeCompressed;
	DWORD		nFileSizeUncompressed;
	FILETIME	ftCreationDate;
	DWORD		dwFileAttributes;
	int			nIndex;
} ZIP_FIND_DATA, *LPZIP_FIND_DATA;

class CZipFile;
class CZipArchive;

//	ZIP file wrapper from zip archive
class CZipFile
{
	friend CZipArchive;
protected:
	// Stream data members
	LPBYTE	m_pData;
	DWORD	m_dwPos;
	DWORD	m_dwSize;
#ifdef ZLIB_DECRYPTION
	// Decryption
	const DWORD*	m_pCrcTable;
	DWORD	        m_dwKey[3];
#endif

public:
	CZipFile(DWORD dwSize=0);
	~CZipFile();
	BOOL Read(void* pBuffer, DWORD dwSize, LPDWORD pdwRead = NULL);
	BOOL Close();
	BOOL IsOpen() const ;
	BYTE* GetData() const;
	DWORD GetSize() const;
	DWORD GetPosition() const;
	DWORD Seek(DWORD dwOffset, UINT nFrom);

	BOOL Attach(LPBYTE pData, DWORD dwSize);
	void Detach();
protected:

#ifdef ZLIB_DECRYPTION
	BOOL _DecryptFile(LPCSTR pstrPassword, LPBYTE& pData, DWORD& dwSize, DWORD crc32);
	DWORD _crc32(DWORD c, BYTE b);
	BOOL _InitKeys(LPCSTR pstrPassword);
	void _UpdateKeys(BYTE c);
	BOOL _DecryptHeader(LPBYTE pData, DWORD dwSize, DWORD crc32);
	BYTE _DecryptByte() const;
	BOOL _DecryptData(LPBYTE& pData, DWORD& dwSize);
#endif	//	ZLIB_DECRYPTION

};

//	ZIP Archive class, load files from a zip archive
class CZipArchive
{
protected:

#pragma pack(2)

	enum
	{
		DIR_SIGNATURE = 0x06054b50,
	};

	struct ZipDirHeader
	{
		DWORD   sig;
		WORD    nDisk;
		WORD    nStartDisk;
		WORD    nDirEntries;
		WORD    totalDirEntries;
		DWORD   dirSize;
		DWORD   dirOffset;
		WORD    cmntLen;
	};

	enum
	{
		FILE_SIGNATURE	= 0x02014b50,
		FILE_COMP_STORE	= 0,
		FILE_COMP_DEFLAT= 8,
	};

	struct ZipDirFileHeader
	{
		DWORD	sig;
		WORD	verMade;
		WORD	verNeeded;
		WORD	flag;
		WORD	compression;		//	COMP_xxxx
		WORD	modTime;
		WORD	modDate;
		DWORD	crc32;
		DWORD	cSize;				//	Compressed size
		DWORD	ucSize;				//	Uncompressed size
		WORD	fnameLen;			//	Filename string follows header.
		WORD	xtraLen;			//	Extra field follows filename.
		WORD	cmntLen;			//	Comment field follows extra field.
		WORD	diskStart;
		WORD	intAttr;
		DWORD	extAttr;
		DWORD	hdrOffset;

		LPSTR GetName() const { return (LPSTR)(this + 1); }
		LPSTR GetExtra() const { return GetName() + fnameLen; }
		LPSTR GetComment() const { return GetExtra() + xtraLen; }
	};

	enum
	{
		LOCAL_SIGNATURE = 0x04034b50,
		LOCAL_COMP_STORE  = 0,
		LOCAL_COMP_DEFLAT = 8,
	};

	struct ZipLocalHeader
	{
		DWORD	sig;
		WORD	version;
		WORD	flag;
		WORD	compression;		//	COMP_xxxx
		WORD	modTime;
		WORD	modDate;
		DWORD	crc32;
		DWORD	cSize;
		DWORD	ucSize;
		WORD	fnameLen;			//	Filename string follows header.
		WORD	xtraLen;			//	Extra field follows filename.
	};

#pragma pack()

	struct FindFileHandle
	{
		TCHAR	szSearch[MAX_PATH];
		int		nPos;
	};

	HANDLE			m_hFile;
	CZipFile		m_fileRes;

	ZipDirHeader	m_Header;
	ZipDirFileHeader** m_Files;
	BYTE*			m_DirData;
	CHAR			m_szPassword[64];

public:
	CZipArchive();
	~CZipArchive();

	BOOL Open(LPCTSTR pszFileName);
	BOOL Open(LPBYTE pBytes, DWORD dwByteCount);
	BOOL Open(HMODULE hModule,LPCTSTR pszName,LPCTSTR pszType=_T("ZIP"));

	void Close();
	BOOL IsOpen() const;
	int GetEntries() const;

	BOOL SetPassword(LPCSTR pstrPassword);

	// ZIP File API

	int GetFileIndex(LPCTSTR pszFileName);
	BOOL GetFile(LPCTSTR pszFileName, CZipFile& file);
	BOOL GetFile(int iIndex, CZipFile& file);
	DWORD GetFileSize(LPCTSTR pszFileName);
	DWORD GetFileSize(int iIndex);
	//file对象中已经分配好内存。
	BOOL GetFile2(LPCTSTR pszFileName, CZipFile& file)
	{
		return GetFile2(GetFileIndex(pszFileName),file);
	}
	BOOL GetFile2(int iIndex, CZipFile& file);
	// FindFile API

	HANDLE FindFirstFile(LPCTSTR pszFileName, LPZIP_FIND_DATA lpFindFileData) const;
	BOOL FindNextFile(HANDLE hFindFile, LPZIP_FIND_DATA lpFindFileData) const;
	BOOL FindClose(HANDLE hFindFile) const;

protected:
	BOOL OpenZip();
	void CloseFile();

	DWORD ReadFile(void* pBuffer, DWORD dwBytes);
	DWORD SeekFile(LONG lOffset, UINT nFrom);
};

#endif	//	__ZIPARCHIVE_H__

#pragma once

namespace SOUI
{
	struct IRenderFactory;
	struct ZIPRES_PARAM
	{
		enum { ZIPFILE, PEDATA, MEMORYDATA } type;
		IRenderFactory *pRenderFac;
		union {
			LPCTSTR pszZipFile;
			struct {
				HINSTANCE hInst;
				LPCTSTR pszResName;
				LPCTSTR pszResType;
			} peInfo;
			struct {
				LPBYTE  pByteBuffer;
				DWORD   dwByteCounts;				
			} Memory;
		};
		LPCSTR          pszPsw; //ZIP密码
		LPCTSTR			pszChildDir;
		void ZipFile(IRenderFactory *_pRenderFac, LPCTSTR _pszFile, LPCSTR _pszPsw = NULL, LPCTSTR _pszChildDir = NULL)
		{
			type = ZIPFILE;
			pszZipFile = _pszFile;
			pszChildDir = _pszChildDir;
			pRenderFac = _pRenderFac;
			pszPsw = _pszPsw;
		}
		void ZipResource(IRenderFactory *_pRenderFac, HINSTANCE hInst, LPCTSTR pszResName, LPCTSTR pszResType = _T("zip"), LPCSTR _pszPsw = NULL, LPCTSTR _pszChildDir = NULL)
		{
			type = PEDATA;
			pRenderFac = _pRenderFac;
			pszChildDir = _pszChildDir;
			peInfo.hInst = hInst;
			peInfo.pszResName = pszResName;
			peInfo.pszResType = pszResType;
			pszPsw = _pszPsw;
		}
		void ZipMemory(IRenderFactory *_pRenderFac, LPBYTE  pByteBuffer, DWORD dwByteCounts, LPCSTR _pszPsw = NULL, LPCTSTR _pszChildDir = NULL)
		{
			type = MEMORYDATA;
			Memory.pByteBuffer = pByteBuffer;
			Memory.dwByteCounts = dwByteCounts;
			pszChildDir = _pszChildDir;
			pRenderFac = _pRenderFac;
			pszPsw = _pszPsw;			
		}
	};
}
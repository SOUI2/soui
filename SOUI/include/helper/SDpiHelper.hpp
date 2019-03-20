#pragma once
#include "SDpiScale.h"
#include <WinUser.h>
#include <wtl.mini/msgcrack.h>
#include <wtl.mini/souimisc.h>
#pragma comment(lib,"version.lib")

namespace SOUI{

	class SDpiHelper
	{
	public:
		//获取一个PE文件的version
		static BOOL PEVersion(LPCTSTR pszFileName, WORD &wMajor, WORD &wMinor, WORD & wVer3, WORD & wVer4)
		{
			DWORD dwResHandle;
			void *pBuf;
			BOOL bRet = FALSE;
			DWORD dwVerInfoSize = GetFileVersionInfoSize(pszFileName, &dwResHandle);
			if (!dwVerInfoSize) return FALSE;
			pBuf = malloc(dwVerInfoSize);
			GetFileVersionInfo(pszFileName, dwResHandle, dwVerInfoSize, pBuf);
			UINT nVersionLen;
			VS_FIXEDFILEINFO *pstFileVersion;
			if (VerQueryValue(pBuf, _T("\\"), (void**)&pstFileVersion, &nVersionLen) && nVersionLen >= sizeof(VS_FIXEDFILEINFO))
			{
				wVer4 = LOWORD(pstFileVersion->dwFileVersionLS);
				wVer3 = HIWORD(pstFileVersion->dwFileVersionLS);
				wMinor = LOWORD(pstFileVersion->dwFileVersionMS);
				wMajor = HIWORD(pstFileVersion->dwFileVersionMS);
			}
			free(pBuf);
			return TRUE;
		}


		static BOOL IsVerOrGreater(WORD wVers[4], WORD wMajor, WORD wMinor, WORD wSpBuild = 0)
		{
			if (wVers[0] < wMajor)
				return FALSE;
			if (wVers[0] > wMajor)
				return TRUE;
			if (wVers[1] < wMinor)
				return FALSE;
			if (wVers[1] > wMinor)
				return TRUE;

			return wVers[2] >= wSpBuild;
		}

		static int getScale(HWND hWnd)
		{
			WORD wVers[4];
			PEVersion(_T("ntdll.dll"), wVers[0], wVers[1], wVers[2], wVers[3]);
			//win7
			if (!IsVerOrGreater(wVers, 6, 1, 7600))
				return 100;
			int nScale = 100;
			//For Win10 1607
			if (IsVerOrGreater(wVers, 10, 0, 14955))
			{
				HMODULE hModule = LoadLibrary(_T("User32.dll"));
				if (hModule)
				{
					typedef UINT(WINAPI *FunGetDpiForWindow)(HWND);

					FunGetDpiForWindow GetDpiForWindow = (FunGetDpiForWindow)GetProcAddress(hModule, "GetDpiForWindow");
					if (GetDpiForWindow)
					{
						UINT dpi = GetDpiForWindow(hWnd);
						nScale = dpi * 100 / 96;
					}
					FreeLibrary(hModule);
				}
			}
			//for win 8.1 or later
			else  if (IsVerOrGreater(wVers, 6, 3))
			{
				HMODULE hModule = LoadLibrary(_T("Shcore.dll"));
				if (hModule)
				{
					CRect winrc;
					GetWindowRect(hWnd, &winrc);
					HMONITOR hMonitor = MonitorFromRect(winrc, MONITOR_DEFAULTTONEAREST);
					UINT dpiX, dpiY;
					typedef enum _MONITOR_DPI_TYPE {
						MDT_EFFECTIVE_DPI = 0,
						MDT_ANGULAR_DPI = 1,
						MDT_RAW_DPI = 2,
						MDT_DEFAULT = MDT_EFFECTIVE_DPI
					} MONITOR_DPI_TYPE;
					typedef HRESULT(WINAPI *FunGetDpiForMonitor)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);

					FunGetDpiForMonitor GetDpiForMonitor = (FunGetDpiForMonitor)GetProcAddress(hModule, "GetDpiForMonitor");
					if (GetDpiForMonitor)
					{
						HRESULT hr = GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
						if (SUCCEEDED(hr))
						{
							nScale = dpiX * 100 / 96;
						}
					}
					FreeLibrary(hModule);
				}
			}
			else
			{
				nScale = getScaleOld(hWnd);
			}
			return nScale;
		}
	private:
		static int getScaleOld(HWND hWnd = NULL)
		{
			HDC screen = ::GetDC(hWnd);
			int nScale = 100;
			nScale = GetDeviceCaps(screen, LOGPIXELSX) * 100 / 96;
			ReleaseDC(hWnd, screen);
			return nScale;
		}
	};

	template <class T>
	class SDpiHandler
	{
	protected:
		void OnDpiChanged(WORD dpi, const RECT* desRect)
		{
			T *pT = static_cast<T*>(this);

			int nScale = dpi * 100 / 96;
			nScale = SDpiScale::NormalizeScale(nScale);
			HandleScaleChange(nScale, desRect);
		}
		//能过重写些函数来屏蔽一些不处理的DPI。。比如只支持 100 150 200。。。所有都支持基本不现实
		virtual void HandleScaleChange(WORD nScale, const RECT *desRect)
		{
			T *pT = static_cast<T*>(this);

			if (nScale != pT->SWindow::GetScale()) //As ShostWnd::GetScale is under protect, here, we using SWindow::GetScale.
			{
				pT->SDispatchMessage(UM_SETSCALE, nScale, 0);
				pT->SetWindowPos(
					NULL,
					desRect->left,
					desRect->top,
					desRect->right - desRect->left,
					desRect->bottom - desRect->top,
					SWP_NOZORDER | SWP_NOACTIVATE);
			}
		}
	public:
		BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0)
		{
			BOOL bHandled = TRUE;

			switch (dwMsgMapID)
			{
			case 0:
				if (uMsg == WM_DPICHANGED)
				{
					OnDpiChanged((WORD)HIWORD(wParam), (RECT* const)lParam);
					lResult = 0;
				}
				break;
			}
			return FALSE;
		}
	};

}

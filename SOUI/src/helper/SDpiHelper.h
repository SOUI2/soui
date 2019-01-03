#pragma once
#include <WinUser.h>
#include "Ms_SouiExpand_Config.hpp"
// #include <d2d1.h>
// 
// #pragma comment(lib,"D2d1.lib")
class MS_SOUIEXPAND_API CDpiHelper
{
public:
	// 	static bool IsVistaOrLater()
	// 	{
	// 		DWORD dwMajorVer, dwMinorVer, dwBuildNumber;
	// 		if (GetNtVersionNumbers(dwMajorVer, dwMinorVer, dwBuildNumber))
	// 		{
	// 			return (dwMinorVer >= 6);
	// 		}
	// 		return false;
	// 	}
	// 	//-------------------------------------------------------------------------
	// 	// 函数    : GetNtVersionNumbers
	// 	// 功能    : 调用RtlGetNtVersionNumbers获取系统版本信息
	// 	// 返回值  : BOOL
	// 	// 参数    : DWORD& dwMajorVer 主版本
	// 	// 参数    : DWORD& dwMinorVer 次版本
	// 	// 参数    : DWORD& dwBuildNumber build号
	// 	// 附注    :
	// 	//-------------------------------------------------------------------------
	static BOOL GetNtVersionNumbers(DWORD&dwMajorVer, DWORD& dwMinorVer, DWORD& dwBuildNumber)
	{
		BOOL bRet = FALSE;
		HMODULE hModNtdll = NULL;
		if (hModNtdll = ::LoadLibraryW(L"ntdll.dll"))
		{
			typedef void (WINAPI *pfRTLGETNTVERSIONNUMBERS)(DWORD*, DWORD*, DWORD*);
			pfRTLGETNTVERSIONNUMBERS pfRtlGetNtVersionNumbers;
			pfRtlGetNtVersionNumbers = (pfRTLGETNTVERSIONNUMBERS)::GetProcAddress(hModNtdll, "RtlGetNtVersionNumbers");
			if (pfRtlGetNtVersionNumbers)
			{
				pfRtlGetNtVersionNumbers(&dwMajorVer, &dwMinorVer, &dwBuildNumber);
				dwBuildNumber &= 0x0ffff;
				bRet = TRUE;
			}

			::FreeLibrary(hModNtdll);
			hModNtdll = NULL;
		}

		return bRet;
	}

	static BOOL IsVerOrGreater(DWORD dwMajorVersion, DWORD dwMinorVersion, DWORD dwBuildNumber = 0)
	{
		OSVERSIONINFOEXW osvi = { 0 };
		DWORDLONG dwlConditionMask = 0;
		ZeroMemory(&osvi, sizeof(osvi));
		osvi.dwOSVersionInfoSize = sizeof(osvi);
		osvi.dwMajorVersion = dwMajorVersion;
		osvi.dwMinorVersion = dwMinorVersion;
		// 主版本号判断
		VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_GREATER);
		if (::VerifyVersionInfo(&osvi, VER_MAJORVERSION, dwlConditionMask))
			return TRUE;
		// 次版本号判断
		VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_EQUAL);
		VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_GREATER);
		if (::VerifyVersionInfo(&osvi, VER_MAJORVERSION | VER_MINORVERSION, dwlConditionMask))
			return TRUE;
		//build num
		VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_EQUAL);
		VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_EQUAL);
		VER_SET_CONDITION(dwlConditionMask, VER_BUILDNUMBER, VER_GREATER_EQUAL);
		return ::VerifyVersionInfo(&osvi, VER_MAJORVERSION | VER_MINORVERSION, dwlConditionMask);
	}

public:
	static int getScaleOld(HWND hWnd = NULL)
	{
		HDC screen = ::GetDC(hWnd);
		int nScale = 100;
		nScale = GetDeviceCaps(screen, LOGPIXELSX) * 100 / 96;
		ReleaseDC(hWnd, screen);
		return nScale;
	}
	//看了网吧的WIN7根本没有这个注册表项
	static int getScaleFormReg()
	{
		//HKEY_CURRENT_USER\Control Panel\Desktop\LogPixels
		HKEY hKey;
		LSTATUS ec = RegOpenKeyEx(HKEY_CURRENT_USER,
			LR"(Control Panel\Desktop)",
			0,
			KEY_READ, &hKey);
		if (ec == ERROR_SUCCESS)
		{
			DWORD dwType = REG_DWORD;
			DWORD dwSize = sizeof(DWORD);
			DWORD dpi;
			if (ERROR_SUCCESS == RegQueryValueEx(hKey, L"LogPixels", 0, &dwType, (LPBYTE)&dpi, &dwSize))
			{

			}
			RegCloseKey(hKey);
		}
		return 0;
	}

	static int getScale(HWND hWnd)
	{
		//win7
		if (!IsVerOrGreater(6, 1, 7600))
			return 100;
		int nScale = 100;
		//For Win10 1607
		if (IsVerOrGreater(10, 0, 14955))
		{
			HMODULE hModule = LoadLibrary(L"User32.dll");
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
		else  if (IsVerOrGreater(6, 3))
		{
			HMODULE hModule = LoadLibrary(L"Shcore.dll");
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
			nScale = getScaleOld();
		}
		return nScale;
	}
};
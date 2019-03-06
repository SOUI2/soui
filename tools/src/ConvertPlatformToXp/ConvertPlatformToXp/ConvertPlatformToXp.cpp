// ConvertPlatformToXp.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <string>
#include <list>
#include <iostream>
#include "XGetopt.h"
#include "pugixml\pugixml.hpp"
using namespace std;
void ScanProFileFormDir(LPCTSTR dir, list<wstring> &prolist, bool scanchild=true)
{
	wstring szFind = dir;
	szFind += _T("\\*.*");
	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile(szFind.c_str(), &wfd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		DWORD ec = GetLastError();
		return;
	}
	do
	{ 
		if (wfd .cFileName[0] == '.')
			continue;
		if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			if (scanchild)
			{
				wstring szFile = dir;
				szFile += _T("\\");
				szFile += wfd.cFileName;
				ScanProFileFormDir(szFile.c_str(), prolist);
			}
		}
		else
		{
			wstring szFile = dir;
			szFile += _T("\\");
			szFile += wfd.cFileName;
			size_t extpos = szFile.rfind(L".");
			if (extpos != wstring::npos)
			{
				wstring ext = szFile.substr(extpos + 1);
				if (ext == L"vcxproj")
				{
					wcout <<L"找到项目文件："<< szFile << endl;
					prolist.push_back(szFile);
				}
			}
		}
	} while (FindNextFile(hFind, &wfd));
	FindClose(hFind);
}
void LoadSubDirsFormXml(LPCTSTR xmlfile,list<wstring>& subdirlist)
{
	pugi::xml_document doc;
	pugi::xml_parse_result res= doc.load_file(xmlfile);
	if (res)
	{
		pugi::xml_node filenodes = doc.child(_T("DIRS"));
		for (auto filenode: filenodes)
		{
			wstring path=filenode.attribute(L"path").as_string();
			if (!path.empty())
			{
				subdirlist.push_back(path);
			}
		}
	}
}
void CovertToXp(LPCTSTR porfile)
{
	pugi::xml_document doc;
	pugi::xml_parse_result res = doc.load_file(porfile);
	if (res)
	{
		pugi::xml_node filenodes = doc.child(_T("Project"));
		for (auto filenode : filenodes)
		{
			//<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'" Label="Configuration">
			if ((wcscmp(filenode.name(), L"PropertyGroup") == 0) && (wcscmp(filenode.attribute(L"Label").as_string(),L"Configuration")==0))
			{
				//<PlatformToolSet>v141_xp< / PlatformToolSet>
				auto toolset = filenode.child(L"PlatformToolSet");
				wstring strToolset = toolset.text().as_string();
				size_t extpos = strToolset.rfind(L"_");
				if (extpos != wstring::npos)
				{
					continue;//暂不测试格式是否正确，有_则认为已经支持XP。
				}
				else
				{
					strToolset += L"_xp";
					toolset.text().set(strToolset.c_str());
					doc.save_file(porfile);
				}
			}		
		}
	}
}
int _tmain(int argc, _TCHAR* argv[])
{
	std::locale loc("");
	std::wcout.imbue(loc);
	list<wstring> SubDirList;
	int c;
	while ((c = getopt(argc, argv, _T("f:"))) != EOF || optarg != NULL)
	{
		switch (c)
		{
		case 'f':LoadSubDirsFormXml(optarg,SubDirList); break;
		}
	}	
	if (SubDirList.size() == 0)
	{
		wcout << L"ERROR:子目录为空！请使用:ConvertPlatformToXp.exe -f \"xmlfilepath\"指定子目录配置文件" << endl;
		return -1;
	}
	list<wstring> liPro;
	for (auto ite : SubDirList)
	{
		ScanProFileFormDir(ite.c_str(), liPro);
	}
	for (auto ite : liPro)
	{
		CovertToXp(ite.c_str());
	}
    return 0;
}


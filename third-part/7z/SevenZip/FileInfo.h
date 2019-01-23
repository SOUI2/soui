#pragma once
#include "SevenString.h"
#include "../CPP/Common/MyWindows.h"

namespace SevenZipEx
{
namespace intl
{
	struct FileInfo
	{
		TString		FileName;
		FILETIME	LastWriteTime;
		FILETIME	CreationTime;
		FILETIME	LastAccessTime;
		ULONGLONG	Size;
		UINT		Attributes;
		bool		IsDirectory;
	};

	struct FilePathInfo : public FileInfo
	{
		TString		FilePath;
	};
}
}

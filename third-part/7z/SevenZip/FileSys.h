#pragma once

#include <vector>
#include "FileInfo.h"
#include "../CPP/Common/MyCom.h"


namespace SevenZip
{
namespace intl
{
	class FileSys
	{
	public:

		static TString GetPath( const TString& filePath );
		static TString GetFileName( const TString& filePathOrName );
		static TString AppendPath( const TString& left, const TString& right );
		static TString ExtractRelativePath( const TString& basePath, const TString& fullPath );

        static TString GetUniquePath(const TString& path);

        static bool DirectoryExists(const TString& path);
        static bool FileExists(const TString& path);
        static bool PathExists(const TString& path);
        static bool RemovePath(const TString& path);

        static bool RenameFile(const TString& oldfile, const TString&newfile);
        static bool BackupFile(const TString&orignal, const TString&backup);
        static bool IsDirectoryEmptyRecursive(const TString& path);

		static bool CreateDirectoryTree( const TString& path );

		static std::vector< FilePathInfo > GetFile( const TString& filePathOrName );
		static std::vector< FilePathInfo > GetFilesInDirectory( const TString& directory, const TString& searchPattern, bool recursive );

		static CMyComPtr< IStream > OpenFileToRead( const TString& filePath );
		static CMyComPtr< IStream > OpenFileToWrite( const TString& filePath );
	};
}
}

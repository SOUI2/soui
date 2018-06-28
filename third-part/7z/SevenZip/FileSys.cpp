#include "FileSys.h"
#include "PathScanner.h"
#include <ShlObj.h>
#include <Shlwapi.h>
#include <ctime>
#include "../CPP/Common/MyCom.h"
#include "../CPP/Common/MyWindows.h"


namespace SevenZip
{
namespace intl
{

class ScannerCallback : public PathScanner::Callback
{
private:

	std::vector< FilePathInfo > m_files;
	bool m_recursive;
	bool m_onlyFirst;

public:

	ScannerCallback( bool recursive, bool onlyFirst = false ) : m_recursive( recursive ), m_onlyFirst( onlyFirst ) {}
	const std::vector< FilePathInfo >& GetFiles() { return m_files; }

	virtual bool ShouldDescend( const FilePathInfo& /*directory*/ ) { return m_recursive; }
	virtual void ExamineFile( const FilePathInfo& file, bool& exit )
	{
		m_files.push_back( file );
		if ( m_onlyFirst )
		{
			exit = true;
		}
	}
};


class IsEmptyCallback : public PathScanner::Callback
{
private:

	bool m_isEmpty;

public:

	IsEmptyCallback() : m_isEmpty( true ) {}
	bool IsEmpty() const { return m_isEmpty; }

	virtual bool ShouldDescend( const FilePathInfo& /*directory*/ ) { return true; }
	virtual void ExamineFile( const FilePathInfo& /*file*/, bool& exit ) { m_isEmpty = false; exit = true; }
};


TString FileSys::GetPath( const TString& filePath )
{
	// Find the last "\" or "/" in the string and return it and everything before it.
	size_t index  = filePath.rfind( _T( '\\' ) );
	size_t index2 = filePath.rfind( _T( '/' ) );

	if ( index2 != std::string::npos && index2 > index )
	{
		index = index2;
	}

	if ( index == std::string::npos )
	{
		// No path sep.
		return TString();
	}
	else if ( index + 1 >= filePath.size() )
	{
		// Last char is path sep, the whole thing is a path.
		return filePath;
	}
	else
	{
		return filePath.substr( 0, index + 1 );
	}
}

TString FileSys::GetFileName( const TString& filePathOrName )
{
	// Find the last "\" or "/" in the string and return everything after it.
	size_t index  = filePathOrName.rfind( _T( '\\' ) );
	size_t index2 = filePathOrName.rfind( _T( '/' ) );

	if ( index2 != std::string::npos && index2 > index )
	{
		index = index2;
	}

	if ( index == std::string::npos )
	{
		// No path sep, return the whole thing.
		return filePathOrName;
	}
	else if ( index + 1 >= filePathOrName.size() )
	{
		// Last char is path sep, no filename.
		return TString();
	}
	else
	{
		return filePathOrName.substr( index + 1, filePathOrName.size() - ( index + 1 ) );
	}
}

TString FileSys::AppendPath( const TString& left, const TString& right )
{
	if ( left.empty() )
	{
		return right;
	}

	TCHAR lastChar = left[ left.size() - 1 ];
	if ( lastChar == _T( '\\' ) || lastChar == _T( '/' ) )
	{
		return left + right;
	}
	else
	{
		return left + _T( "\\" ) + right;
	}
}

TString FileSys::ExtractRelativePath( const TString& basePath, const TString& fullPath )
{
	if ( basePath.size() >= fullPath.size() )
	{
		return TString();
	}

	if ( basePath != fullPath.substr( 0, basePath.size() ) )
	{
		return TString();
	}
	return fullPath.substr( basePath.size(), fullPath.size() - basePath.size() );
}

bool FileSys::DirectoryExists( const TString& path )
{
	DWORD attributes = GetFileAttributes( path.c_str() );

	if ( attributes == INVALID_FILE_ATTRIBUTES )
	{
		return false;
	}
	else
	{
		return ( attributes & FILE_ATTRIBUTE_DIRECTORY ) != 0;
	}
}


bool FileSys::FileExists(const TString& path)
{
    DWORD attributes = GetFileAttributes(path.c_str());

    if (attributes == INVALID_FILE_ATTRIBUTES)
    {
        return false;
    }
    else
    {
        return (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
    }
}


bool FileSys::PathExists(const TString& path)
{
    DWORD attributes = GetFileAttributes(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES;
}



bool FileSys::RemovePath(const TString& path)
{
    if (path.empty() || path.length() > MAX_PATH)
    {
        return false;
    }

    //确保文件或者目录存在
    if (!PathExists(path))
        return true;

    //目录的路径以2个\0结尾
    TString tmp_path = path;
    tmp_path.resize(tmp_path.size() + 2);

    SHFILEOPSTRUCT FileOp;
    ZeroMemory(&FileOp, sizeof(SHFILEOPSTRUCT));
    FileOp.fFlags |= FOF_SILENT;        /*不显示进度*/
    FileOp.fFlags |= FOF_NOERRORUI;        /*不报告错误信息*/
    FileOp.fFlags |= FOF_NOCONFIRMATION;/*直接删除，不进行确认*/
    FileOp.fFlags &= ~FOF_ALLOWUNDO; /*直接删除，不放入回收站*/
    FileOp.hNameMappings = NULL;
    FileOp.hwnd = NULL;
    FileOp.lpszProgressTitle = NULL;
    FileOp.wFunc = FO_DELETE;
    FileOp.pFrom = &tmp_path[0]; /*要删除的目录，必须以2个\0结尾*/
    FileOp.pTo = NULL;

    /*删除目录*/
    if (0 == SHFileOperation(&FileOp))
    {
        return true;
    }
    else
    {
        return false;
    }
}


bool FileSys::RenameFile(const TString& oldfile, const TString&newfile)
{
    return MoveFileEx(oldfile.c_str(), newfile.c_str(), MOVEFILE_REPLACE_EXISTING) != FALSE;
}


bool FileSys::BackupFile(const TString&orignal, const TString&backup)
{
    return MoveFileEx(orignal.c_str(), backup.c_str(), MOVEFILE_REPLACE_EXISTING) != FALSE
        || BackupFile(orignal.c_str(), backup.c_str()) != FALSE;
}

static std::wstring path_without_extension(const std::wstring& lpszOriginalPath)
{
    size_t pos_base_name = lpszOriginalPath.find_last_of(L"/\\");
    size_t pos_dot = lpszOriginalPath.find(L'.', pos_base_name);
    return lpszOriginalPath.substr(0, pos_dot);
}

static std::wstring path_without_extension(const std::wstring& lpszOriginalPath, std::wstring& ext)
{
    size_t pos_base_name = lpszOriginalPath.find_last_of(L"/\\");
    size_t pos_dot = lpszOriginalPath.find(L'.', pos_base_name);
    ext =
        (pos_dot != std::wstring::npos)
        ? lpszOriginalPath.substr(pos_dot) : L"";

    return lpszOriginalPath.substr(0, pos_dot);
}


static std::wstring get_date_time_str()
{
    time_t rawtime;
    struct tm * timeinfo;
    wchar_t buffer[80] = { 0 };
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    wcsftime(buffer, 80, L"%Y%d%m-%H%M%S", timeinfo);
    return  buffer;
}
static std::wstring get_unique_path_with_dt(const std::wstring&path)
{
    std::wstring ext;
    std::wstring base_name = path_without_extension(path, ext);

    // 找第一个不存在的路径
    std::wstring save_path = path;
    while (FileSys::PathExists(save_path))
    {
        save_path = base_name + L'~' + get_date_time_str() + ext;
    }
    return save_path;
}

TString FileSys::GetUniquePath(const TString& path)
{
    std::wstring ext;
    std::wstring base_name = path_without_extension(path, ext);

    // 找第一个不存在的路径
    std::wstring save_path = path;
    while (PathExists(save_path))
    {
        save_path = base_name + L'~' + get_date_time_str() + ext;
    }
    return save_path;
}

bool FileSys::IsDirectoryEmptyRecursive(const TString& path)
{
	IsEmptyCallback cb;
	PathScanner::Scan( path, cb );
	return cb.IsEmpty();
}

bool FileSys::CreateDirectoryTree( const TString& path )
{
	int ret = SHCreateDirectoryEx( NULL, path.c_str(), NULL );
    return ret == ERROR_SUCCESS || ret == ERROR_ALREADY_EXISTS;
}

std::vector< FilePathInfo > FileSys::GetFile( const TString& filePathOrName )
{
	TString path = FileSys::GetPath( filePathOrName );
	TString name = FileSys::GetFileName( filePathOrName );

	ScannerCallback cb( false, true );
	PathScanner::Scan( path, name, cb );
	return cb.GetFiles();
}

std::vector< FilePathInfo > FileSys::GetFilesInDirectory( const TString& directory, const TString& searchPattern, bool recursive )
{
	ScannerCallback cb( recursive );
	PathScanner::Scan( directory, searchPattern, cb );
	return cb.GetFiles();
}

CMyComPtr< IStream > FileSys::OpenFileToRead( const TString& filePath )
{
	CMyComPtr< IStream > fileStream;

#ifdef _UNICODE
	const WCHAR* filePathStr = filePath.c_str();
#else
	WCHAR filePathStr[MAX_PATH];
	MultiByteToWideChar( CP_UTF8, 0, filePath.c_str(), filePath.length() + 1, filePathStr, MAX_PATH );
#endif
	if ( FAILED( SHCreateStreamOnFileEx( filePathStr, STGM_READ, FILE_ATTRIBUTE_NORMAL, FALSE, NULL, &fileStream ) ) )
	{
		return NULL;
	}

	return fileStream;
}

CMyComPtr< IStream > FileSys::OpenFileToWrite( const TString& filePath )
{
	CMyComPtr< IStream > fileStream;
	
#ifdef _UNICODE
	const WCHAR* filePathStr = filePath.c_str();
#else
	WCHAR filePathStr[MAX_PATH];
	MultiByteToWideChar( CP_UTF8, 0, filePath.c_str(), filePath.length() + 1, filePathStr, MAX_PATH );
#endif
	if ( FAILED( SHCreateStreamOnFileEx( filePathStr, STGM_CREATE | STGM_WRITE, FILE_ATTRIBUTE_NORMAL, TRUE, NULL, &fileStream ) ) )
	{
		return NULL;
	}

	return fileStream;
}

}
}

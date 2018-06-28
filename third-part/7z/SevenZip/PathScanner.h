#pragma once


#include <deque>
#include "FileInfo.h"


namespace SevenZip
{
namespace intl
{
	class PathScanner
	{
	public:

		class Callback
		{
		public:

			virtual ~Callback() {}

			virtual void BeginScan() {}
			virtual void EndScan() {}
			virtual bool ShouldDescend( const FilePathInfo& directory ) = 0;
			virtual void EnterDirectory( const TString& /*path*/ ) {}
			virtual void LeaveDirectory( const TString& /*path*/ ) {}
			virtual void ExamineFile( const FilePathInfo& file, bool& exit ) = 0;
		};

	public:

		static void Scan( const TString& root, Callback& cb );
		static void Scan( const TString& root, const TString& searchPattern, Callback& cb );

	private:

		static bool ExamineFiles( const TString& directory, const TString& searchPattern, Callback& cb );
		static void ExamineDirectories( const TString& directory, std::deque< TString >& subDirs, Callback& cb );

		static bool IsAllFilesPattern( const TString& searchPattern );
		static bool IsSpecialFileName( const TString& fileName );
		static bool IsDirectory( const WIN32_FIND_DATA& fdata );
		static FilePathInfo ConvertFindInfo( const TString& directory, const WIN32_FIND_DATA& fdata );
	};
}
}

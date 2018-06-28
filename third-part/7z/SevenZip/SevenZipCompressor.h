#pragma once


#include <vector>
#include <atlbase.h>
#include "SevenZipArchive.h"
#include "FileInfo.h"
#include "CompressionFormat.h"
#include "CompressionLevel.h"
#include "ProgressCallback.h"
#include "SevenZipPwd.h"

namespace SevenZip
{
	class SevenZipCompressor : public SevenZipArchive
	{
	public:

		SevenZipCompressor();
		virtual ~SevenZipCompressor();

		// Includes the last directory as the root in the archive, e.g. specifying "C:\Temp\MyFolder"
		// makes "MyFolder" the single root item in archive with the files within it included.
		virtual HRESULT CompressDirectory(const TString& directory, ProgressCallback* callback, bool includeSubdirs = true, SevenZipPassword *pSevenZipPassword = NULL);

		// Excludes the last directory as the root in the archive, its contents are at root instead. E.g.
		// specifying "C:\Temp\MyFolder" make the files in "MyFolder" the root items in the archive.
		virtual HRESULT CompressFiles(const TString& directory, const TString& searchFilter, ProgressCallback* callback, bool includeSubdirs = true, SevenZipPassword *pSevenZipPassword = NULL);
		virtual HRESULT CompressAllFiles(const TString& directory, ProgressCallback* callback, bool includeSubdirs = true, SevenZipPassword *pSevenZipPassword = NULL);

		// Compress just this single file as the root item in the archive.
		virtual HRESULT CompressFile(const TString& filePath, ProgressCallback* callback, SevenZipPassword *pSevenZipPassword=NULL);

	private:
		TString m_outputPath;					//the final compression result compression path. Used for tracking in callbacks
		CMyComPtr< IStream > OpenArchiveStream();
        HRESULT FindAndCompressFiles(const TString& directory, const TString& searchPattern,
			const TString& pathPrefix, bool recursion, ProgressCallback* callback, SevenZipPassword *pSevenZipPassword);
		HRESULT CompressFilesToArchive(const TString& pathPrefix, const std::vector< intl::FilePathInfo >& filePaths, ProgressCallback* callback, SevenZipPassword *pSevenZipPassword);
        HRESULT SetCompressionProperties(IUnknown* outArchive);
	};
}

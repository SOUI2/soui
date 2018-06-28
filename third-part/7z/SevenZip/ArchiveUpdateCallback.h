// This file is based on the following file from the LZMA SDK (http://www.7-zip.org/sdk.html):
//   ./CPP/7zip/UI/Client7z/Client7z.cpp
#pragma once


#include "../CPP/7zip/Archive/IArchive.h"
#include "../CPP/7zip/ICoder.h"
#include "../CPP/7zip/IPassword.h"
#include "../CPP/Common/MyString.h"
#include <vector>
#include "FileInfo.h"
#include "ProgressCallback.h"

namespace SevenZip
{
namespace intl
{
	class ArchiveUpdateCallback 
        : public IArchiveUpdateCallback
        , public ICryptoGetTextPassword2
		, public ICryptoGetTextPassword
        , public ICompressProgressInfo
	{
	public: 
		bool PasswordIsDefined;
		UString Password;
	private:

		long m_refCount;
		TString m_dirPrefix;
		TString m_outputPath;
		const std::vector< FilePathInfo >& m_filePaths;
		ProgressCallback* m_callback;

		bool AskPassword;
	public:

		ArchiveUpdateCallback( const TString& dirPrefix, const std::vector< FilePathInfo >& filePaths, const TString& outputFilePath, ProgressCallback* callback );
		virtual ~ArchiveUpdateCallback();

		STDMETHOD(QueryInterface)( REFIID iid, void** ppvObject );
		STDMETHOD_(ULONG, AddRef)();
		STDMETHOD_(ULONG, Release)();

		// IProgress
		STDMETHOD(SetTotal)( UInt64 size );
		STDMETHOD(SetCompleted)( const UInt64* completeValue );

		// IArchiveUpdateCallback
		STDMETHOD(GetUpdateItemInfo)( UInt32 index, Int32* newData, Int32* newProperties, UInt32* indexInArchive );
		STDMETHOD(GetProperty)( UInt32 index, PROPID propID, PROPVARIANT* value );
		STDMETHOD(GetStream)( UInt32 index, ISequentialInStream** inStream );
		STDMETHOD(SetOperationResult)( Int32 operationResult );

		// ICryptoGetTextPassword2
		STDMETHOD(CryptoGetTextPassword2)( Int32* passwordIsDefined, BSTR* password );

		//ICryptoGetTextPassword  
		STDMETHOD(CryptoGetTextPassword)(BSTR* password);

		// ICompressProgressInfo
		STDMETHOD(SetRatioInfo)( const UInt64* inSize, const UInt64* outSize );
	};
}
}

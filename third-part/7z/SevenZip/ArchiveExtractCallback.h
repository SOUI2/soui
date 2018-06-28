#pragma once


#include "../CPP/7zip/Archive/IArchive.h"
#include "../CPP/7zip/IPassword.h"
#include "../CPP/7zip/ICoder.h"
#include "../CPP/Common/MyCom.h"
#include "../CPP/Common/MyString.h"

#include "CompressionFormat.h"
#include "ProgressCallback.h"

namespace SevenZip
{
namespace intl
{
	class ArchiveExtractCallback 
        : public IArchiveExtractCallback
        , public ICryptoGetTextPassword
        , public ICompressProgressInfo
	{
	public: 
		bool PasswordIsDefined;
		UString Password;
	private:

		long m_refCount;
		CMyComPtr< IInArchive > m_archiveHandler;
		TString m_directory;

		TString m_relPath;
		TString m_absPath;
		bool m_isDir;

		bool m_hasAttrib;
		UInt32 m_attrib;

		bool m_hasModifiedTime;
		FILETIME m_modifiedTime;
        bool m_hasAccessedTime;
        FILETIME m_accessedTime;
        bool m_hasCreatedTime;
        FILETIME m_createdTime;

		bool m_hasNewFileSize;
		UInt64 m_newFileSize;

        OverwriteModeEnum m_overwriteMode;

		ProgressCallback* m_callback;

        UInt64 m_totalSize;

        struct RollBack_Info
        {
            TString original_path;
            TString backup_path;
        };
        std::vector<RollBack_Info> m_rollbacks;
	public:

        ArchiveExtractCallback(const CMyComPtr< IInArchive >& archiveHandler, const TString& directory, OverwriteModeEnum mode, ProgressCallback* callback);
		virtual ~ArchiveExtractCallback();

		STDMETHOD(QueryInterface)( REFIID iid, void** ppvObject );
		STDMETHOD_(ULONG, AddRef)();
		STDMETHOD_(ULONG, Release)();

        //IProgress
        STDMETHOD(SetTotal)(UInt64 size);
        STDMETHOD(SetCompleted)(const UInt64 *completeValue);

        // ICompressProgressInfo
        STDMETHOD(SetRatioInfo)(const UInt64 *inSize, const UInt64 *outSize);

		// IArchiveExtractCallback
		STDMETHOD(PrepareOperation)( Int32 askExtractMode );
        STDMETHOD(GetStream)(UInt32 index, ISequentialOutStream** outStream, Int32 askExtractMode);
        STDMETHOD(SetOperationResult)(Int32 resultEOperationResult);

		// ICryptoGetTextPassword
		STDMETHOD(CryptoGetTextPassword)( BSTR* password );

	private:

		void GetPropertyFilePath( UInt32 index );
		void GetPropertyAttributes( UInt32 index );
		void GetPropertyIsDir( UInt32 index );
        void GetPropertyModifiedTime(UInt32 index);
        void GetPropertyAccessedTime(UInt32 index);
        void GetPropertyCreatedTime(UInt32 index);
        void GetPropertySize(UInt32 index);
        bool ProcessRollBack();
	};
}
}

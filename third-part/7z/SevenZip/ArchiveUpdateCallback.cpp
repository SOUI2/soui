#include "ArchiveUpdateCallback.h"
#include "PropVariant2.h"
#include "FileSys.h"
#include "InStreamWrapper.h"
#include "../CPP/Common/MyCom.h"
#include "../CPP/Common/Defs.h"

namespace SevenZip
{
namespace intl
{

ArchiveUpdateCallback::ArchiveUpdateCallback( const TString& dirPrefix, const std::vector< FilePathInfo >& filePaths, const TString& outputFilePath, ProgressCallback* callback)
	: m_refCount( 0 )
	, m_dirPrefix( dirPrefix )
	, m_filePaths( filePaths )
	, m_callback(callback)
	, m_outputPath(outputFilePath)
	, PasswordIsDefined(false)
	, AskPassword(false)
{
}

ArchiveUpdateCallback::~ArchiveUpdateCallback()
{
}

STDMETHODIMP ArchiveUpdateCallback::QueryInterface( REFIID iid, void** ppvObject )
{
	if ( iid == __uuidof( IUnknown ) )
	{
		*ppvObject = reinterpret_cast< IUnknown* >( this );
		AddRef();
		return S_OK;
	}

	if ( iid == IID_IArchiveUpdateCallback )
	{
		*ppvObject = static_cast< IArchiveUpdateCallback* >( this );
		AddRef();
		return S_OK;
	}

	if (iid == IID_ICryptoGetTextPassword)
	{
		*ppvObject = static_cast<ICryptoGetTextPassword*>(this);
		AddRef();
		return S_OK;
	}
	 
	if ( iid == IID_ICryptoGetTextPassword2 )
	{
		*ppvObject = static_cast< ICryptoGetTextPassword2* >( this );
		AddRef();
		return S_OK;
	}

	if ( iid == IID_ICompressProgressInfo )
	{
		*ppvObject = static_cast< ICompressProgressInfo* >( this );
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) ArchiveUpdateCallback::AddRef()
{
	return static_cast< ULONG >( InterlockedIncrement( &m_refCount ) );
}

STDMETHODIMP_(ULONG) ArchiveUpdateCallback::Release()
{
	ULONG res = static_cast< ULONG >( InterlockedDecrement( &m_refCount ) );
	if ( res == 0 )
	{
		delete this;
	}
	return res;
}

STDMETHODIMP ArchiveUpdateCallback::SetTotal( UInt64 size )
{
    wprintf_s(L"SetTotal:%llu\n", size);
	if (m_callback)
	{
		m_callback->OnStart(m_outputPath, size);
	}
	return S_OK;
}

STDMETHODIMP ArchiveUpdateCallback::SetCompleted( const UInt64* completeValue )
{
    wprintf_s(L"SetCompleted:%llu\n", *completeValue);
	if (m_callback)
	{
		m_callback->OnProgress(m_outputPath, *completeValue);
	}
	return S_OK;
}

STDMETHODIMP ArchiveUpdateCallback::GetUpdateItemInfo( UInt32 /*index*/, Int32* newData, Int32* newProperties, UInt32* indexInArchive )
{
	// Setting info for Create mode (vs. Append mode).
	// TODO: support append mode
	if ( newData != NULL )
	{
		*newData = 1;
	}

	if ( newProperties != NULL )
	{
		*newProperties = 1;
	}

	if ( indexInArchive != NULL )
	{
		*indexInArchive = static_cast< UInt32 >( -1 ); // TODO: UInt32.Max
	}

	return S_OK;
}

STDMETHODIMP ArchiveUpdateCallback::GetProperty( UInt32 index, PROPID propID, PROPVARIANT* value )
{
	CPropVariant prop;

	if ( propID == kpidIsAnti )
	{
		prop = false;
		prop.Detach( value );
		return S_OK;
	}

	if ( index >= m_filePaths.size() )
	{
		return E_INVALIDARG;
	}

	const FilePathInfo& fileInfo = m_filePaths.at( index );
	switch ( propID )
	{
		case kpidPath:		prop = FileSys::ExtractRelativePath( m_dirPrefix, fileInfo.FilePath ).c_str(); break;
		case kpidIsDir:		prop = fileInfo.IsDirectory; break;
		case kpidSize:		prop = fileInfo.Size; break;
		case kpidAttrib:	prop = fileInfo.Attributes; break;
		case kpidCTime:		prop = fileInfo.CreationTime; break;
		case kpidATime:		prop = fileInfo.LastAccessTime; break;
		case kpidMTime:		prop = fileInfo.LastWriteTime; break;
	}

	prop.Detach( value );
	return S_OK;
}

STDMETHODIMP ArchiveUpdateCallback::GetStream( UInt32 index, ISequentialInStream** inStream )
{
	if ( index >= m_filePaths.size() )
	{
		return E_INVALIDARG;
	}

	const FilePathInfo& fileInfo = m_filePaths.at( index );
	if ( fileInfo.IsDirectory )
	{
		return S_OK;
	}
    wprintf_s(L"GetStream:%s\n", fileInfo.FilePath.c_str());

	CMyComPtr< IStream > fileStream = FileSys::OpenFileToRead( fileInfo.FilePath );
	if ( fileStream == NULL )
	{
		return HRESULT_FROM_WIN32( GetLastError() );
	}

	CMyComPtr< InStreamWrapper > wrapperStream = new InStreamWrapper( fileStream );
	*inStream = wrapperStream.Detach();

	return S_OK;
}

STDMETHODIMP ArchiveUpdateCallback::SetOperationResult( Int32 operationResult )
{
    wprintf_s(L"SetOperationResult:%u\n", operationResult);
	return S_OK;
}

STDMETHODIMP ArchiveUpdateCallback::CryptoGetTextPassword2( Int32* passwordIsDefined, BSTR* password )
{
	if (!PasswordIsDefined)
	{
		if (AskPassword)
		{
			// You can ask real password here from user
			// Password = GetPassword(OutStream);
			// PasswordIsDefined = true;
			printf("Password is not defined");
			return E_ABORT;
		}
	}
	*passwordIsDefined = BoolToInt(PasswordIsDefined);
	return StringToBstr(Password, password); 
}

STDMETHODIMP ArchiveUpdateCallback::CryptoGetTextPassword(BSTR* password)
{
	if (!PasswordIsDefined)
	{
		// You can ask real password here from user
		// Password = GetPassword(OutStream);
		// PasswordIsDefined = true;
		printf("Password is not defined");
		return E_ABORT;
	}
	return StringToBstr(Password, password);
} 

STDMETHODIMP ArchiveUpdateCallback::SetRatioInfo( const UInt64* inSize, const UInt64* outSize )
{
    wprintf_s(L"SetRatioInfo:%llu-%llu\n", *inSize, *outSize);
	return S_OK;
}

}
}

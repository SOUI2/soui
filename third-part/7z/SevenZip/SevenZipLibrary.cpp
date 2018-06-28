
#include "SevenZipLibrary.h"
#include "GUIDs.h"


namespace SevenZip
{

const TString DefaultLibraryPath = _T( "7z.dll" );

SevenZipLibrary::SevenZipLibrary()
	: m_dll( NULL )
	, m_func( NULL )
{
}

SevenZipLibrary::~SevenZipLibrary()
{
	Free();
}

bool SevenZipLibrary::Load()
{
	return Load( DefaultLibraryPath );
}

bool SevenZipLibrary::Load( const TString& libraryPath )
{
	Free();
	m_dll = LoadLibrary( libraryPath.c_str() );
	if ( m_dll == NULL )
	{
		return false;
		//throw SevenZipException( GetWinErrMsg( _T( "LoadLibrary" ), GetLastError() ) );
	}

	m_func = reinterpret_cast< CreateObjectFunc >( GetProcAddress( m_dll, "CreateObject" ) );
	if ( m_func == NULL )
	{
		Free();
		return false;
		//throw SevenZipException( _T( "Loaded library is missing required CreateObject function" ) );
	}
	return true;
}

void SevenZipLibrary::Free()
{
	if ( m_dll != NULL )
	{
		FreeLibrary( m_dll );
		m_dll = NULL;
		m_func = NULL;
	}
}

bool SevenZipLibrary::CreateObject( const GUID& clsID, const GUID& interfaceID, void** outObject ) const
{
	if ( m_func == NULL )
	{
		return false;
		//throw SevenZipException( _T( "Library is not loaded" ) );
	}

	HRESULT hr = m_func( &clsID, &interfaceID, outObject );
	if ( FAILED( hr ) )
	{
		return false;
		//throw SevenZipException( GetCOMErrMsg( _T( "CreateObject" ), hr ) );
	}
	return true;
}

}

#include "OutStreamWrapperMemory.h"


namespace SevenZip
{
namespace intl
{

OutStreamWrapperMemory::OutStreamWrapperMemory( const CMyComPtr< IStream >& baseStream )
	: m_refCount( 0 )
	, m_baseStream( baseStream )
{
}

OutStreamWrapperMemory::~OutStreamWrapperMemory()
{
}

HRESULT STDMETHODCALLTYPE OutStreamWrapperMemory::QueryInterface( REFIID iid, void** ppvObject )
{ 
	if ( iid == __uuidof( IUnknown ) )
	{
		*ppvObject = static_cast< IUnknown* >( this );
		AddRef();
		return S_OK;
	}

	if ( iid == IID_ISequentialOutStream )
	{
		*ppvObject = static_cast< ISequentialOutStream* >( this );
		AddRef();
		return S_OK;
	}

	if ( iid == IID_IOutStream )
	{
		*ppvObject = static_cast< IOutStream* >( this );
		AddRef();
		return S_OK;
	}
	
	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE OutStreamWrapperMemory::AddRef()
{
	return static_cast< ULONG >( InterlockedIncrement( &m_refCount ) );
}

ULONG STDMETHODCALLTYPE OutStreamWrapperMemory::Release()
{
	ULONG res = static_cast< ULONG >( InterlockedDecrement( &m_refCount ) );
	if ( res == 0 )
	{
		delete this;
	}
	return res;
}

STDMETHODIMP OutStreamWrapperMemory::Write( const void* data, UInt32 size, UInt32* processedSize )
{
	ULONG written = 0;
	HRESULT hr = m_baseStream->Write( data, size, &written );
	if ( processedSize != NULL )
	{
		*processedSize = written;
	}
	return hr;
}

STDMETHODIMP OutStreamWrapperMemory::Seek( Int64 offset, UInt32 seekOrigin, UInt64* newPosition )
{
	LARGE_INTEGER move;
	ULARGE_INTEGER newPos;

	move.QuadPart = offset;
	HRESULT hr = m_baseStream->Seek( move, seekOrigin, &newPos );
	if ( newPosition != NULL )
	{
		*newPosition =  newPos.QuadPart;
	}
	return hr;
}

STDMETHODIMP OutStreamWrapperMemory::SetSize( UInt64 newSize )
{
	ULARGE_INTEGER size;
	size.QuadPart = newSize;
	return m_baseStream->SetSize( size );
}

}
}

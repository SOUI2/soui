#pragma once


#include "../CPP/7zip/IStream.h"
#include "../CPP/Common/MyCom.h"


namespace SevenZip
{
namespace intl
{
	class InStreamWrapper : public IInStream, public IStreamGetSize
	{
	private:

		long				m_refCount;
		CMyComPtr< IStream >	m_baseStream;

	public:

		InStreamWrapper( const CMyComPtr< IStream >& baseStream );
		virtual ~InStreamWrapper();

		STDMETHOD(QueryInterface)( REFIID iid, void** ppvObject );
		STDMETHOD_(ULONG, AddRef)();
		STDMETHOD_(ULONG, Release)();

		// ISequentialInStream
		STDMETHOD(Read)( void* data, UInt32 size, UInt32* processedSize );

		// IInStream
		STDMETHOD(Seek)( Int64 offset, UInt32 seekOrigin, UInt64* newPosition );

		// IStreamGetSize
		STDMETHOD(GetSize)( UInt64* size );
	};
}
}

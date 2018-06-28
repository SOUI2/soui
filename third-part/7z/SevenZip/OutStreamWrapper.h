#pragma once


#include "../CPP/7zip/IStream.h"
#include "../CPP/Common/MyCom.h"


namespace SevenZip
{
namespace intl
{
	class OutStreamWrapper : public IOutStream
	{
	private:

		long				m_refCount;
		CMyComPtr< IStream >	m_baseStream;

	public:

		OutStreamWrapper( const CMyComPtr< IStream >& baseStream );
		virtual ~OutStreamWrapper();

		STDMETHOD(QueryInterface)( REFIID iid, void** ppvObject );
		STDMETHOD_(ULONG, AddRef)();
		STDMETHOD_(ULONG, Release)();

		// ISequentialOutStream
		STDMETHOD(Write)( const void* data, UInt32 size, UInt32* processedSize );

		// IOutStream
		STDMETHOD(Seek)( Int64 offset, UInt32 seekOrigin, UInt64* newPosition );
		STDMETHOD(SetSize)( UInt64 newSize );
	};
}
}

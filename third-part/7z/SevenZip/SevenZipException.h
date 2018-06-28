#pragma once


#include <exception>
#include "SevenString.h"
#include <windows.h>

namespace SevenZip
{
	TString StrFmt( const TCHAR* format, ... );
	TString GetWinErrMsg( const TString& contextMessage, DWORD lastError );
	TString GetCOMErrMsg( const TString& contextMessage, HRESULT lastError );

	class SevenZipException
	{
	protected:

		TString m_message;

	public:

		SevenZipException();
		SevenZipException( const TString& message );
		virtual ~SevenZipException();

		const TString& GetMessage() const;
	};
}

/****************************************************************************************
* ///////////////////////////////////////////////////////////////////////////////////////
*	Original Filename: 	HTTPServer.h
*
*	History:
*	Created/Modified by				Date			Main Purpose/Changes
*	Souren M. Abeghyan				2001/05/25		Interface for the CHTTPServer class
*	
*	Comments:	
* \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
****************************************************************************************/
#if !defined(AFX_HTTPSERVER_H__177A41DC_432E_4819_A614_8A0D66ABF434__INCLUDED_)
#define AFX_HTTPSERVER_H__177A41DC_432E_4819_A614_8A0D66ABF434__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GenericServer.h"
#include "filereader-i.h"

#define ERROR404			"/fnf.html"
#define ERROR501			"/mna.html"

#define SERVERNAME			"AcIDSoftWebServer/0.1b"


typedef map<string, string>				MIMETYPES;



class CHTTPServer : public CGenericServer
{
public:
					CHTTPServer(IFileReader *pFileReader = NULL);
	virtual			~CHTTPServer();
	BOOL			Start(string, string, int, int);
	BOOL			IsComplete(string);
	BOOL			ParseRequest(string, string&, BOOL&);
	int				GotConnection(char*, int);
	int				DataSent(DWORD);
private:
	string			m_HomeDir;
	string			m_DefIndex;
	MIMETYPES		MimeTypes;
	
    IFileReader   * m_pFileReader;
};

#endif // !defined(AFX_HTTPSERVER_H__177A41DC_432E_4819_A614_8A0D66ABF434__INCLUDED_)

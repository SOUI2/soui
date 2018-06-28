#include "stdafx.h"
#include "MemFlash.h"
#include <SApp.h>

#define HOMEDIR "souires:"
#define HOMEHEADER "souires:\\/"
#define HOMEHEADER_SIZE (sizeof(HOMEHEADER)-1)
#define NAME_SEP '/'

using namespace SOUI;

class FileID
{
public:
    SStringT strName;
    SStringT strType;
};

const char * CMemFlash::HomeDir()
{
    return HOMEDIR;
}

bool CMemFlash::FileExist( const char * pszFileName )
{
    if(strncmp(pszFileName,HOMEHEADER,HOMEHEADER_SIZE)!=0) return false;
    SStringA str(pszFileName + HOMEHEADER_SIZE);
    int pos=str.ReverseFind(NAME_SEP);
    if(pos==-1) return false;
    SStringA strType=str.Left(pos);
    SStringA strName=str.Right(str.GetLength()-pos-1);
    return !!GETRESPROVIDER->HasResource(S_CA2T(strType),S_CA2T(strName));
}

HANDLE CMemFlash::Open( const char * pszFileName )
{
    if(strncmp(pszFileName,HOMEHEADER,HOMEHEADER_SIZE)!=0) return NULL;
    SStringA str(pszFileName + HOMEHEADER_SIZE);
    int pos=str.ReverseFind(NAME_SEP);
    if(pos==-1) return NULL;
    FileID *pID=new FileID;
    pID->strType=S_CA2T(str.Left(pos));
    pID->strName=S_CA2T(str.Right(str.GetLength()-pos-1));
    return pID;
}

void CMemFlash::Close( HANDLE f )
{
    if(f)
    {
        FileID *pID = (FileID*)f;
        delete pID;
    }
}

__int64 CMemFlash::Length( HANDLE f )
{
    if(!f) return 0;
    FileID *pID = (FileID*)f;
    return GETRESPROVIDER->GetRawBufferSize(pID->strType,pID->strName);
}

__int64 CMemFlash::Read( HANDLE f,char *pbuf,__int64 nSize )
{
    if(!f) return 0;
    FileID *pID = (FileID*)f;
    
    if(GETRESPROVIDER->GetRawBuffer(pID->strType,pID->strName,pbuf,(int)nSize))
        return nSize;
    else
        return 0;
}
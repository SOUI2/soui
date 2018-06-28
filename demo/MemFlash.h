#pragma once

#include "httpsvr/filereader-i.h"

class CMemFlash: public IFileReader
{
public:
    virtual bool FileExist(const char * pszFileName);
    virtual HANDLE Open(const char * pszFileName);
    virtual void Close(HANDLE f);
    virtual __int64 Length(HANDLE f);
    virtual __int64 Read(HANDLE f,char *pbuf,__int64 nSize);

    static const char *HomeDir();
};


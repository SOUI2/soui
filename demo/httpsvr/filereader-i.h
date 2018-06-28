#pragma once

struct IFileReader
{
    virtual bool FileExist(const char * pszFileName) = 0;
    virtual HANDLE Open(const char * pszFileName) = 0;
    virtual void Close(HANDLE f) =0;
    virtual __int64 Length(HANDLE f) = 0;
    virtual __int64 Read(HANDLE f,char *pbuf,__int64 nSize) =0;
};


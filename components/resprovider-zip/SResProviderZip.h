#pragma once

#include <interface/SResProvider-i.h>
#include <unknown/obj-ref-impl.hpp>
#include <string/tstring.h>
#include <string/strcpcvt.h>
#include <souicoll.h>
#define _COLL_NS SOUI
#include <helper/SResID.h>
#include <interface/render-i.h>

#include "ZipArchive.h"
#include "zipresprovider-param.h"

namespace SOUI{

class SResProviderZip : public TObjRefImpl<IResProvider>
{
public:
	SResProviderZip();
	~SResProviderZip(void);
	
    virtual BOOL Init(WPARAM wParam,LPARAM lParam);

    virtual BOOL HasResource(LPCTSTR strType,LPCTSTR pszResName);
    virtual HICON   LoadIcon(LPCTSTR pszResName,int cx,int cy);
    virtual HBITMAP    LoadBitmap(LPCTSTR pszResName);
    virtual HCURSOR LoadCursor(LPCTSTR pszResName);
    virtual IBitmap * LoadImage(LPCTSTR strType,LPCTSTR pszResName);
    virtual IImgX   * LoadImgX(LPCTSTR strType,LPCTSTR pszResName);
    virtual size_t GetRawBufferSize(LPCTSTR strType,LPCTSTR pszResName);
    virtual BOOL GetRawBuffer(LPCTSTR strType,LPCTSTR pszResName,LPVOID pBuf,size_t size);

protected:
    BOOL _Init(LPCTSTR pszZipFile ,LPCSTR pszPsw);
    BOOL _Init(LPBYTE pBytes, DWORD dwByteCounts, LPCSTR pszPsw);    
    BOOL _Init(HINSTANCE hInst,LPCTSTR pszResName,LPCTSTR pszType ,LPCSTR pszPsw);
	BOOL _LoadSkin();
	SStringT _GetFilePath(LPCTSTR pszResName,LPCTSTR pszType);
	
	SMap<SResID,SStringT> m_mapFiles;
    CAutoRefPtr<IRenderFactory> m_renderFactory;
	CZipArchive m_zipFile;
	SStringT m_childDir;
};

namespace RESPROVIDER_ZIP
{
    SOUI_COM_C BOOL SOUI_COM_API SCreateInstance(IObjRef ** ppObj);
}

}//namespace SOUI

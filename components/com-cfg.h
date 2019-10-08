//SOUI组件配置

#pragma  once

#include <com-def.h>
#include <string/tstring.h>

#define COM_IMGDECODER  _T("imgdecoder-gdip")

#ifdef _DEBUG
#define COM_RENDER_GDI  _T("render-gdid.dll")
#define COM_RENDER_SKIA _T("render-skiad.dll")
#define COM_SCRIPT_LUA _T("scriptmodule-luad.dll")
#define COM_TRANSLATOR _T("translatord.dll")
#define COM_ZIPRESPROVIDER _T("resprovider-zipd.dll")
#define COM_LOG4Z   _T("log4zd.dll")
#define COM_7ZIPRESPROVIDER _T("resprovider-7zipd.dll")
#define COM_TASKLOOP _T("taskloopd.dll")
#else
#define COM_RENDER_GDI  _T("render-gdi.dll")
#define COM_RENDER_SKIA _T("render-skia.dll")
#define COM_SCRIPT_LUA _T("scriptmodule-lua.dll")
#define COM_TRANSLATOR _T("translator.dll")
#define COM_ZIPRESPROVIDER _T("resprovider-zip.dll")
#define COM_LOG4Z   _T("log4z.dll")
#define COM_7ZIPRESPROVIDER _T("resprovider-7zip.dll")
#define COM_TASKLOOP _T("taskloop.dll")
#endif	// _DEBUG


#ifdef LIB_SOUI_COM
#pragma message("LIB_SOUI_COM")

#pragma comment(lib,"Usp10")
#pragma comment(lib,"opengl32")

#ifdef _DEBUG
    #pragma comment(lib,"skiad")
    #pragma comment(lib,"zlibd")
    #pragma comment(lib,"pngd")
    #pragma comment(lib,"render-gdid")
    #pragma comment(lib,"render-skiad")
    #pragma comment(lib,"imgdecoder-wicd")
    #pragma comment(lib,"imgdecoder-stbd")
    #pragma comment(lib,"imgdecoder-pngd")
    #pragma comment(lib,"imgdecoder-gdipd")
    #pragma comment(lib,"translatord")
    #pragma comment(lib,"resprovider-zipd")
    #pragma comment(lib,"7zd")
    #pragma comment(lib,"resprovider-7zipd")
    #pragma comment(lib,"log4zd")
	#pragma comment(lib,"taskloopd")
#else//_DEBUG

    #pragma comment(lib,"skia")
    #pragma comment(lib,"zlib")
    #pragma comment(lib,"png")
    #pragma comment(lib,"imgdecoder-wic")
    #pragma comment(lib,"imgdecoder-stb")
    #pragma comment(lib,"imgdecoder-png")
    #pragma comment(lib,"imgdecoder-gdip")
    #pragma comment(lib,"render-gdi")
    #pragma comment(lib,"render-skia")
    #pragma comment(lib,"translator")
    #pragma comment(lib,"resprovider-zip")
    #pragma comment(lib,"7z")
    #pragma comment(lib,"resprovider-7zip")
    #pragma comment(lib,"log4z")
	#pragma comment(lib,"taskloop")
#endif//_DEBUG

namespace SOUI
{
    namespace IMGDECODOR_WIC
    {
        BOOL SCreateInstance(IObjRef **);
    }
    namespace IMGDECODOR_STB
    {
        BOOL SCreateInstance(IObjRef **);
    }
    namespace IMGDECODOR_PNG
    {
        BOOL SCreateInstance(IObjRef **);
    }
    namespace IMGDECODOR_GDIP
    {
        BOOL SCreateInstance(IObjRef **);
    }

    namespace RENDER_GDI
    {
        BOOL SCreateInstance(IObjRef **);
    }
    namespace RENDER_SKIA
    {
        BOOL SCreateInstance(IObjRef **);
    }
    namespace SCRIPT_LUA
    {
        BOOL SCreateInstance(IObjRef **);
    }
    namespace TRANSLATOR
    {
        BOOL SCreateInstance(IObjRef **);
    }
    namespace RESPROVIDER_ZIP
    {
        BOOL SCreateInstance(IObjRef **);
    }
	namespace RESPROVIDER_7ZIP
	{
		BOOL SCreateInstance(IObjRef **);
	} 
    namespace LOG4Z
    {
        BOOL SCreateInstance(IObjRef **);
    }
	namespace TASKLOOP {
		BOOL SCreateInstance(IObjRef **);
	}
}//end of soui

class SComMgr
{
public:
    SComMgr(LPCTSTR pszImgDecoder = NULL)
    {
        if(pszImgDecoder) m_strImgDecoder = pszImgDecoder;
        else m_strImgDecoder = COM_IMGDECODER;
    }

	void SetDllPath(const SOUI::SStringT & strDllPath){}

    BOOL CreateImgDecoder(IObjRef ** ppObj)
    {
        if(m_strImgDecoder == _T("imgdecoder-wic"))
            return SOUI::IMGDECODOR_WIC::SCreateInstance(ppObj);
        else if(m_strImgDecoder == _T("imgdecoder-stb"))
            return SOUI::IMGDECODOR_STB::SCreateInstance(ppObj);
        else if(m_strImgDecoder == _T("imgdecoder-png"))
            return SOUI::IMGDECODOR_PNG::SCreateInstance(ppObj);
        else if(m_strImgDecoder == _T("imgdecoder-gdip"))
            return SOUI::IMGDECODOR_GDIP::SCreateInstance(ppObj);
        else
        {
            SASSERT(0);
            return FALSE;
        }
    }

    BOOL CreateRender_GDI(IObjRef **ppObj)
    {
        return SOUI::RENDER_GDI::SCreateInstance(ppObj);
    }

    BOOL CreateRender_Skia(IObjRef **ppObj)
    {
        return SOUI::RENDER_SKIA::SCreateInstance(ppObj);
    }
    BOOL CreateScrpit_Lua(IObjRef **ppObj)
    {
        return SOUI::SCRIPT_LUA::SCreateInstance(ppObj);
    }

    BOOL CreateTranslator(IObjRef **ppObj)
    {
        return SOUI::TRANSLATOR::SCreateInstance(ppObj);
    }
    BOOL CreateResProvider_ZIP(IObjRef **ppObj)
    {
        return SOUI::RESPROVIDER_ZIP::SCreateInstance(ppObj);
    }


	BOOL CreateResProvider_7ZIP(IObjRef **ppObj)
	{
		return SOUI::RESPROVIDER_7ZIP::SCreateInstance(ppObj);
	}
    
    BOOL CreateLog4z(IObjRef **ppObj)
    {
        return SOUI::LOG4Z::SCreateInstance(ppObj);
    }

	BOOL CreateTaskLoop(IObjRef **ppObj)
	{
		return SOUI::TASKLOOP::SCreateInstance(ppObj);
	}

    SOUI::SStringT    m_strImgDecoder;
};

#else
	
#include <com-loader.hpp>


class SComMgr
{
public:
    SComMgr(LPCTSTR pszImgDecoder = NULL)
    {
        if(pszImgDecoder) m_strImgDecoder = pszImgDecoder;
        else m_strImgDecoder = COM_IMGDECODER;
    }

	void SetDllPath(const SOUI::SStringT & strDllPath)
	{
		m_strDllPath = strDllPath;
		if(!m_strDllPath.IsEmpty())
		{
			if(m_strDllPath.Right(1)!=_T("\\"))
			{
				m_strDllPath+=_T("\\");
			}
		}
	}

    BOOL CreateImgDecoder(IObjRef ** ppObj)
    {
#ifdef _DEBUG
        SOUI::SStringT strImgDecoder = m_strImgDecoder+_T("d.dll");
#else
        SOUI::SStringT strImgDecoder = m_strImgDecoder+_T(".dll");
#endif
        return imgDecLoader.CreateInstance(m_strDllPath+strImgDecoder,ppObj);
    }
    
    BOOL CreateRender_GDI(IObjRef **ppObj)
    {
        return renderLoader.CreateInstance(m_strDllPath+COM_RENDER_GDI,ppObj);
    }

    BOOL CreateRender_Skia(IObjRef **ppObj)
    {
        return renderLoader.CreateInstance(m_strDllPath+COM_RENDER_SKIA,ppObj);
    }
    BOOL CreateScrpit_Lua(IObjRef **ppObj)
    {
        return scriptLoader.CreateInstance(m_strDllPath+COM_SCRIPT_LUA,ppObj);
    }

    BOOL CreateTranslator(IObjRef **ppObj)
    {
        return transLoader.CreateInstance(m_strDllPath+COM_TRANSLATOR,ppObj);
    }
    BOOL CreateResProvider_ZIP(IObjRef **ppObj)
    {
        return zipResLoader.CreateInstance(m_strDllPath+COM_ZIPRESPROVIDER,ppObj);
    }

	BOOL CreateResProvider_7ZIP(IObjRef **ppObj)
	{
		return zip7ResLoader.CreateInstance(m_strDllPath+COM_7ZIPRESPROVIDER, ppObj);
	}
	
    BOOL CreateLog4z(IObjRef **ppObj)
    {
        return log4zLoader.CreateInstance(m_strDllPath+COM_LOG4Z,ppObj);
    }

	BOOL CreateTaskLoop(IObjRef **ppObj)
	{
		return taskLoopLoader.CreateInstance(m_strDllPath + COM_TASKLOOP, ppObj);
	}
protected:
    //SComLoader实现从DLL的指定函数创建符号SOUI要求的类COM组件。
    SComLoader imgDecLoader;
    SComLoader renderLoader;
    SComLoader transLoader;
    SComLoader scriptLoader;
    SComLoader zipResLoader;
    SComLoader log4zLoader;
    SComLoader zip7ResLoader;
	SComLoader taskLoopLoader;

    SOUI::SStringT m_strImgDecoder;
	SOUI::SStringT m_strDllPath;
};
#endif

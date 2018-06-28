#pragma once

//加载组件辅助类
//组件需要提供SCreateInstance接口。接口定义必须是funSCreateInstance
class SComLoader
{
    typedef BOOL (*funSCreateInstance)(IObjRef **);
public:
    SComLoader()
    :m_hMod(0)
    ,m_funCreateInst(NULL)
    {
        m_szDllPath[0]=0;
    }
    ~SComLoader()
    {
        if(m_hMod) FreeLibrary(m_hMod);
    }

    BOOL CreateInstance(LPCTSTR pszDllPath,IObjRef **ppObj,LPCSTR pszFnName = "SCreateInstance")
    {
        if(!m_funCreateInst)
        {
            m_hMod=LoadLibrary(pszDllPath);
            if(!m_hMod) return FALSE;
            m_funCreateInst=(funSCreateInstance)GetProcAddress(m_hMod,pszFnName);
            if(!m_funCreateInst)
            {
                FreeLibrary(m_hMod);
                return FALSE;
            }
            _tcscpy(m_szDllPath,pszDllPath);
        }
        return m_funCreateInst(ppObj);
    }
protected:
    HMODULE m_hMod;
    funSCreateInstance m_funCreateInst;
    TCHAR   m_szDllPath[MAX_PATH];
};
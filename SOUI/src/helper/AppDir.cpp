#include "souistd.h"
#include "helper/AppDir.h"

namespace SOUI
{
    SAppDir::SAppDir(HINSTANCE hInst)
    {
        TCHAR szCurrentDir[MAX_PATH]={0};
        if (GetModuleFileName(hInst, szCurrentDir, MAX_PATH)) 
        {
            LPTSTR lpInsertPos = _tcsrchr(szCurrentDir, _T('\\'));
            if (lpInsertPos) 
            {
                *lpInsertPos = 0;
            }            
            m_strAppPath = szCurrentDir;
        }        
    }

    const SStringT & SAppDir::AppDir()
    {
        return m_strAppPath;
    }
}
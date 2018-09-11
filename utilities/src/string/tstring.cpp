#include "string/tstring.h"

namespace SOUI
{
    // Globals

    // For an empty string, m_pszData will point here
    // (note: avoids special case of checking for NULL m_pszData)
    // empty string data (and locked)
    static int _tstr_rgInitData[] = { -1, 0, 0, 0, 0, 0, 0, 0 };

    TStringData* _tstr_initDataNil = (TStringData*)&_tstr_rgInitData;
    const void* _tstr_initPszNil = (const void*)(((unsigned char*)&_tstr_rgInitData) + sizeof(TStringData));
}

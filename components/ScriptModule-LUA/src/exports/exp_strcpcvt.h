#include <string/strcpcvt.h>

BOOL ExpLua_StrCpCvt(lua_State *L)
{
    try{
        lua_tinker::def(L,"S_W2A",&SStrCpCvt::CvtW2A);
        lua_tinker::def(L,"S_A2W",&SStrCpCvt::CvtA2W);
        lua_tinker::def(L,"S_A2A",&SStrCpCvt::CvtA2A);
        lua_tinker::def(L,"S_W2W",&SStrCpCvt::CvtW2W);

        return TRUE;
    }
    catch(...)
    {
        return FALSE;
    }
}
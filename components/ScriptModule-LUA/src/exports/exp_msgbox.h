#include <control/SMessageBox.h>

BOOL ExpLua_MessageBox(lua_State *L)
{
	try{
		lua_tinker::def(L,"SMessageBox",SMessageBox);
		return TRUE;
	}catch(...)
	{
		return FALSE;
	}
}
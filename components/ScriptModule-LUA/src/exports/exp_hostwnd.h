#include <core/SHostWnd.h>

SHostWnd * toHostWnd(IObject *pObj)
{
    return sobj_cast<SHostWnd>(pObj);
}

BOOL ExpLua_HostWnd(lua_State *L)
{
	try{
		lua_tinker::class_add<SHostWnd>(L,"SHostWnd");
		lua_tinker::class_def<SHostWnd>(L,"AnimateHostWindow",&SHostWnd::AnimateHostWindow);
		lua_tinker::class_def<SHostWnd>(L,"setTimeout",&SHostWnd::setTimeout);
		lua_tinker::class_def<SHostWnd>(L,"setInterval",&SHostWnd::setInterval);
		lua_tinker::class_def<SHostWnd>(L,"clearTimer",&SHostWnd::clearTimer);
        lua_tinker::class_def<SHostWnd>(L,"GetRoot",&SHostWnd::GetRoot);
        
        lua_tinker::def(L,"toHostWnd",toHostWnd);
		return TRUE;
	}catch(...)
	{
		return FALSE;
	}
}
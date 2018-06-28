#include <event/events.h>


EventTimer * toEventTimer(EventArgs *pEvt)
{
    return sobj_cast<EventTimer>(pEvt);
}

EventSwndSize * toEventSize(EventArgs *pEvt)
{
    return sobj_cast<EventSwndSize>(pEvt);
}

BOOL ExpLua_EventArgs(lua_State *L)
{
	try{
		lua_tinker::class_add<EventArgs>(L,"EventArgs");
        lua_tinker::class_inh<EventArgs,IObject>(L);
		lua_tinker::class_mem<EventArgs>(L,"sender",&EventArgs::sender);
        lua_tinker::class_mem<EventArgs>(L,"idFrom",&EventArgs::idFrom);
        lua_tinker::class_mem<EventArgs>(L,"nameFrom",&EventArgs::nameFrom);
        
        lua_tinker::class_add<EventTimer>(L,"EventTimer");
        lua_tinker::class_inh<EventTimer,EventArgs>(L);
        lua_tinker::class_mem<EventTimer>(L,"uID",&EventTimer::uID);
        lua_tinker::def(L,"toEventTimer",toEventTimer);//类型转换

        lua_tinker::class_add<EventSwndSize>(L,"EventSwndSize");
        lua_tinker::class_inh<EventSwndSize,EventArgs>(L);
        lua_tinker::class_mem<EventSwndSize>(L,"szWnd",&EventSwndSize::szWnd);
        lua_tinker::def(L,"toEventSize",toEventSize);//类型转换

		return TRUE;
	}catch(...)
	{
		return FALSE;
	}
}
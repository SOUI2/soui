#include <interface/SScriptModule-i.h>

BOOL ExpLua_ScriptModule(lua_State *L)
{
	try
	{
		lua_tinker::class_add<IScriptModule>(L,"IScriptModule");
		lua_tinker::class_def<IScriptModule>(L,"GetScriptEngine",&IScriptModule::GetScriptEngine);
		lua_tinker::class_def<IScriptModule>(L,"executeScriptFile",&IScriptModule::executeScriptFile);
		lua_tinker::class_def<IScriptModule>(L,"executeString",&IScriptModule::executeString);
		lua_tinker::class_def<IScriptModule>(L,"executeScriptedEventHandler",&IScriptModule::executeScriptedEventHandler);
		lua_tinker::class_def<IScriptModule>(L,"getIdentifierString",&IScriptModule::getIdentifierString);
		lua_tinker::class_def<IScriptModule>(L,"subscribeEvent",&IScriptModule::subscribeEvent);

		return TRUE;
	}catch(...)
	{
		return FALSE;
	}
}
#include <sobject/sobject.hpp>

void SetObjAttr(IObject *pObj,LPCSTR pszAttr,LPCSTR pszValue)
{
    pObj->SetAttribute(pszAttr,pszValue,FALSE);
}

BOOL ExpLua_SObject(lua_State *L)
{
	try{
		lua_tinker::class_add<IObject>(L,"IObject");
		lua_tinker::class_def<IObject>(L,"IsClass",&IObject::IsClass);
        lua_tinker::class_def<IObject>(L,"GetObjectClass",&IObject::GetObjectClass);
        lua_tinker::class_def<IObject>(L,"InitFromXml",&IObject::InitFromXml);
        lua_tinker::class_def<IObject>(L,"SetAttribute",(HRESULT (IObject::*)(const char *, const char *, BOOL))&IObject::SetAttribute);
        lua_tinker::class_def<IObject>(L,"SetAttributeA",(HRESULT (IObject::*)(const SStringA &, const SStringA &, BOOL))&IObject::SetAttribute);
        lua_tinker::class_def<IObject>(L,"SetAttributeW",(HRESULT (IObject::*)(const SStringW &, const SStringW &, BOOL))&IObject::SetAttribute);
        lua_tinker::class_def<IObject>(L,"GetID",&IObject::GetID);
        lua_tinker::class_def<IObject>(L,"GetName",&IObject::GetName);
        lua_tinker::def(L,"SetObjAttr",SetObjAttr);
		return TRUE;
	}catch(...)
	{
		return FALSE;
	}
}
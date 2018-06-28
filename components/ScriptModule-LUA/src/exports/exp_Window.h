#include <core/swnd.h>

//定义一个从SObject转换成SWindow的方法
SWindow * toSWindow(IObject * pObj)
{
    return sobj_cast<SWindow>(pObj);
}

BOOL ExpLua_Window(lua_State *L)
{
	try{
        lua_tinker::def(L,"toSWindow",toSWindow);

		lua_tinker::class_add<SWindow>(L,"SWindow");
        lua_tinker::class_inh<SWindow,IObject>(L);
		lua_tinker::class_con<SWindow>(L,lua_tinker::constructor<SWindow>);
		lua_tinker::class_def<SWindow>(L,"GetContainer",(ISwndContainer *(SWindow::*)(void))&SWindow::GetContainer);
        lua_tinker::class_def<SWindow>(L,"GetRoot",&SWindow::GetRoot);
        lua_tinker::class_def<SWindow>(L,"GetTopLevelParent",&SWindow::GetTopLevelParent);
		lua_tinker::class_def<SWindow>(L,"GetParent",&SWindow::GetParent);
		lua_tinker::class_def<SWindow>(L,"DestroyChild",&SWindow::DestroyChild);
		lua_tinker::class_def<SWindow>(L,"GetChildrenCount",&SWindow::GetChildrenCount);
		lua_tinker::class_def<SWindow>(L,"FindChildByID",&SWindow::FindChildByID);
        lua_tinker::class_def<SWindow>(L,"FindChildByNameA",(SWindow* (SWindow::*)(LPCSTR,int))&SWindow::FindChildByName);
        lua_tinker::class_def<SWindow>(L,"FindChildByNameW",(SWindow* (SWindow::*)(LPCWSTR,int ))&SWindow::FindChildByName);
 		lua_tinker::class_def<SWindow>(L,"CreateChildrenFromString",(SWindow* (SWindow::*)(LPCWSTR))&SWindow::CreateChildren);
		lua_tinker::class_def<SWindow>(L,"GetTextAlign",&SWindow::GetTextAlign);
		lua_tinker::class_def<SWindow>(L,"GetWindowRect",(void (SWindow::*)(LPRECT))&SWindow::GetWindowRect);
        lua_tinker::class_def<SWindow>(L,"GetWindowRect2",(CRect (SWindow::*)())&SWindow::GetWindowRect);
		lua_tinker::class_def<SWindow>(L,"GetClientRect",(void (SWindow::*)(LPRECT)const)&SWindow::GetClientRect);
        lua_tinker::class_def<SWindow>(L,"GetClientRect2",(CRect (SWindow::*)()const)&SWindow::GetClientRect);
		lua_tinker::class_def<SWindow>(L,"GetWindowText",&SWindow::GetWindowText);
		lua_tinker::class_def<SWindow>(L,"SetWindowText",&SWindow::SetWindowText);
		lua_tinker::class_def<SWindow>(L,"SendSwndMessage",&SWindow::SSendMessage);
		lua_tinker::class_def<SWindow>(L,"GetID",&SWindow::GetID);
		lua_tinker::class_def<SWindow>(L,"SetID",&SWindow::SetID);
		lua_tinker::class_def<SWindow>(L,"GetUserData",&SWindow::GetUserData);
		lua_tinker::class_def<SWindow>(L,"SetUserData",&SWindow::SetUserData);
		lua_tinker::class_def<SWindow>(L,"GetName",&SWindow::GetName);
		lua_tinker::class_def<SWindow>(L,"GetSwnd",&SWindow::GetSwnd);
		lua_tinker::class_def<SWindow>(L,"InsertChild",&SWindow::InsertChild);
		lua_tinker::class_def<SWindow>(L,"RemoveChild",&SWindow::RemoveChild);
		lua_tinker::class_def<SWindow>(L,"IsChecked",&SWindow::IsChecked);
		lua_tinker::class_def<SWindow>(L,"IsDisabled",&SWindow::IsDisabled);
		lua_tinker::class_def<SWindow>(L,"IsVisible",&SWindow::IsVisible);
		lua_tinker::class_def<SWindow>(L,"SetVisible",&SWindow::SetVisible);
		lua_tinker::class_def<SWindow>(L,"EnableWindow",&SWindow::EnableWindow);
		lua_tinker::class_def<SWindow>(L,"SetCheck",&SWindow::SetCheck);
		lua_tinker::class_def<SWindow>(L,"SetOwner",&SWindow::SetOwner);
		lua_tinker::class_def<SWindow>(L,"GetOwner",&SWindow::GetOwner);
		lua_tinker::class_def<SWindow>(L,"Invalidate",&SWindow::Invalidate);
        lua_tinker::class_def<SWindow>(L,"InvalidateRect",(void (SWindow::*)(LPCRECT))&SWindow::InvalidateRect);
		lua_tinker::class_def<SWindow>(L,"AnimateWindow",&SWindow::AnimateWindow);
        lua_tinker::class_def<SWindow>(L,"GetScriptModule",&SWindow::GetScriptModule);
        lua_tinker::class_def<SWindow>(L,"Move2",(void (SWindow::*)(int,int,int,int))&SWindow::Move);
        lua_tinker::class_def<SWindow>(L,"Move",(void (SWindow::*)(LPCRECT))&SWindow::Move);
        lua_tinker::class_def<SWindow>(L,"FireCommand",&SWindow::FireCommand);
		lua_tinker::class_def<SWindow>(L,"GetDesiredSize",(CSize (SWindow::*)(int, int))&SWindow::GetDesiredSize);
        lua_tinker::class_def<SWindow>(L,"GetWindow",&SWindow::GetWindow);
        lua_tinker::class_def<SWindow>(L,"SetWindowRgn",&SWindow::SetWindowRgn);
        lua_tinker::class_def<SWindow>(L,"GetWindowRgn",&SWindow::GetWindowRgn);

		return TRUE;
	}catch(...)
	{
		return FALSE;
	}
}
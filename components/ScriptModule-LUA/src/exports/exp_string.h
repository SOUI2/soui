#include <string/tstring.h>

BOOL ExpLua_String(lua_State *L)
{
	try{

		lua_tinker::class_add<SStringA>(L,"SStringA");
        lua_tinker::class_con<SStringA>(L,lua_tinker::constructor<SStringA>);
        lua_tinker::class_con<SStringA>(L,lua_tinker::constructor<SStringA,const SStringA &>);
        lua_tinker::class_con<SStringA>(L,lua_tinker::constructor<SStringA,const char*>);
        
        lua_tinker::class_def<SStringA>(L,"GetLength",&SStringA::GetLength);
        lua_tinker::class_def<SStringA>(L,"IsEmpty",&SStringA::IsEmpty);
        lua_tinker::class_def<SStringA>(L,"Empty",&SStringA::Empty);
        lua_tinker::class_def<SStringA>(L,"GetAt",&SStringA::GetAt);
        lua_tinker::class_def<SStringA>(L,"SetAt",&SStringA::SetAt);
        lua_tinker::class_def<SStringA>(L,"Mid",(SStringA (SStringA::*)(int) const)&SStringA::Mid);
        lua_tinker::class_def<SStringA>(L,"Mid2",(SStringA (SStringA::*)(int,int) const)&SStringA::Mid);
        lua_tinker::class_def<SStringA>(L,"Left",&SStringA::Left);
        lua_tinker::class_def<SStringA>(L,"Right",&SStringA::Right);
        lua_tinker::class_def<SStringA>(L,"MakeUpper",&SStringA::MakeUpper);
        lua_tinker::class_def<SStringA>(L,"MakeLower",&SStringA::MakeLower);
        lua_tinker::class_def<SStringA>(L,"TrimRight",&SStringA::TrimRight);
        lua_tinker::class_def<SStringA>(L,"TrimLeft",&SStringA::TrimLeft);
        lua_tinker::class_def<SStringA>(L,"Trim",&SStringA::Trim);
        lua_tinker::class_def<SStringA>(L,"InsertChar",(int (SStringA::*)(int,char))&SStringA::Insert);
        lua_tinker::class_def<SStringA>(L,"InsertStr",(int (SStringA::*)(int,const char*))&SStringA::Insert);
        lua_tinker::class_def<SStringA>(L,"Delete",&SStringA::Delete);
        lua_tinker::class_def<SStringA>(L,"ReplaceChar",(int (SStringA::*)(char,char))&SStringA::Replace);
        lua_tinker::class_def<SStringA>(L,"ReplaceStr",(int (SStringA::*)(const char*,const char*))&SStringA::Replace);
        lua_tinker::class_def<SStringA>(L,"Remove",&SStringA::Remove);
        lua_tinker::class_def<SStringA>(L,"FindChar",(int (SStringA::*)(char,int) const)&SStringA::Find);
        lua_tinker::class_def<SStringA>(L,"FindStr",(int (SStringA::*)(const char *,int) const)&SStringA::Find);
        lua_tinker::class_def<SStringA>(L,"ReverseFind",&SStringA::ReverseFind);
        lua_tinker::class_def<SStringA>(L,"GetBuffer",&SStringA::GetBuffer);
        lua_tinker::class_def<SStringA>(L,"ReleaseBuffer",&SStringA::ReleaseBuffer);
        lua_tinker::class_def<SStringA>(L,"GetBufferSetLength",&SStringA::GetBufferSetLength);
        lua_tinker::class_def<SStringA>(L,"SetLength",&SStringA::SetLength);
        lua_tinker::class_def<SStringA>(L,"LockBuffer",&SStringA::LockBuffer);
        lua_tinker::class_def<SStringA>(L,"UnlockBuffer",&SStringA::UnlockBuffer);
        lua_tinker::class_def<SStringA>(L,"AppendChar",(const SStringA & (SStringA::*)(char))&SStringA::Append);
        lua_tinker::class_def<SStringA>(L,"AppendPsz",(const SStringA & (SStringA::*)(const char *))&SStringA::Append);
        lua_tinker::class_def<SStringA>(L,"AppendStr",(const SStringA & (SStringA::*)(const SStringA &))&SStringA::Append);


        lua_tinker::class_add<SStringW>(L,"SStringW");
        lua_tinker::class_con<SStringW>(L,lua_tinker::constructor<SStringW>);
        lua_tinker::class_con<SStringW>(L,lua_tinker::constructor<SStringW,const SStringW &>);
        lua_tinker::class_con<SStringW>(L,lua_tinker::constructor<SStringW,const wchar_t*>);

        lua_tinker::class_def<SStringW>(L,"GetLength",&SStringW::GetLength);
        lua_tinker::class_def<SStringW>(L,"IsEmpty",&SStringW::IsEmpty);
        lua_tinker::class_def<SStringW>(L,"Empty",&SStringW::Empty);
        lua_tinker::class_def<SStringW>(L,"GetAt",&SStringW::GetAt);
        lua_tinker::class_def<SStringW>(L,"SetAt",&SStringW::SetAt);
        lua_tinker::class_def<SStringW>(L,"Mid",(SStringW (SStringW::*)(int) const)&SStringW::Mid);
        lua_tinker::class_def<SStringW>(L,"Mid2",(SStringW (SStringW::*)(int,int) const)&SStringW::Mid);
        lua_tinker::class_def<SStringW>(L,"Left",&SStringW::Left);
        lua_tinker::class_def<SStringW>(L,"Right",&SStringW::Right);
        lua_tinker::class_def<SStringW>(L,"MakeUpper",&SStringW::MakeUpper);
        lua_tinker::class_def<SStringW>(L,"MakeLower",&SStringW::MakeLower);
        lua_tinker::class_def<SStringW>(L,"TrimRight",&SStringW::TrimRight);
        lua_tinker::class_def<SStringW>(L,"TrimLeft",&SStringW::TrimLeft);
        lua_tinker::class_def<SStringW>(L,"Trim",&SStringW::Trim);
        lua_tinker::class_def<SStringW>(L,"InsertChar",(int (SStringW::*)(int,wchar_t))&SStringW::Insert);
        lua_tinker::class_def<SStringW>(L,"InsertStr",(int (SStringW::*)(int,const wchar_t*))&SStringW::Insert);
        lua_tinker::class_def<SStringW>(L,"Delete",&SStringW::Delete);
        lua_tinker::class_def<SStringW>(L,"ReplaceChar",(int (SStringW::*)(wchar_t,wchar_t))&SStringW::Replace);
        lua_tinker::class_def<SStringW>(L,"ReplaceStr",(int (SStringW::*)(const wchar_t*,const wchar_t*))&SStringW::Replace);
        lua_tinker::class_def<SStringW>(L,"Remove",&SStringW::Remove);
        lua_tinker::class_def<SStringW>(L,"FindChar",(int (SStringW::*)(wchar_t,int) const)&SStringW::Find);
        lua_tinker::class_def<SStringW>(L,"FindStr",(int (SStringW::*)(const wchar_t *,int) const)&SStringW::Find);
        lua_tinker::class_def<SStringW>(L,"ReverseFind",&SStringW::ReverseFind);
        lua_tinker::class_def<SStringW>(L,"GetBuffer",&SStringW::GetBuffer);
        lua_tinker::class_def<SStringW>(L,"ReleaseBuffer",&SStringW::ReleaseBuffer);
        lua_tinker::class_def<SStringW>(L,"GetBufferSetLength",&SStringW::GetBufferSetLength);
        lua_tinker::class_def<SStringW>(L,"SetLength",&SStringW::SetLength);
        lua_tinker::class_def<SStringW>(L,"LockBuffer",&SStringW::LockBuffer);
        lua_tinker::class_def<SStringW>(L,"UnlockBuffer",&SStringW::UnlockBuffer);
        lua_tinker::class_def<SStringW>(L,"AppendChar",(const SStringW & (SStringW::*)(wchar_t))&SStringW::Append);
        lua_tinker::class_def<SStringW>(L,"AppendPsz",(const SStringW & (SStringW::*)(const wchar_t *))&SStringW::Append);
        lua_tinker::class_def<SStringW>(L,"AppendStr",(const SStringW & (SStringW::*)(const SStringW &))&SStringW::Append);
		return TRUE;
	}catch(...)
	{
		return FALSE;
	}

}
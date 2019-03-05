#pragma once

#define FUNID(id) \
enum{FUN_ID=id};\
UINT GetID() {return FUN_ID;}

#define FUN_BEGIN \
bool HandleFun(UINT uMsg, SOUI::SParamStream &ps){ \
	bool bHandled = false; \

#define FUN_HANDLER(x,fun) \
	if(!bHandled && uMsg == x::FUN_ID) \
	{\
		x param; \
		param.FromStream4Input(ps);\
		fun(param); \
		SOUI::SParamStream psOut(ps.GetBuffer(),true);\
		param.ToStream4Output(psOut);\
		bHandled = true;\
	}

#define FUN_END \
	return bHandled; \
}

#define CHAIN_MSG_MAP_2_IPC(ipc) \
		if(ipc)\
		{\
			BOOL bHandled = FALSE;\
			lResult = (ipc)->OnMessage((ULONG_PTR)hWnd,uMsg,wParam,lParam,bHandled);\
			if(bHandled)\
			{\
				return true;\
			}\
		}

/////////////////////////////////////////////////////////////////////
template<typename P1>
void toParamStream(SOUI::SParamStream &  ps, P1 &p1)
{
	ps << p1;
}
template<typename P1>
void fromParamStream(SOUI::SParamStream &  ps, P1 & p1)
{
	ps >> p1;
}

#define PARAMS1(type,p1) \
void ToStream4##type(SOUI::SParamStream &  ps){ toParamStream(ps,p1);}\
void FromStream4##type(SOUI::SParamStream &  ps){fromParamStream(ps,p1);}\

/////////////////////////////////////////////////////////////
template<typename P1, typename P2>
void toParamStream(SOUI::SParamStream &  ps, P1 &p1, P2 & p2)
{
	ps << p1 << p2;
}
template<typename P1, typename P2>
void fromParamStream(SOUI::SParamStream &  ps, P1 & p1, P2 &p2)
{
	ps >> p1 >> p2;
}

#define PARAMS2(type,p1,p2) \
void ToStream4##type(SOUI::SParamStream &  ps){ toParamStream(ps,p1,p2);}\
void FromStream4##type(SOUI::SParamStream &  ps){fromParamStream(ps,p1,p2);}\


////////////////////////////////////////////////////////////////////
template<typename P1, typename P2, typename P3>
void toParamStream(SOUI::SParamStream &  ps, P1 &p1, P2 & p2, P3 & p3)
{
	ps << p1 << p2 << p3;
}
template<typename P1, typename P2, typename P3>
void fromParamStream(SOUI::SParamStream &  ps, P1 & p1, P2 &p2, P3 & p3)
{
	ps >> p1 >> p2 >> p3;
}

#define PARAMS3(type,p1,p2,p3) \
void ToStream4##type(SOUI::SParamStream &  ps){ toParamStream(ps,p1,p2,p3);}\
void FromStream4##type(SOUI::SParamStream &  ps){fromParamStream(ps,p1,p2,p3);}\

///////////////////////////////////////////////////////////////////
template<typename P1, typename P2, typename P3, typename P4>
void toParamStream(SOUI::SParamStream &  ps, P1 &p1, P2 & p2, P3 & p3, P4 & p4)
{
	ps << p1 << p2 << p3<<p4;
}
template<typename P1, typename P2, typename P3, typename P4>
void fromParamStream(SOUI::SParamStream &  ps, P1 & p1, P2 &p2, P3 & p3, P4 & p4)
{
	ps >> p1 >> p2 >> p3>>p4;
}

#define PARAMS4(type,p1,p2,p3,p4) \
void ToStream4##type(SOUI::SParamStream &  ps){ toParamStream(ps,p1,p2,p3,p4);}\
void FromStream4##type(SOUI::SParamStream &  ps){fromParamStream(ps,p1,p2,p3,p4);}\


/////////////////////////////////////////////////////////////////////////
template<typename P1, typename P2, typename P3, typename P4, typename P5>
void toParamStream(SOUI::SParamStream &  ps, P1 &p1, P2 & p2, P3 & p3, P4 & p4, P5 &p5)
{
	ps << p1 << p2 << p3 << p4 <<p5;
}
template<typename P1, typename P2, typename P3, typename P4, typename P5>
void fromParamStream(SOUI::SParamStream &  ps, P1 & p1, P2 &p2, P3 & p3, P4 & p4, P5 &p5)
{
	ps >> p1 >> p2 >> p3 >> p4>>p5;
}

#define PARAMS5(type,p1,p2,p3,p4,p5) \
void ToStream4##type(SOUI::SParamStream &  ps){ toParamStream(ps,p1,p2,p3,p4,p5);}\
void FromStream4##type(SOUI::SParamStream &  ps){fromParamStream(ps,p1,p2,p3,p4,p5);}\


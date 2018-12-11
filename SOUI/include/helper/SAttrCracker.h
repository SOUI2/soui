/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       SAttrCracker.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/15
* 
* Describe    定义SOUI的XML属性解析宏
*/

#ifndef _SATTRCRACK_H
#define _SATTRCRACK_H

#pragma  once


// Attribute Declaration
#define SOUI_ATTRS_BEGIN()                            \
public:                                                             \
    virtual HRESULT SetAttribute(                                   \
    const SOUI::SStringW & strAttribName,                                     \
    const SOUI::SStringW &  strValue,                                          \
    BOOL     bLoading=FALSE)                                    \
    {                                                               \
    HRESULT hRet = E_FAIL;                                        \
 

//从SObject派生的类是属性结尾
#define SOUI_ATTRS_END()                                        \
		return __super::SetAttribute(                           \
						strAttribName,                          \
						strValue,                               \
						bLoading                                \
						);                                      \
    return AfterAttribute(strAttribName,strValue,bLoading,hRet);         \
    }                                                           \
    

//不交给SObject处理的属性表结尾
#define SOUI_ATTRS_BREAK()                                      \
        return E_NOTIMPL;                                       \
    return hRet;                                                \
    }                                                           \

 
#define ATTR_CHAIN(varname,flag)                               \
    if (SUCCEEDED(hRet = varname.SetAttribute(strAttribName, strValue, bLoading)))   \
        {                                                           \
			hRet |= flag;											\
        }                                                           \
        else                                                        \

#define ATTR_CHAIN_PTR(varname,flag)                               \
	if (varname!= NULL && SUCCEEDED(hRet = varname->SetAttribute(strAttribName, strValue, bLoading)))   \
		{                                                           \
			hRet |= flag;											\
		}                                                           \
		else                                                        \


#define ATTR_CUSTOM(attribname, func)                    \
    if (0 == strAttribName.CompareNoCase(attribname))                            \
        {                                                           \
        hRet = func(strValue, bLoading);                        \
        }                                                           \
        else                                                        \

#define STRINGASBOOL(strValue) (strValue).CompareNoCase(L"0") != 0 && (strValue).CompareNoCase(L"false") != 0

#define ATTR_BOOL(attribname, varname, allredraw)         \
	if (0 == strAttribName.CompareNoCase(attribname))                            \
		{                                                           \
		varname=STRINGASBOOL(strValue); \
		hRet = allredraw ? S_OK : S_FALSE;                      \
		}                                                           \
		else                                                        \

// Int = %d StringA
#define ATTR_INT(attribname, varname, allredraw)         \
    if (0 == strAttribName.CompareNoCase(attribname))                            \
        {                                                           \
        int nRet=0;                                                \
        ::StrToIntExW(strValue,STIF_SUPPORT_HEX,&nRet);            \
        varname=nRet;                                            \
        hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \


#define ATTR_LAYOUTSIZE(attribname, varname, allredraw)             \
	if (0 == strAttribName.CompareNoCase(attribname))               \
		{                                                           \
                varname = GETLAYOUTSIZE(strValue);                  \
                hRet = allredraw ? S_OK : S_FALSE;                  \
		}                                                           \
		else                                                        \



#define ATTR_LAYOUTSIZE2(attribname, varname, allredraw)             \
	if (0 == strAttribName.CompareNoCase(attribname))               \
		{                                                           \
			SStringWList values;									\
			if(SplitString(strValue,L',',values)!=2) return E_INVALIDARG;\
			varname[0]=GETLAYOUTSIZE(values[0]);                    \
			varname[1]=GETLAYOUTSIZE(values[1]);                    \
			hRet = allredraw ? S_OK : S_FALSE;                      \
		}                                                           \
		else                                                        \

#define ATTR_LAYOUTSIZE4(attribname, varname, allredraw)             \
	if (0 == strAttribName.CompareNoCase(attribname))               \
		{                                                           \
			SStringWList values;									\
			if(SplitString(strValue,L',',values)!=4) return E_INVALIDARG;\
			varname[0]=GETLAYOUTSIZE(values[0]);                    \
			varname[1]=GETLAYOUTSIZE(values[1]);                    \
			varname[2]=GETLAYOUTSIZE(values[2]);                    \
			varname[3]=GETLAYOUTSIZE(values[3]);                    \
			hRet = allredraw ? S_OK : S_FALSE;                      \
		}                                                           \
		else                                                        \

#define ATTR_MARGIN(attribname, varname, allredraw)             \
	if (0 == strAttribName.CompareNoCase(attribname))               \
		{                                                           \
			SStringWList values;									\
			if(SplitString(strValue,L',',values)!=4) return E_INVALIDARG;\
			varname.left=GETLAYOUTSIZE(values[0]).toPixelSize(100); \
			varname.top=GETLAYOUTSIZE(values[1]).toPixelSize(100);  \
			varname.right=GETLAYOUTSIZE(values[2]).toPixelSize(100);\
			varname.bottom=GETLAYOUTSIZE(values[3]).toPixelSize(100);\
			hRet = allredraw ? S_OK : S_FALSE;                      \
		}                                                           \
		else                                                        \


// Rect = %d,%d,%d,%d StringA
#define ATTR_RECT(attribname, varname, allredraw)         \
    if (0 == strAttribName.CompareNoCase(attribname))                            \
        {                                                           \
        swscanf_s(strValue,L"%d,%d,%d,%d",&varname.left,&varname.top,&varname.right,&varname.bottom);\
        hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \
 

// Size = %d,%d StringA
#define ATTR_SIZE(attribname, varname, allredraw)         \
    if (0 == strAttribName.CompareNoCase(attribname))                            \
        {                                                           \
        swscanf_s(strValue,L"%d,%d",&varname.cx,&varname.cy);\
        hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \
 
// Point = %d,%d StringA
#define ATTR_POINT(attribname, varname, allredraw)         \
    if (0 == strAttribName.CompareNoCase(attribname))                            \
        {                                                           \
        swscanf_s(strValue,L"%d,%d",&varname.x,&varname.y);\
        hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \
 

// Float = %f StringA
#define ATTR_FLOAT(attribname, varname, allredraw)         \
    if (0 == strAttribName.CompareNoCase(attribname))                            \
        {                                                           \
        swscanf_s(strValue,L"%f",&varname);                        \
        hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \
 
// UInt = %u StringA
#define ATTR_UINT(attribname, varname, allredraw)        \
    if (0 == strAttribName.CompareNoCase(attribname))                            \
        {                                                           \
        int nRet=0;                                                \
        ::StrToIntExW(strValue,STIF_SUPPORT_HEX,&nRet);            \
        varname = (UINT)nRet;                                    \
        hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \
 
// DWORD = %u StringA
#define ATTR_DWORD(attribname, varname, allredraw)       \
    if (0 == strAttribName.CompareNoCase(attribname))                            \
        {                                                           \
        int nRet=0;                                                \
        ::StrToIntExW(strValue,STIF_SUPPORT_HEX,&nRet);            \
        varname = (DWORD)nRet;                                    \
        hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \
 
// WORD = %u StringA
#define ATTR_WORD(attribname, varname, allredraw)       \
    if (0 == strAttribName.CompareNoCase(attribname))                            \
        {                                                           \
        int nRet=0;                                                \
        ::StrToIntExW(strValue,STIF_SUPPORT_HEX,&nRet);            \
        varname = (WORD)nRet;                                    \
        hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \
 

// bool = 0 or 1 StringA
#define ATTR_BIT(attribname, varname, maskbit, allredraw) \
    if (0 == strAttribName.CompareNoCase(attribname))                            \
        {                                                           \
        int nRet=0;                                                \
        ::StrToIntW(strValue,&nRet);                                \
        if(nRet) varname|=maskbit;                                \
            else     varname &=~(maskbit);                            \
            hRet = allredraw ? S_OK : S_FALSE;                        \
        }                                                           \
        else                                                        \
 

// StringA = StringA
#define ATTR_STRINGA(attribname, varname, allredraw)                \
    if (0 == strAttribName.CompareNoCase(attribname))               \
        {                                                           \
        SOUI::SStringW strTmp=GETSTRING(strValue);                      \
        varname = S_CW2A(strTmp);                                   \
        hRet = allredraw ? S_OK : S_FALSE;                          \
        }                                                           \
        else                                                        \
 
// StringW = StringA
#define ATTR_STRINGW(attribname, varname, allredraw)      \
    if (0 == strAttribName.CompareNoCase(attribname))                            \
        {                                                           \
        varname=GETSTRING(strValue);                          \
        hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \
 

// StringT = StringA
#define ATTR_STRINGT(attribname, varname, allredraw)     \
    if (0 == strAttribName.CompareNoCase(attribname))                            \
        {                                                           \
        varname=S_CW2T(GETSTRING(strValue));                          \
        hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \


// StringA = StringA
#define ATTR_I18NSTRA(attribname, varname, allredraw)      \
    if (0 == strAttribName.CompareNoCase(attribname))       \
        {                                                       \
        SOUI::SStringW strTmp=tr(GETSTRING(strValue));               \
        varname = S_CW2A(strTmp);                                     \
        hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \



// STrText = StringA
#define ATTR_I18NSTRT(attribname, varname, allredraw)           \
	if (0 == strAttribName.CompareNoCase(attribname))           \
		{                                                       \
		SOUI::SStringW strTmp=GETSTRING(strValue);              \
		varname.SetText(S_CW2T(strTmp));                        \
		hRet = allredraw ? S_OK : S_FALSE;                      \
		}                                                       \
		else                                                    \


// DWORD = 0x08x StringA
#define ATTR_HEX(attribname, varname, allredraw)                \
    if (0 == strAttribName.CompareNoCase(attribname))           \
        {                                                       \
        int nRet=0;                                             \
        ::StrToIntExW(strValue,STIF_SUPPORT_HEX,&nRet);         \
        varname = nRet;                                         \
        hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                       \
        else                                                    \
 
// COLORREF = #06X or #08x or rgba(r,g,b,a) or rgb(r,g,b)
#define ATTR_COLOR(attribname, varname, allredraw)                  \
    if (0 == strAttribName.CompareNoCase(attribname))               \
        {                                                           \
            if(!strValue.IsEmpty())                                 \
            {                                                       \
                varname = GETCOLOR(strValue);                  \
                hRet = allredraw ? S_OK : S_FALSE;                  \
            }else                                                   \
            {                                                       \
                hRet=E_FAIL;                                        \
            }                                                       \
        }                                                           \
        else                                                        \
 

//font="face:宋体;bold:1;italic:1;underline:1;adding:10"
#define ATTR_FONT(attribname, varname, allredraw)                       \
    if (0 == strAttribName.CompareNoCase(attribname))                   \
    {                                                                   \
        varname.SetFontDesc(strValue,GetScale());                       \
        hRet = allredraw ? S_OK : S_FALSE;                              \
    }                                                                   \
    else                                                                \

//font="face:宋体;bold:1;italic:1;underline:1;adding:10"
#define ATTR_FONT2(attribname, varname, allredraw)                       \
	if (0 == strAttribName.CompareNoCase(attribname))                   \
	{                                                                   \
	varname=SFontPool::getSingleton().GetFont(strValue,GetScale());     \
	hRet = allredraw ? S_OK : S_FALSE;                              \
	}                                                                   \
	else                                                                \


// Value In {String1 : Value1, String2 : Value2 ...}
#define ATTR_ENUM_BEGIN(attribname, vartype, allredraw)        \
    if (0 == strAttribName.CompareNoCase(attribname))                   \
        {                                                           \
        vartype varTemp;                                        \
        \
        hRet = allredraw ? S_OK : S_FALSE;                      \
 
#define ATTR_ENUM_VALUE(enumstring, enumvalue)                     \
    if (strValue == enumstring)                             \
    varTemp = enumvalue;                                \
            else                                                    \
 
#define ATTR_ENUM_END(varname)                                     \
    return E_FAIL;                                      \
    \
    varname = varTemp;                                      \
        }                                                           \
        else                                                        \
 
// SwndStyle From StringA Key
#define ATTR_STYLE(attribname, varname, allredraw)       \
    if (0 == strAttribName.CompareNoCase(attribname))                            \
        {                                                           \
        GETSTYLE(strValue,varname);                  \
        hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \
 
// SSkinPool From StringA Key
#define ATTR_SKIN(attribname, varname, allredraw)                   \
    if (0 == strAttribName.CompareNoCase(attribname))               \
        {                                                           \
        varname = GETSKIN(strValue,GetScale());                     \
        hRet = allredraw ? S_OK : S_FALSE;                          \
        }                                                           \
        else                                                        \
 

// SSkinPool From StringA Key
#define ATTR_INTERPOLATOR(attribname, varname, allredraw)           \
	if (0 == strAttribName.CompareNoCase(attribname))               \
		{                                                           \
		varname.Attach(CREATEINTERPOLATOR(strValue));               \
		hRet = allredraw ? S_OK : S_FALSE;                          \
		}                                                           \
		else                                                        \


//ATTR_IMAGE:直接使用IResProvider::LoadImage创建SOUI::IBitmap对象，创建成功后引用计数为1
//不需要调用AddRef，但是用完后需要调用Release
#define ATTR_IMAGE(attribname, varname, allredraw)                  \
    if (0 == strAttribName.CompareNoCase(attribname))               \
    {                                                               \
        SOUI::IBitmap *pImg=LOADIMAGE2(strValue);                   \
        if(!pImg) hRet =E_FAIL;                                     \
        else{                                                       \
            if(varname) varname->Release();                         \
            varname = pImg;                                         \
            hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
     }                                                              \
     else                                                           \


//ATTR_IMAGEAUTOREF:varname应该是一个CAutoRefPtr<SOUI::IBitmap>对象
#define ATTR_IMAGEAUTOREF(attribname, varname, allredraw)           \
    if (0 == strAttribName.CompareNoCase(attribname))               \
    {                                                               \
        SOUI::IBitmap *pImg=LOADIMAGE2(strValue);                         \
        if(!pImg) hRet =E_FAIL;                                     \
        else{                                                       \
            varname = pImg;                                         \
            pImg->Release();                                        \
            hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
     }                                                              \
     else                                                           \

 
#define ATTR_ICON(attribname, varname, allredraw)                  \
    if (0 == strAttribName.CompareNoCase(attribname))              \
    {                                                              \
        if(varname) DestroyIcon(varname);                         \
        varname = LOADICON2(strValue);                             \
        hRet = allredraw ? S_OK : S_FALSE;                         \
    }                                                              \
    else                                                           \

#define ATTR_CHAR(attribname, varname, allredraw)                  \
    if (0 == strAttribName.CompareNoCase(attribname))              \
    {                                                              \
        varname = *(LPCWSTR)strValue;                              \
        hRet = allredraw ? S_OK : S_FALSE;                         \
    }                                                              \
    else                                                           \

#endif//_SATTRCRACK_H
#pragma once

#include "SSkinObjBase.h"
#include "../layout/SLayoutSize.h"
#include "../helper/SplitString.h"

namespace SOUI
{

// State Define
enum
{
    WndState_Normal       = 0x00000000UL,
    WndState_Hover        = 0x00000001UL,
    WndState_PushDown     = 0x00000002UL,
    WndState_Check        = 0x00000004UL,
    WndState_Invisible    = 0x00000008UL,
    WndState_Disable      = 0x00000010UL,
};

#define IIF_STATE2(the_state, normal_value, hover_value) \
    (((the_state) & SOUI::WndState_Hover) ? (hover_value) : (normal_value))

#define IIF_STATE3(the_state, normal_value, hover_value, pushdown_value) \
    (((the_state) & (SOUI::WndState_PushDown|SOUI::WndState_Check)) ? (pushdown_value) : IIF_STATE2(the_state, normal_value, hover_value))

#define IIF_STATE4(the_state, normal_value, hover_value, pushdown_value, disable_value) \
    (((the_state) & SOUI::WndState_Disable) ? (disable_value) : IIF_STATE3(the_state, normal_value, hover_value, pushdown_value))


//////////////////////////////////////////////////////////////////////////


class SOUI_EXP SSkinImgList: public SSkinObjBase
{
    SOUI_CLASS_NAME(SSkinImgList, L"imglist")

public:
    SSkinImgList();
    virtual ~SSkinImgList();


    virtual SIZE GetSkinSize();

    virtual BOOL IgnoreState();
    
    virtual int GetStates();
    virtual void SetStates(int nStates){m_nStates=nStates;}



    virtual bool SetImage(IBitmap *pImg)
    {
        m_pImg=pImg;
        return true;
    }

    virtual IBitmap * GetImage()
    {
        return m_pImg;
    }

    virtual void SetTile(BOOL bTile){m_bTile=bTile;}
    virtual BOOL IsTile(){return m_bTile;}

    virtual void SetVertical(BOOL bVertical){m_bVertical=bVertical;}
    virtual BOOL IsVertical(){return m_bVertical;}
    
    virtual void OnColorize(COLORREF cr);

protected:
	virtual void _Scale(ISkinObj *skinObj, int nScale);
    virtual void _Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState,BYTE byAlpha);

    virtual UINT GetExpandMode();
	HRESULT OnAttrStateMap(const SStringW & strValue, BOOL bLoading);
    
    CAutoRefPtr<IBitmap> m_pImg;
	int  m_nStates;				  // skin 状态值 
    BOOL m_bTile;
    BOOL m_bAutoFit;
    BOOL m_bVertical;
    CAutoRefPtr<IBitmap> m_imgBackup;   //色调调整前的备分

    FilterLevel m_filterLevel;
	SArray<DWORD> m_arrStateMap;				// 状态 映射
    SOUI_ATTRS_BEGIN()
        ATTR_IMAGEAUTOREF(L"src", m_pImg, FALSE)    //skinObj引用的图片文件定义在uires.idx中的name属性。
        ATTR_INT(L"tile", m_bTile, FALSE)    //绘制是否平铺,0--位伸（默认），其它--平铺
        ATTR_INT(L"autoFit", m_bAutoFit, FALSE)//autoFit为0时不自动适应绘图区大小
        ATTR_INT(L"vertical", m_bVertical, FALSE)//子图是否垂直排列，0--水平排列(默认), 其它--垂直排列
        ATTR_INT(L"states",m_nStates, FALSE)  //子图数量,默认为1
        ATTR_ENUM_BEGIN(L"filterLevel",FilterLevel,FALSE)
            ATTR_ENUM_VALUE(L"none",kNone_FilterLevel)
            ATTR_ENUM_VALUE(L"low",kLow_FilterLevel)
            ATTR_ENUM_VALUE(L"medium",kMedium_FilterLevel)
            ATTR_ENUM_VALUE(L"high",kHigh_FilterLevel)
        ATTR_ENUM_END(m_filterLevel)
		ATTR_CUSTOM(L"stateMap", OnAttrStateMap)			// 状态 映射
    SOUI_ATTRS_END()
};

class SOUI_EXP SSkinImgCenter : public SSkinImgList
{
	SOUI_CLASS_NAME(SSkinImgCenter, L"imgCenter")
public:
	SSkinImgCenter() {}

protected:
	virtual void _Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState, BYTE byAlpha);
};

//////////////////////////////////////////////////////////////////////////
class SOUI_EXP SSkinImgFrame : public SSkinImgList
{
    SOUI_CLASS_NAME(SSkinImgFrame, L"imgframe")

public:
    SSkinImgFrame();


    void SetMargin(const CRect rcMargin){m_rcMargin=rcMargin;}

    CRect GetMargin(){return m_rcMargin;}

protected:
    virtual void _Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState,BYTE byAlpha);
    virtual UINT GetExpandMode();
	virtual void _Scale(ISkinObj *skinObj, int nScale);

    CRect m_rcMargin;

    SOUI_ATTRS_BEGIN()
        ATTR_RECT(L"margin" ,m_rcMargin,FALSE)          //九宫格4周
        ATTR_INT(L"left", m_rcMargin.left, FALSE)        //九宫格左边距
        ATTR_INT(L"top", m_rcMargin.top, FALSE)          //九宫格上边距
        ATTR_INT(L"right", m_rcMargin.right, FALSE)      //九宫格右边距
        ATTR_INT(L"bottom", m_rcMargin.bottom, FALSE)    //九宫格下边距
        ATTR_INT(L"margin-x", m_rcMargin.left=m_rcMargin.right, FALSE)//九宫格左右边距
        ATTR_INT(L"margin-y", m_rcMargin.top=m_rcMargin.bottom, FALSE)//九宫格上下边距
		ATTR_MARGIN(L"margin2", m_rcMargin,FALSE)
    SOUI_ATTRS_END()
};

//////////////////////////////////////////////////////////////////////////
// 加载android .9的皮肤
class SOUI_EXP SSkinImgFrame2 : public SSkinImgFrame
{
	SOUI_CLASS_NAME(SSkinImgFrame2, L"imgframe2")
public:
	SOUI_ATTRS_BEGIN()
		ATTR_CUSTOM(L"src" ,OnAttrSrc)
	SOUI_ATTRS_END()

protected:
	HRESULT OnAttrSrc(const SStringW & strValue,BOOL bLoading);
};

//////////////////////////////////////////////////////////////////////////
class SOUI_EXP SSkinButton : public SSkinObjBase
{
    SOUI_CLASS_NAME(SSkinButton, L"button")

    enum{
        ST_NORMAL=0,
        ST_HOVER,
        ST_PUSHDOWN,
        ST_DISABLE,
    };

    struct BTNCOLORS{
        COLORREF m_crBorder[4];

        COLORREF    m_crUp[4];
        COLORREF    m_crDown[4];
    };
public:
    SSkinButton();


    virtual BOOL IgnoreState();

    virtual int GetStates();

    void SetColors(COLORREF crUp[4],COLORREF crDown[4],COLORREF crBorder[4]);

	virtual ISkinObj * Scale(int nScale);

protected:
    virtual void _Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState,BYTE byAlpha);
    virtual void OnColorize(COLORREF cr);
    BTNCOLORS   m_colors;
    BTNCOLORS   m_colorsBackup;

    int         m_nCornerRadius;
	float		m_fCornerPercent;				// 圆角 百分比 0.5 半圆
    
    SOUI_ATTRS_BEGIN()
        ATTR_COLOR(L"colorBorder", m_colors.m_crBorder[0], TRUE)               //正常 边框颜色
		ATTR_COLOR(L"colorBorderHover", m_colors.m_crBorder[1], TRUE)		 //浮动 边框颜色
		ATTR_COLOR(L"colorBorderPush", m_colors.m_crBorder[2], TRUE)		 //下压 边框颜色
		ATTR_COLOR(L"colorBorderDisable", m_colors.m_crBorder[3], TRUE)	 //禁用 边框颜色
        ATTR_COLOR(L"colorUp", m_colors.m_crUp[ST_NORMAL], TRUE)             //正常状态渐变起始颜色
        ATTR_COLOR(L"colorDown", m_colors.m_crDown[ST_NORMAL], TRUE)         //正常状态渐变终止颜色
        ATTR_COLOR(L"colorUpHover", m_colors.m_crUp[ST_HOVER], TRUE)         //浮动状态渐变起始颜色
        ATTR_COLOR(L"colorDownHover", m_colors.m_crDown[ST_HOVER], TRUE)     //浮动状态渐变终止颜色
        ATTR_COLOR(L"colorUpPush", m_colors.m_crUp[ST_PUSHDOWN], TRUE)       //下压状态渐变起始颜色
        ATTR_COLOR(L"colorDownPush", m_colors.m_crDown[ST_PUSHDOWN], TRUE)   //下压状态渐变终止颜色
        ATTR_COLOR(L"colorUpDisable", m_colors.m_crUp[ST_DISABLE], TRUE)     //禁用状态渐变起始颜色
        ATTR_COLOR(L"colorDownDisable", m_colors.m_crDown[ST_DISABLE], TRUE) //禁用状态渐变终止颜色
        ATTR_INT(L"cornerRadius",m_nCornerRadius,TRUE)              //圆角大小
		ATTR_FLOAT(L"cornerPercent", m_fCornerPercent, TRUE)				// 圆角 百分比 0.5 半圆 会覆盖 cornerRadius 
    SOUI_ATTRS_END()


};

//////////////////////////////////////////////////////////////////////////

class SOUI_EXP SSkinGradation  : public SSkinObjBase
{
    SOUI_CLASS_NAME(SSkinGradation, L"gradation")
public:
    SSkinGradation();
    
    void SetColorFrom(COLORREF crFrom)
    {
        m_crFrom=crFrom;
    }

    void SetColorTo(COLORREF crTo)
    {
        m_crTo=crTo;
    }

    void SetVertical(BOOL bVertical)
    {
        m_bVert=bVertical;
    }

protected:
    virtual void _Draw(IRenderTarget *pRT, LPCRECT prcDraw, DWORD dwState,BYTE byAlpha);
    virtual void OnColorize(COLORREF cr);
	virtual ISkinObj * Scale(int nScale);

    COLORREF m_crFrom;
    COLORREF m_crTo;
    BOOL m_bVert;

    COLORREF    m_crColorize;
    COLORREF    m_crFromBackup,m_crToBackup;

    SOUI_ATTRS_BEGIN()
        ATTR_COLOR(L"colorFrom", m_crFrom, TRUE)    //渐变起始颜色
        ATTR_COLOR(L"colorTo", m_crTo, TRUE)        //渐变终止颜色
        ATTR_INT(L"vertical", m_bVert, TRUE)        //渐变方向,0--水平(默认), 1--垂直
    SOUI_ATTRS_END()


};


//////////////////////////////////////////////////////////////////////////
//
enum SBSTATE{
    SBST_NORMAL=0,    //正常状态
    SBST_HOVER,        //hover状态
    SBST_PUSHDOWN,    //按下状态
    SBST_DISABLE,    //禁用状态
    SBST_INACTIVE,    //失活状态,主要针对两端的箭头
};

#define MAKESBSTATE(sbCode,nState1,bVertical) MAKELONG((sbCode),MAKEWORD((nState1),(bVertical)))
#define SB_CORNOR        10
#define SB_THUMBGRIPPER    11    //滚动条上的可拖动部分

#define THUMB_MINSIZE    18

class SOUI_EXP SSkinScrollbar : public SSkinImgList
{
    SOUI_CLASS_NAME(SSkinScrollbar, L"scrollbar")

public:

    SSkinScrollbar();

    //指示滚动条皮肤是否支持显示上下箭头
    virtual BOOL HasArrow(){return TRUE;}
    
    virtual int GetIdealSize(){
        if(!m_pImg) return 0;
        return m_pImg->Width()/9;
    }

protected:
    virtual void _Draw(IRenderTarget *pRT, LPCRECT prcDraw, DWORD dwState,BYTE byAlpha);
    //返回源指定部分在原位图上的位置。
    virtual CRect GetPartRect(int nSbCode, int nState,BOOL bVertical);
	virtual void _Scale(ISkinObj *skinObj, int nScale);

    int         m_nMargin;
    BOOL        m_bHasGripper;
    BOOL        m_bHasInactive;//有失活状态的箭头时，滚动条皮肤有必须有5行，否则可以是3行或者4行

    SOUI_ATTRS_BEGIN()
        ATTR_INT(L"margin",m_nMargin,FALSE)             //边缘不拉伸大小
        ATTR_INT(L"hasGripper",m_bHasGripper,FALSE)     //滑块上是否有帮手(gripper)
        ATTR_INT(L"hasInactive",m_bHasInactive,FALSE)   //是否有禁用态
    SOUI_ATTRS_END()
};


//////////////////////////////////////////////////////////////////////////
class SOUI_EXP SSkinColorRect : public SSkinObjBase
{
    SOUI_CLASS_NAME(SSkinColorRect,L"colorrect")
public:
    SSkinColorRect();
    virtual ~SSkinColorRect();

protected:
    virtual void _Draw(IRenderTarget *pRT, LPCRECT prcDraw, DWORD dwState,BYTE byAlpha);

	virtual BOOL IgnoreState(){return GetStates()<2;}

	virtual int GetStates();
	virtual ISkinObj * Scale(int nScale);

    SOUI_ATTRS_BEGIN()
        ATTR_COLOR(L"normal",m_crStates[0],FALSE)
        ATTR_COLOR(L"hover",m_crStates[1],FALSE)
        ATTR_COLOR(L"pushdown",m_crStates[2],FALSE)
        ATTR_COLOR(L"disable",m_crStates[3],FALSE)
		ATTR_COLOR(L"normalBorder", m_crBorders[0], FALSE)
		ATTR_COLOR(L"hoverBorder", m_crBorders[1], FALSE)
		ATTR_COLOR(L"pushdownBorder", m_crBorders[2], FALSE)
		ATTR_COLOR(L"disableBorder", m_crBorders[3], FALSE)
		ATTR_INT(L"borderWidth",m_nBorderWidth,FALSE)
        ATTR_INT(L"cornerRadius",m_nRadius,FALSE)
		ATTR_FLOAT(L"cornerPercent", m_fCornerPercent, FALSE)				// 圆角 百分比 0.5 半圆 会覆盖 cornerRadius 
    SOUI_ATTRS_END()


    int      m_nRadius;
	float	m_fCornerPercent;				// 圆角 百分比 0.5 半圆
    COLORREF m_crStates[4];
	COLORREF m_crBorders[4];
	int		m_nBorderWidth;
};

//////////////////////////////////////////////////////////////////////////

class SOUI_EXP SSkinShape : public SSkinObjBase
{
	SOUI_CLASS_NAME(SSkinShape,L"shape")
	enum Shape {rectangle,oval,ring};

	class SGradient : public SObject , public TObjRefImpl<IObjRef>
	{
		SOUI_CLASS_NAME(SGradient,L"gradient")
	public:
		SGradient():m_Type(linear), m_angle(0.0f),m_centerX(0.5f),m_centerY(0.5f){

		}

		void Draw(IRenderTarget *pRT, LPCRECT rcDraw, BYTE byAlpha,int nScale);

		SOUI_ATTRS_BEGIN()
			ATTR_ENUM_BEGIN(L"type",GradientType,TRUE)
				ATTR_ENUM_VALUE(L"linear",linear)
				ATTR_ENUM_VALUE(L"radial",radial)
				ATTR_ENUM_VALUE(L"sweep",sweep)
			ATTR_ENUM_END(m_Type)
			ATTR_FLOAT(L"angle",m_angle,TRUE)
			ATTR_FLOAT(L"centerX",m_centerX,TRUE)
			ATTR_FLOAT(L"centerY",m_centerY,TRUE)
			ATTR_COLOR(L"startColor",m_crStart,TRUE)
			ATTR_COLOR(L"centerColor",m_crCenter,TRUE)
			ATTR_COLOR(L"endColor",m_crEnd,TRUE)
			ATTR_LAYOUTSIZE(L"radius",m_radius,TRUE)
		SOUI_ATTRS_END()
	protected:
		GradientType m_Type;
		float m_angle; //渐变角度，必须为45的倍数，0为从左到右，90为从上到下 
		float m_centerX; //渐变中心X的相当位置，范围为0～1  
		float m_centerY;//渐变中心Y的相当位置，范围为0～1  
		COLORREF m_crStart;//渐变开始点的颜色  
		COLORREF m_crCenter; //渐变中间点的颜色，在开始与结束点之间
		COLORREF m_crEnd;//渐变结束点的颜色  
		SLayoutSize m_radius;//渐变的半径，只有当渐变类型为radial时才能使用  
	};

	class SStroke : public SObject, public TObjRefImpl<IObjRef>
	{
		SOUI_CLASS_NAME(SStroke,L"stroke")
	public:

		SStroke():m_color(CR_INVALID),m_style(PS_SOLID){}

		SOUI_ATTRS_BEGIN()
			ATTR_LAYOUTSIZE(L"width",m_width,TRUE)
			ATTR_COLOR(L"color",m_color,TRUE)
			ATTR_ENUM_BEGIN(L"style",int,TRUE)
				ATTR_ENUM_VALUE(L"solid",PS_SOLID)
				ATTR_ENUM_VALUE(L"dash",PS_DASH)
				ATTR_ENUM_VALUE(L"dashDot",PS_DASHDOT)
				ATTR_ENUM_VALUE(L"dashDotDot",PS_DASHDOTDOT)
			ATTR_ENUM_END(m_style)
		SOUI_ATTRS_END()
	public:
		SLayoutSize m_width;  //描边的宽度  
		COLORREF	m_color;  //描边的颜色
		int			m_style; //线型
	};

	class SCornerSize : public SObject, public TObjRefImpl<IObjRef>
	{
		SOUI_CLASS_NAME(SCornerSize,L"corners")

	public:

		HRESULT OnAttrRadius(const SStringW strValue,BOOL bLoading);

		SOUI_ATTRS_BEGIN()
			ATTR_CUSTOM(L"radius",OnAttrRadius)
			ATTR_LAYOUTSIZE(L"radiusX",m_radiusX,TRUE)
			ATTR_LAYOUTSIZE(L"radiusY",m_radiusY,TRUE)
		SOUI_ATTRS_END()


		POINT GetConner(int nScale)
		{
			POINT pt={m_radiusX.toPixelSize(nScale),m_radiusY.toPixelSize(nScale)};
			return pt;
		}

		SLayoutSize m_radiusX,m_radiusY;
	};

	class SShapeSize : public SObject, public TObjRefImpl<IObjRef>
	{
		SOUI_CLASS_NAME(SShapeSize,L"size")

	public:

		SOUI_ATTRS_BEGIN()
			ATTR_LAYOUTSIZE(L"width",m_width,TRUE)
			ATTR_LAYOUTSIZE(L"height",m_height,TRUE)
		SOUI_ATTRS_END()

		SLayoutSize m_width,m_height;
	};

	class SShapeRing : public SObject, public TObjRefImpl<IObjRef>
	{
		SOUI_CLASS_NAME(SShapeRing, L"ring")

	public:
		SShapeRing() :m_startAngle(0.0f), m_sweepAngle(360.0f) {}

		SOUI_ATTRS_BEGIN()
			ATTR_FLOAT(L"startAngle", m_startAngle, TRUE)
			ATTR_FLOAT(L"sweepAngle", m_sweepAngle, TRUE)
		SOUI_ATTRS_END()


		float m_startAngle;
		float m_sweepAngle;
	};

public:
	SSkinShape();


	virtual SIZE GetSkinSize();

	virtual BOOL IgnoreState(){return TRUE;}

	virtual int GetStates(){return 1;}

	SOUI_ATTRS_BEGIN()
		ATTR_ENUM_BEGIN(L"shape",Shape,TRUE)
			ATTR_ENUM_VALUE(L"rectangle",rectangle)
			ATTR_ENUM_VALUE(L"oval",oval)
			ATTR_ENUM_VALUE(L"ring",ring)
		ATTR_ENUM_END(m_shape)
	SOUI_ATTRS_END()
protected:
	void OnInitFinished(pugi::xml_node xmlNode);

	virtual void _Scale(ISkinObj *pObj, int nScale);

	virtual void _Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState,BYTE byAlpha);

	Shape  m_shape;

	COLORREF	m_crSolid;
	CAutoRefPtr<SShapeSize>		m_shapeSize;
	CAutoRefPtr<SCornerSize>	m_cornerSize;
	CAutoRefPtr<SStroke>	  	m_stroke;
	CAutoRefPtr<SGradient>      m_gradient;
	CAutoRefPtr<SShapeRing>		m_ringParam;
};


class SOUI_EXP SSKinGroup : public SSkinObjBase
{
SOUI_CLASS_NAME(SSKinGroup,L"group")
public:

	virtual int GetStates();
	virtual BOOL IgnoreState(){return FALSE;}
	virtual SIZE GetSkinSize();

	SOUI_ATTRS_BEGIN()
		ATTR_SKIN(L"normal",m_skins[0],FALSE)
		ATTR_SKIN(L"hover",m_skins[1],FALSE)
		ATTR_SKIN(L"pushDown",m_skins[2],FALSE)
		ATTR_SKIN(L"disable",m_skins[3],FALSE)
	SOUI_ATTRS_END()

protected:
	virtual void _Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState,BYTE byAlpha);

	CAutoRefPtr<ISkinObj> m_skins[4];
};
}//namespace SOUI
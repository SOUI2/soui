#pragma once

#include "interface/slayout-i.h"
#include "SGridLayoutParamStruct.h"

namespace SOUI
{

	// Int = %d StringA
	#define ATTR_GRIDGRAVITY(attribname, varname, allredraw)       \
        if (0 == strAttribName.CompareNoCase(attribname))          \
        {                                                          \
		    varname=SGridLayoutParam::parseGridGravity(strValue);  \
		    hRet = allredraw ? S_OK : S_FALSE;                     \
        }                                                          \
        else                                                       \


	class SGridLayoutParam : public SObjectImpl<TObjRefImpl<ILayoutParam>>
		, protected SGridLayoutParamStruct
	{
		SOUI_CLASS_NAME(SGridLayoutParam,L"GridLayoutParam")

		friend class SGridLayout;
	public:

		static GridGravity parseGridGravity(const SStringW & strValue);

		SGridLayoutParam();

		virtual bool IsMatchParent(ORIENTATION orientation) const;
		virtual bool IsWrapContent(ORIENTATION orientation) const;

		virtual bool IsSpecifiedSize(ORIENTATION orientation) const;

		virtual SLayoutSize GetSpecifiedSize(ORIENTATION orientation) const;

		virtual void Clear();

		virtual void SetMatchParent(ORIENTATION orientation);

		virtual void SetWrapContent(ORIENTATION orientation);

		virtual void SetSpecifiedSize(ORIENTATION orientation, const SLayoutSize& layoutSize);

		virtual void * GetRawData();

		virtual ILayoutParam * Clone() const;

	public:
		SOUI_ATTRS_BEGIN()
			ATTR_INT(L"rowSpan",nRowSpan,TRUE)
			ATTR_INT(L"columnSpan",nColSpan,TRUE)
			ATTR_CUSTOM(L"width",OnAttrWidth)
			ATTR_CUSTOM(L"height",OnAttrHeight)
			ATTR_CUSTOM(L"size",OnAttrSize)
			ATTR_GRIDGRAVITY(L"layout_xGravity",layoutGravityX,TRUE)
			ATTR_GRIDGRAVITY(L"layout_yGravity",layoutGravityY,TRUE)
			ATTR_FLOAT(L"columnWeight",fColWeight,TRUE)
			ATTR_FLOAT(L"rowWeight",fRowWeight,TRUE)
		SOUI_ATTRS_BREAK()

	protected:
		HRESULT OnAttrSize(const SStringW & strValue,BOOL bLoading);
		HRESULT OnAttrWidth(const SStringW & strValue,BOOL bLoading);
		HRESULT OnAttrHeight(const SStringW & strValue,BOOL bLoading);

	};

	class SOUI_EXP SGridLayout: public SObjectImpl<TObjRefImpl<ILayout>>
	{
		SOUI_CLASS_NAME_EX(SGridLayout,L"gridLayout",Layout)
	public:
		SGridLayout(void);
		~SGridLayout(void);

		virtual bool IsParamAcceptable(ILayoutParam *pLayoutParam) const;

		virtual void LayoutChildren(SWindow * pParent);

		virtual ILayoutParam * CreateLayoutParam() const;

		virtual CSize MeasureChildren(SWindow * pParent,int nWidth,int nHeight) const;


		SOUI_ATTRS_BEGIN()
			ATTR_INT(L"columnCount",m_nCols,TRUE)
			ATTR_INT(L"rowCount",m_nRows,TRUE)
			ATTR_LAYOUTSIZE(L"xInterval",m_xInterval,TRUE)
			ATTR_LAYOUTSIZE(L"yInterval",m_yInterval,TRUE)
			ATTR_LAYOUTSIZE(L"interval",m_xInterval=m_yInterval,TRUE)
			ATTR_GRIDGRAVITY(L"xGravity",m_GravityX,TRUE)
			ATTR_GRIDGRAVITY(L"yGravity",m_GravityY,TRUE)
			ATTR_GRIDGRAVITY(L"gravity",m_GravityX=m_GravityY,TRUE)
		SOUI_ATTRS_BREAK()
	protected:
		int CalcCells(SWindow *pParent) const;

		int m_nCols;				/**<行数 */
		int m_nRows;				/**<列数 */
		SLayoutSize m_xInterval;	/**<水平间距 */
		SLayoutSize m_yInterval;	/**<垂直间距 */

		GridGravity m_GravityX;		/**<水平排列方式 */
		GridGravity m_GravityY;		/**<垂直排列方式 */

	};

}

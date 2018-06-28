#pragma once

#include "SLayoutSize.h"

namespace SOUI
{

	enum GridGravity{
		gUndef=-1,	/**< 未定义 */
		gLeft=0,	/**< 左对齐 */
		gTop=0,		/**< 上对齐 */
		gCenter=1,	/**< 居中对齐 */
		gRight=2,	/**< 右对齐 */
		gBottom=2,	/**< 底对齐 */
		gFill=3,	/**< 扩展对齐 */
	};

	/*
	* 网格布局单元格参数
	* remark: 如果同一行中多个单无格指定了rowWeight,只有最大的那一个有效，相应的同一列中也只有最大的colWeight有效
	*/
	struct SGridLayoutParamStruct
	{
		int nColSpan;		/**< 网格占列数 */
		int nRowSpan;		/**< 网格占行数 */
		GridGravity layoutGravityX;/**< 网格水平对齐方式 */
		GridGravity layoutGravityY;/**< 网格垂直对齐方式 */
		SLayoutSize  width;	/**< 网格宽度 */
		SLayoutSize  height;/**< 网格高度 */
		float fColWeight;	/**< 网格宽度的加权 */
		float fRowWeight;	/**< 网格高度的加权 */
	};

}
#pragma once

#include "SLayoutSize.h"

#define POSFLAG_REFCENTER      '|'        //参考父窗口中心
#define POSFLAG_REFPREV_NEAR   '['        //参考前一个兄弟窗口与自己近的边
#define POSFLAG_REFNEXT_NEAR   ']'        //参考下一个兄弟窗口与自己近的边
#define POSFLAG_REFPREV_FAR    '{'        //参考前一个兄弟窗口与自己远的边
#define POSFLAG_REFNEXT_FAR    '}'        //参考下一个兄弟窗口与自己远的边
#define POSFLAG_PERCENT        '%'        //采用在父窗口的百分比定义坐标
#define POSFLAG_SIZE           '@'        //在pos属性中定义窗口的size，只在在定义x2,y2时有效

namespace SOUI{

	//坐标类型
	enum PIT{

		PIT_NULL=0,        //无效定义
		PIT_NORMAL,        //锚点坐标
		PIT_CENTER,        //参考父窗口中心点,以"|"开始
		PIT_PERCENT,       //指定在父窗口坐标的中的百分比,以"%"开始
		PIT_PREV_NEAR,     //参考前一个兄弟窗口与自己近的边,以"["开始
		PIT_NEXT_NEAR,     //参考下一个兄弟窗口与自己近的边,以"]"开始
		PIT_PREV_FAR,      //参考前一个兄弟窗口与自己远的边,以"{"开始
		PIT_NEXT_FAR,      //参考下一个兄弟窗口与自己远的边,以"}"开始
		PIT_SIZE,          //指定窗口的宽或者高,以"@"开始
		PIT_SIB_LEFT=10,       //兄弟结点的left,用于X
		PIT_SIB_TOP=10,        //兄弟结点的top，与left相同，用于Y
		PIT_SIB_RIGHT=11,      //兄弟结点的right,用于X 
		PIT_SIB_BOTTOM=11,      //兄弟结点的bottom,与right相同,用于Y 
	};

	struct POS_INFO
	{
		PIT     pit;        /**<坐标类型 */
		int     nRefID;     /**<根据ID引用兄弟窗口时使用的ID,-1代表不参考特定ID的兄弟,使用ID引用的格式为"sib.left@200:10"类似的格式 */
		char    cMinus;     /**<定义的值包含"-", 由于-0不能直接做nPos表示，需要一个单独的标志位 */
		SLayoutSize   nPos; /**<坐标值*/
	};

	struct SouiLayoutParamStruct
	{
		int  nCount;                /**< 定义的坐标个数 */
		POS_INFO posLeft,posRight;       /**< 由pos属性定义的值, nCount >0 时有效*/
		POS_INFO posTop,posBottom;       /**< 由pos属性定义的值, nCount >0 时有效*/

		float fOffsetX,fOffsetY;    /**< 窗口坐标偏移量, x += fOffsetX * width, y += fOffsetY * height  */

		SLayoutSize  width;        /**<使用width属性定义的宽 nCount==0 时有效*/
		SLayoutSize  height;       /**<使用height属性定义的高 nCount==0 时有效*/
	};
}

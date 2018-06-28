#pragma once
namespace IMAGE3D
{

//////////////////////////////////////////////////////////////////////////
// 3D 模拟部分

// 不包含w分量2D向量和点
typedef struct tagVECTOR2D
{
	union
	{
		float M[2];

		struct
		{
			float x , y;
		};
	};
}VECTOR2D, POINT2D, *VECTOR2D_PTR, *POINT2D_PTR;

// 不包含w分量的3D向量和点
typedef struct tagVECTOR3D
{
	union
	{
		float M[3];

		struct  
		{
			float x, y, z;
		};
	};
} VECTOR3D, POINT3D, *VECTOR3D_PTR, *POINT3D_PTR;

// 包含w分量的4D齐次向量和点
typedef struct tagVECTOR4D
{
	union
	{
		float M[4];

		struct
		{
			float x, y, z, w;
		};
	};
} VECTOR4D, POINT4D, *VECTOR4D_PTR, *POINT4D_PTR;

// 2D 顶点
typedef struct tagVERTEX2DI
{
	int x, y;
}VERTEX2DI, *VERTEX2DI_PTR;

typedef struct tagVERTEX2DF
{
	float x, y;
}VERTEX2DF, *VERTEX2DF_PTR;

// 2D参数化直线
typedef struct tagPARAMLINE2D
{
	POINT2D  p0;  // 参数化直线的起点
	POINT2D  p1;  // 参数化直线的终点
	VECTOR2D v;   // 线段的方向向量  |v| = |p0 -> p1|
} PARAMLINE2D, *PARAMLINE2D_PTR;

// 3D参数化直线
typedef struct tagPARAMLINE3D
{
	POINT3D  p0;  // 参数化直线的起点
	POINT3D  p1;  // 参数化直线的终点
	VECTOR3D v;   // 线段的方向向量  |v| = |p0 -> p1|
} PARAMLINE3D, *PARAMLINE3D_PTR;

// 3D 平面
typedef struct tagPLANE3D
{
	POINT3D  p0;  // 平面上的点
	VECTOR3D n;   // 平面上的法线（不必是单位向量）
}PLANE3D, *PLANE3D_PTR;

// 4x4矩阵
typedef struct tagMATRIX_4_4
{
	union
	{
		float M[4][4];

		struct
		{
			float M00, M01, M02, M03;
			float M10, M11, M12, M13;
			float M20, M21, M22, M23;
			float M30, M31, M32, M33;
		};
	};
}MATRIX_4_4, *MATRIX_4_4_PTR;

// 4x3矩阵
typedef struct tagMATRIX_4_3
{
	union
	{
		float M[4][3];

		struct
		{
			float M00, M01, M02;
			float M10, M11, M12;
			float M20, M21, M22;
			float M30, M31, M32;
		};
	};
}MATRIX_4_3, *MATRIX_4_3_PTR;

// 1x4矩阵
typedef struct tagMATRIX_1_4
{
	union
	{
		float M[4];

		struct
		{
			float M00, M01, M02, M03;
		};
	};
}MATRIX_1_4, *MATRIX_1_4_PTR;

// 3x3矩阵
typedef struct tagMATRIX_3_3
{
	union
	{
		float M[3][3];

		struct
		{
			float M00, M01, M02;
			float M10, M11, M12;
			float M20, M21, M22;
		};
	};
}MATRIX_3_3, *MATRIX_3_3_PTR;

// 1x3矩阵
typedef struct tagMATRIX_1_3
{
	union
	{
		float M[3];

		struct
		{
			float M00, M01, M02;
		};
	};
}MATRIX_1_3, *MATRIX_1_3_PTR;

// 3x2矩阵
typedef struct tagMATRIX_3_2
{
	union
	{
		float M[3][2];

		struct
		{
			float M00, M01;
			float M10, M11;
			float M20, M21;
		};
	};
}MATRIX_3_2, *MATRIX_3_2_PTR;

// 2x2矩阵
typedef struct tagMATRIX_2_2
{
	union
	{
		float M[2][2];

		struct
		{
			float M00, M01;
			float M10, M11;
		};
	};
}MATRIX_2_2, *MATRIX_2_2_PTR;


// 1x2矩阵
typedef struct tagMATRIX_1_2
{
	union
	{
		float M[2];

		struct
		{
			float M00, M01;
		};
	};
}MATRIX_1_2, *MATRIX_1_2_PTR;

// 4D 四元数
// q = q0 + q1*i + q2*j + q3*k  或
// q = q0 + <q1, q2, q3>  或
// q = q0 + qv
typedef struct tagQUAT
{
	union
	{
		float M[4];

		struct 
		{
			float    q0; // 实部
			VECTOR3D qv; // 虚部
		};

		struct
		{
			float w, x, y, z;
		};
	};
}QUAT, *QUAT_PTR;

// 2D极坐标
typedef struct tagPOLAR2D
{
	float    r;  // 半径
	float    theta; // 角度
} POLAR2D, *POLAR2D_PTR;

// 3D柱面坐标
typedef struct tagCYLINDRICAL3D
{
	float    r;  // 半径
	float    theta; // 与Z轴的夹角
	float    z;  // z坐标
}CYLINDRICAL3D, *CYLINDRICAL3D_PTR;

// 3D球面坐标
typedef struct tagSPHERICAL3D
{
	float    p;  // 到原点的距离
	float    theta;  // 线段o->p和正z轴之间的夹角
	float    phi; // 线段o->p在x-y平面上的投影与正x轴之间的夹角
}SPHERICAL3D, *SPHERICAL3D_PTR;


// 非常小的数
#define EPSILON_E4  (float)(1E-4)
#define EPSILON_E5  (float)(1E-5)
#define EPSILON_E6  (float)(1E-6)

// 4x4单位矩阵
const MATRIX_4_4 IMAT_4_4 = 
{
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

// 4x3单位矩阵(从数学上4x3没有单位矩阵)
const MATRIX_4_3 IMAT_4_3 = 
{
	1, 0, 0,
	0, 1, 0,
	0, 0, 1,
	0, 0, 0,
};

// 3x3单位矩阵
const MATRIX_3_3 IMAT_3_3 = 
{
	1, 0, 0,
	0, 1, 0,
	0, 0, 1,
};

// 2x2单位矩阵
const MATRIX_2_2 IMAT_2_2 = 
{
	1, 0,
	0, 1,
};

// 通用宏

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (b) : (a))

#define SWAP(a, b, t) { t = a; a = b; b = t; }

#define DEG_2_RAD(ang) ((ang)*PI/180.0)
#define RAD_2_DEG(rads) (rads)*180.0/PI)

#define RAND_RANGE(x, y) ((x) + (rand()%((y)-(x)+1)))

// 向量归零宏
inline  void VECTOR2D_ZERO(VECTOR2D_PTR v)
{
	(v)->x = (v)->y = 0.0;
}
inline  void VECTOR3D_ZERO(VECTOR3D_PTR v)
{
	(v)->x = (v)->y = (v)->z = 0.0;
}
inline  void VECTOR4D_ZERO(VECTOR4D_PTR v)
{
	(v)->x = (v)->y = (v)->z = 0.0;
	(v)->w = 1.0;
}

// 使用分量初始化向量的宏
inline  void VECTOR3D_INITXY(VECTOR2D_PTR v, float x, float y)
{
	(v)->x = x; 
	(v)->y = y;
}
inline  void VECTOR3D_INITXY(VECTOR3D_PTR v, float x, float y, float z)
{
	(v)->x = x; 
	(v)->y = y;
	(v)->z = z;
}
inline  void VECTOR3D_INITXY(VECTOR4D_PTR v, float x, float y, float z)
{
	(v)->x = x; 
	(v)->y = y;
	(v)->z = z;
	(v)->w = 1.0;
}

// 使用另一个向量来初始化向量的宏
inline  void VECTOR2D_INIT(VECTOR2D_PTR vDst, VECTOR2D_PTR vSrc)
{
	(vDst)->x = (vSrc)->x;
	(vDst)->y = (vSrc)->y;
}
inline  void VECTOR3D_INIT(VECTOR3D_PTR vDst, VECTOR3D_PTR vSrc)
{
	(vDst)->x = (vSrc)->x;
	(vDst)->y = (vSrc)->y;
	(vDst)->z = (vSrc)->z;
}
inline  void VECTOR4D_INIT(VECTOR4D_PTR vDst, VECTOR4D_PTR vSrc)
{
	(vDst)->x = (vSrc)->x;
	(vDst)->y = (vSrc)->y;
	(vDst)->z = (vSrc)->z;
	(vDst)->w = (vSrc)->w;
}

// 复制向量的宏
inline  void VECTOR2D_COPY(VECTOR2D_PTR vDst, VECTOR2D_PTR vSrc)
{
	(vDst)->x = (vSrc)->x;
	(vDst)->y = (vSrc)->y;
}
inline  void VECTOR3D_COPY(VECTOR3D_PTR vDst, VECTOR3D_PTR vSrc)
{
	(vDst)->x = (vSrc)->x;
	(vDst)->y = (vSrc)->y;
	(vDst)->z = (vSrc)->z;
}
inline  void VECTOR4D_COPY(VECTOR4D_PTR vDst, VECTOR4D_PTR vSrc)
{
	(vDst)->x = (vSrc)->x;
	(vDst)->y = (vSrc)->y;
	(vDst)->z = (vSrc)->z;
	(vDst)->w = (vSrc)->w;
}

// 初始化点的宏
inline  void POINT2D_INIT(POINT2D_PTR vDst, POINT2D_PTR vSrc)
{
	(vDst)->x = (vSrc)->x;
	(vDst)->y = (vSrc)->y;
}
inline  void POINT3D_INIT(POINT3D_PTR vDst, POINT3D_PTR vSrc)
{
	(vDst)->x = (vSrc)->x;
	(vDst)->y = (vSrc)->y;
	(vDst)->z = (vSrc)->z;
}
inline  void POINT4D_INIT(POINT4D_PTR vDst, POINT4D_PTR vSrc)
{
	(vDst)->x = (vSrc)->x;
	(vDst)->y = (vSrc)->y;
	(vDst)->z = (vSrc)->z;
	(vDst)->w = (vSrc)->w;
}

// 复制点的宏
inline  void POINT2D_COPY(POINT2D_PTR vDst, POINT2D_PTR vSrc)
{
	(vDst)->x = (vSrc)->x;
	(vDst)->y = (vSrc)->y;
}
inline  void POINT3D_COPY(POINT3D_PTR vDst, POINT3D_PTR vSrc)
{
	(vDst)->x = (vSrc)->x;
	(vDst)->y = (vSrc)->y;
	(vDst)->z = (vSrc)->z;
}
inline  void POINT4D_COPY(POINT4D_PTR vDst, POINT4D_PTR vSrc)
{
	(vDst)->x = (vSrc)->x;
	(vDst)->y = (vSrc)->y;
	(vDst)->z = (vSrc)->z;
	(vDst)->w = (vSrc)->w;
}

inline void VECTOR2D_INITXY(VECTOR2D_PTR v, float x, float y) 
{
	(v)->x = (x);
	(v)->y = (y);
}

// vector macros, note the 4D vector sets w=1
inline void VECTOR3D_INITXYZ(VECTOR3D_PTR v, float x, float y, float z) 
{
	(v)->x = (x); 
	(v)->y = (y); 
	(v)->z = (z);
}

inline void VECTOR4D_INITXYZ(VECTOR4D_PTR v, float x,float y,float z) 
{
	(v)->x = (x); 
	(v)->y = (y); 
	(v)->z = (z); 
	(v)->w = 1.0;
}

// used to convert from 4D homogenous to 4D non-homogenous
inline void VECTOR4D_DIV_BY_W(VECTOR4D_PTR v) 
{
	(v)->x/=(v)->w; 
	(v)->y/=(v)->w; 
	(v)->z/=(v)->w; 
}

inline void VECTOR4D_DIV_BY_W_VECTOR3D(VECTOR4D_PTR v4, VECTOR3D_PTR v3) 
{
	(v3)->x = (v4)->x/(v4)->w; 
	(v3)->y = (v4)->y/(v4)->w; 
	(v3)->z = (v4)->z/(v4)->w;  
}

// 矩阵宏

// 清空矩阵
#define MAT_ZERO_2_2(m)  {memset((void*)(m}, 0, sizeof(MATRIX_2_2))
#define MAT_ZERO_3_3(m)  {memset((void*)(m}, 0, sizeof(MATRIX_3_3))
#define MAT_ZERO_4_4(m)  {memset((void*)(m}, 0, sizeof(MATRIX_4_4))
#define MAT_ZERO_4_3(m)  {memset((void*)(m}, 0, sizeof(MATRIX_4_3))

// 设置单位矩阵的宏
#define MAT_IDENTITY_2_2(m) {memcpy((void*)(m), (void*)&IMAT_2_2, sizeof(MATRIX_2_2));}
#define MAT_IDENTITY_3_3(m) {memcpy((void*)(m), (void*)&IMAT_3_3, sizeof(MATRIX_3_3));}
#define MAT_IDENTITY_4_4(m) {memcpy((void*)(m), (void*)&IMAT_4_4, sizeof(MATRIX_4_4));}
#define MAT_IDENTITY_4_3(m) {memcpy((void*)(m), (void*)&IMAT_4_3, sizeof(MATRIX_4_3));}

// 复制矩阵的宏
#define MAT_COPY_2_2(src_mat, dest_mat) {memcpy((void*)(dest_mat), (void*)(src_mat), sizeof(MATRIX_2_2));}
#define MAT_COPY_3_3(src_mat, dest_mat) {memcpy((void*)(dest_mat), (void*)(src_mat), sizeof(MATRIX_3_3));}
#define MAT_COPY_4_4(src_mat, dest_mat) {memcpy((void*)(dest_mat), (void*)(src_mat), sizeof(MATRIX_4_4));}
#define MAT_COPY_4_3(src_mat, dest_mat) {memcpy((void*)(dest_mat), (void*)(src_mat), sizeof(MATRIX_4_3));}

// 对矩阵进行转置的宏
inline  void MAT_TRANSPOSE_3_3(MATRIX_3_3_PTR m)
{
	MATRIX_3_3 mt;
	mt.M00 = m->M00;  mt.M01 = m->M10;  mt.M02 = m->M20;
	mt.M10 = m->M01;  mt.M11 = m->M11;  mt.M12 = m->M21;
	mt.M20 = m->M02;  mt.M21 = m->M12;  mt.M22 = m->M22;
	memcpy((void*)m, (void*)&mt, sizeof(MATRIX_3_3));
}
inline  void MAT_TRANSPOSE_4_4(MATRIX_4_4_PTR m)
{
	MATRIX_4_4 mt;
	mt.M00 = m->M00;  mt.M01 = m->M10;  mt.M02 = m->M20;  mt.M03 = m->M30;
	mt.M10 = m->M01;  mt.M11 = m->M11;  mt.M12 = m->M21;  mt.M13 = m->M31;
	mt.M20 = m->M02;  mt.M21 = m->M12;  mt.M22 = m->M22;  mt.M23 = m->M32;
	mt.M30 = m->M03;  mt.M31 = m->M13;  mt.M32 = m->M23;  mt.M33 = m->M33;
	memcpy((void*)m, (void*)&mt, sizeof(MATRIX_4_4));
}

inline  void MAT_TRANSPOSE_3_3(MATRIX_3_3_PTR m, MATRIX_3_3_PTR mt)
{
	mt->M00 = m->M00;  mt->M01 = m->M10;  mt->M02 = m->M20;
	mt->M10 = m->M01;  mt->M11 = m->M11;  mt->M12 = m->M21;
	mt->M20 = m->M02;  mt->M21 = m->M12;  mt->M22 = m->M22;
}
inline  void MAT_TRANSPOSE_4_4(MATRIX_4_4_PTR m, MATRIX_4_4_PTR mt)
{
	mt->M00 = m->M00;  mt->M01 = m->M10;  mt->M02 = m->M20;  mt->M03 = m->M30;
	mt->M10 = m->M01;  mt->M11 = m->M11;  mt->M12 = m->M21;  mt->M13 = m->M31;
	mt->M20 = m->M02;  mt->M21 = m->M12;  mt->M22 = m->M22;  mt->M23 = m->M32;
	mt->M30 = m->M03;  mt->M31 = m->M13;  mt->M32 = m->M23;  mt->M33 = m->M33;
}

// 矩阵和向量列互换宏
inline  void  MAT_COLUMN_SWAP_2_2(MATRIX_2_2_PTR m, int c, MATRIX_1_2_PTR v)
{
	m->M[0][c] = v->M[0];
	m->M[1][c] = v->M[1];
}
inline  void  MAT_COLUMN_SWAP_3_3(MATRIX_3_3_PTR m, int c, MATRIX_1_3_PTR v)
{
	m->M[0][c] = v->M[0];
	m->M[1][c] = v->M[1];
	m->M[2][c] = v->M[2];
}
inline  void  MAT_COLUMN_SWAP_4_4(MATRIX_4_4_PTR m, int c, MATRIX_1_4_PTR v)
{
	m->M[0][c] = v->M[0];
	m->M[1][c] = v->M[1];
	m->M[2][c] = v->M[2];
	m->M[3][c] = v->M[3];
}
inline  void  MAT_COLUMN_SWAP_4_3(MATRIX_4_3_PTR m, int c, MATRIX_1_4_PTR v)
{
	m->M[0][c] = v->M[0];
	m->M[1][c] = v->M[1];
	m->M[2][c] = v->M[2];
	m->M[3][c] = v->M[3];
}

// 四元数宏
inline void QUAT_ZERO(QUAT_PTR q) 
{
	(q)->x = (q)->y = (q)->z = (q)->w = 0.0;
}

inline void QUAT_INITWXYZ(QUAT_PTR q, float w, float x,float y,float z) 
{ 
	(q)->w = (w); (q)->x = (x); (q)->y = (y); (q)->z = (z);
}

inline void QUAT_INIT_VECTOR3D(QUAT_PTR q, VECTOR3D_PTR v) 
{ 
	(q)->w = 0; (q)->x = (v->x); (q)->y = (v->y); (q)->z = (v->z); 
}

inline void QUAT_INIT(QUAT_PTR qdst, QUAT_PTR qsrc) 
{
	(qdst)->w = (qsrc)->w;  (qdst)->x = (qsrc)->x;  
	(qdst)->y = (qsrc)->y;  (qdst)->z = (qsrc)->z;  
}

inline void QUAT_COPY(QUAT_PTR qdst, QUAT_PTR qsrc) 
{
	(qdst)->x = (qsrc)->x; (qdst)->y = (qsrc)->y;  
	(qdst)->z = (qsrc)->z; (qdst)->w = (qsrc)->w;  
}

 // 相机
typedef struct tagCAMERA
{
	POINT4D WorldPos;    // 相机在世界的坐标
	double  AngelX, AngelY, AngleZ; // 相机的朝向，使用绕X,Y,Z轴分别转多少度来表示
} CAMERA, *CAMERA_PTR;


// 定点数宏
// pi defines
#define PI         ((float)3.141592654f)
#define PI2        ((float)6.283185307f)
#define PI_DIV_2   ((float)1.570796327f)
#define PI_DIV_4   ((float)0.785398163f) 
#define PI_INV     ((float)0.318309886f) 

// fixed point mathematics constants
#define FIXP16_SHIFT     16
#define FIXP16_MAG       65536
#define FIXP16_DP_MASK   0x0000ffff
#define FIXP16_WP_MASK   0xffff0000
#define FIXP16_ROUND_UP  0x00008000

// 从16.16格式的定点数中提取整数部分和小数部分
#define FIXP16_WP(fp) ((fp) >> FIXP16_SHIFT)
#define FIXP16_DP(fp) ((fp) && FIXP16_DP_MASK)

// 将整数和浮点数转换为16.16格式的数
#define INT_TO_FIXP16(i) ((i) <<  FIXP16_SHIFT)
#define FLOAT_TO_FIXP16(fv) (((float)(fv) * (float)FIXP16_MAG+0.5f))

// 将定点数转换为浮点数
#define FIXP16_TO_FLOAT(fp) ( ((float)fp)/FIXP16_MAG)


// storage for our lookup tables
extern float cos_look[361]; // 1 extra so we can store 0-360 inclusive
extern float sin_look[361]; // 1 extra so we can store 0-360 inclusive

void  Build_Sin_Cos_Tables(void);

// trig functions
float Fast_Sin(float theta);
float Fast_Cos(float theta);

// math functions
int   Fast_Distance_2D(int x, int y);
float Fast_Distance_3D(float x, float y, float z);

}
#include "3dmatrix.h"
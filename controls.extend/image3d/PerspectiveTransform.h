#ifndef PERSPECTIVETRANSFORM_H_6D536C9D_C5B0_4f25_8CE9_B6EC29ECC0EF
#define PERSPECTIVETRANSFORM_H_6D536C9D_C5B0_4f25_8CE9_B6EC29ECC0EF

#include "3dlib.h"
#include <math.h>

// 通过移植agg开源代码，计算任意四边形的拉伸算法 


// http://www.docin.com/p-114884109.html
// http://www.codeproject.com/Articles/13201/Anti-Aliased-Image-Transformation-Aaform
// http://blog.csdn.net/wshjldaxiong/article/details/7446393
// http://blog.csdn.net/wshjldaxiong/article/details/7531902
// http://bbs.csdn.net/topics/340208048

// http://blog.csdn.net/liujb861213/article/details/7520658
// 图片任意角度旋转 梯形变换 任意四边形变换 .
// http://blog.csdn.net/hitwengqi/article/details/6888783
// 第六章 - 图像变换 - 图像拉伸、收缩、扭曲、旋转[1] - 仿射变换(cvWarpAffine) .
// http://msdn.microsoft.com/zh-cn/library/dd470131(VS.95).aspx

//透视变换 透视转换

// ftp://ftp.gimp.org/pub/gimp/v2.8/
// http://www.gimp.org/downloads/
// 'gimp_drawable_transform_tiles_affine' 

// 有源码的能运行的仿PS http://www.antigrain.com/download/index.html 

namespace IMAGE3D
{
	// 
	// 01.u   =   (Ax + By + C) / (Gx + Hy + 1)  
	// 02.v   =   (Dx + Ey + F) / (Gx + Hy + 1)
	//

// 	w0 -> G
//	w1 -> H
// 	w2 -> I == 1
// 	sx -> A
// 	shx-> B
// 	tx -> C
// 	shy-> D
// 	sy -> E
// 	ty -> F
// 

typedef float _pttype;


	// 检测是否接近于0.0
	template<class T>
	inline bool is_equal_eps(T v1, T v2, double epsilon)
	{
		return fabs(v1 - v2) <= double(epsilon);
	}

#define perspective_transform(p, x, y, px, py) \
	{ \
		_pttype m = (_pttype)1.0f / ((x)*p.G + (y)*p.H + p.I); \
		*(px) = m * ((x)*p.A + (y)*p.B + p.C); \
		*(py) = m * ((x)*p.D + (y)*p.E + p.F); \
	}

	// 采用定点数的方法优化
	// PS: 优化的结果不是很明显，关键是这个除法没有优化,
	//     以及如何仍然返回一个浮点数
#define perspective_transform_fp(p, x, y, px, py) \
	{ \
		_pttype m = (_pttype)1.0f / (x*p.G_16 + y*p.H_16 + p.I_16); \
		*px = m * (x*p.A_16 + y*p.B_16 + p.C_16); \
		*py = m * (x*p.D_16 + y*p.E_16 + p.F_16); \
	}
	
	//
	//   A ┌---------------┐ B
	//     |               |
	//     |               |
	//     |               |
	//   D └---------------┘ C
	//
	struct Quad
	{
		union
		{
			int pos[8];

			struct
			{
				int Ax;
				int Ay;
				int Bx;
				int By;
				int Cx;
				int Cy;
				int Dx;
				int Dy;
			};
		};
		

		bool SetByRect(LPRECT prc)
		{
			if (NULL == prc)
				return false;

			Ax = Dx = prc->left;
			Bx = Cx = prc->right;
			Ay = By = prc->top;
			Cy = Dy = prc->bottom;
			
			return true;
		}
	};
	

	struct PerspectiveTransform
	{
		bool SetQuad(Quad* pQuad, RECT* lprcOri)
		{
			if (NULL == pQuad || NULL == lprcOri)
				return false;

			if (!quad_to_square(pQuad)) 
				return false;

			PerspectiveTransform p;
			Quad q;
			q.SetByRect(lprcOri);
			if (!p.square_to_quad(&q)) 
				return false;

			this->multiply(p);
			return true;
		}

		// 将浮点数类型转化为定点数，加快运行速度
		// 改为定点数之后，应改为调用perspective_transform_fp
		void ChangeToFixedPoint()
		{
			A_16 = (int)FLOAT_TO_FIXP16(A);
			B_16 = (int)FLOAT_TO_FIXP16(B);
			C_16 = (int)FLOAT_TO_FIXP16(C);
			D_16 = (int)FLOAT_TO_FIXP16(D);
			E_16 = (int)FLOAT_TO_FIXP16(E);
			F_16 = (int)FLOAT_TO_FIXP16(F);
			G_16 = (int)FLOAT_TO_FIXP16(G);
			H_16 = (int)FLOAT_TO_FIXP16(H);
			I_16 = (int)FLOAT_TO_FIXP16(I);
		}


		inline bool quad_to_square(Quad* pQuad)
		{
			if (!square_to_quad(pQuad)) 
				return false;

			invert();
			return true;
		}


		inline bool square_to_quad(Quad* pQuad)
		{
			_pttype dx = (_pttype)(pQuad->Ax - pQuad->Bx + pQuad->Cx - pQuad->Dx);
			_pttype dy = (_pttype)(pQuad->Ay - pQuad->By + pQuad->Cy - pQuad->Dy);
			if(dx == 0.0 && dy == 0.0)
			{   
				// Affine case (parallelogram)
				//---------------
				A = (_pttype)(pQuad->Bx - pQuad->Ax);
				D = (_pttype)(pQuad->By - pQuad->Ay);
				G = (_pttype)0.0;
				B = (_pttype)(pQuad->Cx - pQuad->Bx);
				E = (_pttype)(pQuad->Cy - pQuad->By);
				H = (_pttype)0.0;
				C = (_pttype)(pQuad->Ax);
				F = (_pttype)(pQuad->Ay);
				I = (_pttype)1.0;
			}
			else
			{
				_pttype dx1 = (_pttype)(pQuad->Bx - pQuad->Cx);
				_pttype dy1 = (_pttype)(pQuad->By - pQuad->Cy);
				_pttype dx2 = (_pttype)(pQuad->Dx - pQuad->Cx);
				_pttype dy2 = (_pttype)(pQuad->Dy - pQuad->Cy);
				_pttype den = dx1 * dy2 - dx2 * dy1;
				if(den == 0.0)
				{
					// Singular case
					//---------------
					A = D = G = B = E = H = C = F = I = 0.0;
					return false;
				}
				// General case
				//---------------
				_pttype u = (dx * dy2 - dy * dx2) / den;
				_pttype v = (dy * dx1 - dx * dy1) / den;
				A = (_pttype)(pQuad->Bx - pQuad->Ax + u * pQuad->Bx);
				D = (_pttype)(pQuad->By - pQuad->Ay + u * pQuad->By);
				G = u;
				B = (_pttype)(pQuad->Dx - pQuad->Ax + v * pQuad->Dx);
				E = (_pttype)(pQuad->Dy - pQuad->Ay + v * pQuad->Dy);
				H = v;
				C = (_pttype)(pQuad->Ax);
				F = (_pttype)(pQuad->Ay);
				I = 1.0;
			}
			return true;
		}

		inline const PerspectiveTransform& multiply(const PerspectiveTransform& a)
		{
			PerspectiveTransform b = *this;
			A = a.A * b.A  +  a.B * b.D  +  a.C * b.G;
			B = a.A * b.B  +  a.B * b.E  +  a.C * b.H;
			C = a.A * b.C  +  a.B * b.F  +  a.C * b.I;
			D = a.D * b.A  +  a.E * b.D  +  a.F * b.G;
			E = a.D * b.B  +  a.E * b.E  +  a.F * b.H;
			F = a.D * b.C  +  a.E * b.F  +  a.F * b.I;
			G = a.G * b.A  +  a.H * b.D  +  a.I * b.G;
			H = a.G * b.B  +  a.H * b.E  +  a.I * b.H;
			I = a.G * b.C  +  a.H * b.F  +  a.I * b.I;
			return *this;
		}

		//------------------------------------------------------------------------
		inline bool invert()
		{
			_pttype d0 = E*I  - H*F;
			_pttype d1 = G*F  - D*I;
			_pttype d2 = D*H  - G*E;
			_pttype d  = A*d0 + B*d1 + C*d2;

			if (d == 0.0) 
			{
				A = D = G = B = E = H = C = F = I = 0.0;
				return false;
			}
			d = 1.0f / d;

			PerspectiveTransform a = *this;
			A  = d * d0;
			D  = d * d1;
			G  = d * d2;
			B  = d * (a.H * a.C - a.B * a.I);
			E  = d * (a.A * a.I - a.G * a.C);
			H  = d * (a.G * a.B - a.A * a.H);
			C  = d * (a.B * a.F - a.E * a.C);
			F  = d * (a.D * a.C - a.A * a.F);
			I  = d * (a.A * a.E - a.D * a.B);
			return true;
		}

		inline bool is_valid(_pttype epsilon) const
		{
			return fabs(A) > epsilon && fabs(E) > epsilon && fabs(I) > epsilon;
		}

		//------------------------------------------------------------------------
		inline bool is_identiF(double epsilon) const
		{
			return  is_equal_eps(A, 1.0f, epsilon) &&
					is_equal_eps(D, 0.0f, epsilon) &&
					is_equal_eps(G, 0.0f, epsilon) &&
					is_equal_eps(B, 0.0f, epsilon) && 
					is_equal_eps(E, 1.0f, epsilon) &&
					is_equal_eps(H, 0.0f, epsilon) &&
					is_equal_eps(C, 0.0f, epsilon) &&
					is_equal_eps(F, 0.0f, epsilon) &&
					is_equal_eps(I, 1.0f, epsilon);
		}

		//------------------------------------------------------------------------
		inline bool is_equal(const PerspectiveTransform& m, 
			double epsilon) const
		{
			return  is_equal_eps(A, m.A, epsilon) &&
					is_equal_eps(D, m.D, epsilon) &&
					is_equal_eps(G, m.G, epsilon) &&
					is_equal_eps(B, m.B, epsilon) && 
					is_equal_eps(E, m.E, epsilon) &&
					is_equal_eps(H, m.H, epsilon) &&
					is_equal_eps(C, m.C, epsilon) &&
					is_equal_eps(F, m.F, epsilon) &&
					is_equal_eps(I, m.I, epsilon);
		}

		// 注：该函数可能会被密集调用时，请使用宏perspective_transform
		inline void transform(int x, int y, _pttype* px, _pttype* py) const
		{
			_pttype m = (_pttype)1.0f / (x*G + y*H + I);
			*px = m * (x*A  + y*B + C);
			*py = m * (x*D + y*E  + F);
		}
		inline void transform_fp(int x, int y, _pttype* px, _pttype* py) const
		{
			_pttype m = (_pttype)1.0f / (x*G_16 + y*H_16 + I_16);
			*px = m * (x*A_16  + y*B_16 + C_16);
			*py = m * (x*D_16 + y*E_16  + F_16);
		}

	public:
		_pttype   A, B, C;
		_pttype   D, E, F;
		_pttype   G, H, I;

		// 定点数形式
		int A_16, B_16, C_16, D_16, E_16, F_16, G_16, H_16, I_16;
	};
}

#endif  //  PERSPECTIVETRANSFORM_H_6D536C9D_C5B0_4f25_8CE9_B6EC29ECC0EF
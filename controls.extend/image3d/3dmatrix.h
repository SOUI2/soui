#pragma once
#include "3dlib.h"

namespace IMAGE3D
{
	// 4x4 matrix functions
	void Mat_Add_4X4(MATRIX_4_4_PTR ma, MATRIX_4_4_PTR mb, MATRIX_4_4_PTR msum);
	void Mat_Mul_4X4(MATRIX_4_4_PTR ma, MATRIX_4_4_PTR mb, MATRIX_4_4_PTR mprod);
	void Mat_Mul_1X4_4X4(MATRIX_1_4_PTR ma, MATRIX_4_4_PTR mb, MATRIX_1_4_PTR mprod);
	void Mat_Mul_VECTOR3D_4X4(VECTOR3D_PTR  va, MATRIX_4_4_PTR mb, VECTOR3D_PTR  vprod);
	void Mat_Mul_VECTOR3D_4X3(VECTOR3D_PTR  va, MATRIX_4_3_PTR mb, VECTOR3D_PTR  vprod);
	void Mat_Mul_VECTOR4D_4X4(VECTOR4D_PTR  va, MATRIX_4_4_PTR mb, VECTOR4D_PTR  vprod);
	void Mat_Mul_VECTOR4D_4X3(VECTOR4D_PTR  va, MATRIX_4_4_PTR mb, VECTOR4D_PTR  vprod);
	int  Mat_Inverse_4X4(MATRIX_4_4_PTR m, MATRIX_4_4_PTR mi);
	void Mat_Init_4X4(MATRIX_4_4_PTR ma, 
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33);
}
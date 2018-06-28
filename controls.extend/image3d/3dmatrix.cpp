#include "stdafx.h"
#include "3dmatrix.h"

#include <math.h>

namespace IMAGE3D
{

void Mat_Add_4X4(MATRIX_4_4_PTR ma, 
				 MATRIX_4_4_PTR mb,
				 MATRIX_4_4_PTR msum)
{
	// this function adds two 4x4 matrices together and 
	// and stores the result

	for (int row=0; row<4; row++)
	{
		for (int col=0; col<4; col++)
		{
			// insert resulting row,col element
			msum->M[row][col] = ma->M[row][col] + mb->M[row][col];
		} // end for col

	} // end for row

} // end Mat_Add_4X4

///////////////////////////////////////////////////////////////

void Mat_Mul_4X4(MATRIX_4_4_PTR ma, 
				 MATRIX_4_4_PTR mb,
				 MATRIX_4_4_PTR mprod)
{
	// this function multiplies two 4x4 matrices together and 
	// and stores the result in mprod
	// note later we will take advantage of the fact that we know
	// that w=1 always, and that the last column of a 4x4 is
	// always 0

	for (int row=0; row<4; row++)
	{
		for (int col=0; col<4; col++)
		{
			// compute dot product from row of ma 
			// and column of mb

			float sum = 0; // used to hold result

			for (int index=0; index<4; index++)
			{
				// add in next product pair
				sum+=(ma->M[row][index]*mb->M[index][col]);
			} // end for index

			// insert resulting row,col element
			mprod->M[row][col] = sum;

		} // end for col

	} // end for row

} // end Mat_Mul_4X4

////////////////////////////////////////////////////////////////

void Mat_Mul_1X4_4X4(MATRIX_1_4_PTR ma, 
					 MATRIX_4_4_PTR mb,
					 MATRIX_1_4_PTR mprod)
{
	// this function multiplies a 1x4 matrix against a 
	// 4x4 matrix - ma*mb and stores the result
	// no tricks or assumptions here, just a straight multiply

	for (int col=0; col<4; col++)
	{
		// compute dot product from row of ma 
		// and column of mb
		float sum = 0; // used to hold result

		for (int row=0; row<4; row++)
		{
			// add in next product pair
			sum+=(ma->M[row] * mb->M[row][col]);
		} // end for index

		// insert resulting col element
		mprod->M[col] = sum;

	} // end for col

} // end Mat_Mul_1X4_4X4

////////////////////////////////////////////////////////////////////

void Mat_Mul_VECTOR3D_4X4(VECTOR3D_PTR  va, 
						  MATRIX_4_4_PTR mb,
						  VECTOR3D_PTR  vprod)
{
	// this function multiplies a VECTOR3D against a 
	// 4x4 matrix - ma*mb and stores the result in mprod
	// the function assumes that the vector refers to a 
	// 4D homogenous vector, thus the function assumes that
	// w=1 to carry out the multiply, also the function
	// does not carry out the last column multiply since
	// we are assuming w=1, there is no point

	for (int col=0; col < 3; col++)
	{
		// compute dot product from row of ma 
		// and column of mb
		float sum = 0; // used to hold result

		int row = 0;
		for (; row<3; row++)
		{
			// add in next product pair
			sum+=(va->M[row]*mb->M[row][col]);
		} // end for index

		// add in last element in column or w*mb[3][col]
		sum+=mb->M[row][col];    

		// insert resulting col element
		vprod->M[col] = sum;

	} // end for col

} // end Mat_Mul_VECTOR3D_4X4

///////////////////////////////////////////////////////////////

void Mat_Mul_VECTOR3D_4X3(VECTOR3D_PTR  va, 
						  MATRIX_4_3_PTR mb,
						  VECTOR3D_PTR  vprod)
{
	// this function multiplies a VECTOR3D against a 
	// 4x3 matrix - ma*mb and stores the result in mprod
	// the function assumes that the vector refers to a 
	// 4D homogenous vector, thus the function assumes that
	// w=1 to carry out the multiply, also the function
	// does not carry out the last column multiply since
	// we are assuming w=1, there is no point

	for (int col=0; col < 3; col++)
	{
		// compute dot product from row of ma 
		// and column of mb
		float sum = 0; // used to hold result

		int row = 0;
		for (; row<3; row++)
		{
			// add in next product pair
			sum+=(va->M[row]*mb->M[row][col]);
		} // end for index

		// add in last element in column or w*mb[3][col]
		sum+=mb->M[row][col];    

		// insert resulting col element
		vprod->M[col] = sum;

	} // end for col

} // end Mat_Mul_VECTOR3D_4X3

////////////////////////////////////////////////////////////////////

void Mat_Mul_VECTOR4D_4X4(VECTOR4D_PTR  va, 
						  MATRIX_4_4_PTR mb,
						  VECTOR4D_PTR  vprod)
{
	// this function multiplies a VECTOR4D against a 
	// 4x4 matrix - ma*mb and stores the result in mprod
	// the function makes no assumptions

	for (int col=0; col < 4; col++)
	{
		// compute dot product from row of ma 
		// and column of mb
		float sum = 0; // used to hold result

		for (int row=0; row<4; row++)
		{
			// add in next product pair
			sum+=(va->M[row]*mb->M[row][col]);
		} // end for index

		// insert resulting col element
		vprod->M[col] = sum;

	} // end for col

} // end Mat_Mul_VECTOR4D_4X4

////////////////////////////////////////////////////////////////////

void Mat_Mul_VECTOR4D_4X3(VECTOR4D_PTR  va, 
						  MATRIX_4_4_PTR mb,
						  VECTOR4D_PTR  vprod)
{
	// this function multiplies a VECTOR4D against a 
	// 4x3 matrix - ma*mb and stores the result in mprod
	// the function assumes that the last column of
	// mb is [0 0 0 1]t , thus w just gets replicated
	// from the vector [x y z w]

	for (int col=0; col < 3; col++)
	{
		// compute dot product from row of ma 
		// and column of mb
		float sum = 0; // used to hold result

		for (int row=0; row<4; row++)
		{
			// add in next product pair
			sum+=(va->M[row]*mb->M[row][col]);
		} // end for index

		// insert resulting col element
		vprod->M[col] = sum;

	} // end for col

	// copy back w element
	vprod->M[3] = va->M[3];

} // end Mat_Mul_VECTOR4D_4X3

///////////////////////////////////////////////////////////////

void Mat_Init_4X4(MATRIX_4_4_PTR ma, 
				  float m00, float m01, float m02, float m03,
				  float m10, float m11, float m12, float m13,
				  float m20, float m21, float m22, float m23,
				  float m30, float m31, float m32, float m33)

{
	// this function fills a 4x4 matrix with the sent data in 
	// row major form
	ma->M00 = m00; ma->M01 = m01; ma->M02 = m02; ma->M03 = m03;
	ma->M10 = m10; ma->M11 = m11; ma->M12 = m12; ma->M13 = m13;
	ma->M20 = m20; ma->M21 = m21; ma->M22 = m22; ma->M23 = m23;
	ma->M30 = m30; ma->M31 = m31; ma->M32 = m32; ma->M33 = m33;

} // end Mat_Init_4X4

////////////////////////////////////////////////////////////////

int Mat_Inverse_4X4(MATRIX_4_4_PTR m, MATRIX_4_4_PTR mi)
{
	// computes the inverse of a 4x4, assumes that the last
	// column is [0 0 0 1]t

	float det =  ( m->M00 * ( m->M11 * m->M22 - m->M12 * m->M21 ) -
		m->M01 * ( m->M10 * m->M22 - m->M12 * m->M20 ) +
		m->M02 * ( m->M10 * m->M21 - m->M11 * m->M20 ) );

	// test determinate to see if it's 0
	if (fabs(det) < EPSILON_E5)
		return(0);

	float det_inv  = 1.0f / det;

	mi->M00 =  det_inv * ( m->M11 * m->M22 - m->M12 * m->M21 );
	mi->M01 = -det_inv * ( m->M01 * m->M22 - m->M02 * m->M21 );
	mi->M02 =  det_inv * ( m->M01 * m->M12 - m->M02 * m->M11 );
	mi->M03 = 0.0f; // always 0

	mi->M10 = -det_inv * ( m->M10 * m->M22 - m->M12 * m->M20 );
	mi->M11 =  det_inv * ( m->M00 * m->M22 - m->M02 * m->M20 );
	mi->M12 = -det_inv * ( m->M00 * m->M12 - m->M02 * m->M10 );
	mi->M13 = 0.0f; // always 0

	mi->M20 =  det_inv * ( m->M10 * m->M21 - m->M11 * m->M20 );
	mi->M21 = -det_inv * ( m->M00 * m->M21 - m->M01 * m->M20 );
	mi->M22 =  det_inv * ( m->M00 * m->M11 - m->M01 * m->M10 );
	mi->M23 = 0.0f; // always 0

	mi->M30 = -( m->M30 * mi->M00 + m->M31 * mi->M10 + m->M32 * mi->M20 );
	mi->M31 = -( m->M30 * mi->M01 + m->M31 * mi->M11 + m->M32 * mi->M21 );
	mi->M32 = -( m->M30 * mi->M02 + m->M31 * mi->M12 + m->M32 * mi->M22 );
	mi->M33 = 1.0f; // always 0

	// return success 
	return(1);

} // end Mat_Inverse_4X4

/////////////////////////////////////////////////////////////////

}
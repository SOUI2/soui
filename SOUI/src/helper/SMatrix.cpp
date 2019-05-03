#include "souistd.h"
#include "helper/SMatrix.h"
#include "layout/SLayoutSize.h"
#pragma warning (push)
#pragma warning (disable: 4985) // disable the warning message during the include
#include <math.h>               // this is where I would normally get the warning message
#pragma warning (pop)

namespace SOUI
{
	const static FLOAT KPI     =        3.14159265f;
	const static FLOAT kDegreesToRadians = KPI/180;

	SMatrix::SMatrix()
    {
		reset();
    }

    SMatrix::SMatrix(const SMatrix & src)
    {
		memcpy(m_mat,src.m_mat,sizeof(m_mat));
    }

    SMatrix::SMatrix(FLOAT m11, FLOAT m12, FLOAT m21, FLOAT m22, FLOAT dx, FLOAT dy)
    {
		setMatrix(m11,m12,m21,m22,dx,dy);
    }


    void SMatrix::setMatrix(FLOAT m11, FLOAT m12, FLOAT m21, FLOAT m22, FLOAT dx, FLOAT dy)
    {
		m_mat[kMScaleX] = m11;
		m_mat[kMSkewX] = m21;
		m_mat[kMSkewY] = m12;
		m_mat[kMScaleY] = m22;
		m_mat[kMTransX] = dx;
		m_mat[kMTransY] = dy;
		m_mat[kMPerspX] = m_mat[kMPerspY] = 0.0f;
		m_mat[kMPersp2] = 1.0f;
    }

    void SMatrix::reset()
    {
		memset(m_mat,0,sizeof(m_mat));
		m_mat[kMScaleX] = m_mat[kMScaleY] = m_mat[kMPersp2] = 1.0f;
    }

    bool SMatrix::isIdentity() const
    {
		return SLayoutSize::fequal(m_mat[kMScaleX], 1.0f) && SLayoutSize::fequal(m_mat[kMScaleY],1.0f) 
			&& SLayoutSize::fequal(m_mat[kMSkewX],0.0f) && SLayoutSize::fequal(m_mat[kMSkewY],0.0f)
            && SLayoutSize::fequal(m_mat[kMTransX],0.0f) && SLayoutSize::fequal(m_mat[kMTransY],0.0f)
		    && SLayoutSize::fequal(m_mat[kMPerspX],0.0f) && SLayoutSize::fequal(m_mat[kMPerspY],0.0f) && SLayoutSize::fequal(m_mat[kMPersp2],1.0f);
    }


	float SMatrix::GetScaleX() const
	{
		return m_mat[kMScaleX];
	}

	float SMatrix::GetScaleY() const
	{
		return m_mat[kMScaleY];
	}

	float SMatrix::GetSkewX() const
	{
		return m_mat[kMSkewX];
	}

	float SMatrix::GetSkewY() const
	{
		return m_mat[kMSkewY];
	}

	float SMatrix::GetTranslateX() const
	{
		return m_mat[kMTransX];
	}

	float SMatrix::GetTranslateY() const
	{
		return m_mat[kMTransY];
	}

	float SMatrix::GetPerspX() const
	{
		return m_mat[kMPerspX];
	}

	float SMatrix::GetPerspY() const
	{
		return m_mat[kMPerspY];
	}

	float SMatrix::GetPersp2() const
	{
		return m_mat[kMPersp2];
	}

	void SMatrix::SetScaleX(float v)
	{
		m_mat[kMScaleX] = v;
	}

	void SMatrix::SetScaleY(float v)
	{
		m_mat[kMScaleY] = v;
	}

	void SMatrix::SetSkewY(float v)
	{
		m_mat[kMSkewX] = v;
	}

	void SMatrix::SetSkewX(float v)
	{
		m_mat[kMSkewY] = v;
	}

	void SMatrix::SetTranslateX(float v)
	{
		m_mat[kMTransX] = v;
	}

	void SMatrix::SetTranslateY(float v)
	{
		m_mat[kMTransY] = v;
	}

	void SMatrix::SetPerspX(float v)
	{
		m_mat[kMPerspX] = v;
	}

	void SMatrix::SetPerspY(float v)
	{
		m_mat[kMPerspY] = v;
	}

	void SMatrix::SetPersp2(float v)
	{
		m_mat[kMPersp2] = v;
	}


#define eM11 m_mat[kMScaleX]
#define eM21 m_mat[kMSkewX] 
#define eM12 m_mat[kMSkewY] 
#define eM22 m_mat[kMScaleY]
#define eDx  m_mat[kMTransX]
#define eDy  m_mat[kMTransY]

    SMatrix & SMatrix::translate(FLOAT dx, FLOAT dy)
    {
        eDx += dx*eM11 + dy*eM21;
        eDy += dy*eM22 + dx*eM12;
        return *this;
    }

    SMatrix & SMatrix::scale(FLOAT sx, FLOAT sy)
    {
        eM11 *= sx;
        eM12 *= sx;
        eM21 *= sy;
        eM22 *= sy;
        return *this;
    }

	SMatrix & SMatrix::setScale(FLOAT sx, FLOAT sy, FLOAT px, FLOAT py)
	{
		eM11 = sx;
		eM22 = sy;
		eDx = px - sx * px;
		eDy = py - sy * py;

		eM12 = eM21 = m_mat[kMPerspX] = m_mat[kMPerspY] = 0.0f;
		m_mat[kMPersp2] = 1.0f;
		return *this;
	}

    SMatrix & SMatrix::shear(FLOAT sh, FLOAT sv)
    {
        FLOAT tm11 = sv*eM21;
        FLOAT tm12 = sv*eM22;
        FLOAT tm21 = sh*eM11;
        FLOAT tm22 = sh*eM12;
        eM11 += tm11;
        eM12 += tm12;
        eM21 += tm21;
        eM22 += tm22;
        return *this;
    }

    SMatrix & SMatrix::rotate(FLOAT a)
    {
        FLOAT sina = 0;
        FLOAT cosa = 0;
		if (SLayoutSize::fequal(a , 90.0f) || SLayoutSize::fequal(a,-270.0f))
            sina = 1.f;
        else if (SLayoutSize::fequal(a,270.0f) || SLayoutSize::fequal(a,-90.0f))
            sina = -1.f;
        else if (SLayoutSize::fequal(a,180.0f))
            cosa = -1.f;
        else{
            FLOAT b = kDegreesToRadians*a;          // convert to radians
            sina = (FLOAT)sin(b);               // fast and convenient
            cosa = (FLOAT)cos(b);
        }
        FLOAT tm11 = cosa*eM11 + sina*eM21;
        FLOAT tm12 = cosa*eM12 + sina*eM22;
        FLOAT tm21 = -sina*eM11 + cosa*eM21;
        FLOAT tm22 = -sina*eM12 + cosa*eM22;
        eM11 = tm11; eM12 = tm12;
        eM21 = tm21; eM22 = tm22;
        return *this;
    }


    void SMatrix::inverted(SMatrix *pOut, bool *invertible) const
    {
        FLOAT dtr = determinant();
        if (dtr == 0.0) {
            if (invertible)
                *invertible = false;                // singular matrix
            pOut->reset();
        }
        else {                                        // invertible matrix
            if (invertible)
                *invertible = true;
            FLOAT dinv = 1.0f/dtr;
            pOut->setMatrix((eM22*dinv),        (-eM12*dinv),
                (-eM21*dinv), (eM11*dinv),
                ((eM21*eDy - eM22*eDx)*dinv),
                ((eM12*eDx - eM11*eDy)*dinv));
        }
    }


    /*!
    \fn bool SMatrix::operator==(const SMatrix &matrix) const

    Returns true if this matrix is equal to the given \a matrix,
    otherwise returns false.
    */

    bool SMatrix::operator==(const SMatrix &m) const
    {
        return !((*this)!=m);
    }

    /*!
    \fn bool SMatrix::operator!=(const SMatrix &matrix) const

    Returns true if this matrix is not equal to the given \a matrix,
    otherwise returns false.
    */

    bool SMatrix::operator!=(const SMatrix &src) const
    {
        return  !(SLayoutSize::fequal(eM11 , src.eM11)
                &&SLayoutSize::fequal(eM12 , src.eM12)
                &&SLayoutSize::fequal(eM21 , src.eM21)
                &&SLayoutSize::fequal(eM22 , src.eM22)
                &&SLayoutSize::fequal(eDx , src.eDx)  
                &&SLayoutSize::fequal(eDy , src.eDy)
				&&SLayoutSize::fequal(m_mat[kMPerspX] , src.m_mat[kMPerspX])
				&&SLayoutSize::fequal(m_mat[kMPerspY] , src.m_mat[kMPerspY])  
				&&SLayoutSize::fequal(m_mat[kMPersp2] , src.m_mat[kMPersp2])
				) ;
    }

    /*!
    \fn SMatrix &SMatrix::operator *=(const SMatrix &matrix)
    \overload

    Returns the result of multiplying this matrix by the given \a
    matrix.
    */

    SMatrix &SMatrix::operator *=(const SMatrix &src)
    {
        FLOAT tm11 = eM11*src.eM11 + eM12*src.eM21;
        FLOAT tm12 = eM11*src.eM12 + eM12*src.eM22;
        FLOAT tm21 = eM21*src.eM11 + eM22*src.eM21;
        FLOAT tm22 = eM21*src.eM12 + eM22*src.eM22;

        FLOAT tdx  = eDx*src.eM11  + eDy*src.eM21 + src.eDx;
        FLOAT tdy =  eDx*src.eM12  + eDy*src.eM22 + src.eDy;

        eM11 = tm11; eM12 = tm12;
        eM21 = tm21; eM22 = tm22;
        eDx = tdx; eDy = tdy;
        return *this;
    }

    /*!
    \fn SMatrix SMatrix::operator *(const SMatrix &matrix) const

    Returns the result of multiplying this matrix by the given \a
    matrix.

    Note that matrix multiplication is not commutative, i.e. a*b !=
    b*a.
    */

    SMatrix SMatrix::operator *(const SMatrix &m) const
    {
        FLOAT tm11 = eM11*m.eM11 + eM12*m.eM21;
        FLOAT tm12 = eM11*m.eM12 + eM12*m.eM22;
        FLOAT tm21 = eM21*m.eM11 + eM22*m.eM21;
        FLOAT tm22 = eM21*m.eM12 + eM22*m.eM22;

        FLOAT tdx  = eDx*m.eM11  + eDy*m.eM21 + m.eDx;
        FLOAT tdy =  eDx*m.eM12  + eDy*m.eM22 + m.eDy;
        return SMatrix(tm11, tm12, tm21, tm22, tdx, tdy);
    }

    /*!
    Assigns the given \a matrix's values to this matrix.
    */
    SMatrix &SMatrix::operator=(const SMatrix &src)
    {
        eM11 = src.eM11;
        eM12 = src.eM12;
        eM21 = src.eM21;
        eM22 = src.eM22;
        eDx  = src.eDx;
        eDy  = src.eDy;
		m_mat[kMPerspX] = src.m_mat[kMPerspX];
		m_mat[kMPerspY] = src.m_mat[kMPerspY];
		m_mat[kMPersp2] = src.m_mat[kMPersp2];
        return *this;
    }

    FLOAT SMatrix::determinant() const
    {
        return eM11*eM22 - eM12*eM21;
    }

	SMatrix & SMatrix::perspective(FLOAT perspX, FLOAT perspY)
	{
		m_mat[kMPerspX] = perspX;
		m_mat[kMPerspY] = perspY;
		return *this;
	}

}
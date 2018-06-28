#include "souistd.h"
#include "helper/SMatrix.h"
#pragma warning (push)
#pragma warning (disable: 4985) // disable the warning message during the include
#include <math.h>               // this is where I would normally get the warning message
#pragma warning (pop)

namespace SOUI
{
    const FLOAT deg2rad = FLOAT(0.017453292519943295769);        // pi/180

    inline bool SFuzzyIsNull(FLOAT d)
    {
        return fabs(d) <= 0.000000000001;
    }

    SMatrix::SMatrix()
    {
        eM11 = (1.);
        eM12 = (0.);
        eM21 = (0.);
        eM22 = (1.);
            
        eDx  = (0.);
        eDy  = (0.);

    }

    SMatrix::SMatrix(const SMatrix & src)
    {
        eM11 = (src.eM11),eM12 = (src.eM12),eM21 = (src.eM21),eM22 = (src.eM22),eDx = (src.eDx),eDy = (src.eDy);
    }

    SMatrix::SMatrix(FLOAT m11, FLOAT m12, FLOAT m21, FLOAT m22, FLOAT dx, FLOAT dy)
    {
        eM11 = (m11),eM12 = (m12),eM21 = (m21),eM22 = (m22),eDx = (dx),eDy = (dy);
    }


    void SMatrix::setMatrix(FLOAT m11, FLOAT m12, FLOAT m21, FLOAT m22, FLOAT dx, FLOAT dy)
    {
        eM11 = m11;
        eM12 = m12;
        eM21 = m21;
        eM22 = m22;
        eDx  = dx;
        eDy  = dy;
    }

    FLOAT SMatrix::m11() const
    {
        return eM11;
    }

    FLOAT SMatrix::m12() const
    {
        return eM12;
    }

    FLOAT SMatrix::m21() const
    {
        return eM21;
    }

    FLOAT SMatrix::m22() const
    {
        return eM22;
    }

    FLOAT SMatrix::dx() const
    {
        return eDx;
    }

    FLOAT SMatrix::dy() const
    {
        return eDy;
    }

    void SMatrix::reset()
    {
        eM11 = eM22 = 1.0;
        eM12 = eM21 = eDx = eDy = 0.0;
    }

    bool SMatrix::isIdentity() const
    {
        return SFuzzyIsNull(eM11 - 1) && SFuzzyIsNull(eM22 - 1) && SFuzzyIsNull(eM12)
            && SFuzzyIsNull(eM21) && SFuzzyIsNull(eDx) && SFuzzyIsNull(eDy);
    }

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
        if (a == 90. || a == -270.)
            sina = 1.;
        else if (a == 270. || a == -90.)
            sina = -1.;
        else if (a == 180.)
            cosa = -1.;
        else{
            FLOAT b = deg2rad*a;          // convert to radians
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

    bool SMatrix::operator!=(const SMatrix &m) const
    {
        return  !SFuzzyIsNull(eM11 - m.eM11) ||
                !SFuzzyIsNull(eM12 - m.eM12) ||
                !SFuzzyIsNull(eM21 - m.eM21) ||
                !SFuzzyIsNull(eM22 - m.eM22) ||
                !SFuzzyIsNull(eDx - m.eDx)   ||
                !SFuzzyIsNull(eDy - m.eDy) ;
    }

    /*!
    \fn SMatrix &SMatrix::operator *=(const SMatrix &matrix)
    \overload

    Returns the result of multiplying this matrix by the given \a
    matrix.
    */

    SMatrix &SMatrix::operator *=(const SMatrix &m)
    {
        FLOAT tm11 = eM11*m.eM11 + eM12*m.eM21;
        FLOAT tm12 = eM11*m.eM12 + eM12*m.eM22;
        FLOAT tm21 = eM21*m.eM11 + eM22*m.eM21;
        FLOAT tm22 = eM21*m.eM12 + eM22*m.eM22;

        FLOAT tdx  = eDx*m.eM11  + eDy*m.eM21 + m.eDx;
        FLOAT tdy =  eDx*m.eM12  + eDy*m.eM22 + m.eDy;

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
    SMatrix &SMatrix::operator=(const SMatrix &matrix)
    {
        eM11 = matrix.eM11;
        eM12 = matrix.eM12;
        eM21 = matrix.eM21;
        eM22 = matrix.eM22;
        eDx  = matrix.eDx;
        eDy  = matrix.eDy;
        return *this;
    }

    FLOAT SMatrix::determinant() const
    {
        return eM11*eM22 - eM12*eM21;
    }


}
#pragma once

#include <interface/render-i.h>

namespace SOUI
{
    
    class SOUI_EXP SMatrix : public IxForm
    {
    public:
        SMatrix();

        SMatrix(const SMatrix & src);
        SMatrix(FLOAT m11, FLOAT m12, FLOAT m21, FLOAT m22, FLOAT dx, FLOAT dy);

    public:
        bool operator==(const SMatrix &) const;
        bool operator!=(const SMatrix &) const;

        SMatrix &operator*=(const SMatrix &);
        SMatrix operator*(const SMatrix &o) const;

        SMatrix &operator=(const SMatrix &);

    public:
        void setMatrix(FLOAT m11, FLOAT m12, FLOAT m21, FLOAT m22, FLOAT dx, FLOAT dy);

        FLOAT m11() const;

        FLOAT m12() const;

        FLOAT m21() const;

        FLOAT m22() const;

        FLOAT dx() const;

        FLOAT dy() const;

        void reset();

        bool isIdentity() const;

        SMatrix & translate(FLOAT dx, FLOAT dy);

        SMatrix & scale(FLOAT sx, FLOAT sy);

        SMatrix & shear(FLOAT sh, FLOAT sv);

        SMatrix & rotate(FLOAT a);

        void inverted(SMatrix *pOut, bool *invertible = 0) const;

        FLOAT determinant() const;

    };


}

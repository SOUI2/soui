#pragma once

#include <interface/render-i.h>

namespace SOUI
{
    
    class SOUI_EXP SMatrix : public IxForm
    {
	protected:
		float m_mat[9];
    public:
        SMatrix();

        SMatrix(const SMatrix & src);
        SMatrix(FLOAT m11, FLOAT m12, FLOAT m21, FLOAT m22, FLOAT dx, FLOAT dy);
		SMatrix(const FLOAT mat[9]);
	public://IxForm
		virtual float GetScaleX() const ;
		virtual float GetScaleY() const ;
		virtual float GetSkewX() const ;
		virtual float GetSkewY() const ;
		virtual float GetTranslateX() const ;
		virtual float GetTranslateY() const ;
		virtual float GetPersp0() const ;
		virtual float GetPersp1() const ;
		virtual float GetPersp2() const ;
		virtual float GetValue(Index idx) const;
		virtual const float *GetData() const;

		virtual void SetScaleX(float v) ;
		virtual void SetScaleY(float v) ;
		virtual void SetSkewY(float v) ;
		virtual void SetSkewX(float v) ;
		virtual void SetTranslateX(float v) ;
		virtual void SetTranslateY(float v) ;
		virtual void SetPersp0(float v) ;
		virtual void SetPersp1(float v) ;
		virtual void SetPersp2(float v) ;
		virtual void SetValue(Index idx, float v);
		virtual void SetData(const float fMat[9]);
    public:
        bool operator==(const SMatrix &src) const;
        bool operator!=(const SMatrix &src) const;

        SMatrix &operator*=(const SMatrix &src);
        SMatrix operator*(const SMatrix &src) const;

        SMatrix &operator=(const SMatrix &src);

    public:
        void setMatrix(FLOAT m11, FLOAT m12, FLOAT m21, FLOAT m22, FLOAT dx, FLOAT dy);


        void reset();

        bool isIdentity() const;

        SMatrix & translate(FLOAT dx, FLOAT dy);

        SMatrix & scale(FLOAT sx, FLOAT sy);

		/** Set the matrix to scale by sx and sy, with a pivot point at (px, py).
		The pivot point is the coordinate that should remain unchanged by the
		specified transformation.
		*/
		SMatrix & setScale(FLOAT sx, FLOAT sy, FLOAT px, FLOAT py);

        SMatrix & shear(FLOAT sh, FLOAT sv);

        SMatrix & rotate(FLOAT a);

        void inverted(SMatrix *pOut, bool *invertible = 0) const;

        FLOAT determinant() const;

    };


}

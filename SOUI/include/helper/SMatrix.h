#pragma once

#include <interface/render-i.h>

namespace SOUI
{
    
    class SOUI_EXP SMatrix : public IxForm
    {
	protected:
		enum{
			kMScaleX = 0,	//m11
			kMSkewX,		//m21
			kMTransX,		//eDx
			kMSkewY,		//m12
			kMScaleY,		//m22
			kMTransY,		//eDy
			kMPerspX,
			kMPerspY,
			kMPersp2,
		};
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
		virtual float GetPerspX() const ;
		virtual float GetPerspY() const ;
		virtual float GetPersp2() const ;

		virtual void SetScaleX(float v) ;
		virtual void SetScaleY(float v) ;
		virtual void SetSkewY(float v) ;
		virtual void SetSkewX(float v) ;
		virtual void SetTranslateX(float v) ;
		virtual void SetTranslateY(float v) ;
		virtual void SetPerspX(float v) ;
		virtual void SetPerspY(float v) ;
		virtual void SetPersp2(float v) ;

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

		SMatrix & perspective(FLOAT perspX, FLOAT perspY);

        void inverted(SMatrix *pOut, bool *invertible = 0) const;

        FLOAT determinant() const;

    };


}

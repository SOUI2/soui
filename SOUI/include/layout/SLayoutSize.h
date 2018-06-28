#pragma once

namespace SOUI
{

	class SOUI_EXP SLayoutSize
	{
	public:
		enum Unit{
			px=0,dp,dip,sp
		};

		SLayoutSize();

		SLayoutSize(float fSize,Unit unit);
		float fSize;
		Unit  unit;

		void setWrapContent();
		bool isWrapContent() const;

		void setMatchParent();
		bool isMatchParent() const;

		void setSize(float fSize, Unit unit);
		bool isSpecifiedSize() const;
		
		void setInvalid();
		bool isValid() const;

		int  toPixelSize(int scale) const;

		SStringW toString() const;
		
		bool isZero() const ;

		void parseString(const SStringW & strSize);

		SLayoutSize & operator = (const SLayoutSize & src);
		
		bool valueEqual(float value);

		static SLayoutSize fromString(const SStringW & strSize);
		static bool fequal(float a, float b);
	};


}
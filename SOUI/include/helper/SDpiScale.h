#pragma once
namespace SOUI
{
	class SDpiScale
	{
	public:
		static int GetBuiltinScaleCount();

		static const int * GetBuiltinScales();

		/*标准化放大比例, 选择比自己指定比例小一号的比例*/
		static int NormalizeScale(int nScale);
	};


}

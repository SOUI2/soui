#include "souistd.h"
#include "helper/SDpiScale.h"

namespace SOUI
{
	static const int KBuiltinScales[] =
	{
		100,125,150,200,250,300
	};

	int SDpiScale::GetBuiltinScaleCount()
	{
		return ARRAYSIZE(KBuiltinScales);
	}

	const int * SDpiScale::GetBuiltinScales() {
		return KBuiltinScales;
	}

	/*标准化放大比例, 选择比自己指定比例小一号的比例*/
	int SDpiScale::NormalizeScale(int nScale)
	{
		for (int i = 1; i < ARRAYSIZE(KBuiltinScales); i++)
		{
			if (nScale < KBuiltinScales[i])
			{
				return KBuiltinScales[i - 1];
			}
		}
		return KBuiltinScales[ARRAYSIZE(KBuiltinScales) - 1];
	}
}

#pragma once


#include "Enum.h"


namespace SevenZip
{
	struct CompressionLevel
	{
		enum _Enum
		{
			Default,
			Fast,
			Normal
		};
	
		typedef intl::EnumerationDefinitionNoStrings _Definition;
		typedef intl::EnumerationValue< _Enum, _Definition, Normal > _Value;
	};
	
	typedef CompressionLevel::_Value CompressionLevelEnum;
}

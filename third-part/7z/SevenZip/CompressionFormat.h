#pragma once


#include "Enum.h"


namespace SevenZip
{
	struct CompressionFormat
	{
		enum _Enum
		{
			Unknown,
            SevenZip,
			Zip,
			GZip,
			BZip2,
			Rar,
			Tar,
			Iso,
			Cab,
			Lzma,
			Lzma86
		};
	
		typedef intl::EnumerationDefinitionNoStrings _Definition;
		typedef intl::EnumerationValue< _Enum, _Definition, Unknown > _Value;
	};
	
	typedef CompressionFormat::_Value CompressionFormatEnum;


    struct PathMode
    {
        enum _Enum
        {
            kFullPathnames,
            kCurrentPathnames,
            kNoPathnames
        };
        typedef intl::EnumerationDefinitionNoStrings _Definition;
        typedef intl::EnumerationValue< _Enum, _Definition, kFullPathnames > _Value;
    };

    typedef PathMode::_Value PathModeEnum;

    struct OverwriteMode
    {
        enum _Enum
        {
            kAbort,
            kWithoutPrompt,
            kSkipExisting,
            kAutoRename,
            kAutoRenameExisting,
            kRollBack,
        };
        typedef intl::EnumerationDefinitionNoStrings _Definition;
        typedef intl::EnumerationValue< _Enum, _Definition, kAbort > _Value;
    };
    typedef OverwriteMode::_Value OverwriteModeEnum;

}

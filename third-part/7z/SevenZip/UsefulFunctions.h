#pragma once
#include "CompressionFormat.h"
#include "../CPP/7zip/Archive/IArchive.h"
#include "../C/7zTypes.h"
#include "GUIDs.h"
#include "FileSys.h"
#include "ArchiveOpenCallback.h"
#include "InStreamWrapper.h"

namespace SevenZip
{
    using namespace intl;

    class UsefulFunctions
    {
    public:
        static const GUID* GetCompressionGUID(const CompressionFormatEnum& format);

        static CMyComPtr< IInArchive > GetArchiveReader(const CompressionFormatEnum& format);
        static CMyComPtr< IOutArchive > GetArchiveWriter(const CompressionFormatEnum& format);
        static bool DetectCompressionFormat(const TString& archivePath,
            CompressionFormatEnum & archiveCompressionFormat);

        static bool UsefulFunctions::GetNumberOfItems(const TString & archivePath,
            CompressionFormatEnum & format, size_t & numberofitems);

        static bool UsefulFunctions::GetItemsNames(const TString & archivePath,
            CompressionFormatEnum & format, size_t & numberofitems,
            std::vector<TString> & itemnames, std::vector<size_t> & origsizes);

        static const TString EndingFromCompressionFormat(const CompressionFormatEnum& format);
    };
}

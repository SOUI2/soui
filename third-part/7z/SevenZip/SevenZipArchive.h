#pragma once
#include "../CPP/Common/MyCom.h"
#include "FileInfo.h"
#include "CompressionFormat.h"
#include "CompressionLevel.h"
#include <vector>

namespace SevenZip
{
	class SevenZipArchive
	{
	public:
		SevenZipArchive();
		virtual ~SevenZipArchive();

        virtual bool SetArchivePath(const TString& archivePath);

		virtual bool ReadInArchiveMetadata();

		virtual void SetCompressionFormat(const CompressionFormatEnum& format);
		virtual CompressionFormatEnum GetCompressionFormat();

		virtual void SetCompressionLevel(const CompressionLevelEnum& level);
		virtual CompressionLevelEnum GetCompressionLevel();

		virtual void SetEncryptFileName(bool bEnable);
		virtual bool IsEncryptFileName();

		virtual bool DetectCompressionFormat();

		virtual size_t GetNumberOfItems();
		virtual std::vector<TString> GetItemsNames();
		virtual std::vector<size_t>  GetOrigSizes();

	protected:
		bool m_EncryptFileName;
		bool m_ReadMetadata;
		bool m_OverrideCompressionFormat;
		TString m_archivePath;
		CompressionFormatEnum m_compressionFormat;
		CompressionLevelEnum m_compressionLevel;
		size_t m_numberofitems;
		std::vector<TString> m_itemnames;
		std::vector<size_t> m_origsizes;
	private:
		bool pri_GetNumberOfItems();
		bool pri_GetItemsNames();
        bool pri_DetectCompressionFormat(CompressionFormatEnum& format);
	};
}

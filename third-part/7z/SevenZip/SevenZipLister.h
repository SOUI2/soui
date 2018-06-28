#pragma once
#include "SevenZipArchive.h"
#include "CompressionFormat.h"
#include "ListCallback.h"


namespace SevenZip
{
	class SevenZipLister : public SevenZipArchive
	{
	public:

		SevenZipLister();
		virtual ~SevenZipLister();

		virtual bool ListArchive(ListCallback* callback);

	private:
		bool ListArchive(const CMyComPtr< IStream >& archiveStream, ListCallback* callback);
	};
}

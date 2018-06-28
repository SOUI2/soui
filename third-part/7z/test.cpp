// test.cpp : Defines the entry point for the console application.

#include "SevenZip/SevenZipCompressor.h"
#include "SevenZip/SevenZipExtractor.h"
#include "SevenZip/SevenZipExtractorMemory.h"
#include "SevenZip/SevenZipLister.h"
#include "testing/gtest/include/gtest/gtest.h"

using testing::Test;

class all_test_7z : public testing::Test
{
public:
	all_test_7z() {}
	 
	virtual void SetUp()
	{
		//初始化函数		
	}

	virtual	void TearDown()
	{ 
	}  

	void unzip() 
	{
		SevenZip::SevenZipPassword pwd(true, L"123456");
		SevenZip::SevenZipExtractor decompress;
		decompress.SetArchivePath(L"D:\\test.7z");

		HRESULT ret = S_OK;
		if (ret != decompress.ExtractArchive(L"D:\\test", NULL, &pwd))
		{
			wprintf_s(L"decompress 7z to dir failed\n");
		}

		EXPECT_EQ(ret, S_OK);
	}

	void unzipmemory()
	{
		CFileStream fileStreams;
		SevenZip::SevenZipPassword pwd(true, L"123456");
		SevenZip::SevenZipExtractorMemory decompress;
		decompress.SetArchivePath(L"D:\\test.7z");

		HRESULT ret = S_OK;
		if (ret != decompress.ExtractArchive(L"D:\\test", NULL, &pwd, fileStreams))
		{
			wprintf_s(L"decompress 7z to dir failed\n");
		}

		EXPECT_EQ(ret, S_OK);
	}

	void zip()
	{
		SevenZip::SevenZipPassword pwd(true, L"123456");
		SevenZip::SevenZipCompressor compress;
		compress.SetArchivePath(L"D:\\test.7z");
		compress.SetEncryptFileName(true);
		HRESULT ret = S_OK;
		if (ret != compress.CompressFiles(L"D:\\abc1", L"*.*", NULL,true, &pwd))
		{
		       wprintf_s(L"compress dir to 7z failed\n");
		}

		EXPECT_EQ(ret, S_OK);
	}
};


TEST_F(all_test_7z, TestZipFeature)
{
	zip();
}


TEST_F(all_test_7z, TestUnzipFeature)
{
	unzip(); 
}



int main(int argc, char* argv[]) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();

//    
}


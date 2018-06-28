// test.cpp : Defines the entry point for the console application.
//
#include "SevenZip/SevenZipExtractor.h"
#include "SevenZip/SevenZipExtractorMemory.h"
#include "SevenZip/SevenZipLister.h"
#include "testing/gtest/include/gtest/gtest.h"

using testing::Test;

class UnProgressCallback : public SevenZip::ProgressCallback
{
public:

	virtual bool OnFileCount(unsigned int fileCount)
	{ 
		fileCount = fileCount;
		return true; 
	
	};

	virtual bool OnFileItems(std::vector<std::wstring>&itemNames, std::vector<unsigned __int64>&itemSizes)
	{
		unsigned __int64 sz = 0;
		for (unsigned int i = 0; i < itemNames.size(); i++)
		{
			std::wstring a =itemNames[i];
			sz = itemSizes[i];
			(void)a;
			if (sz > 0)
				sz = sz;
			else
				sz = 0;
		}
		return false; 
	};

};

class all_test_7z : public testing::Test
{
public:
	all_test_7z() {}

	void unzip() 
	{
		SevenZip::SevenZipPassword pwd(true, L"wuding");
		UnProgressCallback call;
		SevenZip::SevenZipExtractor decompress;
		decompress.SetArchivePath(L"D:\\test.7z");
		HRESULT ret = S_OK;
		if (ret != decompress.ExtractArchive(L"D:\\abc", &call, &pwd))
		{
			wprintf_s(L"decompress 7z to dir failed %s \n", decompress.GetErrorString().c_str());
		} 

		if (ret != decompress.ExtractArchive(L"D:\\abc", NULL, &pwd))
		{
			wprintf_s(L"decompress 7z to dir failed %s \n", decompress.GetErrorString().c_str());
		}

		EXPECT_EQ(ret, S_OK);
	}
 
	void unzipmeory()
	{
		SevenZip::SevenZipPassword pwd(true, L"wuding");
		CFileStream fileStreams;
		UnProgressCallback call;
		SevenZip::SevenZipExtractorMemory decompress;
		decompress.SetArchivePath(L"D:\\test.7z");
		 

		if (S_OK != decompress.ExtractArchive(fileStreams,&call, &pwd))
		{
			wprintf_s(L"decompress 7z to dir failed %s \n", decompress.GetErrorString().c_str());
		}


		HRESULT ret = decompress.ExtractArchive(fileStreams, NULL, &pwd);
		if (S_OK!=ret)
		{
			wprintf_s(L"decompress 7z to dir failed %s \n", decompress.GetErrorString().c_str());
		}

		std::string sfile = "test\\20131127茶叶包装\\茶叶\\安装光盘02.00.0038820131127_171555\\HELP\\IMAGE\\价格小数.JPG"; 

		unsigned long as = fileStreams.GetFileSize(sfile.c_str());
		(void)as;
		BlobBuffer blob;
		if(fileStreams.GetFile(sfile.c_str(),blob))
			blob.SaveBlobToFile("d:\\aab.jpg");
		
		int count=  fileStreams.GetFileCount();
		unsigned int si = fileStreams.GetAllSize();
		const unsigned char *ptr = fileStreams.GetAllBasePtr();
		(void)count;
		(void)si;
		(void)ptr;

		EXPECT_EQ(ret, S_OK);
	}

};

TEST_F(all_test_7z, TestUnzipFeature)
{
	//unzip(); 
}

TEST_F(all_test_7z, TestUnzipMemoryFeature)
{
	unzipmeory();
}
  
TEST(Test7zunzipTest, unzip)
{
	SevenZip::SevenZipExtractor decompress;
	decompress.SetArchivePath(L"D:\\test.7z");
	HRESULT ret = S_OK;
	if (ret != decompress.ExtractArchive(L"D:\\test", nullptr))
	{
		wprintf_s(L"decompress 7z to dir failed\n");
	}

	EXPECT_EQ(ret, S_OK); 
}
 

int main(int argc, char* argv[]) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();

//    
}


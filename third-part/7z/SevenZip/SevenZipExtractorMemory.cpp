#include "SevenZipExtractorMemory.h"
#include "GUIDs.h"
#include "FileSys.h"
#include "ArchiveOpenCallback.h"
#include "ArchiveExtractCallbackMemory.h"
#include "InStreamWrapper.h"
#include "PropVariant2.h"
#include "UsefulFunctions.h"


namespace SevenZip
{

    using namespace intl;

    SevenZipExtractorMemory::SevenZipExtractorMemory()
        : SevenZipArchive()        
    {
    }

    SevenZipExtractorMemory::~SevenZipExtractorMemory()
    {
    }


	HRESULT SevenZipExtractorMemory::ExtractArchive(CFileStream &fileStreams, ProgressCallback* callback, SevenZipPassword *pSevenZipPassword)
    {
        DetectCompressionFormat();
        CMyComPtr< IStream > fileStream = FileSys::OpenFileToRead(m_archivePath);

        if (fileStream == NULL)
        {
            LPVOID msgBuf;
            if (::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, ERROR_OPEN_FAILED, 0, (LPTSTR)&msgBuf, 0, NULL) == 0)
            {
                m_message = (LPCTSTR)msgBuf;
                ::LocalFree(msgBuf);
            }
            return ERROR_OPEN_FAILED;	//Could not open archive
        }

		return ExtractArchive(fileStreams,fileStream, callback, pSevenZipPassword);
    }

	HRESULT SevenZipExtractorMemory::ExtractArchive(CFileStream &fileStreams,const CMyComPtr< IStream >& archiveStream, ProgressCallback* callback, SevenZipPassword *pSevenZipPassword)
    {
        CMyComPtr< IInArchive > archive = UsefulFunctions::GetArchiveReader(m_compressionFormat);
        CMyComPtr< InStreamWrapper > inFile = new InStreamWrapper(archiveStream);
        CMyComPtr< ArchiveOpenCallback > openCallback = new ArchiveOpenCallback();
		 
		if (NULL != pSevenZipPassword)
		{ 
			openCallback->PasswordIsDefined = pSevenZipPassword->PasswordIsDefined;
			openCallback->Password = pSevenZipPassword->Password.c_str();
		}

        HRESULT hr = archive->Open(inFile, 0, openCallback);
        if (hr != S_OK)
        {
            LPVOID msgBuf;
            if (::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, hr, 0, (LPTSTR)&msgBuf, 0, NULL) == 0)
            {
                m_message = (LPCTSTR)msgBuf;
                ::LocalFree(msgBuf);
            }
            return hr;	//Open archive error
        }

        if (callback)
        {
            UInt32 mynumofitems = 0;
            hr = archive->GetNumberOfItems(&mynumofitems);
            if (callback->OnFileCount(mynumofitems))
            {
                size_t numberofitems = size_t(mynumofitems);
                
                std::vector<std::wstring> itemnames;
                itemnames.reserve(numberofitems);

                std::vector<unsigned __int64> origsizes;
                origsizes.reserve(numberofitems);

                bool succ = true;
                for (UInt32 i = 0; i < numberofitems; ++i)
                {
                    // Get uncompressed size of file
                    CPropVariant prop;
                    hr = archive->GetProperty(i, kpidSize, &prop);
                    if (hr != S_OK)
                    {
                        succ = false;
                        break;
                    }

                    unsigned __int64 size = prop.uhVal.QuadPart;
                    origsizes.push_back(size);

                    // Get name of file
                    hr = archive->GetProperty(i, kpidPath, &prop);
                    if (hr != S_OK)
                    {
                        succ = false;
                        break;
                    }
                    itemnames.push_back(prop.vt == VT_BSTR?prop.bstrVal:L"");
                }

                if (!succ)
                {
                    archive->Close();
                    return E_FAIL;
                    
                }

                if (!callback->OnFileItems(itemnames, origsizes))
                {
                    //只为了取得文件信息,所以直接返回.
                    archive->Close();
                    return S_OK;
                }
            }
        }

		CMyComPtr< ArchiveExtractCallbackMemory > extractCallback = new ArchiveExtractCallbackMemory(archive, callback,fileStreams);
		if (NULL != pSevenZipPassword)
		{
			extractCallback->PasswordIsDefined = pSevenZipPassword->PasswordIsDefined;
			extractCallback->Password = pSevenZipPassword->Password.c_str();
		}

        hr = archive->Extract(NULL, UInt32(-1), false, extractCallback);
        if (hr != S_OK)
        {
            LPVOID msgBuf;
            if (::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, hr, 0, (LPTSTR)&msgBuf, 0, NULL) == 0)
            {
                m_message = (LPCTSTR)msgBuf;
                ::LocalFree(msgBuf);
            }
        }
        if (callback)
        {
            callback->OnEnd(m_archivePath);
        }
        archive->Close();
        return hr;
    }

    const TString& SevenZipExtractorMemory::GetErrorString()
    {
        return m_message;
    }


}


#include "UsefulFunctions.h"
#include <vector>
#include <set>
#include "PropVariant2.h"
#include "../CPP/7zip/ICoder.h"


 
#include "../CPP/7zip/Archive/7z/7zRegister.cpp"
#include "../CPP/7zip/Archive/Cab/CabRegister.cpp"
#include "../CPP/7zip/Archive/Zip/ZipRegister.cpp"
#include "../CPP/7zip/Archive/LzmaHandler.cpp"
// #include "../CPP/7zip/Archive/ZHandler.cpp"
#include "../CPP/7zip/Archive/XzHandler.cpp"

// #include "../CPP/7zip/Archive/Chm/ChmHandler.cpp"
// #include "../CPP/7zip/Archive/Iso/IsoRegister.cpp"
// #include "../CPP/7zip/Archive/Nsis/NsisRegister.cpp"
// #include "../CPP/7zip/Archive/Tar/TarRegister.cpp"
// #include "../CPP/7zip/Archive/Wim/WimRegister.cpp"

// #include "../CPP/7zip/Archive/Udf/UdfHandler.cpp"
// #include "../CPP/7zip/Archive/Rar/RarHandler.cpp"
// #include "../CPP/7zip/Archive/Rar/Rar5Handler.cpp"

// #include "../CPP/7zip/Archive/ApmHandler.cpp"
// #include "../CPP/7zip/Archive/ArHandler.cpp"
// #include "../CPP/7zip/Archive/ArjHandler.cpp"
// #include "../CPP/7zip/Archive/Bz2Handler.cpp"
// #include "../CPP/7zip/Archive/ComHandler.cpp"
// #include "../CPP/7zip/Archive/CpioHandler.cpp"
// #include "../CPP/7zip/Archive/CramfsHandler.cpp"
// #include "../CPP/7zip/Archive/DmgHandler.cpp"
// #include "../CPP/7zip/Archive/ElfHandler.cpp"
// #include "../CPP/7zip/Archive/ExtHandler.cpp"
// #include "../CPP/7zip/Archive/FatHandler.cpp"
// #include "../CPP/7zip/Archive/FlvHandler.cpp"
// #include "../CPP/7zip/Archive/GptHandler.cpp"
// #include "../CPP/7zip/Archive/GzHandler.cpp"
// #include "../CPP/7zip/Archive/HfsHandler.cpp"
// #include "../CPP/7zip/Archive/IhexHandler.cpp"
// #include "../CPP/7zip/Archive/LzhHandler.cpp"
// #include "../CPP/7zip/Archive/MachoHandler.cpp"
// #include "../CPP/7zip/Archive/MbrHandler.cpp"
// #include "../CPP/7zip/Archive/MslzHandler.cpp"
// #include "../CPP/7zip/Archive/MubHandler.cpp"
// #include "../CPP/7zip/Archive/NtfsHandler.cpp"
// #include "../CPP/7zip/Archive/PeHandler.cpp"
// #include "../CPP/7zip/Archive/PpmdHandler.cpp"
// #include "../CPP/7zip/Archive/QcowHandler.cpp"
// #include "../CPP/7zip/Archive/RpmHandler.cpp"
// #include "../CPP/7zip/Archive/SplitHandler.cpp"
// #include "../CPP/7zip/Archive/SquashfsHandler.cpp"
// #include "../CPP/7zip/Archive/SwfHandler.cpp"
// #include "../CPP/7zip/Archive/UefiHandler.cpp"
// #include "../CPP/7zip/Archive/VdiHandler.cpp"
// #include "../CPP/7zip/Archive/VhdHandler.cpp"
// #include "../CPP/7zip/Archive/VmdkHandler.cpp"
// #include "../CPP/7zip/Archive/XarHandler.cpp"

 
#include "../CPP/7zip/Compress/ByteSwap.cpp"
#include "../CPP/7zip/Compress/CopyRegister.cpp"
#include "../CPP/7zip/Compress/Lzma2Register.cpp"
#include "../CPP/7zip/Compress/LzmaRegister.cpp"
 
#include "../CPP/7zip/Compress/bcj2Register.cpp"
#include "../CPP/7zip/Compress/BcjRegister.cpp"

// #include "../CPP/7zip/Compress/BranchRegister.cpp"
// #include "../CPP/7zip/Compress/PpmdRegister.cpp"
// #include "../CPP/7zip/Compress/BZip2Register.cpp"
#include "../CPP/7zip/Compress/RarCodecsRegister.cpp"

#include "../CPP/7zip/Compress/DeflateRegister.cpp"
//#include "../CPP/7zip/Compress/DeflateRegister.cpp"
#include "../CPP/7zip/Compress/DeltaFilter.cpp"


#include "../CPP/7zip/Crypto/7zAesRegister.cpp"
#include "../CPP/7zip/Crypto/MyAesReg.cpp"




#include "../CPP/Common/CrcReg.cpp"
// #include "../CPP/Common/Sha1Reg.cpp"
// #include "../CPP/Common/Sha256Reg.cpp"
// #include "../CPP/Common/XzCrc64Reg.cpp"

STDAPI CreateObject(const GUID *clsid, const GUID *iid, void **outObject);

namespace SevenZip
{
    using namespace intl;

    const GUID* UsefulFunctions::GetCompressionGUID(const CompressionFormatEnum& format)
    {
        const GUID* guid = NULL;

        switch (format)
        {
        case CompressionFormat::Zip:
            guid = &SevenZip::intl::CLSID_CFormatZip;
            break;

//         case CompressionFormat::GZip:
//             guid = &SevenZip::intl::CLSID_CFormatGZip;
//             break;
// 
//         case CompressionFormat::BZip2:
//             guid = &SevenZip::intl::CLSID_CFormatBZip2;
//             break;
// 
//         case CompressionFormat::Rar:
//             guid = &SevenZip::intl::CLSID_CFormatRar;
//             break;
// 
//         case CompressionFormat::Tar:
//             guid = &SevenZip::intl::CLSID_CFormatTar;
//             break;
// 
//         case CompressionFormat::Iso:
//             guid = &SevenZip::intl::CLSID_CFormatIso;
//             break;

        case CompressionFormat::Cab:
            guid = &SevenZip::intl::CLSID_CFormatCab;
            break;

        case CompressionFormat::Lzma:
            guid = &SevenZip::intl::CLSID_CFormatLzma;
            break;

        case CompressionFormat::Lzma86:
            guid = &SevenZip::intl::CLSID_CFormatLzma86;
            break;

        default:
            guid = &SevenZip::intl::CLSID_CFormat7z;
            break;
        }
        return guid;
    }

    void RegisterArchive()
    {
        static bool inited = false;
        if (!inited)
        {
            RegisterArc(&NArchive::NCab::g_ArcInfo);
            RegisterArc(&NArchive::N7z::g_ArcInfo);
            RegisterArc(&NArchive::NLzma::NLzmaAr::g_ArcInfo);
            RegisterArc(&NArchive::NLzma::NLzma86Ar::g_ArcInfo);
            RegisterArc(&NArchive::NZip::g_ArcInfo);

//          RegisterArc(&NArchive::NRar::g_ArcInfo);
//          RegisterArc(&NArchive::NBz2::g_ArcInfo);
//          RegisterArc(&NArchive::NGz::g_ArcInfo);
//          RegisterArc(&NArchive::NTar::g_ArcInfo);
//          RegisterArc(&NArchive::NIso::g_ArcInfo);

//          RegisterArc(&NArchive::NSplit::g_ArcInfo);
//          RegisterArc(&NArchive::NXz::g_ArcInfo);
//          RegisterArc(&NArchive::NZ::g_ArcInfo);
//          RegisterArc(&NArchive::NWim::g_ArcInfo);
//          RegisterArc(&NArchive::NUdf::g_ArcInfo);
//          RegisterArc(&NArchive::NChm::NHxs::g_ArcInfo);
//          RegisterArc(&NArchive::NChm::NChm::g_ArcInfo);
//          RegisterArc(&NArchive::NNsis::g_ArcInfo);
            
			RegisterCodec(&NCompress::g_CodecInfo);
			RegisterCodec(&NCompress::g_CodecInfo);
			RegisterCodec(&NCompress::NLzma2::g_CodecInfo);
			RegisterCodec(&NCompress::NLzma::g_CodecInfo); 
			
//          RegisterCodec(&NCompress::NPpmd::g_CodecInfo);
//          RegisterCodec(&NCompress::NBZip2::g_CodecInfo);
			RegisterCodec(&NCompress::NDeflate::g_CodecInfo);

			RegisterCodec(&NCompress::NByteSwap::g_CodecsInfo[0]);
			RegisterCodec(&NCompress::NByteSwap::g_CodecsInfo[1]);

//          RegisterCodec(&NCompress::NBranch::g_CodecsInfo[0]);
//          RegisterCodec(&NCompress::NBranch::g_CodecsInfo[1]);
//          RegisterCodec(&NCompress::NBranch::g_CodecsInfo[2]);
//          RegisterCodec(&NCompress::NBranch::g_CodecsInfo[3]);
//          RegisterCodec(&NCompress::NBranch::g_CodecsInfo[4]);

//          RegisterCodec(&NCompress::g_CodecsInfo[0]);
//          RegisterCodec(&NCompress::g_CodecsInfo[1]);
//          RegisterCodec(&NCompress::g_CodecsInfo[2]);
//          RegisterCodec(&NCompress::g_CodecsInfo[3]);

			RegisterCodec(&NCrypto::N7z::g_CodecInfo);
			RegisterCodec(&NCrypto::g_CodecInfo);

            inited = true;
            CrcGenerateTable();
        }
    }


    CMyComPtr< IInArchive > UsefulFunctions::GetArchiveReader(const CompressionFormatEnum& format)
    {
        RegisterArchive();
        CMyComPtr<IInArchive> archive;
        const GUID* guid = GetCompressionGUID(format);
        CreateObject(guid, &IID_IInArchive, reinterpret_cast<void**>(&archive));
        return archive;
    }

    CMyComPtr< IOutArchive > UsefulFunctions::GetArchiveWriter(const CompressionFormatEnum& format)
    {
        RegisterArchive();
        CMyComPtr<IOutArchive> archive;
        const GUID* guid = GetCompressionGUID(format);
        CreateObject(guid, &IID_IOutArchive, reinterpret_cast<void**>(&archive));
        return archive;
    }

    bool UsefulFunctions::GetNumberOfItems(const TString & archivePath,
        CompressionFormatEnum &format, size_t & numberofitems)
    {
        CMyComPtr< IStream > fileStream = FileSys::OpenFileToRead(archivePath);

        if (fileStream == NULL)
        {
            return false;
            //throw SevenZipException( StrFmt( _T( "Could not open archive \"%s\"" ), m_archivePath.c_str() ) );
        }

        CMyComPtr< IInArchive > archive = UsefulFunctions::GetArchiveReader(format);
        CMyComPtr< InStreamWrapper > inFile = new InStreamWrapper(fileStream);
        CMyComPtr< ArchiveOpenCallback > openCallback = new ArchiveOpenCallback();

        HRESULT hr = archive->Open(inFile, 0, openCallback);
        if (hr != S_OK)
        {
            return false;
            //throw SevenZipException( GetCOMErrMsg( _T( "Open archive" ), hr ) );
        }

        UInt32 mynumofitems;
        hr = archive->GetNumberOfItems(&mynumofitems);
        if (hr != S_OK)
        {
            return false;
            //throw SevenZipException( GetCOMErrMsg( _T( "Open archive" ), hr ) );
        }
        numberofitems = size_t(mynumofitems);

        archive->Close();
        return true;
    }

    bool UsefulFunctions::GetItemsNames(const TString & archivePath,
        CompressionFormatEnum &format, size_t & numberofitems,
        std::vector<TString> & itemnames, std::vector<size_t> & origsizes)
    {
        CMyComPtr< IStream > fileStream = FileSys::OpenFileToRead(archivePath);

        if (fileStream == NULL)
        {
            return false;
            //throw SevenZipException( StrFmt( _T( "Could not open archive \"%s\"" ), m_archivePath.c_str() ) );
        }

        CMyComPtr< IInArchive > archive = UsefulFunctions::GetArchiveReader(format);
        CMyComPtr< InStreamWrapper > inFile = new InStreamWrapper(fileStream);
        CMyComPtr< ArchiveOpenCallback > openCallback = new ArchiveOpenCallback();

        HRESULT hr = archive->Open(inFile, 0, openCallback);
        if (hr != S_OK)
        {
            return false;
            //throw SevenZipException( GetCOMErrMsg( _T( "Open archive" ), hr ) );
        }

        UInt32 mynumofitems;
        hr = archive->GetNumberOfItems(&mynumofitems);
        if (hr != S_OK)
        {
            return false;
            //throw SevenZipException( GetCOMErrMsg( _T( "Open archive" ), hr ) );
        }
        numberofitems = size_t(mynumofitems);

        itemnames.clear();
        itemnames.resize(numberofitems);

        origsizes.clear();
        origsizes.resize(numberofitems);

        for (UInt32 i = 0; i < numberofitems; i++)
        {
            {
                // Get uncompressed size of file
                CPropVariant prop;
                hr = archive->GetProperty(i, kpidSize, &prop);
                if (hr != S_OK)
                {
                    return false;
                    //throw SevenZipException( GetCOMErrMsg( _T( "Open archive" ), hr ) );
                }

                int size = prop.intVal;
                origsizes[i] = size_t(size);

                // Get name of file
                hr = archive->GetProperty(i, kpidPath, &prop);
                if (hr != S_OK)
                {
                    return false;
                    //throw SevenZipException( GetCOMErrMsg( _T( "Open archive" ), hr ) );
                }

                //valid string? pass back the found value and call the callback function if set
                if (prop.vt == VT_BSTR) {
                    WCHAR* path = prop.bstrVal;
                    std::wstring mypath(path);
                    itemnames[i] = mypath;
                }
            }
        }

        archive->Close();
        return true;
    }

    bool UsefulFunctions::DetectCompressionFormat(const TString & archivePath,
        CompressionFormatEnum & archiveCompressionFormat)
    {
        CMyComPtr< IStream > fileStream = FileSys::OpenFileToRead(archivePath);

        if (fileStream == NULL)
        {
            return false;
            //throw SevenZipException( StrFmt( _T( "Could not open archive \"%s\"" ), m_archivePath.c_str() ) );
        }

        std::vector<CompressionFormatEnum> myAvailableFormats;
        myAvailableFormats.reserve(12);

        //use default format as first.
        myAvailableFormats.push_back(archiveCompressionFormat);

        // Add more formats here if 7zip supports more formats in the future
        myAvailableFormats.push_back(CompressionFormat::SevenZip);
        myAvailableFormats.push_back(CompressionFormat::Lzma86);
        myAvailableFormats.push_back(CompressionFormat::Lzma);
        myAvailableFormats.push_back(CompressionFormat::Cab);
        myAvailableFormats.push_back(CompressionFormat::Zip);
//         myAvailableFormats.push_back(CompressionFormat::Rar);
//         myAvailableFormats.push_back(CompressionFormat::BZip2);
//         myAvailableFormats.push_back(CompressionFormat::Tar);
//         myAvailableFormats.push_back(CompressionFormat::Iso);

        for (size_t i = myAvailableFormats.size(); i > 0; --i)             {
            if (myAvailableFormats[i - 1] == archiveCompressionFormat) {
                myAvailableFormats.erase(myAvailableFormats.begin()+(i-1));
                break;
            }
        }

        // Check each format for one that works
        CMyComPtr< ArchiveOpenCallback > openCallback = new ArchiveOpenCallback();
        
        for (size_t i =0; i< myAvailableFormats.size(); ++i)
        {
            archiveCompressionFormat = myAvailableFormats[i];

            CMyComPtr< IInArchive > archive = UsefulFunctions::GetArchiveReader(archiveCompressionFormat);

			CMyComPtr< InStreamWrapper > inFile = new InStreamWrapper(fileStream);
			inFile->Seek(0, STREAM_SEEK_SET, NULL);
            UInt64 max_check_start_pos = 0; //允许偏移头部一定距离查找压缩包头部.对于单纯的压缩包文件而言,设置为0即可.
            HRESULT hr = archive->Open(inFile, &max_check_start_pos, openCallback);
            if (hr == S_OK)
            {
                archive->Close();
                return true;
            }

            archive->Close();
        }

//         myAvailableFormats.push_back(CompressionFormat::GZip);
        //
        //  There is a problem that GZip files will not be detected using the above method.
        //  This is a fix.
        //
//         if (true)
//         {
//             size_t myNumOfItems = 0;
//             archiveCompressionFormat = CompressionFormat::GZip;
//             bool result = GetNumberOfItems(archivePath, archiveCompressionFormat, myNumOfItems);
//             if (result == true && myNumOfItems > 0)
//             {
//                 // We know this file is a GZip file, so return
//                 return true;
//             }
//         }

        // If you get here, the format is unknown
        archiveCompressionFormat = CompressionFormat::Unknown;
        return false;
    }

    const TString UsefulFunctions::EndingFromCompressionFormat(const CompressionFormatEnum& format)
    {
        switch (format)
        {
        case CompressionFormat::Zip:
            return L".zip";
            break;
		case CompressionFormat::SevenZip:
			return L".7z";
			break;
//         case CompressionFormat::Rar:
//             return L".rar";
//             break;
//         case CompressionFormat::GZip:
//             return L".gz";
//             break;
//         case CompressionFormat::BZip2:
//             return L".bz";
//             break;
//         case CompressionFormat::Tar:
//             return L".tar";
//             break;
        case CompressionFormat::Lzma:
            return L".lzma";
            break;
        case CompressionFormat::Lzma86:
            return L".lzma86";
            break;
        case CompressionFormat::Cab:
            return L".cab";
            break;
//         case CompressionFormat::Iso:
//             return L".iso";
            break;
        }
        return L".7z";
    }

}


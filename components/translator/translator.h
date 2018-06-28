// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TRANSLATOR_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TRANSLATOR_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#include <souicoll.h>
#include <unknown/obj-ref-impl.hpp>
#include <string/tstring.h>
#include <string/strcpcvt.h>
#include <pugixml/pugixml.hpp>
#include <interface/STranslator-i.h>

namespace SOUI
{
    class SStrMap;
    class SStrMapEntry;

    enum LANGDATA{
        LD_UNKNOWN=0,
        LD_XML,
        LD_COMPILEDFILE,
        LD_COMPILEDDATA,
    };

		struct TrFontInfo{
			BYTE lfCharset;
			SStringT strFaceName;
		};


    class STranslator : public TObjRefImpl<ITranslator>
    {
        friend class STranslatorMgr;

    public:
        STranslator();
        ~STranslator();

        virtual BOOL Load(LPVOID pData,UINT uType);

        virtual SStringW name();
        virtual GUID     guid();
        virtual BOOL tr(const SStringW & strSrc,const SStringW & strCtx,SStringW & strRet);
		virtual BOOL updateLogfont(const SStringW & strName,LOGFONT *pFont);
		virtual BYTE charsetFromString(const SStringW & strCharset) const;
    protected:
        BOOL LoadFromXml(pugi::xml_node xmlLang);



        SStringW m_strLang;
        GUID     m_guid;
        SArray<SStrMapEntry*> * m_arrEntry;
		SMap<SStringW,TrFontInfo> m_mapFonts;//字体替换信息
    };

    class STranslatorMgr : public TObjRefImpl<ITranslatorMgr>
    {
    public:
        STranslatorMgr(void);
        ~STranslatorMgr(void);

		virtual void SetLanguage(const SStringW & strLang);
		virtual SStringW GetLanguage() const;

        BOOL CreateTranslator(ITranslator ** ppTranslator);

		BOOL InstallTranslator(ITranslator *pTranslator);

		BOOL UninstallTranslator(REFGUID id);

		SStringW tr(const SStringW & strSrc,const SStringW & strCtx);

		BOOL updateLogfont(const SStringW & strName,LOGFONT * pfont);


	protected:

		SStringW m_strLang;

        SList<ITranslator*> *m_lstLang;

	};

    namespace TRANSLATOR
    {
        SOUI_COM_C BOOL SOUI_COM_API SCreateInstance(IObjRef **ppTrans);
    }

}

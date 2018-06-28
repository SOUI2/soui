#if !defined(AFX_OLERICHEDITCTRL_H__3DFF15EE_7336_4297_9620_7F00B611DAA1__INCLUDED_)
#define AFX_OLERICHEDITCTRL_H__3DFF15EE_7336_4297_9620_7F00B611DAA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <richole.h>

#include "../smiley/ssmileyctrl.h"
#include <control/srichedit.h>

using namespace SOUI;

#define ID_INVALID  -1
class ImageID
{
public:
    ImageID():m_uID(ID_INVALID){}
    
    UINT      m_uID;
    SStringW m_strFilename;
    
    ImageID & operator = (const ImageID & src)
    {
        m_strFilename = src.m_strFilename;
        m_uID = src.m_uID;
        return *this;
    }

    bool operator == (const ImageID & src) const
    {
        return m_uID == src.m_uID && m_strFilename.CompareNoCase(src.m_strFilename)==0;
    }

    bool operator < (const ImageID & src) const
    {
        return m_uID<src.m_uID || m_strFilename.CompareNoCase(src.m_strFilename)<0;
    }
};

namespace SOUI
{
    template<>
    class CElementTraits<ImageID > :
        public CElementTraitsBase<ImageID >
    {
    public:
        static ULONG Hash( INARGTYPE imgid )
        {
            ULONG_PTR uRet=0;
            if(imgid.m_uID!=ID_INVALID) return imgid.m_uID;
            return SStringElementTraits<SStringW>::Hash(imgid.m_strFilename)+10000;
        }

        static bool CompareElements( INARGTYPE element1, INARGTYPE element2 )
        {
            return element1.m_uID == element2.m_uID
                && element1.m_strFilename == element2.m_strFilename;
        }

        static int CompareElementsOrdered( INARGTYPE element1, INARGTYPE element2 )
        {
            int nRet = (int)element1.m_uID - (int)element2.m_uID;
            if(nRet==0) nRet= element1.m_strFilename.Compare(element2.m_strFilename);
            return nRet;
        }
    };

}

class ImageItem
{  
private:
    HDC          m_hMemDC;
    UINT*        m_pFrameDelays;
    int          m_nFrameCount;
    CSize         m_FrameSize;

    ImageID     m_imgid;
    int          m_nRef;  
public:
    ImageItem();
    ~ImageItem();
    BOOL LoadImage(const ImageID & imgid);
    bool IsEqual(const ImageID & id) 
    {return m_imgid == id; }
    ImageID GetImageID()
    {return m_imgid;}

    void AddRef()
    {m_nRef++;}
    int Release();

    UINT * GetFrameDelays() 
    { return m_pFrameDelays;} 
    UINT GetFrameCount()    
    { return m_nFrameCount; }
    CSize GetFrameSize() 
    { return m_FrameSize; }
    
    void Draw(HDC hdc,LPCRECT pRect,int iFrame);
};

class CSmileySource : public ISmileySource
{
public:
    CSmileySource();
    virtual ~CSmileySource();

    static bool GdiplusStartup(void);

    static void GdiplusShutdown(void);
    
protected:
    //获对ID对应的图片路径
    virtual SStringW ImageID2Path(UINT nID);
    
public:
    virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
    {
        return E_NOINTERFACE;
    }

    virtual ULONG STDMETHODCALLTYPE AddRef( void)
    { return ++m_cRef;}

    virtual ULONG STDMETHODCALLTYPE Release( void)
    {
        int nRet = --m_cRef;
        if(m_cRef == 0)
        {
            delete this;
        }
        return nRet;
    }

private:
    LONG m_cRef;
public:
    virtual HRESULT STDMETHODCALLTYPE Stream_Load( 
        /* [in] */ LPSTREAM pStm);

    virtual HRESULT STDMETHODCALLTYPE Stream_Save( 
        /* [in] */ LPSTREAM pStm);

    virtual HRESULT STDMETHODCALLTYPE LoadFromID( 
        /* [in] */ UINT uID);

    virtual HRESULT STDMETHODCALLTYPE LoadFromFile( 
        /* [in] */ LPCWSTR pszFilePath);
        
    virtual HRESULT STDMETHODCALLTYPE GetID( 
        /* [out] */ UINT *pID);

    virtual HRESULT STDMETHODCALLTYPE GetFile( 
        /* [out] */ BSTR *bstrFile);

    virtual HRESULT STDMETHODCALLTYPE GetFrameCount( 
        /* [out] */ int *pFrameCount);

    virtual HRESULT STDMETHODCALLTYPE GetFrameDelay( 
        /* [in] */ int iFrame,
        /* [out] */ int *pFrameDelay);

    virtual HRESULT STDMETHODCALLTYPE GetSize( 
        /* [out] */ LPSIZE pSize);

    virtual HRESULT STDMETHODCALLTYPE Draw( 
        /* [in] */ HDC hdc,
        /* [in] */ LPCRECT pRect,
        /* [in] */ int iFrameIndex);


protected:    

    HRESULT Init(const ImageID &imgid);

    ImageItem    * m_pImg;

    typedef SMap<ImageID, ImageItem* > IMAGEPOOL;
    static IMAGEPOOL   s_imgPool;
    static ULONG_PTR   s_gdiPlusToken;
};

typedef ISmileySource * (* FunCreateSource)();

class CRichEditOleCallback : public IRichEditOleCallback
{
public:
    CRichEditOleCallback(ISmileyHost *pSmileyHost);

public:
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject);
    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();
public:
    virtual HRESULT STDMETHODCALLTYPE GetNewStorage(LPSTORAGE* lplpstg);
    virtual HRESULT STDMETHODCALLTYPE GetInPlaceContext(LPOLEINPLACEFRAME FAR *lplpFrame,
        LPOLEINPLACEUIWINDOW FAR *lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo);
    virtual HRESULT STDMETHODCALLTYPE ShowContainerUI(BOOL fShow);
    virtual HRESULT STDMETHODCALLTYPE QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp);
    virtual HRESULT STDMETHODCALLTYPE DeleteObject(LPOLEOBJECT lpoleobj);
    virtual HRESULT STDMETHODCALLTYPE QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR *lpcfFormat,
        DWORD reco, BOOL fReally, HGLOBAL hMetaPict);
    virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode);
    virtual HRESULT STDMETHODCALLTYPE GetClipboardData(CHARRANGE FAR *lpchrg, DWORD reco, LPDATAOBJECT FAR *lplpdataobj);
    virtual HRESULT STDMETHODCALLTYPE GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect);
    virtual HRESULT STDMETHODCALLTYPE GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE FAR *lpchrg,
        HMENU FAR *lphmenu);

protected:
    virtual ~CRichEditOleCallback();

    SComPtr<IStorage>   m_stg;
    int                 m_iStorage;

    DWORD m_dwRef;
    ISmileyHost *       m_pSmileyHost;
};

class CSmileyHost : public ISmileyHost , public ITimelineHandler
{
public:
    CSmileyHost(SRichEdit *pRichedit,FunCreateSource pCreateSource);
    ~CSmileyHost();

public://ISmileyHost
    virtual HRESULT STDMETHODCALLTYPE SendMessage( 
        /* [in] */ UINT uMsg,
        /* [in] */ WPARAM wParam,
        /* [in] */ LPARAM lParam,
        /* [out] */ LRESULT *pRet);

    virtual HRESULT STDMETHODCALLTYPE GetHostRect( 
        /* [out] */ LPRECT prcHost);

//     virtual HRESULT STDMETHODCALLTYPE InvalidateRect( LPCRECT pRect);

//     virtual HRESULT STDMETHODCALLTYPE InvalidateRect( 
//         /* [in] */ LPCRECT pRect);

    virtual HRESULT STDMETHODCALLTYPE CreateSource(
        /* [in,out] */ ISmileySource ** ppSource);

    virtual HRESULT STDMETHODCALLTYPE SetTimer(
        /* [in] */ ITimerHandler * pTimerHander,
        /* [in] */ int nInterval);

    virtual HRESULT STDMETHODCALLTYPE KillTimer(
        /* [in] */ ITimerHandler * pTimerHander);

    virtual HRESULT STDMETHODCALLTYPE  OnTimer(int nInterval);

    virtual HRESULT STDMETHODCALLTYPE  ClearTimer();
    virtual HRESULT STDMETHODCALLTYPE InvalidateRect(LPCRECT pRect);

public://ITimelineHandler
    virtual void OnNextFrame(){OnTimer(10);}
public:
    bool OnHostUpdate(SOUI::EventArgs *pEvt);
    bool OnHostVisibleChanged(SOUI::EventArgs *pEvt);

protected:

    struct TIMERINFO
    {
        TIMERINFO(ITimerHandler *_p,int _nInterval)
            :pHandler(_p),nInterval(_nInterval)
        {}
        ITimerHandler * pHandler;
        int nInterval;
    };

    typedef SList<TIMERINFO*> TIMERHANDLER_LIST;
    TIMERHANDLER_LIST m_lstTimerInfo;

    SRichEdit * m_pHost;
    int         m_cTime;

    FunCreateSource m_pCreateSource;
};



BOOL SetSRicheditOleCallback(SRichEdit *pRichedit,FunCreateSource pCreateSource = NULL);

#endif // !defined(AFX_OLERICHEDITCTRL_H__3DFF15EE_7336_4297_9620_7F00B611DAA1__INCLUDED_)

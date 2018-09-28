// RichEditOle.cpp : implementation file
//
#include "stdafx.h"
#include "RichEditOle.h"
#include <atl.mini/SComHelper.h>
#include <gdialpha.h>
#include <GdiPlus.h>
#pragma comment(lib,"gdiplus")

#include "dataobject.h"

//////////////////////////////////////////////////////////////////////////
//  ImageItem

using namespace Gdiplus;

HBITMAP CreateGDIBitmap(HDC hdc, int nWid,int nHei )
{
    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = nWid;
    bmi.bmiHeader.biHeight      = -nHei; // top-down image 
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage   = 0;

    HBITMAP hBmp=CreateDIBSection(hdc,&bmi,DIB_RGB_COLORS,NULL,0,0);
    return hBmp;
}

HBITMAP GetBitmapFromFile(const SStringW& strFilename, 
                       int& nFrameCount, CSize& szImg,
                       UINT* &pFrameDelays )
{
    Gdiplus::Bitmap * bmpSrc= new Gdiplus::Bitmap((LPCWSTR)strFilename);    
    if (!bmpSrc) return NULL;
    GUID   pageGuid = FrameDimensionTime;
    // Get the number of frames in the first dimension.
    nFrameCount = max(1, bmpSrc->GetFrameCount(&pageGuid));


    CSize imSize(bmpSrc->GetWidth(),bmpSrc->GetHeight());
    szImg=imSize;
    
    HDC hdc = GetDC(NULL);
    HDC hMemDC = CreateCompatibleDC(hdc);
    HBITMAP hBmp = CreateGDIBitmap(hdc,szImg.cx*nFrameCount, szImg.cy);
    SelectObject(hMemDC,hBmp);

    Graphics *g = new Gdiplus::Graphics(hMemDC);

    g->Clear(Color(0));
    if (nFrameCount>1)
    {
        pFrameDelays=new UINT[nFrameCount];
        int nSize = bmpSrc->GetPropertyItemSize(PropertyTagFrameDelay);
        // Allocate a buffer to receive the property item.
        PropertyItem* pDelays = (PropertyItem*) new char[nSize];
        bmpSrc->GetPropertyItem(PropertyTagFrameDelay, nSize, pDelays);
        for (int i=0; i<nFrameCount; i++)
        {
            GUID pageGuid = FrameDimensionTime;
            bmpSrc->SelectActiveFrame(&pageGuid, i);
            Rect rect(i*szImg.cx,0,szImg.cx, szImg.cy);
            g->DrawImage(bmpSrc,rect,0,0,bmpSrc->GetWidth(),bmpSrc->GetHeight(), UnitPixel/*, &attr*/);
            pFrameDelays[i]=10*max(((int*) pDelays->value)[i], 10);
        }   
        delete [] pDelays;
    }
    else
    {
        Rect rect(0,0,szImg.cx, szImg.cy);
        g->DrawImage(bmpSrc,rect,0,0,bmpSrc->GetWidth(),bmpSrc->GetHeight(), UnitPixel);
        pFrameDelays=NULL;
    }
    delete g;
    delete bmpSrc;
    DeleteDC(hMemDC);
    ReleaseDC(NULL,hdc);
    return hBmp;
}


ImageItem::ImageItem() : 
m_nRef( 0 ), 
m_hMemDC( NULL ),
m_nFrameCount( 0 ),
m_pFrameDelays( NULL )
{

}
ImageItem::~ImageItem()
{
    if(m_hMemDC)
    {
        HBITMAP hBmp = (HBITMAP)::GetCurrentObject(m_hMemDC,OBJ_BITMAP);
        DeleteDC(m_hMemDC);
        DeleteObject(hBmp);
        m_hMemDC=NULL;
    }
    if ( m_pFrameDelays ) delete [] m_pFrameDelays;
}

BOOL ImageItem::LoadImage(const ImageID & imgid)
{
    ATLASSERT(m_hMemDC == NULL);
    m_imgid = imgid;

    HBITMAP hBmp = GetBitmapFromFile(imgid.m_strFilename, m_nFrameCount, m_FrameSize, m_pFrameDelays );
    if(!hBmp) return FALSE;
    HDC hDC = ::GetDC(NULL);
    m_hMemDC = CreateCompatibleDC(hDC);
    ::SelectObject(m_hMemDC,hBmp);
    ReleaseDC(NULL,hDC);
    return TRUE;
}

int ImageItem::Release()
{
    int nRef = --m_nRef;
    if (nRef < 1)
    {
        delete this;
    }
    return nRef;
}

void ImageItem::Draw( HDC hdc,LPCRECT pRect,int iFrame )
{
    if(m_hMemDC)
    {
        BLENDFUNCTION bf={ AC_SRC_OVER,0,0xFF,AC_SRC_ALPHA};
        AlphaBlend(hdc,pRect->left,pRect->top,pRect->right-pRect->left,pRect->bottom-pRect->top,
                m_hMemDC,m_FrameSize.cx*iFrame,0,m_FrameSize.cx,m_FrameSize.cy,
                bf);
    }
}
/////////////////////////////////////////////////////////////////////////////
ULONG_PTR   CSmileySource::s_gdiPlusToken    = 0;
CSmileySource::IMAGEPOOL CSmileySource::s_imgPool;


bool CSmileySource::GdiplusStartup( void )
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;

    return Gdiplus::Ok == Gdiplus::GdiplusStartup(&s_gdiPlusToken, &gdiplusStartupInput, NULL);
}

void CSmileySource::GdiplusShutdown( void )
{
    if (s_gdiPlusToken != 0)
    {
        Gdiplus::GdiplusShutdown(s_gdiPlusToken);
        s_gdiPlusToken = 0;
    }
}

CSmileySource::CSmileySource():m_pImg(NULL),m_cRef(1)
{

}

CSmileySource::~CSmileySource()
{
    if(m_pImg)
    {
        ImageID id = m_pImg->GetImageID();
        if(m_pImg->Release()==0)
        {
            s_imgPool.RemoveKey(id);
        }
    }
}


HRESULT CSmileySource::Stream_Load( /* [in] */ LPSTREAM pStm )
{
    UINT uID;
    pStm->Read(&uID,4,NULL);
    
    int nFileLen=0;
    pStm->Read(&nFileLen,4,NULL);
    wchar_t *pszFileName=new wchar_t[nFileLen+1];
    pStm->Read(pszFileName,nFileLen*2,NULL);
    pszFileName[nFileLen]=0;
    
    HRESULT hr = S_FALSE;
    if(uID != -1) hr = LoadFromID(uID);
    else hr = LoadFromFile(pszFileName);
    
    delete []pszFileName;
    return hr;
}

HRESULT CSmileySource::Stream_Save( /* [in] */ LPSTREAM pStm )
{
    if(!m_pImg) return E_FAIL;
    ImageID id = m_pImg->GetImageID();
    pStm->Write(&id.m_uID,4,NULL);
    int nFileLen = id.m_strFilename.GetLength();
    pStm->Write(&nFileLen,4,NULL);
    pStm->Write((LPCWSTR)id.m_strFilename,nFileLen*2,NULL);

    return S_OK;
}

HRESULT CSmileySource::GetFrameCount( /* [out] */ int *pFrameCount )
{
    if(!m_pImg) return E_FAIL;
    *pFrameCount = m_pImg->GetFrameCount();
    return S_OK;
}

HRESULT CSmileySource::GetFrameDelay( /* [in] */ int iFrame, /* [out] */ int *pFrameDelay )
{
    if(!m_pImg || m_pImg->GetFrameCount()<=1) return E_FAIL;
    if(iFrame>=(int)m_pImg->GetFrameCount()) return E_INVALIDARG;
    *pFrameDelay = m_pImg->GetFrameDelays()[iFrame];
    return S_OK;
}


HRESULT CSmileySource::GetSize( /* [out] */ LPSIZE pSize )
{
    if(!m_pImg) return E_FAIL;
    *pSize = m_pImg->GetFrameSize();
    return S_OK;
}

HRESULT CSmileySource::Draw( /* [in] */ HDC hdc, /* [in] */ LPCRECT pRect , int iFrameIndex)
{
    if(!m_pImg) return E_FAIL;
    m_pImg->Draw(hdc,pRect,iFrameIndex);
    return S_OK;
}

SOUI::SStringW CSmileySource::ImageID2Path(UINT nID)
{
    return L"";
}

HRESULT CSmileySource::Init(const ImageID &imgid)
{    
    if(m_pImg)
    {
        if(!m_pImg->IsEqual(imgid))
        {//设置新图
            ImageID oldID = m_pImg->GetImageID();
            if(m_pImg->Release() ==0 )
                s_imgPool.RemoveKey(oldID);
            m_pImg = NULL;
        }else
        {//相同的图，直接返回
            return S_OK;
        }
    }
    
    SPOSITION pos = s_imgPool.Lookup(imgid);
    
    if(!pos)
    {//在pool中没有找到
        ImageItem *pImg = new ImageItem;
        if(!pImg->LoadImage(imgid))
        {
            delete pImg;
            return E_INVALIDARG;
        }
        s_imgPool[imgid] = pImg;
        m_pImg = pImg;
    }else
    {
        m_pImg = s_imgPool.GetAt(pos)->m_value;
    }
    m_pImg->AddRef();
    return S_OK;
}


HRESULT STDMETHODCALLTYPE CSmileySource::LoadFromID(/* [in] */ UINT uID)
{
    SStringW strFileName = ImageID2Path(uID);
    if(strFileName.IsEmpty()) return E_INVALIDARG;
    ImageID imgID;
    imgID.m_uID = uID;
    imgID.m_strFilename = strFileName;
    return Init(imgID);
}

HRESULT STDMETHODCALLTYPE CSmileySource::LoadFromFile(/* [in] */ LPCWSTR pszFilePath)
{
    ImageID imgID;
    imgID.m_uID = ID_INVALID;
    imgID.m_strFilename =pszFilePath;
    return Init(imgID);
}


HRESULT STDMETHODCALLTYPE CSmileySource::GetID(/* [out] */ UINT *pID)
{
    if(!m_pImg) return E_FAIL;
    if(m_pImg->GetImageID().m_uID == ID_INVALID) return E_FAIL;
    *pID = m_pImg->GetImageID().m_uID;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CSmileySource::GetFile(/* [out] */ BSTR *bstrFile)
{
    if(!m_pImg) return E_FAIL;
    *bstrFile = ::SysAllocString((const OLECHAR*)(LPCWSTR)m_pImg->GetImageID().m_strFilename);
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// CRichEditOleCallback

static const UINT KCF_SMILEY = RegisterClipboardFormat(_T("richedit_smiley_enable"));

CRichEditOleCallback::CRichEditOleCallback(ISmileyHost *pSmileyHost)
:m_dwRef(1)
,m_iStorage(0)
{
    HRESULT hResult = ::StgCreateDocfile(NULL,
        STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE| STGM_DELETEONRELEASE,
        0, &m_stg );

    if ( m_stg == NULL ||
        hResult != S_OK )
    {
//         AfxThrowOleException( hResult );
    }

    m_pSmileyHost = pSmileyHost;
    SASSERT(m_pSmileyHost);
    m_pSmileyHost->AddRef();
}

CRichEditOleCallback::~CRichEditOleCallback()
{
    m_pSmileyHost->ClearTimer();
    m_pSmileyHost->Release();
}

HRESULT STDMETHODCALLTYPE 
CRichEditOleCallback::GetNewStorage(LPSTORAGE* ppStg)
{
    WCHAR tName[150];
    swprintf(tName, L"REStorage_%d", ++m_iStorage);

    if(m_iStorage%100 == 0)
    {//每100个对象提交一次,避免创建stream or storage由于内存不足而失败
        m_stg->Commit(STGC_DEFAULT);
    }
    HRESULT hr = m_stg->CreateStorage(tName, 
        STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE ,
        0, 0, ppStg );    
    if(FAILED(hr) && (hr & E_OUTOFMEMORY))
    {//失败后向storage提交后重试
        m_stg->Commit(STGC_DEFAULT);
        hr = m_stg->CreateStorage(tName, 
            STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE ,
            0, 0, ppStg );    
    }
    return hr;
}

HRESULT STDMETHODCALLTYPE 
CRichEditOleCallback::QueryInterface(REFIID iid, void ** ppvObject)
{

    HRESULT hr = S_OK;
    *ppvObject = NULL;

    if ( iid == IID_IUnknown ||
        iid == IID_IRichEditOleCallback )
    {
        *ppvObject = this;
        AddRef();
    }else if( iid == __uuidof(ISmileyHost))
    {
        *ppvObject = m_pSmileyHost;
        m_pSmileyHost->AddRef();
    }
    else
    {
        hr = E_NOINTERFACE;
    }

    return hr;
}



ULONG STDMETHODCALLTYPE 
CRichEditOleCallback::AddRef()
{
    return ++m_dwRef;
}



ULONG STDMETHODCALLTYPE 
CRichEditOleCallback::Release()
{
    if ( --m_dwRef == 0 )
    {
        delete this;
        return 0;
    }

    return m_dwRef;
}


HRESULT STDMETHODCALLTYPE 
CRichEditOleCallback::GetInPlaceContext(LPOLEINPLACEFRAME FAR *lplpFrame,
                                        LPOLEINPLACEUIWINDOW FAR *lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
    return S_OK;
}


HRESULT STDMETHODCALLTYPE 
CRichEditOleCallback::ShowContainerUI(BOOL fShow)
{
    return S_OK;
}



HRESULT STDMETHODCALLTYPE 
CRichEditOleCallback::QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp)
{
    return S_OK;
}


HRESULT STDMETHODCALLTYPE 
CRichEditOleCallback::DeleteObject(LPOLEOBJECT lpoleobj)
{
    return S_OK;
}


HRESULT STDMETHODCALLTYPE 
CRichEditOleCallback::GetClipboardData(CHARRANGE FAR *lpchrg, DWORD reco, LPDATAOBJECT FAR *lplpdataobj)
{
    /*演示自定义剪贴板格式的复制
    if(RECO_COPY == reco || RECO_CUT == reco)
    {
        wchar_t * pBuf = new WCHAR[lpchrg->cpMax - lpchrg->cpMin +1];
        TEXTRANGE txtRng;
        txtRng.chrg = *lpchrg;
        txtRng.lpstrText = pBuf;
        m_pRichedit->SSendMessage(EM_GETTEXTRANGE,0,(LPARAM)&txtRng);
        pBuf[lpchrg->cpMax - lpchrg->cpMin] =0;
        
        int  strBytes=  (lpchrg->cpMax - lpchrg->cpMin +1) * 2;  
        HGLOBAL hG = GlobalAlloc(GMEM_DDESHARE, strBytes);  
        void* pBuffer = GlobalLock(hG);  
        {  
            memcpy(pBuffer, pBuf, strBytes);  
            GlobalUnlock(hG);  
        }  
        delete []txtRng.lpstrText;

        FORMATETC fmt;  
        fmt.cfFormat = KCF_SMILEY;  
        fmt.dwAspect = DVASPECT_CONTENT;  
        fmt.lindex = -1;  
        fmt.ptd = NULL;  
        fmt.tymed = TYMED_HGLOBAL;  

        STGMEDIUM stg;  
        stg.tymed = TYMED_HGLOBAL;  
        stg.hGlobal = hG;  
        stg.pUnkForRelease = NULL;  

        HRESULT hr =CreateDataObject(&fmt,&stg,1,lplpdataobj);


        return hr;
    }
    */
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE 
CRichEditOleCallback::QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR *lpcfFormat,
                                      DWORD reco, BOOL fReally, HGLOBAL hMetaPict)
{
    if(!fReally) return S_OK;

    /*演示自定义剪贴板格式的粘贴
    if(RECO_DROP == reco || RECO_PASTE == reco)
    {
        FORMATETC fmt;  
        fmt.cfFormat = KCF_SMILEY;  
        fmt.dwAspect = DVASPECT_CONTENT;  
        fmt.lindex = -1;  
        fmt.ptd = NULL;  
        fmt.tymed = TYMED_HGLOBAL;  
        //如果KCF_SMILEY 剪贴板格式可用  
        if (SUCCEEDED(lpdataobj->QueryGetData(&fmt)) )
        {  
            STGMEDIUM stg;  
            HRESULT hr = lpdataobj->GetData(&fmt, &stg);  

            int nSize = GlobalSize(stg.hGlobal);  
            void* pBuffer = GlobalLock(stg.hGlobal);  
            {  

                STRACE(L"QueryAcceptData:%s",(LPWSTR)pBuffer);
                GlobalUnlock(stg.hGlobal);  
            }  
            return S_OK;  
        } 
    }
    */
    return E_NOTIMPL;
}



HRESULT STDMETHODCALLTYPE 
CRichEditOleCallback::ContextSensitiveHelp(BOOL fEnterMode)
{
    return S_OK;
}



HRESULT STDMETHODCALLTYPE 
CRichEditOleCallback::GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect)
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE 
CRichEditOleCallback::GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE FAR *lpchrg,
                                     HMENU FAR *lphmenu)
{
    return S_OK;
}



//////////////////////////////////////////////////////////////////////////
//  CSmileyHost

CSmileyHost::CSmileyHost(SRichEdit *pRichedit,FunCreateSource pCreateSource) 
:m_pHost(pRichedit)
,m_pCreateSource(pCreateSource)
,m_cTime(0)
{
    SASSERT(m_pHost);
    //订阅richedit的EN_UPDATE消息,用来更新表情坐标
    m_pHost->GetEventSet()->subscribeEvent(EVT_VISIBLECHANGED,Subscriber(&CSmileyHost::OnHostVisibleChanged,this));
    m_pHost->GetEventSet()->subscribeEvent(EventRENotify::EventID,Subscriber(&CSmileyHost::OnHostUpdate,this));
    if(pRichedit->IsVisible(TRUE))
    {
        m_pHost->GetContainer()->RegisterTimelineHandler(this);
    }
}

CSmileyHost::~CSmileyHost()
{
    m_pHost->GetContainer()->UnregisterTimelineHandler(this);
}


HRESULT STDMETHODCALLTYPE CSmileyHost::ClearTimer()
{
    SPOSITION pos = m_lstTimerInfo.GetHeadPosition();
    while(pos)
    {
        TIMERINFO *pTi = m_lstTimerInfo.GetNext(pos);
        pTi->pHandler->Clear();
        delete pTi;
    }
    m_lstTimerInfo.RemoveAll();
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CSmileyHost::SetTimer( /* [in] */ ITimerHandler * pTimerHander, /* [in] */ int nInterval )
{
    SPOSITION pos = m_lstTimerInfo.GetHeadPosition();
    while(pos)
    {
        if(m_lstTimerInfo.GetNext(pos)->pHandler == pTimerHander) return S_FALSE;
    } 

    m_lstTimerInfo.AddTail(new TIMERINFO(pTimerHander,nInterval));

    return S_OK;
}


HRESULT STDMETHODCALLTYPE CSmileyHost::KillTimer(/* [in] */ ITimerHandler * pTimerHander)
{
    SPOSITION pos = m_lstTimerInfo.GetHeadPosition();
    while(pos)
    {
        SPOSITION pos2 = pos;
        TIMERINFO *pTi = m_lstTimerInfo.GetNext(pos);
        if(pTi->pHandler == pTimerHander)
        {
            pTi->pHandler->Clear();
            delete pTi;
            m_lstTimerInfo.RemoveAt(pos2);
            break;
        }
    }
    return S_OK;
}


#define INTERVAL    2
HRESULT STDMETHODCALLTYPE  CSmileyHost::OnTimer( int nInterval )
{
    if(++m_cTime<INTERVAL) return S_OK;
    m_cTime=0;

    //找到所有到时间的定时器,防止在执行定时器时插入新定时器，需要先查找再执行。
    TIMERHANDLER_LIST lstDone;
    SPOSITION pos = m_lstTimerInfo.GetHeadPosition();
    while(pos)
    {
        SPOSITION pos2 = pos;
        TIMERINFO *pti = m_lstTimerInfo.GetNext(pos);
        pti->nInterval -= nInterval*INTERVAL;
        if(pti->nInterval <= 0)
        {
            lstDone.AddTail(pti);
            m_lstTimerInfo.RemoveAt(pos2);
        }
    }
    if(lstDone.IsEmpty()) return S_OK;

    //计算出刷新区域
    CAutoRefPtr<IRegion> rgn;
    GETRENDERFACTORY->CreateRegion(&rgn);;
    RECT rcSmiley;
    pos = lstDone.GetHeadPosition();
    while(pos)
    {
        TIMERINFO *pTi = lstDone.GetNext(pos);
        pTi->pHandler->GetRect(&rcSmiley);
        int nWid=rcSmiley.right-rcSmiley.left;
        rgn->CombineRect(&rcSmiley,RGN_OR);
    }

    CRect rcClient;
    m_pHost->GetClientRect(&rcClient);
    rgn->CombineRect(&rcClient,RGN_AND);

    //刷新表情
    IRenderTarget *pRT = m_pHost->GetRenderTarget(OLEDC_PAINTBKGND,rgn);
    m_pHost->SSendMessage(WM_ERASEBKGND,(WPARAM)pRT);

    HDC hdc = pRT->GetDC(0);

    pRT->GetClipBox(&rcClient);
    ALPHAINFO ai;
    CGdiAlpha::AlphaBackup(hdc,&rcClient,ai);

    pos = lstDone.GetHeadPosition();
    while(pos)
    {
        TIMERINFO *pTi = lstDone.GetNext(pos);
        pTi->pHandler->OnTimer(hdc);
        delete pTi;
    }
    CGdiAlpha::AlphaRestore(ai);

    pRT->ReleaseDC(hdc);
    m_pHost->ReleaseRenderTarget(pRT);

    return S_OK;
}


HRESULT STDMETHODCALLTYPE CSmileyHost::CreateSource( ISmileySource ** ppSource )
{
    if(m_pCreateSource) 
        *ppSource = m_pCreateSource();
    else
        *ppSource  = new CSmileySource;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CSmileyHost::InvalidateRect( /* [in] */ LPCRECT pRect )
{
    m_pHost->InvalidateRect(pRect);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CSmileyHost::GetHostRect( /* [out] */ LPRECT prcHost )
{
    ::GetWindowRect(m_pHost->GetContainer()->GetHostHwnd(),prcHost);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CSmileyHost::SendMessage( /* [in] */ UINT uMsg, /* [in] */ WPARAM wParam, /* [in] */ LPARAM lParam, /* [out] */ LRESULT *pRet )
{
    LRESULT lRet=m_pHost->SSendMessage(uMsg,wParam,lParam);
    if(pRet) *pRet = lRet;
    return S_OK;
}


bool CSmileyHost::OnHostVisibleChanged(SOUI::EventArgs *pEvt)
{
    if(m_pHost->IsVisible(TRUE))
        m_pHost->GetContainer()->RegisterTimelineHandler(this);
    else
        m_pHost->GetContainer()->UnregisterTimelineHandler(this);
    return false;
}

bool CSmileyHost::OnHostUpdate(SOUI::EventArgs *pEvt)
{
    EventRENotify *pReNotify = (EventRENotify*)pEvt;
    if(pReNotify->iNotify == EN_UPDATE)
    {
        ClearTimer();
    }
    return false; 
}



//////////////////////////////////////////////////////////////////////////
BOOL SetSRicheditOleCallback(SRichEdit *pRichedit,FunCreateSource pCreateSource /*= NULL*/)
{
    ISmileyHost *pHost = new SUnknownImpl<CSmileyHost>(pRichedit,pCreateSource);
    CRichEditOleCallback *pCallback = new CRichEditOleCallback(pHost);
    BOOL bRet=(BOOL)pRichedit->SSendMessage(EM_SETOLECALLBACK,0,(LPARAM)pCallback);
    pCallback->Release();
    pHost->Release();
    return bRet;
}

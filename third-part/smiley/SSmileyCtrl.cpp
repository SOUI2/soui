// SSmileyCtrl.cpp : Implementation of SSmileyCtrl
#include <Windows.h>
#include "../controls.extend/smiley/SSmileyCtrl.h"

#include <TOM.h>
#include <richedit.h>

#include <atlbase.h>
#include <atlcom.h>
#include <atlwin.h>
#include <atltypes.h>
#include <atlctl.h>
#include <atlhost.h>

const GUID  CLSID_SSmileyCtrl =
{0xd29e0bde,0xcfda,0x4b93,{0x92,0x9a,0x87,0x7a,0xb4,0x55,0x7b,0xd9}};


DWORD _GenerateID()
{
    static DWORD s_id=1;
    return s_id++;
}

// CCSmiley

class ATL_NO_VTABLE SSmileyCtrl :
public CComObjectRootEx<CComSingleThreadModel>,
public CComControl<SSmileyCtrl>,
public IOleObjectImpl<SSmileyCtrl>,
public IOleInPlaceObjectWindowlessImpl<SSmileyCtrl>,
public IViewObjectExImpl<SSmileyCtrl>,
public IPersistStreamInitImpl<SSmileyCtrl>,
public IPersistStorageImpl<SSmileyCtrl>,
public IOleControlImpl<SSmileyCtrl>,
public ITimerHandler,
public ISmileyCtrl
{
public:

    DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
    OLEMISC_CANTLINKINSIDE |
        OLEMISC_INSIDEOUT |
        OLEMISC_ACTIVATEWHENVISIBLE |
        OLEMISC_SETCLIENTSITEFIRST
        )

        BEGIN_COM_MAP(SSmileyCtrl)
            COM_INTERFACE_ENTRY(ITimerHandler)
            COM_INTERFACE_ENTRY(ISmileyCtrl)
            COM_INTERFACE_ENTRY(IOleControl)
            COM_INTERFACE_ENTRY(IOleObject)
            COM_INTERFACE_ENTRY(IViewObject)
            COM_INTERFACE_ENTRY(IViewObject2)
            COM_INTERFACE_ENTRY(IViewObjectEx)
            COM_INTERFACE_ENTRY(IPersistStreamInit)
            COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
            COM_INTERFACE_ENTRY(IPersistStorage)
        END_COM_MAP()

        BEGIN_PROP_MAP(SSmileyCtrl)
        END_PROP_MAP()

        BEGIN_MSG_MAP(SSmileyCtrl)
            CHAIN_MSG_MAP(CComControl<SSmileyCtrl>)
            DEFAULT_REFLECTION_HANDLER()
        END_MSG_MAP()

        DECLARE_PROTECT_FINAL_CONSTRUCT()

public:
    SSmileyCtrl();
    ~SSmileyCtrl();

    static CLSID GetObjectCLSID(){
        return CLSID_SSmileyCtrl;
    }
    


public://ISoSmileyCtrl
    STDMETHOD(Insert2Richedit)(IRichEditOle * ole);
    STDMETHOD(SetSource)(ISmileySource * pSource);    
    STDMETHOD(GetSource)(ISmileySource ** ppSource);    

public://IOleObject
    STDMETHOD(SetClientSite)(IOleClientSite *pClientSite);
    STDMETHOD(GetMiscStatus)(DWORD dwAspect, DWORD *pdwStatus);

public://IPersistStreamInitImpl
    HRESULT IPersistStreamInit_Load(LPSTREAM pStm, const ATL_PROPMAP_ENTRY* pMap);
    HRESULT IPersistStreamInit_Save(LPSTREAM pStm, BOOL fClearDirty, const ATL_PROPMAP_ENTRY* pMap);
    HRESULT FireViewChange();
    HRESULT	OnDraw(ATL_DRAWINFO& di);
public://IPersistStorage
    STDMETHOD(Load)(IStorage* pStorage);//重载IPersistStorage::Load，初始化完成后写入几个标志位

public://ITimerHander
    STDMETHOD(OnTimer)(HDC hdc);
    STDMETHOD(Clear)();;
    STDMETHOD(GetRect)(LPRECT pRect){memcpy(pRect,&m_rcPos,sizeof(RECT));return S_OK;};

private:
    void UpdateSmiley(HDC hdc);
    void UpdateSmileyFlag();
    DWORD GetSmileyFlag(IRichEditOle *ole,int iFirst,int iLast);

    CComPtr<ISmileyHost>    m_pSmileyHost;
    CComPtr<ISmileySource>  m_pSmileySource;
    int                     m_iFrameIndex;
    DWORD                   m_dwID;

    DWORD                   m_dwDrawFlag;
};


SSmileyCtrl::SSmileyCtrl():m_iFrameIndex(0),m_dwDrawFlag(0)
{
    m_dwID = _GenerateID();
    memset(&m_rcPos,0,sizeof(RECT));
	m_bRecomposeOnResize=1;
}

SSmileyCtrl::~SSmileyCtrl()
{
    if(m_pSmileyHost) 
    {//保证定时器不再引用自己
        m_pSmileyHost->KillTimer(this);
    }
}

STDMETHODIMP SSmileyCtrl::Insert2Richedit(IRichEditOle * ole)
{
    if(!m_pSmileySource) return E_FAIL;
    
    HRESULT hr;

    //insert this to host
    CComPtr<IOleObject>	pOleObject;
    CComPtr<IOleClientSite> pClientSite;  

    // Get site
    ole->GetClientSite(&pClientSite);
    ATLASSERT(pClientSite != NULL);
    
    CComPtr<IRichEditOleCallback> pCallback;
    hr=ole->QueryInterface(IID_IRichEditOleCallback,(void**)&pCallback);
    if(!SUCCEEDED(hr)) return E_FAIL;
    
    //get the IOleObject
    hr = _InternalQueryInterface(IID_IOleObject, (void**)&pOleObject);
    if (FAILED(hr))
    {
        return	 E_FAIL;
    }

    //to insert into richedit, you need a struct of REOBJECT
    REOBJECT reobject;
    ZeroMemory(&reobject, sizeof(REOBJECT));

    reobject.cbStruct = sizeof(REOBJECT);	

    CLSID clsid;
    pOleObject->GetUserClassID(&clsid);
    
    //get storage
    pCallback->GetNewStorage(&reobject.pstg);
    
    //set clsid
    reobject.clsid = clsid;
    //can be selected
    reobject.cp = REO_CP_SELECTION;
    //content, but not static
    reobject.dvaspect = DVASPECT_CONTENT;
    //goes in the same line of text line
    reobject.dwFlags = REO_BELOWBASELINE;
    reobject.dwUser = (DWORD)m_dwID;
    //the very object
    reobject.poleobj = pOleObject;
    //client site contain the object
    reobject.polesite = pClientSite;

    SIZEL sizel={0};
    reobject.sizel = sizel;

    hr = pOleObject->SetClientSite(pClientSite);
    
    if(SUCCEEDED(hr))
    {
        hr = ole->InsertObject(&reobject);

        FireViewChange();
    }
    
    if(reobject.pstg)
    {
        reobject.pstg->Release();
    }
    
    return hr;
}


STDMETHODIMP SSmileyCtrl::SetSource(ISmileySource * pSource)
{
    m_pSmileySource = pSource;
    if(m_pSmileySource)
    {
        //更新表情大小
        SIZE sz;
        m_pSmileySource->GetSize(&sz);

        HDC hDC = ::GetDC(NULL);
        m_sizeExtent.cx = ::MulDiv(sz.cx, 2540, GetDeviceCaps(hDC, LOGPIXELSX));
        m_sizeExtent.cy = ::MulDiv(sz.cy, 2540, GetDeviceCaps(hDC, LOGPIXELSY));
        ::ReleaseDC(NULL, hDC);
    }

    return S_OK;
}

STDMETHODIMP SSmileyCtrl::GetSource(ISmileySource ** ppSource)
{
    if(! m_pSmileySource) return E_FAIL;
    *ppSource = m_pSmileySource;
    (*ppSource)->AddRef();
    return S_OK;
}

STDMETHODIMP SSmileyCtrl::OnTimer(HDC hdc)
{
    int cFrame = 1;
    m_pSmileySource->GetFrameCount(&cFrame);
    m_iFrameIndex++;
    m_iFrameIndex %= cFrame;
    UpdateSmiley(hdc);
    return S_OK;
}


HRESULT SSmileyCtrl::IPersistStreamInit_Load(LPSTREAM pStm, const ATL_PROPMAP_ENTRY* pMap)
{
    ATLASSERT(!m_pSmileySource);
    ATLASSERT(m_pSmileyHost);
    CComPtr<ISmileySource> pSource;
    HRESULT hr =m_pSmileyHost->CreateSource(&pSource);
    if(!SUCCEEDED(hr)) return hr;
    hr = pSource->Stream_Load(pStm);
    if(!SUCCEEDED(hr)) return hr;
    SetSource(pSource);

    hr = IPersistStreamInitImpl<SSmileyCtrl>::IPersistStreamInit_Load( pStm, pMap);

    if(!SUCCEEDED(hr)) return hr;
    FireViewChange();
    return hr;
}

HRESULT SSmileyCtrl::IPersistStreamInit_Save(LPSTREAM pStm, BOOL fClearDirty, const ATL_PROPMAP_ENTRY* pMap)
{
    if(!m_pSmileySource) return E_FAIL;
    HRESULT hr = m_pSmileySource->Stream_Save(pStm);
    if(!SUCCEEDED(hr)) return hr;
    return IPersistStreamInitImpl<SSmileyCtrl>::IPersistStreamInit_Save( pStm, fClearDirty, pMap);
}

HRESULT SSmileyCtrl::FireViewChange()
{
    if (m_bInPlaceActive)
    {
        // Active
        if (m_hWndCD != NULL)
            ::InvalidateRect(m_hWndCD, NULL, TRUE); // Window based
        else if (m_bWndLess && m_spInPlaceSite != NULL)
            m_spInPlaceSite->InvalidateRect(NULL, TRUE); // Windowless
    }
    else if (!m_pSmileyHost)
    {
        SendOnViewChange(DVASPECT_CONTENT);
    }
    else
    {
//         UpdateSmiley(NULL);
    }
    return S_OK;
}

HRESULT SSmileyCtrl::OnDraw(ATL_DRAWINFO& di)
{
    if(!m_pSmileySource) return E_FAIL;
    RECT rc={di.prcBounds->left,di.prcBounds->top,di.prcBounds->right,di.prcBounds->bottom};
    m_pSmileySource->Draw(di.hdcDraw,&rc,m_iFrameIndex);
    if(!di.bZoomed)
    {//在copy时会有bZoomed标志，把矩形放大。这里先清除掉
        m_rcPos = rc;//cache pos, position will be updated in response to EN_UPDATE message come from it's host
    }
    
    int nDelay=0;
    HRESULT hr = m_pSmileySource->GetFrameDelay(m_iFrameIndex,&nDelay);
    if(SUCCEEDED(hr) && m_pSmileyHost)
    {
        m_pSmileyHost->SetTimer(this,nDelay);
    }
    return S_OK;    
}


void SSmileyCtrl::UpdateSmiley(HDC hdc)
{
    if(!m_pSmileyHost) return ;
    if(::IsRectEmpty(&m_rcPos)) return;
    
    if(!hdc) return;
    
    UpdateSmileyFlag();
    
    m_pSmileySource->Draw(hdc,&m_rcPos,m_iFrameIndex);
    
    if (m_dwDrawFlag == REO_INVERTEDSELECT)
    {
        //Invert entire object
        InvertRect(hdc, &m_rcPos);
    }
    else if(m_dwDrawFlag == REO_SELECTED)
    {
        // Just the border, so use a null brush
        SaveDC(hdc);
        SetROP2(hdc, R2_NOT);
        SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Rectangle(hdc, m_rcPos.left, m_rcPos.top, m_rcPos.right, m_rcPos.bottom);
        RestoreDC(hdc, -1);
    }
    
    int nDelay=0;
    HRESULT hr = m_pSmileySource->GetFrameDelay(m_iFrameIndex,&nDelay);
    if(SUCCEEDED(hr) && m_pSmileyHost)
    {
        m_pSmileyHost->SetTimer(this,nDelay);
    }
}


HRESULT SSmileyCtrl::Load( IStorage* pStorage )
{
    //向RichEditFlags字段中写入3个REOBJECT标志。分析了Richedit20的wince源代码后发现对象初始化完成插入richedit前会从"RichEditFlags"流中读3个标志位
    CComPtr<IStream> spStream;
    static LPCOLESTR vszRichEditFlags = OLESTR("RichEditFlags");
    HRESULT hr = pStorage->CreateStream(vszRichEditFlags,
        STGM_WRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE,
        0, 0, &spStream);
    if (FAILED(hr)) return hr;
    DWORD dwFlag=REO_BELOWBASELINE,dwUser=m_dwID,dwAspect=DVASPECT_CONTENT;
    spStream->Write(&dwFlag,sizeof(dwFlag),NULL);
    spStream->Write(&dwUser,sizeof(dwUser),NULL);
    spStream->Write(&dwAspect,sizeof(dwAspect),NULL);

    return IPersistStorageImpl<SSmileyCtrl>::Load(pStorage);
}

STDMETHODIMP SSmileyCtrl::GetMiscStatus(DWORD dwAspect, DWORD *pdwStatus)
{
    (dwAspect);
    *pdwStatus = OLEMISC_RECOMPOSEONRESIZE |
        OLEMISC_CANTLINKINSIDE |
        OLEMISC_INSIDEOUT |
        OLEMISC_ACTIVATEWHENVISIBLE |
        OLEMISC_SETCLIENTSITEFIRST;
    return S_OK;
}

STDMETHODIMP SSmileyCtrl::SetClientSite(IOleClientSite *pClientSite)
{
    if(pClientSite) 
    {
        CComPtr<IRichEditOleCallback> pCallback;
        HRESULT hr=pClientSite->QueryInterface(IID_IRichEditOleCallback,(void**)&pCallback);
        if(FAILED(hr)) return E_FAIL;

        //从callback中获取host
        hr = pCallback->QueryInterface(__uuidof(ISmileyHost),(LPVOID*)&m_pSmileyHost);
        if(FAILED(hr)) return E_FAIL;
    }
    
    return __super::SetClientSite(pClientSite);
}


LONG GetOleCP(IRichEditOle *pOle, int iOle)
{
    REOBJECT reobj={0};
    reobj.cbStruct=sizeof(REOBJECT);
    pOle->GetObject(iOle,&reobj,REO_GETOBJ_NO_INTERFACES);
    return reobj.cp;
}

//find first Ole Object in char range of [cpMin,cpMax)
int FindFirstOleInrange(IRichEditOle *pOle, int iBegin,int iEnd,int cpMin,int cpMax)
{
    if(iBegin==iEnd) return -1;

    int iMid = (iBegin + iEnd)/2;

    LONG cp = GetOleCP(pOle,iMid);

    if(cp < cpMin)
    {
        return FindFirstOleInrange(pOle,iMid+1,iEnd,cpMin,cpMax);
    }else if(cp >= cpMax)
    {
        return FindFirstOleInrange(pOle,iBegin,iMid,cpMin,cpMax);
    }else
    {
        int iRet = iMid;
        while(iRet>iBegin)
        {
            cp = GetOleCP(pOle,iRet-1);
            if(cp<cpMin) break;
            iRet --;
        }
        return iRet;
    }
}

//find Last Ole Object in char range of [cpMin,cpMax)
int FindLastOleInrange(IRichEditOle *pOle, int iBegin,int iEnd,int cpMin,int cpMax)
{
    if(iBegin==iEnd) return -1;

    int iMid = (iBegin + iEnd)/2;

    LONG cp = GetOleCP(pOle,iMid);

    if(cp < cpMin)
    {
        return FindLastOleInrange(pOle,iMid+1,iEnd,cpMin,cpMax);
    }else if(cp >= cpMax)
    {
        return FindLastOleInrange(pOle,iBegin,iMid,cpMin,cpMax);
    }else
    {
        int iRet = iMid;
        while(iRet<(iEnd-1))
        {
            cp = GetOleCP(pOle,iRet+1);
            if(cp>=cpMax) break;
            iRet ++;
        }
        return iRet;
    }
}


//获得表情的绘制状态
DWORD SSmileyCtrl::GetSmileyFlag(IRichEditOle *ole,int iFirst,int iLast)
{
    ATLASSERT(m_pSmileyHost);

    DWORD dwFlag =0;
    for(int i=iFirst;i<=iLast;i++)
    {
        REOBJECT reobj={0};
        reobj.cbStruct=sizeof(REOBJECT);
        HRESULT hr=ole->GetObject(i,&reobj,REO_GETOBJ_NO_INTERFACES);
        if(FAILED(hr)) break;

        if (reobj.clsid==CLSID_SSmileyCtrl && reobj.dwUser==m_dwID)
        {
            //选中单个对象时绘制边框，选中多个对象时反色
            CHARRANGE chr={0};
            m_pSmileyHost->SendMessage(EM_EXGETSEL, 0, (LPARAM)&chr,NULL);

            if  ( chr.cpMin!=chr.cpMax 
                && (reobj.cp<chr.cpMax && reobj.cp>=chr.cpMin) )
            {
                if((chr.cpMax-chr.cpMin) > 1)
                    dwFlag=REO_INVERTEDSELECT;
                else
                    dwFlag=REO_SELECTED;                
            }

            break;            
        }
    }
    return dwFlag;
}

void SSmileyCtrl::UpdateSmileyFlag()
{
    if(!m_pSmileyHost) return ;
    
    if(m_dwDrawFlag!=-1) return;
    
    CComPtr<IRichEditOle>  ole;
    LRESULT lMsgRet = 0;
    m_pSmileyHost->SendMessage(EM_GETOLEINTERFACE, 0, (LPARAM)&ole,&lMsgRet);
    if (!lMsgRet) return;
        
    //获得可见字符范围
    m_pSmileyHost->SendMessage(EM_GETFIRSTVISIBLELINE,0,0,&lMsgRet);

    int iFirstLine = (int)lMsgRet;
    RECT rcView;
    m_pSmileyHost->SendMessage(EM_GETRECT,0,(LPARAM)&rcView,&lMsgRet);
    POINT pt={rcView.right+1,rcView.bottom-2};

    m_pSmileyHost->SendMessage(EM_LINEINDEX,iFirstLine,0,&lMsgRet);
    LONG cpFirst = (LONG)lMsgRet;
    m_pSmileyHost->SendMessage(EM_CHARFROMPOS,0,(LPARAM)&pt,&lMsgRet);
    LONG cpLast  = (LONG)lMsgRet;

    //采用两分法查找在可见范围中的OLE对象
    int nCount=ole->GetObjectCount();

    int iFirstVisibleOle = FindFirstOleInrange(ole,0,nCount,cpFirst,cpLast);
    if(iFirstVisibleOle==-1) return;
    int iLastVisibleOle = FindLastOleInrange(ole,iFirstVisibleOle,nCount,cpFirst,cpLast);
    
    m_dwDrawFlag = GetSmileyFlag(ole,iFirstVisibleOle,iLastVisibleOle);
}

STDMETHODIMP SSmileyCtrl::Clear()
{
    memset(&m_rcPos,0,sizeof(RECT));
    m_dwDrawFlag = -1;
    return S_OK;
}


HRESULT CreateSmiley(REFIID riid,void ** ppUnk)
{
    SSmileyCtrl * pRet = new SComObject<SSmileyCtrl>;
    HRESULT hr = pRet->QueryInterface(riid,ppUnk);
    if(hr != S_OK) delete pRet;
    return hr;
}

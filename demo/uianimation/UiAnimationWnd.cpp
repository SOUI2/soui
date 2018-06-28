#include "stdafx.h"
#include "UiAnimationWnd.h"
#include <math.h>
#include <helper/splitstring.h>

#ifndef IID_PPV_ARGS
extern "C++"
{
    template<typename T> void** IID_PPV_ARGS_Helper(T** pp) 
    {
        // make sure everyone derives from IUnknown
        static_cast<IUnknown*>(*pp);
        
        return reinterpret_cast<void**>(pp);
    }
}

#define IID_PPV_ARGS(ppType) __uuidof(**(ppType)), IID_PPV_ARGS_Helper(ppType)
#endif

namespace SOUI{
    
    #define TIEMRID_MODE    1

    //////////////////////////////////////////////////////////////////////////
    //  CUiAnimation
    template<> CUiAnimation* SSingleton<CUiAnimation>::ms_Singleton = NULL;
    

    CUiAnimation::CUiAnimation(IUIAnimationTimer *pUiAniTimer)
    {
        SASSERT(pUiAniTimer);
        m_pAnimationTimer = pUiAniTimer;
        
        // Create Animation Transition Library

        HRESULT hr = CoCreateInstance(
            __uuidof(UIAnimationTransitionLibrary),
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&m_pTransitionLibrary)
            );
        if (SUCCEEDED(hr))
        {
            // Create the Transition Factory to wrap interpolators in transitions

            hr = CoCreateInstance(
                __uuidof(UIAnimationTransitionFactory),
                NULL,
                CLSCTX_INPROC_SERVER,
                IID_PPV_ARGS(&m_pTransitionFactory)
                );
        }
        
        SASSERT(SUCCEEDED(hr));
    }

    HRESULT CUiAnimation::Init()
    {
        SASSERT(!getSingletonPtr());
        
            // Create Animation Timer
        SComPtr<IUIAnimationTimer>    pAnimationTimer;

        HRESULT hr = CoCreateInstance(
            __uuidof(UIAnimationTimer),
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&pAnimationTimer)
            );
        if (SUCCEEDED(hr))
        {
            //创建一个UIAnimation的单例
            new CUiAnimation(pAnimationTimer);
        }

        return hr;
    }

    void CUiAnimation::Free()
    {
        if(getSingletonPtr())
        {
            delete getSingletonPtr();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // CUIAnimationManagerEventHandler
    class CUIAnimationManagerEventHandler : public SUnknownImpl<IUIAnimationManagerEventHandler>
    {
    public:
        CUIAnimationManagerEventHandler(SUiAnimationWnd * pOwner);
        ~CUIAnimationManagerEventHandler();
        
        virtual /* [annotation] */ 
            __checkReturn
            HRESULT STDMETHODCALLTYPE OnManagerStatusChanged( 
            /* [annotation][in] */ 
            __in  UI_ANIMATION_MANAGER_STATUS newStatus,
            /* [annotation][in] */ 
            __in  UI_ANIMATION_MANAGER_STATUS previousStatus);

        COM_INTERFACE_BEGIN()
            COM_INTERFACE(IUIAnimationManagerEventHandler)
        COM_INTERFACE_END()

    protected:
        SUiAnimationWnd *m_pOwner;
    };

    CUIAnimationManagerEventHandler::CUIAnimationManagerEventHandler(SUiAnimationWnd * pOwner):m_pOwner(pOwner)
    {
        m_pOwner->AddRef();
    }

    HRESULT STDMETHODCALLTYPE CUIAnimationManagerEventHandler::OnManagerStatusChanged(/* [annotation][in] */ __in UI_ANIMATION_MANAGER_STATUS newStatus, /* [annotation][in] */ __in UI_ANIMATION_MANAGER_STATUS previousStatus)
    {
        if (newStatus == UI_ANIMATION_MANAGER_BUSY)
        {
            m_pOwner->Invalidate();
        }

        return S_OK;
    }

    CUIAnimationManagerEventHandler::~CUIAnimationManagerEventHandler()
    {
        m_pOwner->Release();
    }

    //////////////////////////////////////////////////////////////////////////
    //  CImgThumnail
    CImgThumnail::CImgThumnail():m_pImg(NULL)
    {

    }

    HRESULT CImgThumnail::Init(IUIAnimationManager * pAnimationMgr,ISkinObj * pImg, double x, double y)
    {
        m_pImg = pImg;

        // Create the animation variables for the x and y coordinates

        HRESULT hr = pAnimationMgr->CreateAnimationVariable(
            x,
            &m_pAnimationVariableX
            );
        if (SUCCEEDED(hr))
        {
            hr = pAnimationMgr->CreateAnimationVariable(
                y,
                &m_pAnimationVariableY
                );
        }
        return hr;
    }

    void CImgThumnail::Render(IRenderTarget *pRT,const CRect &rcClient)
    {
        SASSERT(m_pImg);
        DOUBLE x = 0;
        HRESULT hr = m_pAnimationVariableX->GetValue(
            &x
            );
        if (SUCCEEDED(hr))
        {
            DOUBLE y = 0;
            hr = m_pAnimationVariableY->GetValue(
                &y
                );
            if (SUCCEEDED(hr))
            {
                CRect rc(CPoint((int)x,(int)y)+rcClient.TopLeft(),GetSize());
                m_pImg->Draw(pRT,&rc,0,0xFF);
            }
        }
    }

    CSize CImgThumnail::GetSize()
    {
        SASSERT(m_pImg);
        return m_pImg->GetSkinSize();
    }

    //////////////////////////////////////////////////////////////////////////
    //  CUiAnimationIconLayout
    CUiAnimationIconLayout::CUiAnimationIconLayout(SUiAnimationWnd *pOwner,IBitmap *pBmpMode)
        :m_pOwner(pOwner),m_plstIcon(NULL),m_nIcons(0),m_iModal(-1)
    {
        HRESULT hr = CoCreateInstance(
            __uuidof(UIAnimationManager),
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&m_pAnimationManager)
            );
        if(SUCCEEDED(hr))
        {
            CUIAnimationManagerEventHandler      *  pAnimationEvtHandler = new CUIAnimationManagerEventHandler(m_pOwner);
            hr = m_pAnimationManager->SetManagerEventHandler(pAnimationEvtHandler);
            pAnimationEvtHandler->Release();
        }

        LPDWORD pPixels = (LPDWORD)pBmpMode->LockPixelBits();
        SIZE    sz = pBmpMode->Size();
        int iX1=0;
        for(int i=0;i<=sz.cx;i++)
        {
            if(i==sz.cx || pPixels[i] == 0xFFFF00FF) //分割线
            {
                int iX2=i;
                SIZE szWord={iX2-iX1,sz.cy};
                
                CHARBITS *pCharBits = new CHARBITS;
                pCharBits->pBits = new char[szWord.cx * szWord.cy];
                pCharBits->sz=szWord;
                
                for(int y =0 ;y< szWord.cy;y++)
                {
                    LPDWORD pLine = pPixels + y * sz.cx;
                    for(int x=0;x<szWord.cx;x++)
                    {
                        pCharBits->pBits[y*szWord.cx+x] = pLine[iX1+x] != 0xFFFFFFFF;
                    }
                }
                m_arrCharBits.Add(pCharBits);
                iX1=iX2+1;
            }
        }
    }


    CUiAnimationIconLayout::~CUiAnimationIconLayout()
    {
        if(m_plstIcon) delete [] m_plstIcon;
        for(UINT i=0;i<m_arrCharBits.GetCount();i++)
        {
            delete m_arrCharBits[i];
        }
    }

    void CUiAnimationIconLayout::SetIcons(ISkinObj *pIcon,int nIcons )
    {
        m_plstIcon = new CImgThumnail[nIcons];
        m_nIcons=nIcons;
        for(int i=0;i<m_nIcons;i++)
        {
            m_plstIcon[i].Init(GetAnimationMgr(),pIcon,0.0,0.0);
        }
    }

    HRESULT CUiAnimationIconLayout::Arrange( const CSize & sz, int iModal )
    {
        if(iModal >= (int)m_arrCharBits.GetCount()) return E_NOTIMPL;
        if(iModal <0) iModal = m_iModal;
        if(iModal <0) iModal = 0;
        
        IUIAnimationStoryboard *pStoryboard;
        HRESULT hr = m_pAnimationManager->CreateStoryboard(
            &pStoryboard
            );
        if (SUCCEEDED(hr))
        {
            // Arrange the thumbnails, adding transitions to move each thumbnail to a random new location
            POINT *pts = new POINT[m_nIcons];
            GetIconsPos(iModal,pts);
            
            SIZE szModel = m_arrCharBits[iModal]->sz;
            CSize szClient = sz-m_plstIcon[0].GetSize();

            double fScale=1.0;
            if(szModel.cx * szClient.cy > szModel.cy* szClient.cx)
            {
                fScale = szClient.cx*1.0/szModel.cx;
            }else
            {
                fScale = szClient.cy*1.0/szModel.cy;
            }
            int xOffset=0, yOffset=0;
            int nWid=(int)(szModel.cx*fScale);
            int nHei=(int)(szModel.cy*fScale);

            xOffset = (szClient.cx-nWid)/2;
            yOffset = (szClient.cy-nHei)/2;

            for (int i = 0; i < m_nIcons; i++)
            {
                CSize sizeIcon = m_plstIcon[i].GetSize();
                DOUBLE xDest, yDest;
                xDest = xOffset + pts[i].x * fScale;
                yDest = yOffset + pts[i].y * fScale;

                // Get the current position
                // Note that this technique is valid only when the storyboard will begin playing immediately

                DOUBLE xCur;
                hr = m_plstIcon[i].m_pAnimationVariableX->GetValue(&xCur);
                if (SUCCEEDED(hr))
                {
                    DOUBLE yCur;
                    hr = m_plstIcon[i].m_pAnimationVariableY->GetValue(&yCur);
                    if (SUCCEEDED(hr))
                    {
                        // Add transitions for x and y movement

                        if (fabs(xDest - xCur) > fabs(yDest - yCur))
                        {
                            // If the thumbnail has further to travel horizontally than vertically, use a parabolic transition
                            // on X that will determine the duration of the storyboard, and stretch an accelerate-decelerate
                            // transition on Y to have the same duration.

                            hr = AddIconTransitions(
                                pStoryboard,
                                m_plstIcon[i].m_pAnimationVariableX,
                                xDest,
                                m_plstIcon[i].m_pAnimationVariableY,
                                yDest
                                );
                        }
                        else
                        {
                            // If the thumbnail has further to travel vertically than horizontally, use a parabolic transition
                            // on Y that will determine the duration of the storyboard, and stretch an accelerate-decelerate
                            // transition on X to have the same duration.

                            hr = AddIconTransitions(
                                pStoryboard,
                                m_plstIcon[i].m_pAnimationVariableY,
                                yDest,
                                m_plstIcon[i].m_pAnimationVariableX,
                                xDest
                                );
                        }
                    }
                }

                if (FAILED(hr))
                {
                    break;
                }
            }

            if (SUCCEEDED(hr))
            {
                hr = ScheduleStoryboard(pStoryboard);
            }
            delete []pts;
            pStoryboard->Release();
            m_iModal = iModal;
        }

        return hr;
    }



    // Adds two transitions to a storyboard: one primary parabolic transition, which will determine
    // the storyboard duration, and a secondary accelerate-decelerate transition, which will be
    // stretched to that duration.

    HRESULT CUiAnimationIconLayout::AddIconTransitions(
        IUIAnimationStoryboard *pStoryboard,
        IUIAnimationVariable *pVariablePrimary,
        DOUBLE valuePrimary,
        IUIAnimationVariable *pVariableSecondary,
        DOUBLE valueSecondary
        )
    {
        const DOUBLE ACCELERATION = 2000;
        const DOUBLE ACCELERATION_RATIO = 0.3;
        const DOUBLE DECELERATION_RATIO = 0.3;

        IUIAnimationTransition *pTransitionPrimary;
        HRESULT hr = CUiAnimation::getSingleton().m_pTransitionLibrary->CreateParabolicTransitionFromAcceleration(
            valuePrimary,
            0.0,
            ACCELERATION,
            &pTransitionPrimary
            );
        if (SUCCEEDED(hr))
        {
            hr = pStoryboard->AddTransition(
                pVariablePrimary,
                pTransitionPrimary
                );
            if (SUCCEEDED(hr))
            {
                UI_ANIMATION_KEYFRAME keyframeEnd;
                hr = pStoryboard->AddKeyframeAfterTransition(
                    pTransitionPrimary,
                    &keyframeEnd
                    );
                if (SUCCEEDED(hr))
                {
                    IUIAnimationTransition *pTransitionSecondary;
                    hr = CUiAnimation::getSingleton().m_pTransitionLibrary->CreateAccelerateDecelerateTransition(
                        1.0,    // Will be overwritten, so unimportant
                        valueSecondary,
                        ACCELERATION_RATIO,
                        DECELERATION_RATIO,    
                        &pTransitionSecondary
                        );
                    if (SUCCEEDED(hr))
                    {
                        hr = pStoryboard->AddTransitionBetweenKeyframes(
                            pVariableSecondary,
                            pTransitionSecondary,
                            UI_ANIMATION_KEYFRAME_STORYBOARD_START,
                            keyframeEnd
                            );
                        pTransitionSecondary->Release();
                    }	            
                }
            }

            pTransitionPrimary->Release();
        }

        return hr;
    }

    HRESULT CUiAnimationIconLayout::ScheduleStoryboard( IUIAnimationStoryboard *pStoryboard )
    {
        UI_ANIMATION_SECONDS secondsNow;
        HRESULT hr = CUiAnimation::getSingleton().m_pAnimationTimer->GetTime(
            &secondsNow
            );
        if (SUCCEEDED(hr))
        {
            hr = pStoryboard->Schedule(
                secondsNow
                );
        }

        return hr;

    }

    void CUiAnimationIconLayout::GetIconsPos(int iModal,LPPOINT pts)
    {
        SASSERT(iModal<(int)m_arrCharBits.GetCount());

        CHARBITS *pCharBit = m_arrCharBits[iModal];
        int idx=0;
        
        for(int y=0;y<pCharBit->sz.cy && idx<m_nIcons-1;y++)
        {
            for(int x=0;x<pCharBit->sz.cx && idx<m_nIcons-1;x++)
            {
                if(pCharBit->pBits[y*pCharBit->sz.cx+x])
                {
                    SASSERT(idx<m_nIcons);
                    pts[idx++] = CPoint(x,y);
                }
            }
        }

        for(;idx<m_nIcons;idx++)
        {
            pts[idx] = CPoint(0,0);
        }
    }

    int CUiAnimationIconLayout::NextModal()
    {
        return (m_iModal+1)%m_arrCharBits.GetCount();
    }

    void CUiAnimationIconLayout::OnPaint( IRenderTarget *pRT ,const CRect &rcClient)
    {
        UI_ANIMATION_SECONDS secondsNow;
        HRESULT hr = CUiAnimation::getSingleton().m_pAnimationTimer->GetTime(
            &secondsNow
            );
        if(SUCCEEDED(hr))
        {
            m_pAnimationManager->Update(
                secondsNow
                );
        }

        for(int i=0;i< m_nIcons;i++)
        {
            m_plstIcon[i].Render(pRT,rcClient);
        }

        UI_ANIMATION_MANAGER_STATUS status;
        hr = m_pAnimationManager->GetStatus(
            &status
            );
        if (SUCCEEDED(hr) && status == UI_ANIMATION_MANAGER_BUSY)
        {
            m_pOwner->Invalidate();
        }

    }
    //////////////////////////////////////////////////////////////////////////
    //  SUiAnimationWnd
    SUiAnimationWnd::SUiAnimationWnd(void):m_pSkinIcon(NULL),m_pLayout(NULL),m_pAniMode(NULL),m_bResized(FALSE)
    {
        m_bClipClient = TRUE;
    }

    SUiAnimationWnd::~SUiAnimationWnd(void)
    {
        if(m_pAniMode) m_pAniMode->Release();
    }

    
    int SUiAnimationWnd::OnCreate(void *)
    {
        SWindow::OnCreate(NULL);
        if(!m_pSkinIcon || !m_pAniMode)
        {
            SASSERT_FMT(FALSE,_T("please set icon and bmpmode value in xml!"));
            return -1;
        }
                
        m_pLayout = new CUiAnimationIconLayout(this,m_pAniMode);
        m_pLayout->SetIcons(m_pSkinIcon,460);
        
        SetTimer(TIEMRID_MODE,3000);
        return 0;
    }

    void SUiAnimationWnd::OnDestroy()
    {
        SWindow::OnDestroy();
        if(m_pLayout) delete m_pLayout;
    }

    void SUiAnimationWnd::OnPaint( IRenderTarget *pRT )
    {
        if(m_pLayout) 
        {
            CRect rcClient;
            GetClientRect(&rcClient);
            m_pLayout->OnPaint(pRT,rcClient);
        }
    }

    void SUiAnimationWnd::OnTimer(char cEvt)
    {
        if(cEvt == TIEMRID_MODE)
        {
            if(IsVisible(TRUE))
            {
                CRect rc;
                GetClientRect(&rc);
                CSize sz=rc.Size();
                m_pLayout->Arrange(sz,m_pLayout->NextModal());
            }
        }
    }

    void SUiAnimationWnd::OnSize(UINT nType, CSize size)
    { 
        SWindow::OnSize(nType,size);
        if(m_pLayout && IsVisible(TRUE))
        {
            m_bResized = FALSE;
            m_pLayout->Arrange(size,-1);
        }
        else m_bResized =TRUE;
    }

    void SUiAnimationWnd::OnShowWindow( BOOL bShow, UINT nStatus )
    {
        SWindow::OnShowWindow(bShow,nStatus);
        if(bShow && m_bResized)
        {
            CRect rcClient;
            GetClientRect(&rcClient);
            OnSize(0,rcClient.Size());
        }
    }

}
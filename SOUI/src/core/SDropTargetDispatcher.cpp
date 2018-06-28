#include "souistd.h"
#include "core/SDropTargetDispatcher.h"
#include "core/SwndContainerImpl.h"

namespace SOUI{
    SDropTargetDispatcher::SDropTargetDispatcher(SWindow * pOwner)
        :m_pOwner(pOwner)
        ,m_pDataObj(NULL)
        ,m_hHover(0)
    {
    }

    SDropTargetDispatcher::~SDropTargetDispatcher(void)
    {
        DragLeave();
        SPOSITION pos=m_mapDropTarget.GetStartPosition();
        while(pos)
        {
            DTMAP::CPair *pPair=m_mapDropTarget.GetNext(pos);
            pPair->m_value->Release();
        }
    }

    BOOL SDropTargetDispatcher::RegisterDragDrop( SWND swnd,IDropTarget *pDropTarget )
    {
        if(m_mapDropTarget.Lookup(swnd)) return FALSE;
        m_mapDropTarget[swnd]=pDropTarget;
        pDropTarget->AddRef();
        return TRUE;
    }

    BOOL SDropTargetDispatcher::RevokeDragDrop( SWND swnd )
    {
        DTMAP::CPair *pPair=m_mapDropTarget.Lookup(swnd);
        if(!pPair) return FALSE;
        pPair->m_value->Release();
        m_mapDropTarget.RemoveKey(swnd);
        return TRUE;
    }


    HRESULT STDMETHODCALLTYPE SDropTargetDispatcher::QueryInterface( /* [in] */ REFIID riid, /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject )
    {
        HRESULT hr=S_FALSE;
        if(riid==__uuidof(IUnknown))
            *ppvObject=(IUnknown*) this,hr=S_OK;
        else if(riid==__uuidof(IDropTarget))
            *ppvObject=(IDropTarget*)this,hr=S_OK;
        return hr;
    }

    HRESULT STDMETHODCALLTYPE SDropTargetDispatcher::DragEnter( /* [unique][in] */ IDataObject *pDataObj, /* [in] */ DWORD grfKeyState, /* [in] */ POINTL pt, /* [out][in] */ DWORD *pdwEffect )
    {
        m_pDataObj=pDataObj;
        m_pDataObj->AddRef();
        return DragOver(grfKeyState,pt,pdwEffect);
    }

    HRESULT STDMETHODCALLTYPE SDropTargetDispatcher::DragOver( /* [in] */ DWORD grfKeyState, /* [in] */ POINTL pt, /* [out][in] */ DWORD *pdwEffect )
    {
		*pdwEffect=DROPEFFECT_NONE;
        SWND hHover=m_pOwner->SwndFromPoint(PointL2FrameClient(pt),FALSE);
        
        if(hHover != m_hHover)
        {
            DTMAP::CPair *pPair=m_mapDropTarget.Lookup(m_hHover);
            if(m_hHover && pPair)
                pPair->m_value->DragLeave();
            m_hHover=hHover;
            pPair=m_mapDropTarget.Lookup(m_hHover);
            if(pPair && m_hHover)
                pPair->m_value->DragEnter(m_pDataObj,grfKeyState,pt,pdwEffect);
        }else
        {
            DTMAP::CPair *pPair=m_mapDropTarget.Lookup(m_hHover);
            if(m_hHover && pPair)
                pPair->m_value->DragOver(grfKeyState,pt,pdwEffect);
        }

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE SDropTargetDispatcher::DragLeave( void )
    {
        if(m_pDataObj)
        {
            m_pDataObj->Release();
            m_pDataObj=NULL;
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE SDropTargetDispatcher::Drop( /* [unique][in] */ IDataObject *pDataObj, /* [in] */ DWORD grfKeyState, /* [in] */ POINTL pt, /* [out][in] */ DWORD *pdwEffect )
    {
        DTMAP::CPair *pPair=m_mapDropTarget.Lookup(m_hHover);
        if(m_hHover && pPair)
            pPair->m_value->Drop(pDataObj,grfKeyState,pt,pdwEffect);
        m_hHover=NULL;
        m_pDataObj->Release();
        m_pDataObj=NULL;
        return S_OK;
    }

    POINT SDropTargetDispatcher::PointL2FrameClient( const POINTL & pt )
    {
        CPoint pt2(pt.x,pt.y);
        ScreenToClient(m_pOwner->GetContainer()->GetHostHwnd(),&pt2);
        return pt2;
    }

}


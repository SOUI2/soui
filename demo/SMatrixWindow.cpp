#include "stdafx.h"
#include "SMatrixWindow.h"
#include "../third-part/skmatrix/include/SkCamera.h"
namespace SOUI
{

	S3DView::S3DView(void):m_iStep(0),m_fRotate(0),m_rotateDir(RotateY),m_nSpeed(5)
	{
	}

	S3DView::~S3DView(void)
	{
	}


	void S3DView::OnShowWindow(BOOL bShow, UINT nStatus)
	{
		__super::OnShowWindow(bShow,nStatus);
		if(IsVisible(TRUE))
		{
			GetContainer()->RegisterTimelineHandler(this);
		}else
		{
			GetContainer()->UnregisterTimelineHandler(this);
		}
	}


	void S3DView::OnPaint(IRenderTarget *pRT)
	{
		SPainter painter;
		BeforePaint(pRT,painter);
		CRect rc = GetClientRect();
		if(!m_pBgSkin)
		{
			pRT->DrawText(_T("没有指定skin对象"),-1,rc,DT_SINGLELINE|DT_VCENTER|DT_VCENTER);
		}else
		{
			Sk3DView view;
			switch(m_rotateDir)
			{
			case RotateX:view.rotateX(m_fRotate);break;
			case RotateY:view.rotateY(m_fRotate);break;
			case RotateZ:view.rotateZ(m_fRotate);break;
			}
			SkMatrix mat;
			view.getMatrix(&mat);
			SIZE szSkin = m_pBgSkin->GetSkinSize();
			int wid = rc.Width()/2+rc.left;
			int hei = rc.Height()/2+rc.top;
			mat.preTranslate(-wid,-hei);
			mat.postTranslate(wid,hei);

			pRT->SetTransform(&mat);
			m_pBgSkin->Draw(pRT,rc,0);
			mat.setIdentity();
			pRT->SetTransform(&mat);
		}
		AfterPaint(pRT,painter);
	}

	void S3DView::OnNextFrame()
	{
		m_iStep ++;
		if(m_iStep==m_nSpeed)
		{
			m_fRotate += 5;
			if(m_fRotate>360) m_fRotate-=360;
			Invalidate();
			m_iStep=0;
		}
	}


	/////////////////////////////////////////////////////////////////////////
SMatrixWindow::SMatrixWindow(void)
:m_fRotate(.0f)
,m_fScaleX(1.0F),m_fScaleY(1.0f)
,m_fSkewX(.0f),m_fSkewY(.0f)
,m_fTransX(.0f),m_fTransY(.0f)
{
}

SMatrixWindow::~SMatrixWindow(void)
{
}

HRESULT SMatrixWindow::OnAttrRotate(const SStringW & strValue,BOOL bLoading)
{
    float fRotate = 0.0f;
    if(1==swscanf(strValue,L"%f",&fRotate))
        m_fRotate = fRotate;
    return bLoading?S_FALSE:S_OK;
}

HRESULT SMatrixWindow::OnAttrScale(const SStringW & strValue,BOOL bLoading)
{
    float fScaleX=0.0f,fScaleY=0.0f;
    if(2!=swscanf(strValue,L"%f,%f",&fScaleX,&fScaleY)) return E_FAIL;
    m_fScaleX = fScaleX;
    m_fScaleY = fScaleY;
    return bLoading?S_FALSE:S_OK;
}

HRESULT SMatrixWindow::OnAttrSkew(const SStringW & strValue,BOOL bLoading)
{
    float fSkewX=0.0f,fSkewY=0.0f;
    if(2!=swscanf(strValue,L"%f,%f",&fSkewX,&fSkewY)) return E_FAIL;
    m_fSkewX = fSkewX;
    m_fSkewY = fSkewY;
    return bLoading?S_FALSE:S_OK;
}

HRESULT SMatrixWindow::OnAttrTranslate(const SStringW & strValue,BOOL bLoading)
{
    float fTransX=0.0f,fTransY=0.0f;
    if(2!=swscanf(strValue,L"%f,%f",&fTransX,&fTransY)) return E_FAIL;
    m_fTransX = fTransX;
    m_fTransY = fTransY;
    return bLoading?S_FALSE:S_OK;
}


void SMatrixWindow::OnPaint(IRenderTarget *pRT)
{
    SPainter painter;
    BeforePaint(pRT,painter);
    CRect rc = GetClientRect();
    if(!m_pBgSkin)
    {
        pRT->DrawText(_T("没有指定skin对象"),-1,rc,DT_SINGLELINE|DT_VCENTER|DT_VCENTER);
    }else
    {
        SMatrix m,m2;
		
         m.rotate(m_fRotate)
         .scale(m_fScaleX,m_fScaleY)
         .shear(m_fSkewX,m_fSkewY)
         .translate(m_fTransX,m_fTransY);
        
        pRT->SetTransform(&m,&m2);
        m_pBgSkin->Draw(pRT,rc,0);
        pRT->SetTransform(&m2);
    }
    AfterPaint(pRT,painter);
}
}
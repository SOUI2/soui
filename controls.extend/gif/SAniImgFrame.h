#pragma once

#include <interface/render-i.h>
namespace SOUI
{
    class SAniImgFrame
    {
    public:
        CAutoRefPtr<IBitmap> pBmp;
        int                  nDelay;
    };

    class SSkinAni : public SSkinObjBase
    {
        SOUI_CLASS_NAME(SSkinAni,L"skinani")
    public:
        SSkinAni():m_nFrames(0),m_iFrame(0),m_pFrames(NULL)
        {

        }

        virtual ~SSkinAni()
        {
            if(m_pFrames) delete [] m_pFrames;
        }


        /**
        * GetStates
        * @brief    获得GIF帧数
        * @return   int -- 帧数
        * Describe  
        */    
        virtual int GetStates(){return m_nFrames;}

        /**
        * GetSkinSize
        * @brief    获得图片大小
        * @return   SIZE -- 图片大小
        * Describe  
        */    
        virtual SIZE GetSkinSize()
        {
            SIZE sz={0};
            if(m_nFrames>0 && m_pFrames)
            {
                sz=m_pFrames[0].pBmp->Size();
            }
            return sz;
        }

        /**
        * GetFrameDelay
        * @brief    获得指定帧的显示时间
        * @param    int iFrame --  帧号,为-1时代表获得当前帧的延时
        * @return   long -- 延时时间(*10ms)
        * Describe  
        */    
        long GetFrameDelay(int iFrame=-1)
        {
            if(iFrame==-1) iFrame=m_iFrame;
            long nRet=-1;
            if(m_nFrames>1 && iFrame>=0 && iFrame<m_nFrames)
            {
                nRet=m_pFrames[iFrame].nDelay;
            }
            return nRet;
        }
        
        IBitmap * GetFrameImage(int iFrame=-1)
        {
            if(iFrame==-1) iFrame=m_iFrame;
            long nRet=-1;
            if(m_nFrames>1 && iFrame>=0 && iFrame<m_nFrames)
            {
                return m_pFrames[iFrame].pBmp;
            }else
            {
                return NULL;
            }
        }


        /**
        * ActiveNextFrame
        * @brief    激活下一帧
        * @return   void 
        * Describe  
        */    
        void ActiveNextFrame()
        {
            if(m_nFrames>1)
            {
                m_iFrame++;
                if(m_iFrame==m_nFrames) m_iFrame=0;
                SelectActiveFrame(m_iFrame);
            }
        }

        /**
        * SelectActiveFrame
        * @brief    激活指定帧
        * @param    int iFrame --  帧号
        * @return   void
        * Describe  
        */    
        void SelectActiveFrame(int iFrame)
        {
            if(m_nFrames>1 && iFrame<m_nFrames)
            {
                m_iFrame = iFrame;
            }
        }

    public:
        virtual int LoadFromFile(LPCTSTR pszFileName){return 0;}
        virtual int LoadFromMemory(LPVOID pData,size_t len){return 0;}
    protected:
        /**
        * Draw
        * @brief    绘制指定帧的GIF图
        * @param    IRenderTarget * pRT --  绘制目标
        * @param    LPCRECT rcDraw --  绘制范围
        * @param    DWORD dwState --  绘制状态，这里被解释为帧号
        * @param    BYTE byAlpha --  透明度
        * @return   void
        * Describe  
        */    
        virtual void _Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState,BYTE byAlpha=0xFF)
        {
            if(m_nFrames == 0 || !m_pFrames) return;
            if(dwState!=-1) SelectActiveFrame(dwState);
            CRect rcSrc(CPoint(0,0),GetSkinSize());
            pRT->DrawBitmapEx(rcDraw,m_pFrames[m_iFrame].pBmp,rcSrc,EM_STRETCH,byAlpha);
        }

        int m_nFrames;
        int m_iFrame;

        SAniImgFrame * m_pFrames;
    };

}
#include "gdialpha.h"
#include <malloc.h>

namespace SOUI
{

#define  MAX_ALPHABUF    1<<16

BYTE CGdiAlpha::s_byAlphaBack[MAX_ALPHABUF];

LPBYTE CGdiAlpha::ALPHABACKUP(BITMAP *pBitmap,int x,int y,int cx,int cy)
{
    LPBYTE lpAlpha=s_byAlphaBack;
    if(x+cx>=pBitmap->bmWidth) cx=pBitmap->bmWidth-x;
    if(y+cy>=pBitmap->bmHeight) cy=pBitmap->bmHeight-y;
    if(cx<0 || cy<0 ||pBitmap->bmBits==NULL) return NULL;

    if(cx*cy>MAX_ALPHABUF) lpAlpha=(LPBYTE)malloc(cx*cy);
    LPBYTE lpBits=NULL;
    for(int iRow=0; iRow<cy; iRow++)
    {
        lpBits=(LPBYTE)pBitmap->bmBits+(y+iRow)*pBitmap->bmWidth*4+x*4;
        lpBits+=3;
        for(int iCol=0; iCol<cx; iCol++)
        {
            lpAlpha[iRow*cx+iCol]=*lpBits;
            lpBits+=4;
        }
    }
    return lpAlpha;
}

//恢复位图的Alpha通道
void CGdiAlpha::ALPHARESTORE(BITMAP *pBitmap,int x,int y,int cx,int cy,LPBYTE lpAlpha)
{
    if(x+cx>=pBitmap->bmWidth) cx=pBitmap->bmWidth-x;
    if(y+cy>=pBitmap->bmHeight) cy=pBitmap->bmHeight-y;
    if(cx<0 || cy<0) return;
    LPBYTE lpBits=NULL;
    for(int iRow=0; iRow<cy; iRow++)
    {
        lpBits=(LPBYTE)pBitmap->bmBits+(y+iRow)*pBitmap->bmWidth*4+x*4;
        lpBits+=3;
        for(int iCol=0; iCol<cx; iCol++)
        {
            *lpBits=lpAlpha[iRow*cx+iCol];
            lpBits+=4;
        }
    }
    if(lpAlpha!=s_byAlphaBack)    free(lpAlpha);
}

BOOL CGdiAlpha::AlphaBackup(HDC hdc,LPCRECT pRect,ALPHAINFO &alphaInfo)
{
    alphaInfo.lpBuf=NULL;
    HBITMAP hBmp=(HBITMAP)GetCurrentObject(hdc,OBJ_BITMAP);
    SASSERT(hBmp);
    GetObject(hBmp,sizeof(BITMAP),&alphaInfo.bm);

    if(alphaInfo.bm.bmBitsPixel!=32) return FALSE;
    alphaInfo.rc=*pRect;
    //draw rectangle need extend the right and bottom 1 px;
    alphaInfo.rc.right ++;
    alphaInfo.rc.bottom ++;
    POINT pt;
    GetViewportOrgEx(hdc,&pt);
    RECT rcImg= {0,0,alphaInfo.bm.bmWidth,alphaInfo.bm.bmHeight};
    OffsetRect(&alphaInfo.rc,pt.x,pt.y);
    IntersectRect(&alphaInfo.rc,&alphaInfo.rc,&rcImg);
    alphaInfo.lpBuf=ALPHABACKUP(&alphaInfo.bm,alphaInfo.rc.left,alphaInfo.rc.top,alphaInfo.rc.right - alphaInfo.rc.left, alphaInfo.rc.bottom - alphaInfo.rc.top);
    return TRUE;
}

void CGdiAlpha::AlphaRestore(ALPHAINFO &alphaInfo)
{
    if(!alphaInfo.lpBuf) return;
    ALPHARESTORE(&alphaInfo.bm,alphaInfo.rc.left,alphaInfo.rc.top,alphaInfo.rc.right - alphaInfo.rc.left, alphaInfo.rc.bottom - alphaInfo.rc.top,alphaInfo.lpBuf);
}

}//namespace SOUI
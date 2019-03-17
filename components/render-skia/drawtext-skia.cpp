#include "drawtext-skia.h"

#define DT_ELLIPSIS (DT_PATH_ELLIPSIS|DT_END_ELLIPSIS|DT_WORD_ELLIPSIS)
#define CH_ELLIPSIS L"..."
#define MAX(a,b)    (((a) > (b)) ? (a) : (b))

static size_t breakTextEx(const SkPaint *pPaint, const wchar_t* textD, size_t length, SkScalar maxWidth,
                          SkScalar* measuredWidth) 
{
    size_t nLineLen=pPaint->breakText(textD,length*sizeof(wchar_t),maxWidth,measuredWidth,SkPaint::kForward_TextBufferDirection);
    if(nLineLen==0) return 0;
    nLineLen/=sizeof(wchar_t);

    const wchar_t * p=textD;
    for(size_t i=0;i<nLineLen;i++, p++)
    {
        if(*p == L'\r')
        {
            if(i<nLineLen-1 && p[1]==L'\n') return i+2;
            else return i;
        }else if(*p == L'\n')
        {
            return i+1;
        }
    }
    return nLineLen;
}

SkRect DrawText_Skia(SkCanvas* canvas,const wchar_t *text,int len,SkRect box,const SkPaint& paint,UINT uFormat)
{
	if(len<0)	len = wcslen(text);
    SkTextLayoutEx layout;
    layout.init(text,len,box,paint,uFormat);
    
    return layout.draw(canvas);
}

//////////////////////////////////////////////////////////////////////////
void SkTextLayoutEx::init( const wchar_t text[], size_t length,SkRect rc, const SkPaint &paint,UINT uFormat )
{
    if(uFormat & DT_NOPREFIX)
    {
        m_text.setCount(length);
        memcpy(m_text.begin(),text,length*sizeof(wchar_t));
    }else
    {
        m_prefix.reset();
        SkTDArray<wchar_t> tmp;
        tmp.setCount(length);
        memcpy(tmp.begin(),text,length*sizeof(wchar_t));
        for(int i=0;i<tmp.count();i++)
        {
            if(tmp[i]==L'&' && i+1 < tmp.count())
            {
				if(tmp[i+1]==L'&')
				{
					tmp.remove(i,1);
				}else
				{
					tmp.remove(i,1);
					m_prefix.push(i);
				}
            }
        }
        m_text=tmp;
    }

    m_paint=&paint;
    m_rcBound=rc;
    m_uFormat=uFormat;
    buildLines();
}

void SkTextLayoutEx::buildLines()
{
    m_lines.reset();

    if(m_uFormat & DT_SINGLELINE)
    {
        m_lines.push(0);
    }else
    {
        const wchar_t *text = m_text.begin();
        const wchar_t* stop = m_text.begin() + m_text.count();
        SkScalar maxWid=m_rcBound.width();
        if(m_uFormat & DT_CALCRECT && maxWid < 1.0f)
            maxWid=10000.0f;
        int lineHead=0;
        while(lineHead<m_text.count())
        {
            m_lines.push(lineHead);
            size_t line_len = breakTextEx(m_paint,text, stop - text, maxWid,0);
			if (0 == line_len) break;
			text += line_len;
            lineHead += line_len;
        };
    }
}

SkScalar SkTextLayoutEx::drawLine( SkCanvas *canvas, SkScalar x, SkScalar y, int iBegin,int iEnd )
{
    const wchar_t *text=m_text.begin()+iBegin;

    if(!(m_uFormat & DT_CALCRECT))
    {
        canvas->drawText(text,(iEnd-iBegin)*sizeof(wchar_t),x,y,*m_paint);
        int i=0;
        while(i<m_prefix.count())
        {
            if(m_prefix[i]>=iBegin)
                break;
            i++;
        }
        
        SkScalar xBase = x;
        if(m_paint->getTextAlign() != SkPaint::kLeft_Align)
        {
            SkScalar nTextWidth = m_paint->measureText(text,(iEnd-iBegin)*sizeof(wchar_t));
            switch(m_paint->getTextAlign())
            {
            case SkPaint::kCenter_Align:
                xBase = x - nTextWidth/2.0f;
                break;
            case SkPaint::kRight_Align:
                xBase = x- nTextWidth;
                break;
            }
        }
        
        while(i<m_prefix.count() && m_prefix[i]<iEnd)
        {
            SkScalar x1 = m_paint->measureText(text,(m_prefix[i]-iBegin)*sizeof(wchar_t));
            SkScalar x2 = m_paint->measureText(text,(m_prefix[i]-iBegin+1)*sizeof(wchar_t));
            canvas->drawLine(xBase+x1,y+1,xBase+x2,y+1,*m_paint); //绘制下划线
            i++;
        }
    }
    return m_paint->measureText(text,(iEnd-iBegin)*sizeof(wchar_t));
}

SkScalar SkTextLayoutEx::drawLineEndWithEllipsis( SkCanvas *canvas, SkScalar x, SkScalar y, int iBegin,int iEnd,SkScalar maxWidth )
{
    SkScalar widReq=m_paint->measureText(m_text.begin()+iBegin,(iEnd-iBegin)*sizeof(wchar_t));
    if(widReq<=m_rcBound.width())
    {
        return drawLine(canvas,x,y,iBegin,iEnd);
    }else
    {
        SkScalar fWidEllipsis = m_paint->measureText(CH_ELLIPSIS,sizeof(CH_ELLIPSIS)-sizeof(wchar_t));
        maxWidth-=fWidEllipsis;
        
        int i=0;
        const wchar_t *text=m_text.begin()+iBegin;
        SkScalar fWid=0.0f;
        while(i<(iEnd-iBegin))
        {
            SkScalar fWord = m_paint->measureText(text+i,sizeof(wchar_t));
            if(fWid + fWord > maxWidth) break;
            fWid += fWord;
            i++;
        }
        if(!(m_uFormat & DT_CALCRECT))
        {
            wchar_t *pbuf=new wchar_t[i+3];
            memcpy(pbuf,text,i*sizeof(wchar_t));
            memcpy(pbuf+i,CH_ELLIPSIS,3*sizeof(wchar_t));
            canvas->drawText(pbuf,(i+3)*sizeof(wchar_t),x,y,*m_paint);
            delete []pbuf;
        }
        return fWid+fWidEllipsis;
    }
}

SkRect SkTextLayoutEx::draw( SkCanvas* canvas )
{
    SkPaint::FontMetrics metrics;

    m_paint->getFontMetrics(&metrics);
    float lineSpan = metrics.fBottom-metrics.fTop;

    SkRect rcDraw = m_rcBound;

    float  x;
    switch (m_paint->getTextAlign()) 
    {
    case SkPaint::kCenter_Align:
        x = SkScalarHalf(m_rcBound.width());
        break;
    case SkPaint::kRight_Align:
        x = m_rcBound.width();
        break;
    default://SkPaint::kLeft_Align:
        x = 0;
        break;
    }
    x += m_rcBound.fLeft;

    canvas->save();

    canvas->clipRect(m_rcBound);

    float height = m_rcBound.height();
    float y=m_rcBound.fTop - metrics.fTop;
    if(m_uFormat & DT_SINGLELINE)
    {//单行显示
        rcDraw.fBottom = rcDraw.fTop + lineSpan;
        if(m_uFormat & DT_VCENTER) 
        {
            y += (height - lineSpan)/2.0f;
        }
		else if (m_uFormat & DT_BOTTOM)
		{
			y += (height - lineSpan);
		}
        if(m_uFormat & DT_ELLIPSIS)
        {//只支持在行尾增加省略号
            rcDraw.fRight = rcDraw.fLeft + drawLineEndWithEllipsis(canvas,x,y,0,m_text.count(),m_rcBound.width());
        }else
        {
            rcDraw.fRight = rcDraw.fLeft + drawLine(canvas,x,y,0,m_text.count());
        }
    }else
    {//多行显示
        SkScalar maxLineWid=0;
        int iLine = 0;
        while(iLine<m_lines.count())
        {
            if(y + lineSpan + metrics.fTop >= m_rcBound.fBottom) 
                break;  //the last visible line
            int iBegin=m_lines[iLine];
            int iEnd = iLine<(m_lines.count()-1)?m_lines[iLine+1]:m_text.count();
            SkScalar lineWid = drawLine(canvas,x,y,iBegin,iEnd);
            maxLineWid = MAX(maxLineWid,lineWid);
            y += lineSpan;
            iLine ++;
        }
        if(iLine<m_lines.count())
        {//draw the last visible line
            int iBegin=m_lines[iLine];
            int iEnd = iLine<(m_lines.count()-1)?m_lines[iLine+1]:m_text.count();
            SkScalar lineWid;
            if(m_uFormat & DT_ELLIPSIS)
            {//只支持在行尾增加省略号
                lineWid=drawLineEndWithEllipsis(canvas,x,y,iBegin,iEnd,m_rcBound.width());
            }else
            {
                lineWid=drawLine(canvas,x,y,iBegin,iEnd);
            }
            maxLineWid = MAX(maxLineWid,lineWid);
            y += lineSpan;
        }
        rcDraw.fRight = rcDraw.fLeft + maxLineWid;
        rcDraw.fBottom = y + metrics.fTop;
    }
    canvas->restore();
    return rcDraw;
}

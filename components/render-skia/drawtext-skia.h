#pragma once

#include <core/SkPaint.h>
#include <core/SkCanvas.h>
#include <core/sktdarray.h>

class SkTextLayoutEx {
public:
    //not support for DT_PREFIXONLY
    void init(const wchar_t text[], size_t length,SkRect rc, const SkPaint &paint,UINT uFormat);

    SkRect draw(SkCanvas* canvas);

private:
    SkScalar drawLineEndWithEllipsis(SkCanvas *canvas, SkScalar x, SkScalar y, int iBegin,int iEnd,SkScalar maxWidth);

    SkScalar drawLine(SkCanvas *canvas, SkScalar x, SkScalar y, int iBegin,int iEnd);

    void buildLines();

private:
    SkTDArray<wchar_t> m_text;   //文本内容
    SkTDArray<int>  m_prefix;    //前缀符索引
    SkTDArray<int> m_lines;      //分行索引
    UINT            m_uFormat;    //显示标志
    SkRect          m_rcBound;    //限制矩形
    const SkPaint  *m_paint;
};


SkRect DrawText_Skia(SkCanvas* canvas,const wchar_t *text,int len,SkRect box,const SkPaint& paint,UINT uFormat);

// Windows Template Library - WTL version 8.1
// Copyright (C) Microsoft Corporation. All rights reserved.
//
// This file is a part of the Windows Template Library.
// The use and distribution terms for this software are covered by the
// Common Public License 1.0 (http://opensource.org/licenses/cpl1.0.php)
// which can be found in the file CPL.TXT at the root of this distribution.
// By using this software in any fashion, you are agreeing to be bound by
// the terms of this license. You must not remove this notice, or
// any other, from this software.

#ifndef __SOUIMISC_H__
#define __SOUIMISC_H__

#pragma once


///////////////////////////////////////////////////////////////////////////////
// Classes in this file:
//
// CSize
// CPoint
// CRect


namespace SOUI
{

// forward declarations
class CSize;
class CPoint;
class CRect;

///////////////////////////////////////////////////////////////////////////////
// CSize - Wrapper for Windows SIZE structure.

class CSize : public SIZE
{
public:
// Constructors
    CSize()
    {
        cx = 0;
        cy = 0;
    }

    CSize(int initCX, int initCY)
    {
        cx = initCX;
        cy = initCY;
    }

    CSize(SIZE initSize)
    {
        *(SIZE*)this = initSize;
    }

    CSize(POINT initPt)
    {
        *(POINT*)this = initPt;
    }

    CSize(DWORD dwSize)
    {
        cx = (short)LOWORD(dwSize);
        cy = (short)HIWORD(dwSize);
    }

// Operations
    BOOL operator ==(SIZE size) const
    {
        return (cx == size.cx && cy == size.cy);
    }

    BOOL operator !=(SIZE size) const
    {
        return (cx != size.cx || cy != size.cy);
    }

    void operator +=(SIZE size)
    {
        cx += size.cx;
        cy += size.cy;
    }

    void operator -=(SIZE size)
    {
        cx -= size.cx;
        cy -= size.cy;
    }

    void SetSize(int CX, int CY)
    {
        cx = CX;
        cy = CY;
    }

// Operators returning CSize values
    CSize operator +(SIZE size) const
    {
        return CSize(cx + size.cx, cy + size.cy);
    }

    CSize operator -(SIZE size) const
    {
        return CSize(cx - size.cx, cy - size.cy);
    }

    CSize operator -() const
    {
        return CSize(-cx, -cy);
    }

// Operators returning CPoint values
    CPoint operator +(POINT point) const;
    CPoint operator -(POINT point) const;

// Operators returning CRect values
    CRect operator +(const RECT* lpRect) const;
    CRect operator -(const RECT* lpRect) const;
};


///////////////////////////////////////////////////////////////////////////////
// CPoint - Wrapper for Windows POINT structure.

class CPoint : public POINT
{
public:
// Constructors
    CPoint()
    {
        x = 0;
        y = 0;
    }

    CPoint(int initX, int initY)
    {
        x = initX;
        y = initY;
    }

    CPoint(POINT initPt)
    {
        *(POINT*)this = initPt;
    }

    CPoint(SIZE initSize)
    {
        *(SIZE*)this = initSize;
    }

    CPoint(DWORD dwPoint)
    {
        x = (short)LOWORD(dwPoint);
        y = (short)HIWORD(dwPoint);
    }

// Operations
    void Offset(int xOffset, int yOffset)
    {
        x += xOffset;
        y += yOffset;
    }

    void Offset(POINT point)
    {
        x += point.x;
        y += point.y;
    }

    void Offset(SIZE size)
    {
        x += size.cx;
        y += size.cy;
    }

    BOOL operator ==(POINT point) const
    {
        return (x == point.x && y == point.y);
    }

    BOOL operator !=(POINT point) const
    {
        return (x != point.x || y != point.y);
    }

    void operator +=(SIZE size)
    {
        x += size.cx;
        y += size.cy;
    }

    void operator -=(SIZE size)
    {
        x -= size.cx;
        y -= size.cy;
    }

    void operator +=(POINT point)
    {
        x += point.x;
        y += point.y;
    }

    void operator -=(POINT point)
    {
        x -= point.x;
        y -= point.y;
    }

    void SetPoint(int X, int Y)
    {
        x = X;
        y = Y;
    }

// Operators returning CPoint values
    CPoint operator +(SIZE size) const
    {
        return CPoint(x + size.cx, y + size.cy);
    }

    CPoint operator -(SIZE size) const
    {
        return CPoint(x - size.cx, y - size.cy);
    }

    CPoint operator -() const
    {
        return CPoint(-x, -y);
    }

    CPoint operator +(POINT point) const
    {
        return CPoint(x + point.x, y + point.y);
    }

// Operators returning CSize values
    CSize operator -(POINT point) const
    {
        return CSize(x - point.x, y - point.y);
    }

// Operators returning CRect values
    CRect operator +(const RECT* lpRect) const;
    CRect operator -(const RECT* lpRect) const;
};


///////////////////////////////////////////////////////////////////////////////
// CRect - Wrapper for Windows RECT structure.

class CRect : public RECT
{
public:
// Constructors
    CRect()
    {
        left = 0;
        top = 0;
        right = 0;
        bottom = 0;
    }

    CRect(int l, int t, int r, int b)
    {
        left = l;
        top = t;
        right = r;
        bottom = b;
    }

    CRect(const RECT& srcRect)
    {
        ::CopyRect(this, &srcRect);
    }

    CRect(LPCRECT lpSrcRect)
    {
        ::CopyRect(this, lpSrcRect);
    }

    CRect(POINT point, SIZE size)
    {
        right = (left = point.x) + size.cx;
        bottom = (top = point.y) + size.cy;
    }

    CRect(POINT topLeft, POINT bottomRight)
    {
        left = topLeft.x;
        top = topLeft.y;
        right = bottomRight.x;
        bottom = bottomRight.y;
    }

// Attributes (in addition to RECT members)
    int Width() const
    {
        return right - left;
    }

    int Height() const
    {
        return bottom - top;
    }

    CSize Size() const
    {
        return CSize(right - left, bottom - top);
    }

    CPoint& TopLeft()
    {
        return *((CPoint*)this);
    }

    CPoint& BottomRight()
    {
        return *((CPoint*)this + 1);
    }

    const CPoint& TopLeft() const
    {
        return *((CPoint*)this);
    }

    const CPoint& BottomRight() const
    {
        return *((CPoint*)this + 1);
    }

    CPoint CenterPoint() const
    {
        return CPoint((left + right) / 2, (top + bottom) / 2);
    }

    // convert between CRect and LPRECT/LPCRECT (no need for &)
    operator LPRECT()
    {
        return this;
    }

    operator LPCRECT() const
    {
        return this;
    }

    BOOL IsRectEmpty() const
    {
        return ::IsRectEmpty(this);
    }

    BOOL IsRectNull() const
    {
        return (left == 0 && right == 0 && top == 0 && bottom == 0);
    }

    BOOL PtInRect(POINT point) const
    {
        return ::PtInRect(this, point);
    }

// Operations
    void SetRect(int x1, int y1, int x2, int y2)
    {
        ::SetRect(this, x1, y1, x2, y2);
    }

    void SetRect(POINT topLeft, POINT bottomRight)
    {
        ::SetRect(this, topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
    }

    void SetRectEmpty()
    {
        ::SetRectEmpty(this);
    }

    void CopyRect(LPCRECT lpSrcRect)
    {
        ::CopyRect(this, lpSrcRect);
    }

    BOOL EqualRect(LPCRECT lpRect) const
    {
        return ::EqualRect(this, lpRect);
    }

    void InflateRect(int x, int y)
    {
        ::InflateRect(this, x, y);
    }

    void InflateRect(SIZE size)
    {
        ::InflateRect(this, size.cx, size.cy);
    }

    void InflateRect(LPCRECT lpRect)
    {
        left -= lpRect->left;
        top -= lpRect->top;
        right += lpRect->right;
        bottom += lpRect->bottom;
    }

    void InflateRect(int l, int t, int r, int b)
    {
        left -= l;
        top -= t;
        right += r;
        bottom += b;
    }

    void DeflateRect(int x, int y)
    {
        ::InflateRect(this, -x, -y);
    }

    void DeflateRect(SIZE size)
    {
        ::InflateRect(this, -size.cx, -size.cy);
    }

    void DeflateRect(LPCRECT lpRect)
    {
        left += lpRect->left;
        top += lpRect->top;
        right -= lpRect->right;
        bottom -= lpRect->bottom;
    }

    void DeflateRect(int l, int t, int r, int b)
    {
        left += l;
        top += t;
        right -= r;
        bottom -= b;
    }

    void OffsetRect(int x, int y)
    {
        ::OffsetRect(this, x, y);
    }
    void OffsetRect(SIZE size)
    {
        ::OffsetRect(this, size.cx, size.cy);
    }

    void OffsetRect(POINT point)
    {
        ::OffsetRect(this, point.x, point.y);
    }

    void NormalizeRect()
    {
        int nTemp;
        if (left > right)
        {
            nTemp = left;
            left = right;
            right = nTemp;
        }
        if (top > bottom)
        {
            nTemp = top;
            top = bottom;
            bottom = nTemp;
        }
    }

    // absolute position of rectangle
    void MoveToY(int y)
    {
        bottom = Height() + y;
        top = y;
    }

    void MoveToX(int x)
    {
        right = Width() + x;
        left = x;
    }

    void MoveToXY(int x, int y)
    {
        MoveToX(x);
        MoveToY(y);
    }

    void MoveToXY(POINT pt)
    {
        MoveToX(pt.x);
        MoveToY(pt.y);
    }

    // operations that fill '*this' with result
    BOOL IntersectRect(LPCRECT lpRect1, LPCRECT lpRect2)
    {
        return ::IntersectRect(this, lpRect1, lpRect2);
    }

    BOOL UnionRect(LPCRECT lpRect1, LPCRECT lpRect2)
    {
        return ::UnionRect(this, lpRect1, lpRect2);
    }

    BOOL SubtractRect(LPCRECT lpRectSrc1, LPCRECT lpRectSrc2)
    {
        return ::SubtractRect(this, lpRectSrc1, lpRectSrc2);
    }

// Additional Operations
    void operator =(const RECT& srcRect)
    {
        ::CopyRect(this, &srcRect);
    }

    BOOL operator ==(const RECT& rect) const
    {
        return ::EqualRect(this, &rect);
    }

    BOOL operator !=(const RECT& rect) const
    {
        return !::EqualRect(this, &rect);
    }

    void operator +=(POINT point)
    {
        ::OffsetRect(this, point.x, point.y);
    }

    void operator +=(SIZE size)
    {
        ::OffsetRect(this, size.cx, size.cy);
    }

    void operator +=(LPCRECT lpRect)
    {
        InflateRect(lpRect);
    }

    void operator -=(POINT point)
    {
        ::OffsetRect(this, -point.x, -point.y);
    }

    void operator -=(SIZE size)
    {
        ::OffsetRect(this, -size.cx, -size.cy);
    }

    void operator -=(LPCRECT lpRect)
    {
        DeflateRect(lpRect);
    }

    void operator &=(const RECT& rect)
    {
        ::IntersectRect(this, this, &rect);
    }

    void operator |=(const RECT& rect)
    {
        ::UnionRect(this, this, &rect);
    }

// Operators returning CRect values
    CRect operator +(POINT pt) const
    {
        CRect rect(*this);
        ::OffsetRect(&rect, pt.x, pt.y);
        return rect;
    }

    CRect operator -(POINT pt) const
    {
        CRect rect(*this);
        ::OffsetRect(&rect, -pt.x, -pt.y);
        return rect;
    }

    CRect operator +(LPCRECT lpRect) const
    {
        CRect rect(this);
        rect.InflateRect(lpRect);
        return rect;
    }

    CRect operator +(SIZE size) const
    {
        CRect rect(*this);
        ::OffsetRect(&rect, size.cx, size.cy);
        return rect;
    }

    CRect operator -(SIZE size) const
    {
        CRect rect(*this);
        ::OffsetRect(&rect, -size.cx, -size.cy);
        return rect;
    }

    CRect operator -(LPCRECT lpRect) const
    {
        CRect rect(this);
        rect.DeflateRect(lpRect);
        return rect;
    }

    CRect operator &(const RECT& rect2) const
    {
        CRect rect;
        ::IntersectRect(&rect, this, &rect2);
        return rect;
    }

    CRect operator |(const RECT& rect2) const
    {
        CRect rect;
        ::UnionRect(&rect, this, &rect2);
        return rect;
    }

    CRect MulDiv(int nMultiplier, int nDivisor) const
    {
        return CRect(
                   ::MulDiv(left, nMultiplier, nDivisor),
                   ::MulDiv(top, nMultiplier, nDivisor),
                   ::MulDiv(right, nMultiplier, nDivisor),
                   ::MulDiv(bottom, nMultiplier, nDivisor));
    }
};


// CSize implementation

inline CPoint CSize::operator +(POINT point) const
{
    return CPoint(cx + point.x, cy + point.y);
}

inline CPoint CSize::operator -(POINT point) const
{
    return CPoint(cx - point.x, cy - point.y);
}

inline CRect CSize::operator +(const RECT* lpRect) const
{
    return CRect(lpRect) + *this;
}

inline CRect CSize::operator -(const RECT* lpRect) const
{
    return CRect(lpRect) - *this;
}


// CPoint implementation

inline CRect CPoint::operator +(const RECT* lpRect) const
{
    return CRect(lpRect) + *this;
}

inline CRect CPoint::operator -(const RECT* lpRect) const
{
    return CRect(lpRect) - *this;
}


template <class Num>
inline CSize operator *(SIZE s, Num n)
{
    return CSize((int)(s.cx * n), (int)(s.cy * n));
};

template <class Num>
inline void operator *=(SIZE & s, Num n)
{
    s = s * n;
};

template <class Num>
inline CSize operator /(SIZE s, Num n)
{
    return CSize((int)(s.cx / n), (int)(s.cy / n));
};

template <class Num>
inline void operator /=(SIZE & s, Num n)
{
    s = s / n;
};


///////////////////////////////////////////////////////////////////////////////
// CRgn

template <bool t_bManaged>
class CRgnT
{
public:
    // Data members
    HRGN m_hRgn;

    // Constructor/destructor/operators
    CRgnT(HRGN hRgn = NULL) : m_hRgn(hRgn)
    { }

    ~CRgnT()
    {
        if(t_bManaged && m_hRgn != NULL)
            DeleteObject();
    }

    CRgnT<t_bManaged>& operator =(HRGN hRgn)
    {
        Attach(hRgn);
        return *this;
    }

    void Attach(HRGN hRgn)
    {
        if(t_bManaged && m_hRgn != NULL && m_hRgn != hRgn)
            ::DeleteObject(m_hRgn);
        m_hRgn = hRgn;
    }

    HRGN Detach()
    {
        HRGN hRgn = m_hRgn;
        m_hRgn = NULL;
        return hRgn;
    }

    operator HRGN() const { return m_hRgn; }

    bool IsNull() const { return (m_hRgn == NULL); }

    // Create methods
    HRGN CreateRectRgn(int x1, int y1, int x2, int y2)
    {
        SASSERT(m_hRgn == NULL);
        m_hRgn = ::CreateRectRgn(x1, y1, x2, y2);
        return m_hRgn;
    }

    HRGN CreateRectRgnIndirect(LPCRECT lpRect)
    {
        SASSERT(m_hRgn == NULL);
        m_hRgn = ::CreateRectRgnIndirect(lpRect);
        return m_hRgn;
    }

#ifndef _WIN32_WCE
    HRGN CreateEllipticRgn(int x1, int y1, int x2, int y2)
    {
        SASSERT(m_hRgn == NULL);
        m_hRgn = ::CreateEllipticRgn(x1, y1, x2, y2);
        return m_hRgn;
    }

    HRGN CreateEllipticRgnIndirect(LPCRECT lpRect)
    {
        SASSERT(m_hRgn == NULL);
        m_hRgn = ::CreateEllipticRgnIndirect(lpRect);
        return m_hRgn;
    }

    HRGN CreatePolygonRgn(LPPOINT lpPoints, int nCount, int nMode)
    {
        SASSERT(m_hRgn == NULL);
        m_hRgn = ::CreatePolygonRgn(lpPoints, nCount, nMode);
        return m_hRgn;
    }

    HRGN CreatePolyPolygonRgn(LPPOINT lpPoints, LPINT lpPolyCounts, int nCount, int nPolyFillMode)
    {
        SASSERT(m_hRgn == NULL);
        m_hRgn = ::CreatePolyPolygonRgn(lpPoints, lpPolyCounts, nCount, nPolyFillMode);
        return m_hRgn;
    }

    HRGN CreateRoundRectRgn(int x1, int y1, int x2, int y2, int x3, int y3)
    {
        SASSERT(m_hRgn == NULL);
        m_hRgn = ::CreateRoundRectRgn(x1, y1, x2, y2, x3, y3);
        return m_hRgn;
    }

    HRGN CreateFromPath(HDC hDC)
    {
        SASSERT(m_hRgn == NULL);
        SASSERT(hDC != NULL);
        m_hRgn = ::PathToRegion(hDC);
        return m_hRgn;
    }

    HRGN CreateFromData(const XFORM* lpXForm, int nCount, const RGNDATA* pRgnData)
    {
        SASSERT(m_hRgn == NULL);
        m_hRgn = ::ExtCreateRegion(lpXForm, nCount, pRgnData);
        return m_hRgn;
    }
#endif // !_WIN32_WCE

    BOOL DeleteObject()
    {
        SASSERT(m_hRgn != NULL);
        BOOL bRet = ::DeleteObject(m_hRgn);
        if(bRet)
            m_hRgn = NULL;
        return bRet;
    }

    // Operations
    void SetRectRgn(int x1, int y1, int x2, int y2)
    {
        SASSERT(m_hRgn != NULL);
        ::SetRectRgn(m_hRgn, x1, y1, x2, y2);
    }

    void SetRectRgn(LPCRECT lpRect)
    {
        SASSERT(m_hRgn != NULL);
        ::SetRectRgn(m_hRgn, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
    }

    int CombineRgn(HRGN hRgnSrc1, HRGN hRgnSrc2, int nCombineMode)
    {
        SASSERT(m_hRgn != NULL);
        return ::CombineRgn(m_hRgn, hRgnSrc1, hRgnSrc2, nCombineMode);
    }

    int CombineRgn(HRGN hRgnSrc, int nCombineMode)
    {
        SASSERT(m_hRgn != NULL);
        return ::CombineRgn(m_hRgn, m_hRgn, hRgnSrc, nCombineMode);
    }

    int CopyRgn(HRGN hRgnSrc)
    {
        SASSERT(m_hRgn != NULL);
        return ::CombineRgn(m_hRgn, hRgnSrc, NULL, RGN_COPY);
    }

    BOOL EqualRgn(HRGN hRgn) const
    {
        SASSERT(m_hRgn != NULL);
        return ::EqualRgn(m_hRgn, hRgn);
    }

    int OffsetRgn(int x, int y)
    {
        SASSERT(m_hRgn != NULL);
        return ::OffsetRgn(m_hRgn, x, y);
    }

    int OffsetRgn(POINT point)
    {
        SASSERT(m_hRgn != NULL);
        return ::OffsetRgn(m_hRgn, point.x, point.y);
    }

    int GetRgnBox(LPRECT lpRect) const
    {
        SASSERT(m_hRgn != NULL);
        return ::GetRgnBox(m_hRgn, lpRect);
    }

    BOOL PtInRegion(int x, int y) const
    {
        SASSERT(m_hRgn != NULL);
        return ::PtInRegion(m_hRgn, x, y);
    }

    BOOL PtInRegion(POINT point) const
    {
        SASSERT(m_hRgn != NULL);
        return ::PtInRegion(m_hRgn, point.x, point.y);
    }

    BOOL RectInRegion(LPCRECT lpRect) const
    {
        SASSERT(m_hRgn != NULL);
        return ::RectInRegion(m_hRgn, lpRect);
    }

    int GetRegionData(LPRGNDATA lpRgnData, int nDataSize) const
    {
        SASSERT(m_hRgn != NULL);
        return (int)::GetRegionData(m_hRgn, nDataSize, lpRgnData);
    }
};

typedef CRgnT<false>   CRgnHandle;
typedef CRgnT<true>    CRgn;

}; // namespace SOUI

#endif // __SOUIMISC_H__

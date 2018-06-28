/*
 * Cursor and icon support
 *
 * Copyright 1995 Alexandre Julliard
 *           1996 Martin Von Loewis
 *           1997 Alex Korobka
 *           1998 Turchanov Sergey
 *           2007 Henri Verbeet
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <windows.h>

#define WARN printf
#pragma warning(disable:4018)
#pragma pack(push,1)
typedef struct {
    BYTE bWidth;
    BYTE bHeight;
    BYTE bColorCount;
    BYTE bReserved;
    WORD xHotspot;
    WORD yHotspot;
    DWORD dwDIBSize;
    DWORD dwDIBOffset;
} CURSORICONFILEDIRENTRY;

typedef struct
{
    WORD                idReserved;
    WORD                idType;
    WORD                idCount;
    CURSORICONFILEDIRENTRY  idEntries[1];
} CURSORICONFILEDIR;


typedef struct
{
	BYTE   bWidth;
	BYTE   bHeight;
	BYTE   bColorCount;
	BYTE   bReserved;
} ICONRESDIR;

typedef struct
{
	WORD   wWidth;
	WORD   wHeight;
} CURSORDIR;

typedef struct
{   union
{ ICONRESDIR icon;
CURSORDIR  cursor;
} ResInfo;
WORD   wPlanes;
WORD   wBitCount;
DWORD  dwBytesInRes;
WORD   wResId;
} CURSORICONDIRENTRY;

typedef struct
{
	WORD                idReserved;
	WORD                idType;
	WORD                idCount;
	CURSORICONDIRENTRY  idEntries[1];
} CURSORICONDIR;


#pragma pack(pop)


static HDC screen_dc;

static const WCHAR DISPLAYW[] = {'D','I','S','P','L','A','Y',0};


/***********************************************************************
 *             map_fileW
 *
 * Helper function to map a file to memory:
 *  name			-	file name
 *  [RETURN] ptr		-	pointer to mapped file
 *  [RETURN] filesize           -       pointer size of file to be stored if not NULL
 */
static const void *map_fileW( LPCWSTR name, LPDWORD filesize )
{
    HANDLE hFile, hMapping;
    LPVOID ptr = NULL;

    hFile = CreateFileW( name, GENERIC_READ, FILE_SHARE_READ, NULL,
                         OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, 0 );
    if (hFile != INVALID_HANDLE_VALUE)
    {
        hMapping = CreateFileMappingW( hFile, NULL, PAGE_READONLY, 0, 0, NULL );
        if (hMapping)
        {
            ptr = MapViewOfFile( hMapping, FILE_MAP_READ, 0, 0, 0 );
            CloseHandle( hMapping );
            if (filesize)
                *filesize = GetFileSize( hFile, NULL );
        }
        CloseHandle( hFile );
    }
    return ptr;
}


/***********************************************************************
 *          get_dib_image_size
 *
 * Return the size of a DIB bitmap in bytes.
 */
static int get_dib_image_size( int width, int height, int depth )
{
    return (((width * depth + 31) / 8) & ~3) * abs( height );
}


/***********************************************************************
 *           bitmap_info_size
 *
 * Return the size of the bitmap info structure including color table.
 */
static int bitmap_info_size( const BITMAPINFO * info, WORD coloruse )
{
    unsigned int colors, size, masks = 0;

    if (info->bmiHeader.biSize == sizeof(BITMAPCOREHEADER))
    {
        const BITMAPCOREHEADER *core = (const BITMAPCOREHEADER *)info;
        colors = (core->bcBitCount <= 8) ? 1 << core->bcBitCount : 0;
        return sizeof(BITMAPCOREHEADER) + colors *
             ((coloruse == DIB_RGB_COLORS) ? sizeof(RGBTRIPLE) : sizeof(WORD));
    }
    else  /* assume BITMAPINFOHEADER */
    {
        colors = info->bmiHeader.biClrUsed;
        if (colors > 256) /* buffer overflow otherwise */
                colors = 256;
        if (!colors && (info->bmiHeader.biBitCount <= 8))
            colors = 1 << info->bmiHeader.biBitCount;
        if (info->bmiHeader.biCompression == BI_BITFIELDS) masks = 3;
        size = (std::max)((int) info->bmiHeader.biSize, (int)(sizeof(BITMAPINFOHEADER) + masks * sizeof(DWORD) ));
        return size + colors * ((coloruse == DIB_RGB_COLORS) ? sizeof(RGBQUAD) : sizeof(WORD));
    }
}

/***********************************************************************
 *          is_dib_monochrome
 *
 * Returns whether a DIB can be converted to a monochrome DDB.
 *
 * A DIB can be converted if its color table contains only black and
 * white. Black must be the first color in the color table.
 *
 * Note : If the first color in the color table is white followed by
 *        black, we can't convert it to a monochrome DDB with
 *        SetDIBits, because black and white would be inverted.
 */
static BOOL is_dib_monochrome( const BITMAPINFO* info )
{
    if (info->bmiHeader.biBitCount != 1) return FALSE;

    if (info->bmiHeader.biSize == sizeof(BITMAPCOREHEADER))
    {
        const RGBTRIPLE *rgb = ((const BITMAPCOREINFO*)info)->bmciColors;

        /* Check if the first color is black */
        if ((rgb->rgbtRed == 0) && (rgb->rgbtGreen == 0) && (rgb->rgbtBlue == 0))
        {
            rgb++;

            /* Check if the second color is white */
            return ((rgb->rgbtRed == 0xff) && (rgb->rgbtGreen == 0xff)
                 && (rgb->rgbtBlue == 0xff));
        }
        else return FALSE;
    }
    else  /* assume BITMAPINFOHEADER */
    {
        const RGBQUAD *rgb = info->bmiColors;

        /* Check if the first color is black */
        if ((rgb->rgbRed == 0) && (rgb->rgbGreen == 0) &&
            (rgb->rgbBlue == 0) && (rgb->rgbReserved == 0))
        {
            rgb++;

            /* Check if the second color is white */
            return ((rgb->rgbRed == 0xff) && (rgb->rgbGreen == 0xff)
                 && (rgb->rgbBlue == 0xff) && (rgb->rgbReserved == 0));
        }
        else return FALSE;
    }
}

/***********************************************************************
 *           DIB_GetBitmapInfo
 *
 * Get the info from a bitmap header.
 * Return 1 for INFOHEADER, 0 for COREHEADER, -1 in case of failure.
 */
static int DIB_GetBitmapInfo( const BITMAPINFOHEADER *header, LONG *width,
                              LONG *height, WORD *bpp, DWORD *compr )
{
    if (header->biSize == sizeof(BITMAPCOREHEADER))
    {
        const BITMAPCOREHEADER *core = (const BITMAPCOREHEADER *)header;
        *width  = core->bcWidth;
        *height = core->bcHeight;
        *bpp    = core->bcBitCount;
        *compr  = 0;
        return 0;
    }
    else if (header->biSize == sizeof(BITMAPINFOHEADER) ||
             header->biSize == sizeof(BITMAPV4HEADER) ||
             header->biSize == sizeof(BITMAPV5HEADER))
    {
        *width  = header->biWidth;
        *height = header->biHeight;
        *bpp    = header->biBitCount;
        *compr  = header->biCompression;
        return 1;
    }
    return -1;
}

/*
 *  The following macro functions account for the irregularities of
 *   accessing cursor and icon resources in files and resource entries.
 */
typedef BOOL (*fnGetCIEntry)( LPCVOID dir, DWORD size, int n,
                              int *width, int *height, int *bits );

/**********************************************************************
 *	    CURSORICON_FindBestIcon
 *
 * Find the icon closest to the requested size and bit depth.
 */
static int CURSORICON_FindBestIcon( LPCVOID dir, DWORD size, fnGetCIEntry get_entry,
                                    int width, int height, int depth, UINT loadflags )
{
    int i, cx, cy, bits, bestEntry = -1;
    UINT iTotalDiff, iXDiff=0, iYDiff=0, iColorDiff;
    UINT iTempXDiff, iTempYDiff, iTempColorDiff;

    /* Find Best Fit */
    iTotalDiff = 0xFFFFFFFF;
    iColorDiff = 0xFFFFFFFF;

    if (loadflags & LR_DEFAULTSIZE)
    {
        if (!width) width = GetSystemMetrics( SM_CXICON );
        if (!height) height = GetSystemMetrics( SM_CYICON );
    }
    else if (!width && !height)
    {
        /* use the size of the first entry */
        if (!get_entry( dir, size, 0, &width, &height, &bits )) return -1;
        iTotalDiff = 0;
    }

    for ( i = 0; iTotalDiff && get_entry( dir, size, i, &cx, &cy, &bits ); i++ )
    {
        iTempXDiff = abs(width - cx);
        iTempYDiff = abs(height - cy);

        if(iTotalDiff > (iTempXDiff + iTempYDiff))
        {
            iXDiff = iTempXDiff;
            iYDiff = iTempYDiff;
            iTotalDiff = iXDiff + iYDiff;
        }
    }

    /* Find Best Colors for Best Fit */
    for ( i = 0; get_entry( dir, size, i, &cx, &cy, &bits ); i++ )
    {
        if(abs(width - cx) == (int)iXDiff && abs(height - cy) == (int)iYDiff)
        {
            iTempColorDiff = abs(depth - bits);
            if(iColorDiff > iTempColorDiff)
            {
                bestEntry = i;
                iColorDiff = iTempColorDiff;
            }
        }
    }

    return bestEntry;
}

static BOOL CURSORICON_GetResIconEntry( LPCVOID dir, DWORD size, int n,
                                        int *width, int *height, int *bits )
{
    const CURSORICONDIR *resdir = (const CURSORICONDIR *)dir;
    const ICONRESDIR *icon;

    if ( resdir->idCount <= n )
        return FALSE;
    if ((const char *)&resdir->idEntries[n + 1] - (const char *)dir > size)
        return FALSE;
    icon = &resdir->idEntries[n].ResInfo.icon;
    *width = icon->bWidth;
    *height = icon->bHeight;
    *bits = resdir->idEntries[n].wBitCount;
    return TRUE;
}

/**********************************************************************
 *	    CURSORICON_FindBestCursor
 *
 * Find the cursor closest to the requested size.
 *
 * FIXME: parameter 'color' ignored.
 */
static int CURSORICON_FindBestCursor( LPCVOID dir, DWORD size, fnGetCIEntry get_entry,
                                      int width, int height, int depth, UINT loadflags )
{
    int i, maxwidth, maxheight, cx, cy, bits, bestEntry = -1;

    if (loadflags & LR_DEFAULTSIZE)
    {
        if (!width) width = GetSystemMetrics( SM_CXCURSOR );
        if (!height) height = GetSystemMetrics( SM_CYCURSOR );
    }
    else if (!width && !height)
    {
        /* use the first entry */
        if (!get_entry( dir, size, 0, &width, &height, &bits )) return -1;
        return 0;
    }

    /* Double height to account for AND and XOR masks */

    height *= 2;

    /* First find the largest one smaller than or equal to the requested size*/

    maxwidth = maxheight = 0;
    for ( i = 0; get_entry( dir, size, i, &cx, &cy, &bits ); i++ )
    {
        if ((cx <= width) && (cy <= height) &&
            (cx > maxwidth) && (cy > maxheight))
        {
            bestEntry = i;
            maxwidth  = cx;
            maxheight = cy;
        }
    }
    if (bestEntry != -1) return bestEntry;

    /* Now find the smallest one larger than the requested size */

    maxwidth = maxheight = 255;
    for ( i = 0; get_entry( dir, size, i, &cx, &cy, &bits ); i++ )
    {
        if (((cx < maxwidth) && (cy < maxheight)) || (bestEntry == -1))
        {
            bestEntry = i;
            maxwidth  = cx;
            maxheight = cy;
        }
    }

    return bestEntry;
}

static BOOL CURSORICON_GetResCursorEntry( LPCVOID dir, DWORD size, int n,
                                          int *width, int *height, int *bits )
{
    const CURSORICONDIR *resdir = (const CURSORICONDIR *)dir;
    const CURSORDIR *cursor;

    if ( resdir->idCount <= n )
        return FALSE;
    if ((const char *)&resdir->idEntries[n + 1] - (const char *)dir > size)
        return FALSE;
    cursor = &resdir->idEntries[n].ResInfo.cursor;
    *width = cursor->wWidth;
    *height = cursor->wHeight;
    *bits = resdir->idEntries[n].wBitCount;
    return TRUE;
}

static const CURSORICONDIRENTRY *CURSORICON_FindBestIconRes( const CURSORICONDIR * dir, DWORD size,
                                                             int width, int height, int depth,
                                                             UINT loadflags )
{
    int n;

    n = CURSORICON_FindBestIcon( dir, size, CURSORICON_GetResIconEntry,
                                 width, height, depth, loadflags );
    if ( n < 0 )
        return NULL;
    return &dir->idEntries[n];
}

static const CURSORICONDIRENTRY *CURSORICON_FindBestCursorRes( const CURSORICONDIR *dir, DWORD size,
                                                               int width, int height, int depth,
                                                               UINT loadflags )
{
    int n = CURSORICON_FindBestCursor( dir, size, CURSORICON_GetResCursorEntry,
                                       width, height, depth, loadflags );
    if ( n < 0 )
        return NULL;
    return &dir->idEntries[n];
}

static BOOL CURSORICON_GetFileEntry( LPCVOID dir, DWORD size, int n,
                                     int *width, int *height, int *bits )
{
    const CURSORICONFILEDIR *filedir = (const CURSORICONFILEDIR *)dir;
    const CURSORICONFILEDIRENTRY *entry;
    const BITMAPINFOHEADER *info;

    if ( filedir->idCount <= n )
        return FALSE;
    if ((const char *)&filedir->idEntries[n + 1] - (const char *)dir > size)
        return FALSE;
    entry = &filedir->idEntries[n];
    info = (const BITMAPINFOHEADER *)((const char *)dir + entry->dwDIBOffset);
    if ((const char *)(info + 1) - (const char *)dir > size) return FALSE;
    *width = entry->bWidth;
    *height = entry->bHeight;
    *bits = info->biBitCount;
    return TRUE;
}

static const CURSORICONFILEDIRENTRY *CURSORICON_FindBestCursorFile( const CURSORICONFILEDIR *dir, DWORD size,
                                                                    int width, int height, int depth,
                                                                    UINT loadflags )
{
    int n = CURSORICON_FindBestCursor( dir, size, CURSORICON_GetFileEntry,
                                       width, height, depth, loadflags );
    if ( n < 0 )
        return NULL;
    return &dir->idEntries[n];
}

static const CURSORICONFILEDIRENTRY *CURSORICON_FindBestIconFile( const CURSORICONFILEDIR *dir, DWORD size,
                                                                  int width, int height, int depth,
                                                                  UINT loadflags )
{
    int n = CURSORICON_FindBestIcon( dir, size, CURSORICON_GetFileEntry,
                                     width, height, depth, loadflags );
    if ( n < 0 )
        return NULL;
    return &dir->idEntries[n];
}

/***********************************************************************
 *          bmi_has_alpha
 */
static BOOL bmi_has_alpha( const BITMAPINFO *info, const void *bits )
{
    int i;
    BOOL has_alpha = FALSE;
    const unsigned char *ptr = (const unsigned char *)bits;

    if (info->bmiHeader.biBitCount != 32) return FALSE;
	for (i = 0; i < info->bmiHeader.biWidth * abs(info->bmiHeader.biHeight); i++, ptr += 4)
	{
		has_alpha = (ptr[3] != 0);
		if ((has_alpha)) break;
	}
	
    return has_alpha;
}

/***********************************************************************
 *          create_alpha_bitmap
 *
 * Create the alpha bitmap for a 32-bpp icon that has an alpha channel.
 */
static HBITMAP create_alpha_bitmap(const BITMAPINFO *src_info, const void *color_bits,int bmWidth,int bmHeight )
{
    HBITMAP alpha = 0;
    BITMAPINFO *info = NULL;
    HDC hdc;
    void *bits;
    unsigned char *ptr;
    int i;

	hdc = CreateCompatibleDC(0);
	if (!(hdc)) return 0;
	info = (BITMAPINFO *)HeapAlloc(GetProcessHeap(), 0, FIELD_OFFSET(BITMAPINFO, bmiColors[256]));
	if (!(info)) goto done;
    info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info->bmiHeader.biWidth = bmWidth;
    info->bmiHeader.biHeight = -bmHeight;
    info->bmiHeader.biPlanes = 1;
    info->bmiHeader.biBitCount = 32;
    info->bmiHeader.biCompression = BI_RGB;
    info->bmiHeader.biSizeImage = bmWidth * bmHeight * 4;
    info->bmiHeader.biXPelsPerMeter = 0;
    info->bmiHeader.biYPelsPerMeter = 0;
    info->bmiHeader.biClrUsed = 0;
    info->bmiHeader.biClrImportant = 0;
	alpha = CreateDIBSection(hdc, info, DIB_RGB_COLORS, &bits, NULL, 0);
	if (!(alpha)) goto done;

	SelectObject( hdc, alpha );
	StretchDIBits( hdc, 0, 0,bmWidth, bmHeight,
		0, 0, src_info->bmiHeader.biWidth, src_info->bmiHeader.biHeight,
		color_bits, src_info, DIB_RGB_COLORS, SRCCOPY );


	/* pre-multiply by alpha */
    for (i = 0, ptr = (unsigned char *)bits; i <bmWidth * bmHeight; i++, ptr += 4)
    {
        unsigned int alpha = ptr[3];
        ptr[0] = ptr[0] * alpha / 255;
        ptr[1] = ptr[1] * alpha / 255;
        ptr[2] = ptr[2] * alpha / 255;
    }

done:
    DeleteDC( hdc );
    HeapFree( GetProcessHeap(), 0, info );
    return alpha;
}


/***********************************************************************
 *          create_icon_from_bmi
 *
 * Create an icon from its BITMAPINFO.
 */
static HICON create_icon_from_bmi( const BITMAPINFO *bmi, DWORD maxsize, HMODULE module, LPCWSTR resname,
                                   HRSRC rsrc, POINT hotspot, BOOL bIcon, INT width, INT height,
                                   UINT cFlag )
{
    DWORD size, color_size, mask_size;
    HBITMAP color = 0, mask = 0, alpha = 0;
    const void *color_bits, *mask_bits;
    BITMAPINFO *bmi_copy;
    BOOL ret = FALSE;
    BOOL do_stretch;
    HICON hObj = 0;
    HDC hdc = 0;

    /* Check bitmap header */

    if (maxsize < sizeof(BITMAPCOREHEADER))
    {
        return 0;
    }
    if (maxsize < bmi->bmiHeader.biSize)
    {
        WARN( "invalid header size %u\n", bmi->bmiHeader.biSize );
        return 0;
    }
    if ( (bmi->bmiHeader.biSize != sizeof(BITMAPCOREHEADER)) &&
         (bmi->bmiHeader.biSize != sizeof(BITMAPINFOHEADER)  ||
         (bmi->bmiHeader.biCompression != BI_RGB &&
          bmi->bmiHeader.biCompression != BI_BITFIELDS)) )
    {
        WARN( "invalid bitmap header %u\n", bmi->bmiHeader.biSize );
        return 0;
    }

    size = bitmap_info_size( bmi, DIB_RGB_COLORS );
    color_size = get_dib_image_size( bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight / 2,
                                     bmi->bmiHeader.biBitCount );
    mask_size = get_dib_image_size( bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight / 2, 1 );
    if (size > maxsize || color_size > maxsize - size)
    {
        WARN( "truncated file %u < %u+%u+%u\n", maxsize, size, color_size, mask_size );
        return 0;
    }
    if (mask_size > maxsize - size - color_size) mask_size = 0;  /* no mask */

    if (cFlag & LR_DEFAULTSIZE)
    {
        if (!width) width = GetSystemMetrics( bIcon ? SM_CXICON : SM_CXCURSOR );
        if (!height) height = GetSystemMetrics( bIcon ? SM_CYICON : SM_CYCURSOR );
    }
    else
    {
        if (!width) width = bmi->bmiHeader.biWidth;
        if (!height) height = bmi->bmiHeader.biHeight/2;
    }
    do_stretch = (bmi->bmiHeader.biHeight/2 != height) ||
                 (bmi->bmiHeader.biWidth != width);

    /* Scale the hotspot */
    if (bIcon)
    {
        hotspot.x = width / 2;
        hotspot.y = height / 2;
    }
    else if (do_stretch)
    {
        hotspot.x = (hotspot.x * width) / bmi->bmiHeader.biWidth;
        hotspot.y = (hotspot.y * height) / (bmi->bmiHeader.biHeight / 2);
    }

    if (!screen_dc) screen_dc = CreateDCW( DISPLAYW, NULL, NULL, NULL );
    if (!screen_dc) return 0;

	bmi_copy = (BITMAPINFO*)HeapAlloc(GetProcessHeap(), 0, (std::max)((int)size, (int)FIELD_OFFSET(BITMAPINFO, bmiColors[2])));

	if (!(bmi_copy))
        return 0;
	hdc = CreateCompatibleDC(0);
	if (!(hdc)) goto done;

    memcpy( bmi_copy, bmi, size );
    bmi_copy->bmiHeader.biHeight /= 2;

    color_bits = (const char*)bmi + size;
    mask_bits = (const char*)color_bits + color_size;

    alpha = 0;
    if (is_dib_monochrome( bmi ))
    {
		mask = CreateBitmap(width, height * 2, 1, 1, NULL);
		if (!(mask)) goto done;
        color = 0;

        /* copy color data into second half of mask bitmap */
        SelectObject( hdc, mask );
        StretchDIBits( hdc, 0, height, width, height,
                       0, 0, bmi_copy->bmiHeader.biWidth, bmi_copy->bmiHeader.biHeight,
                       color_bits, bmi_copy, DIB_RGB_COLORS, SRCCOPY );
    }
    else
    {
		mask = CreateBitmap(width, height, 1, 1, NULL);
		if (!(mask)) goto done;
		color = CreateBitmap(width, height, GetDeviceCaps(screen_dc, PLANES),
			GetDeviceCaps(screen_dc, BITSPIXEL), NULL);
		if (!(color))
        {
            DeleteObject( mask );
            goto done;
        }
        SelectObject( hdc, color );
        StretchDIBits( hdc, 0, 0, width, height,
                       0, 0, bmi_copy->bmiHeader.biWidth, bmi_copy->bmiHeader.biHeight,
                       color_bits, bmi_copy, DIB_RGB_COLORS, SRCCOPY );

        if (bmi_has_alpha( bmi_copy, color_bits ))
            alpha = create_alpha_bitmap(bmi_copy, color_bits ,width,height);

        /* convert info to monochrome to copy the mask */
        bmi_copy->bmiHeader.biBitCount = 1;
        if (bmi_copy->bmiHeader.biSize != sizeof(BITMAPCOREHEADER))
        {
            RGBQUAD *rgb = bmi_copy->bmiColors;

            bmi_copy->bmiHeader.biClrUsed = bmi_copy->bmiHeader.biClrImportant = 2;
            rgb[0].rgbBlue = rgb[0].rgbGreen = rgb[0].rgbRed = 0x00;
            rgb[1].rgbBlue = rgb[1].rgbGreen = rgb[1].rgbRed = 0xff;
            rgb[0].rgbReserved = rgb[1].rgbReserved = 0;
        }
        else
        {
            RGBTRIPLE *rgb = (RGBTRIPLE *)(((BITMAPCOREHEADER *)bmi_copy) + 1);

            rgb[0].rgbtBlue = rgb[0].rgbtGreen = rgb[0].rgbtRed = 0x00;
            rgb[1].rgbtBlue = rgb[1].rgbtGreen = rgb[1].rgbtRed = 0xff;
        }
    }

    if (mask_size)
    {
        SelectObject( hdc, mask );
        StretchDIBits( hdc, 0, 0, width, height,
                       0, 0, bmi_copy->bmiHeader.biWidth, bmi_copy->bmiHeader.biHeight,
                       mask_bits, bmi_copy, DIB_RGB_COLORS, SRCCOPY );
    }
    ret = TRUE;

done:
    DeleteDC( hdc );
    HeapFree( GetProcessHeap(), 0, bmi_copy );
	if(ret)
	{
		ICONINFO iconInfo={0};
		iconInfo.fIcon=bIcon;
		iconInfo.xHotspot=hotspot.x;
		iconInfo.yHotspot=hotspot.y;
		if(alpha)
		{
			iconInfo.hbmColor=alpha;
			iconInfo.hbmMask=mask;
		}
		else
		{
			iconInfo.hbmColor=color;
			iconInfo.hbmMask=mask;
		}

		hObj=CreateIconIndirect(&iconInfo);
		if(color) DeleteObject( color );
		if(alpha) DeleteObject( alpha );
		if(mask) DeleteObject( mask );
	}
	return hObj;
}

HICON CURSORICON_LoadFromBuf(const BYTE * bits,DWORD filesize,INT width, INT height,BOOL fCursor, UINT loadflags)
{
	const CURSORICONFILEDIRENTRY *entry;
	const CURSORICONFILEDIR *dir;
	POINT hotspot;
	INT depth=1;

	/* Check for .ani. */
	if (memcmp( bits, "RIFF", 4 ) == 0)
	{//not support
        return  (HCURSOR)CreateIconFromResource((PBYTE)bits,filesize,FALSE,0x00030000);
	}

	dir = (const CURSORICONFILEDIR*) bits;
	if ( filesize < FIELD_OFFSET( CURSORICONFILEDIR, idEntries[dir->idCount] ))
		return 0;

	if(!(loadflags & LR_MONOCHROME))
	{
		HDC hdc=GetDC(NULL);
		depth=GetDeviceCaps(hdc,BITSPIXEL);
		ReleaseDC(NULL,hdc);
	}
	if ( fCursor )
		entry = CURSORICON_FindBestCursorFile( dir, filesize, width, height, depth, loadflags );
	else
		entry = CURSORICON_FindBestIconFile( dir, filesize, width, height, depth, loadflags );

	/* check that we don't run off the end of the file */
	if ( !entry || entry->dwDIBOffset > filesize || entry->dwDIBOffset + entry->dwDIBSize > filesize )
		return 0;

	hotspot.x = entry->xHotspot;
	hotspot.y = entry->yHotspot;
	return create_icon_from_bmi( (const BITMAPINFO *)&bits[entry->dwDIBOffset], filesize - entry->dwDIBOffset,
		NULL, NULL, NULL, hotspot, !fCursor, width, height, loadflags );
}

HICON CURSORICON_LoadFromFile( LPCWSTR filename,
									 INT width, INT height,
									 BOOL fCursor, UINT loadflags)
{
	HICON hIcon=0;
	DWORD filesize = 0;
	const BYTE *bits;
	bits = (const BYTE *)map_fileW( filename, &filesize );
	if (!bits)
		return 0;
	hIcon=CURSORICON_LoadFromBuf(bits,filesize,width,height,fCursor,loadflags);
	UnmapViewOfFile( bits );
	return hIcon;
}
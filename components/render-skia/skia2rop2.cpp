
#include "skia2rop2.h"
#include <core/skcolorpriv.h>
#include <src/core/SkReadBuffer.h>
#include <include/core/SkPreConfig.h>

// 扩展Skia, 实现Rop2操作
static SkPMColor rdRop2Proc_Clear(SkPMColor src, SkPMColor dst)
{
	(src); (dst);
    return 0;
}
static SkPMColor rdRop2Proc_Copy(SkPMColor src, SkPMColor dst)
{
	(src); (dst);
	return src;
}
static SkPMColor rdRop2Proc_CopyInvert(SkPMColor src, SkPMColor dst)
{
	(dst);
    SkPMColor res = ~src;
    res |= (SK_A32_MASK << SK_A32_SHIFT); // force it to be opaque
    return res;
}
static SkPMColor rdRop2Proc_Invert(SkPMColor src, SkPMColor dst)
{
	(src);
    SkPMColor res = ~dst;
    res |= (SK_A32_MASK << SK_A32_SHIFT); // force it to be opaque
    return res;
}
static SkPMColor rdRop2Proc_Xor(SkPMColor src, SkPMColor dst)
{
    SkPMColor res = src ^ dst;
    res |= (SK_A32_MASK << SK_A32_SHIFT); // force it to be opaque
    return res;
}
static SkPMColor rdRop2Proc_Or(SkPMColor src, SkPMColor dst)
{
    SkPMColor res = src | dst;
    res |= (SK_A32_MASK << SK_A32_SHIFT); // force it to be opaque
    return res;
}
static SkPMColor rdRop2Proc_And(SkPMColor src, SkPMColor dst)
{
    SkPMColor res = src & dst;
    res |= (SK_A32_MASK << SK_A32_SHIFT); // force it to be opaque
    return res;
}
static SkPMColor rdRop2Proc_AndReverse(SkPMColor src, SkPMColor dst)
{
    SkPMColor res = src & (~dst);
    res |= (SK_A32_MASK << SK_A32_SHIFT); // force it to be opaque
    return res;
}
static SkPMColor rdRop2Proc_AndInvert(SkPMColor src, SkPMColor dst)
{
    SkPMColor res = (~src) & dst;
    res |= (SK_A32_MASK << SK_A32_SHIFT); // force it to be opaque
    return res;
}
static SkPMColor rdRop2Proc_NAnd(SkPMColor src, SkPMColor dst)
{
    SkPMColor res = (~src) | (~dst);
    res |= (SK_A32_MASK << SK_A32_SHIFT); // force it to be opaque
    return res;
}
static SkPMColor rdRop2Proc_Equiv(SkPMColor src, SkPMColor dst)
{
    SkPMColor res = (~src) ^ dst;
    res |= (SK_A32_MASK << SK_A32_SHIFT); // force it to be opaque
    return res;
}
static SkPMColor rdRop2Proc_Noop(SkPMColor src, SkPMColor dst)
{
	(src);
    return dst;
}
static SkPMColor rdRop2Proc_OrInvert(SkPMColor src, SkPMColor dst)
{
    SkPMColor res = (~src) | dst;
    res |= (SK_A32_MASK << SK_A32_SHIFT); // force it to be opaque
    return res;
}
static SkPMColor rdRop2Proc_OrReverse(SkPMColor src, SkPMColor dst)
{
    SkPMColor res = src | (~dst);
    res |= (SK_A32_MASK << SK_A32_SHIFT); // force it to be opaque
    return res;
}
static SkPMColor rdRop2Proc_Set(SkPMColor src, SkPMColor dst)
{
	(src); (dst);
    SkPMColor res = 1;
    res |= (SK_A32_MASK << SK_A32_SHIFT); // force it to be opaque
    return res;
}
// 
// static SkPMColor SkFourByteInterp(SkPMColor src, SkPMColor dst, U8CPU alpha)
// {
//     unsigned scale = SkAlpha255To256(alpha);
//     unsigned a = SkAlphaBlend(SkGetPackedA32(src), SkGetPackedA32(dst), scale);
//     unsigned r = SkAlphaBlend(SkGetPackedR32(src), SkGetPackedR32(dst), scale);
//     unsigned g = SkAlphaBlend(SkGetPackedG32(src), SkGetPackedG32(dst), scale);
//     unsigned b = SkAlphaBlend(SkGetPackedB32(src), SkGetPackedB32(dst), scale);
//     return SkPackARGB32(a, r, g, b);
// }

// rop2操作，结束
typedef struct _Rop2Proc
{
    SkXfermodeProc proc;
}Rop2Proc;

static Rop2Proc rop2_map[] = {
    {rdRop2Proc_Clear}, 
    {rdRop2Proc_Copy}, 
    {rdRop2Proc_AndInvert}, 
    {rdRop2Proc_CopyInvert}, 
    {rdRop2Proc_AndReverse}, 
    {rdRop2Proc_Invert}, 
    {rdRop2Proc_Xor}, 
    {rdRop2Proc_NAnd}, 
    {rdRop2Proc_And}, 
    {rdRop2Proc_Equiv},
    {rdRop2Proc_Noop}, 
    {rdRop2Proc_OrInvert}, 
    {rdRop2Proc_OrReverse},
    {rdRop2Proc_Or},
    {rdRop2Proc_Set},
};

ProcXfermode::ProcXfermode( int rop ) : rop2(rop)
{
    if(rop < Rop2_End)
    {
        fProc = rop2_map[rop].proc;
    }
    else
    {
        fProc = NULL;
    }
}

void ProcXfermode::xfer32( SkPMColor  dst[],
                          const  SkPMColor  src[], int count,
                          const  SkAlpha  aa[]) const 
{
    SkASSERT(dst && src && count >= 0);
    SkXfermodeProc proc = fProc;
    if (NULL != proc) {
        if (NULL == aa) {
            for (int i = count - 1; i >= 0; --i) {
                dst[i] = proc(src[i], dst[i]);
            }
        } else {
            for (int i = count - 1; i >= 0; --i) {
                unsigned a = aa[i];
                if (0 != a) {
                    SkPMColor dstC = dst[i];
                    SkPMColor C = proc(src[i], dstC);
                    if (a != 0xFF) {
                        C = SkFourByteInterp(C, dstC, a);
                    }
                    dst[i] = C;
                }
            }
        }
    }
}
void ProcXfermode::xfer16( uint16_t dst[],
                          const  SkPMColor src[], int count,
                          const  SkAlpha aa[]) const
{
    SkASSERT(dst && src && count >= 0);
    SkXfermodeProc proc = fProc;
    if (NULL != proc) {
        if (NULL == aa) {
            for (int i = count - 1; i >= 0; --i) {
                SkPMColor dstC = SkPixel16ToPixel32(dst[i]);
                dst[i] = SkPixel32ToPixel16_ToU16(proc(src[i], dstC));
            }
        } else {
            for (int i = count - 1; i >= 0; --i) {
                unsigned a = aa[i];
                if (0 != a) {
                    SkPMColor dstC = SkPixel16ToPixel32(dst[i]);
                    SkPMColor C = proc(src[i], dstC);
                    if (0xFF != a) {
                        C = SkFourByteInterp(C, dstC, a);
                    }
                    dst[i] = SkPixel32ToPixel16_ToU16(C);
                }
            }
        }
    }
}

void ProcXfermode::xferA8( SkAlpha dst[],
                          const  SkPMColor src[], int count,
                          const  SkAlpha aa[]) const
{
    SkASSERT(dst && src && count >= 0);
    SkXfermodeProc proc = fProc;
    if (NULL != proc) {
        if (NULL == aa) {
            for (int i = count - 1; i >= 0; --i) {
                SkPMColor res = proc(src[i], dst[i] << SK_A32_SHIFT);
                dst[i] = SkToU8(SkGetPackedA32(res));
            }
        } else {
            for (int i = count - 1; i >= 0; --i) {
                unsigned a = aa[i];
                if (0 != a) {
                    SkAlpha dstA = dst[i];
                    SkPMColor res = proc(src[i], dstA << SK_A32_SHIFT);
                    unsigned A = SkGetPackedA32(res);
                    if (0xFF != a) {
                        A = SkAlphaBlend(A, dstA, SkAlpha255To256(a));
                    }
                    dst[i] = SkToU8(A);
                }
            }
        }
    }
}
ProcXfermode::ProcXfermode(SkReadBuffer& buffer)
: SkXfermode(buffer)
{
    fProc = (SkXfermodeProc)buffer.readFunctionPtr();
    rop2 = buffer.readInt();
}


void ProcXfermode::flatten(SkWriteBuffer& buffer)
{
    buffer.writeFunctionPtr((void*)fProc);
    buffer.write32(rop2);
}

#pragma once

#include <core/SkXfermode.h>

class ProcXfermode : public SkXfermode {
public:

enum Xfer_Rop2{
    Rop2_Clear=0, 
    Rop2_Copy, 
    Rop2_AndInvert, 
    Rop2_CopyInvert, 
    Rop2_AndReverse, 
    Rop2_Invert, 
    Rop2_Xor, 
    Rop2_NAnd, 
    Rop2_And, 
    Rop2_Equiv,
    Rop2_Noop, 
    Rop2_OrInvert, 
    Rop2_OrReverse,
    Rop2_Or,
    Rop2_Set,
    
    Rop2_End,
    };

    ProcXfermode(int rop);
    // overrides from SkXfermode
    virtual void xfer32(SkPMColor dst[], const SkPMColor src[], int count,
        const SkAlpha aa[]) const;
    virtual void xfer16(uint16_t dst[], const SkPMColor src[], int count,
        const SkAlpha aa[]) const;
    virtual void xferA8(SkAlpha dst[], const SkPMColor src[], int count,
        const SkAlpha aa[]) const;

    // overrides from SkFlattenable
    virtual Factory getFactory() const { return CreateProc; }
    virtual void flatten(SkWriteBuffer&);
    
#ifndef SK_IGNORE_TO_STRING
    void ProcXfermode::toString(SkString* str) const {
		(str);
//         this->INHERITED::toString(str);
    }
#endif

protected:
    ProcXfermode(SkReadBuffer&);
private:
    SkXfermodeProc fProc;
    int rop2;
    static SkFlattenable* CreateProc(SkReadBuffer& buffer) {
        return SkNEW_ARGS(ProcXfermode, (buffer)); }
    typedef SkXfermode INHERITED;
};

#include "soui_mem_wrapper.h"
#include <malloc.h>
#include "utilities-def.h"

namespace SOUI
{
    
    void * soui_mem_wrapper::SouiMalloc( size_t szMem )
    {
        return malloc(szMem);
    }

    void * soui_mem_wrapper::SouiRealloc( void *p,size_t szMem )
    {
        return realloc(p,szMem);
    }

    void * soui_mem_wrapper::SouiCalloc( size_t count, size_t szEle )
    {
        return calloc(count,szEle);
    }

    void soui_mem_wrapper::SouiFree( void *p )
    {
        free(p);
    }


}

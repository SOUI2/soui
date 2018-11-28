// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __SOUICOLL_H__
#define __SOUICOLL_H__

#pragma once

#include <windows.h>
#include <limits.h>
#include <soui_mem_wrapper.h>
#include "snew.h"

#pragma warning(push)
#pragma warning(disable: 4702)  // Unreachable code.  This file will have lots of it, especially without EH enabled.
#pragma warning(disable: 4512)  // assignment operator could not be generated
#pragma warning(disable: 4290)  // C++ Exception Specification ignored
#pragma warning(disable: 4127)  // conditional expression constant
#pragma warning(disable: 4571)  //catch(...) blocks compiled with /EHs do NOT catch or re-throw Structured Exceptions

// abstract iteration position

#ifndef _S_PACKING
#define _S_PACKING 8
#endif

#ifndef SASSERT
#define SASSERT(expr) __noop;
#endif // SASSERT

#define SThrow(expr) __noop;
#define SASSERT_VALID(x) __noop;

#ifndef SASSUME
#define SASSUME(expr) do { SASSERT(expr); __analysis_assume(!!(expr)); } while(0)
#endif // SASSUME

#ifndef SENSURE
#define SENSURE(expr) __noop;
#endif // SENSURE

/**
 * Avoid "unused parameter" warnings
 */
#ifndef SUNUSED
#define SUNUSED(x) (void)x;
#endif //SUNUSED

#if !defined(_W64)
#if !defined(__midl) &&    (defined(_X86_)    || defined(_M_IX86))
#define    _W64 __w64
#else
#define    _W64
#endif
#endif


#ifndef _STRY
#define _STRY try
#endif
#define _SCATCHALL() __pragma(warning(push)) __pragma(warning(disable: 4571)) catch( ... ) __pragma(warning(pop))


#pragma pack(push,_S_PACKING)
namespace SOUI
{

struct __SPOSITION
{
};
typedef __SPOSITION* SPOSITION;

template<typename T>
class SLimits;

template<>
class SLimits<int _W64>
{
public:
    static const int _Min=INT_MIN;
    static const int _Max=INT_MAX;
};

template<>
class SLimits<unsigned int _W64>
{
public:
    static const unsigned int _Min=0;
    static const unsigned int _Max=UINT_MAX;
};

template<>
class SLimits<long _W64>
{
public:
    static const long _Min=LONG_MIN;
    static const long _Max=LONG_MAX;
};

template<>
class SLimits<unsigned long _W64>
{
public:
    static const unsigned long _Min=0;
    static const unsigned long _Max=ULONG_MAX;
};

template<>
class SLimits<long long>
{
public:
    static const long long _Min=LLONG_MIN;
    static const long long _Max=LLONG_MAX;
};

template<>
class SLimits<unsigned long long>
{
public:
    static const unsigned long long _Min=0;
    static const unsigned long long _Max=ULLONG_MAX;
};

/* generic version */
template<typename T>
inline HRESULT SAdd(T* ptResult, T tLeft, T tRight)
{
    if(SLimits<T>::_Max-tLeft < tRight)
    {
        return E_INVALIDARG;
    }
    *ptResult= tLeft + tRight;
    return S_OK;
}

/* generic but compariatively slow version */
template<typename T>
inline HRESULT SMultiply(T* ptResult,    T tLeft, T tRight)
{
    /* avoid divide 0 */
    if(tLeft==0)
    {
        *ptResult=0;
        return S_OK;
    }
    if(SLimits<T>::_Max/tLeft < tRight)
    {
        return E_INVALIDARG;
    }
    *ptResult= tLeft * tRight;
    return S_OK;
}

/* fast    version    for    32 bit integers    */
template<>
inline HRESULT SMultiply(int _W64    *piResult, int _W64    iLeft, int _W64 iRight)
{
    __int64 i64Result=static_cast<__int64>(iLeft) * static_cast<__int64>(iRight);
    if(i64Result>INT_MAX || i64Result < INT_MIN)
    {
        return E_INVALIDARG;
    }
    *piResult=static_cast<int _W64>(i64Result);
    return S_OK;
}

template<>
inline HRESULT SMultiply(unsigned int    _W64 *piResult, unsigned int _W64 iLeft, unsigned int _W64 iRight)
{
    unsigned __int64 i64Result=static_cast<unsigned __int64>(iLeft) * static_cast<unsigned __int64>(iRight);
    if(i64Result>UINT_MAX)
    {
        return E_INVALIDARG;
    }
    *piResult=static_cast<unsigned int _W64>(i64Result);
    return S_OK;
}

template<>
inline HRESULT SMultiply(long    _W64 *piResult, long _W64 iLeft, long _W64 iRight)
{
    __int64 i64Result=static_cast<__int64>(iLeft) * static_cast<__int64>(iRight);
    if(i64Result>LONG_MAX || i64Result < LONG_MIN)
    {
        return E_INVALIDARG;
    }
    *piResult=static_cast<long _W64>(i64Result);
    return S_OK;
}

template<>
inline HRESULT SMultiply(unsigned long _W64 *piResult, unsigned long _W64 iLeft, unsigned long _W64 iRight)
{
    unsigned __int64 i64Result=static_cast<unsigned __int64>(iLeft) * static_cast<unsigned __int64>(iRight);
    if(i64Result>ULONG_MAX)
    {
        return E_INVALIDARG;
    }
    *piResult=static_cast<unsigned long _W64>(i64Result);
    return S_OK;
}

struct SPlex     // warning variable length structure
{
    SPlex* pNext;
#if (_S_PACKING >= 8)
    DWORD dwReserved[1];    // align on 8 byte boundary
#endif
    // BYTE data[maxNum*elementSize];

    void* data()
    {
        return this+1;
    }

    static SPlex* Create(SPlex*& head, size_t nMax, size_t cbElement);
    // like 'calloc' but no zero fill
    // may throw memory exceptions

    void FreeDataChain();       // soui_mem_wrapper::SouiFree this one and links
};

inline SPlex* SPlex::Create( SPlex*& pHead, size_t nMax, size_t nElementSize )
{
    SPlex* pPlex;

    SASSERT( nMax > 0 );
    SASSERT( nElementSize > 0 );

    size_t nBytes=0;
    if(     FAILED(SMultiply(&nBytes, nMax, nElementSize)) ||
            FAILED(SAdd(&nBytes, nBytes, sizeof(SPlex))) )
    {
        return NULL;
    }
    pPlex = static_cast< SPlex* >( soui_mem_wrapper::SouiMalloc( nBytes ) );
    if( pPlex == NULL )
    {
        return( NULL );
    }

    pPlex->pNext = pHead;
    pHead = pPlex;

    return( pPlex );
}

inline void SPlex::FreeDataChain()
{
    SPlex* pPlex;

    pPlex = this;
    while( pPlex != NULL )
    {
        SPlex* pNext;

        pNext = pPlex->pNext;
        soui_mem_wrapper::SouiFree( pPlex );
        pPlex = pNext;
    }
}

template< typename T >
class CElementTraitsBase
{
public:
    typedef const T& INARGTYPE;
    typedef T& OUTARGTYPE;

    static void CopyElements( T* pDest, const T* pSrc, size_t nElements )
    {
        for( size_t iElement = 0; iElement < nElements; iElement++ )
        {
            pDest[iElement] = pSrc[iElement];
        }
    }

    static void RelocateElements( T* pDest, T* pSrc, size_t nElements )
    {
        // A simple memmove works for nearly all types.
        // You'll have to override this for types that have pointers to their
        // own members.
        memmove_s( pDest, nElements*sizeof( T ), pSrc, nElements*sizeof( T ));
    }
};

template< typename T >
class CDefaultHashTraits
{
public:
    static ULONG Hash( const T& element )
    {
        return( ULONG( ULONG_PTR( element ) ) );
    }
};

template< typename T >
class CDefaultCompareTraits
{
public:
    static bool CompareElements( const T& element1, const T& element2 )
    {
        return( (element1 == element2) != 0 );  // != 0 to handle overloads of operator== that return BOOL instead of bool
    }

    static int CompareElementsOrdered( const T& element1, const T& element2 )
    {
        if( element1 < element2 )
        {
            return( -1 );
        }
        else if( element1 == element2 )
        {
            return( 0 );
        }
        else
        {
            SASSERT( element1 > element2 );
            return( 1 );
        }
    }
};

template< typename T >
class CDefaultElementTraits :
    public CElementTraitsBase< T >,
    public CDefaultHashTraits< T >,
    public CDefaultCompareTraits< T >
{
};

template< typename T >
class CElementTraits :
    public CDefaultElementTraits< T >
{
};

template<>
class CElementTraits< GUID > :
    public CElementTraitsBase< GUID >
{
public:
    static ULONG Hash( INARGTYPE guid )
    {
        const DWORD* pdwData = reinterpret_cast< const DWORD* >( &guid );

        return( pdwData[0]^pdwData[1]^pdwData[2]^pdwData[3] );
    }

    static bool CompareElements( INARGTYPE element1, INARGTYPE element2 )
    {
        return( (element1 == element2) != 0 );  // != 0 to handle overloads of operator== that return BOOL instead of bool
    }

    static int CompareElementsOrdered( INARGTYPE element1, INARGTYPE element2 )
    {
        const DWORD* pdwData1 = reinterpret_cast< const DWORD* >( &element1 );
        const DWORD* pdwData2 = reinterpret_cast< const DWORD* >( &element2 );

        for( int iDWORD = 3; iDWORD >= 0; iDWORD-- )
        {
            if( pdwData1[iDWORD] > pdwData2[iDWORD] )
            {
                return( 1 );
            }
            else if( pdwData1[iDWORD] < pdwData2[iDWORD] )
            {
                return( -1 );
            }
        }

        return( 0 );
    }
};
 

template< typename T >
class SStringRefElementTraits :
    public CElementTraitsBase< T >
{
public:
    static ULONG Hash(typename CElementTraitsBase<T>::INARGTYPE str )
    {
        ULONG nHash = 0;

        const T::XCHAR* pch = str;

        SENSURE( pch != NULL );

        while( *pch != 0 )
        {
            nHash = (nHash<<5)+nHash+(*pch);
            pch++;
        }

        return( nHash );
    }

    static bool CompareElements(typename CElementTraitsBase<T>::INARGTYPE element1, typename CElementTraitsBase<T>::INARGTYPE element2 )
    {
        return( element1 == element2 );
    }

    static int CompareElementsOrdered(typename CElementTraitsBase<T>::INARGTYPE str1, typename CElementTraitsBase<T>::INARGTYPE str2 )
    {
        return( str1.Compare( str2 ) );
    }
};

template< typename T >
class CPrimitiveElementTraits :
    public CDefaultElementTraits< T >
{
public:
    typedef T INARGTYPE;
    typedef T& OUTARGTYPE;
};

#define _DECLARE_PRIMITIVE_TRAITS( T ) \
    template<> \
    class CElementTraits< T > : \
    public CPrimitiveElementTraits< T > \
    { \
    };

_DECLARE_PRIMITIVE_TRAITS( unsigned char )
_DECLARE_PRIMITIVE_TRAITS( unsigned short )
_DECLARE_PRIMITIVE_TRAITS( unsigned int )
_DECLARE_PRIMITIVE_TRAITS( unsigned long )
_DECLARE_PRIMITIVE_TRAITS( unsigned __int64 )
_DECLARE_PRIMITIVE_TRAITS( signed char )
_DECLARE_PRIMITIVE_TRAITS( char )
_DECLARE_PRIMITIVE_TRAITS( short )
_DECLARE_PRIMITIVE_TRAITS( int )
_DECLARE_PRIMITIVE_TRAITS( long )
_DECLARE_PRIMITIVE_TRAITS( __int64 )
_DECLARE_PRIMITIVE_TRAITS( float )
_DECLARE_PRIMITIVE_TRAITS( double )
_DECLARE_PRIMITIVE_TRAITS( bool )
#ifdef _NATIVE_WCHAR_T_DEFINED
_DECLARE_PRIMITIVE_TRAITS( wchar_t )
#endif
_DECLARE_PRIMITIVE_TRAITS( void* )

template< typename E, class ETraits = CElementTraits< E > >
class SArray
{
public:
    typedef typename ETraits::INARGTYPE INARGTYPE;
    typedef typename ETraits::OUTARGTYPE OUTARGTYPE;

public:
    SArray();

    size_t GetCount() const;
    bool IsEmpty() const;
    bool SetCount( size_t nNewSize, int nGrowBy = -1 );

    void FreeExtra();
    void RemoveAll();

    const E& GetAt( size_t iElement ) const;
    void SetAt( size_t iElement, INARGTYPE element );
    E& GetAt( size_t iElement );

    const E* GetData() const;
    E* GetData();

    void SetAtGrow( size_t iElement, INARGTYPE element );
    // Add an empty element to the end of the array
    size_t Add();
    // Add an element to the end of the array
    size_t Add( INARGTYPE element );
    size_t Append( const SArray< E, ETraits >& aSrc );
    void Copy( const SArray< E, ETraits >& aSrc );

    const E& operator[]( size_t iElement ) const;
    E& operator[]( size_t iElement );

    void InsertAt( size_t iElement, INARGTYPE element, size_t nCount = 1 );
    void InsertArrayAt( size_t iStart, const SArray< E, ETraits >* paNew );
    void RemoveAt( size_t iElement, size_t nCount = 1 );

#ifdef _DEBUG
    void AssertValid() const;
#endif  // _DEBUG

private:
    bool GrowBuffer( size_t nNewSize );

    // Implementation
private:
    E* m_pData;
    size_t m_nSize;
    size_t m_nMaxSize;
    int m_nGrowBy;

private:
    static void CallConstructors( E* pElements, size_t nElements );
    static void CallDestructors( E* pElements, size_t nElements );

public:
    ~SArray();

};

template< typename E, class ETraits >
inline size_t SArray< E, ETraits >::GetCount() const
{
    return( m_nSize );
}

template< typename E, class ETraits >
inline bool SArray< E, ETraits >::IsEmpty() const
{
    return( m_nSize == 0 );
}

template< typename E, class ETraits >
inline void SArray< E, ETraits >::RemoveAll()
{
    SetCount( 0, -1 );
}

template< typename E, class ETraits >
inline const E& SArray< E, ETraits >::GetAt( size_t iElement ) const
{
    SASSERT( iElement < m_nSize );
    if(iElement >= m_nSize)
        SThrow(E_INVALIDARG);

    return( m_pData[iElement] );
}

template< typename E, class ETraits >
inline void SArray< E, ETraits >::SetAt( size_t iElement, INARGTYPE element )
{
    SASSERT( iElement < m_nSize );
    if(iElement >= m_nSize)
        SThrow(E_INVALIDARG);

    m_pData[iElement] = element;
}

template< typename E, class ETraits >
inline E& SArray< E, ETraits >::GetAt( size_t iElement )
{
    SASSERT( iElement < m_nSize );
    if(iElement >= m_nSize)
        SThrow(E_INVALIDARG);

    return( m_pData[iElement] );
}

template< typename E, class ETraits >
inline const E* SArray< E, ETraits >::GetData() const
{
    return( m_pData );
}

template< typename E, class ETraits >
inline E* SArray< E, ETraits >::GetData()
{
    return( m_pData );
}

template< typename E, class ETraits >
inline size_t SArray< E, ETraits >::Add()
{
    size_t iElement;

    iElement = m_nSize;
    bool bSuccess=SetCount( m_nSize+1 );
    if( !bSuccess )
    {
        SThrow( E_OUTOFMEMORY );
    }

    return( iElement );
}

#pragma push_macro("new")
#undef new

template< typename E, class ETraits >
inline size_t SArray< E, ETraits >::Add( INARGTYPE element )
{
    size_t iElement;

    iElement = m_nSize;
    if( iElement >= m_nMaxSize )
    {
        bool bSuccess = GrowBuffer( iElement+1 );
        if( !bSuccess )
        {
            SThrow( E_OUTOFMEMORY );
        }
    }
    ::new( m_pData+iElement ) E( element );
    m_nSize++;

    return( iElement );
}

#pragma pop_macro("new")

template< typename E, class ETraits >
inline const E& SArray< E, ETraits >::operator[]( size_t iElement ) const
{
    SASSERT( iElement < m_nSize );
    if(iElement >= m_nSize)
        SThrow(E_INVALIDARG);

    return( m_pData[iElement] );
}

template< typename E, class ETraits >
inline E& SArray< E, ETraits >::operator[]( size_t iElement )
{
    SASSERT( iElement < m_nSize );
    if(iElement >= m_nSize)
        SThrow(E_INVALIDARG);

    return( m_pData[iElement] );
}

template< typename E, class ETraits >
SArray< E, ETraits >::SArray() :
    m_pData( NULL ),
    m_nSize( 0 ),
    m_nMaxSize( 0 ),
    m_nGrowBy( 0 )
{
}

template< typename E, class ETraits >
SArray< E, ETraits >::~SArray()
{
    if( m_pData != NULL )
    {
        CallDestructors( m_pData, m_nSize );
        soui_mem_wrapper::SouiFree( m_pData );
    }
}

template< typename E, class ETraits >
bool SArray< E, ETraits >::GrowBuffer( size_t nNewSize )
{
    if( nNewSize > m_nMaxSize )
    {
        if( m_pData == NULL )
        {
            size_t nAllocSize =  size_t( m_nGrowBy ) > nNewSize ? size_t( m_nGrowBy ) : nNewSize ;
            m_pData = static_cast< E* >( soui_mem_wrapper::SouiCalloc( nAllocSize,sizeof( E ) ) );
            if( m_pData == NULL )
            {
                return( false );
            }
            m_nMaxSize = nAllocSize;
        }
        else
        {
            // otherwise, grow array
            size_t nGrowBy = m_nGrowBy;
            if( nGrowBy == 0 )
            {
                // heuristically determine growth when nGrowBy == 0
                //  (this avoids heap fragmentation in many situations)
                nGrowBy = m_nSize/8;
                nGrowBy = (nGrowBy < 4) ? 4 : ((nGrowBy > 1024) ? 1024 : nGrowBy);
            }
            size_t nNewMax;
            if( nNewSize < (m_nMaxSize+nGrowBy) )
                nNewMax = m_nMaxSize+nGrowBy;  // granularity
            else
                nNewMax = nNewSize;  // no slush

            SASSERT( nNewMax >= m_nMaxSize );  // no wrap around
#ifdef SIZE_T_MAX
            SASSERT( nNewMax <= SIZE_T_MAX/sizeof( E ) ); // no overflow
#endif
            E* pNewData = static_cast< E* >( soui_mem_wrapper::SouiCalloc( nNewMax,sizeof( E ) ) );
            if( pNewData == NULL )
            {
                return false;
            }

            // copy new data from old
            ETraits::RelocateElements( pNewData, m_pData, m_nSize );

            // get rid of old stuff (note: no destructors called)
            soui_mem_wrapper::SouiFree( m_pData );
            m_pData = pNewData;
            m_nMaxSize = nNewMax;
        }
    }

    return true;
}

template< typename E, class ETraits >
bool SArray< E, ETraits >::SetCount( size_t nNewSize, int nGrowBy )
{
    SASSERT_VALID(this);

    if( nGrowBy != -1 )
    {
        m_nGrowBy = nGrowBy;  // set new size
    }

    if( nNewSize == 0 )
    {
        // shrink to nothing
        if( m_pData != NULL )
        {
            CallDestructors( m_pData, m_nSize );
            soui_mem_wrapper::SouiFree( m_pData );
            m_pData = NULL;
        }
        m_nSize = 0;
        m_nMaxSize = 0;
    }
    else if( nNewSize <= m_nMaxSize )
    {
        // it fits
        if( nNewSize > m_nSize )
        {
            // initialize the new elements
            CallConstructors( m_pData+m_nSize, nNewSize-m_nSize );
        }
        else if( m_nSize > nNewSize )
        {
            // destroy the old elements
            CallDestructors( m_pData+nNewSize, m_nSize-nNewSize );
        }
        m_nSize = nNewSize;
    }
    else
    {
        bool bSuccess;

        bSuccess = GrowBuffer( nNewSize );
        if( !bSuccess )
        {
            return( false );
        }

        // construct new elements
        SASSERT( nNewSize > m_nSize );
        CallConstructors( m_pData+m_nSize, nNewSize-m_nSize );

        m_nSize = nNewSize;
    }

    return true;
}

template< typename E, class ETraits >
size_t SArray< E, ETraits >::Append( const SArray< E, ETraits >& aSrc )
{
    SASSERT_VALID(this);
    SASSERT( this != &aSrc );   // cannot append to itself

    size_t nOldSize = m_nSize;
    bool bSuccess=SetCount( m_nSize+aSrc.m_nSize );
    if( !bSuccess )
    {
        SThrow( E_OUTOFMEMORY );
    }

    ETraits::CopyElements( m_pData+nOldSize, aSrc.m_pData, aSrc.m_nSize );

    return( nOldSize );
}

template< typename E, class ETraits >
void SArray< E, ETraits >::Copy( const SArray< E, ETraits >& aSrc )
{
    SASSERT_VALID(this);
    SASSERT( this != &aSrc );   // cannot append to itself

    bool bSuccess=SetCount( aSrc.m_nSize );
    if( !bSuccess )
    {
        SThrow( E_OUTOFMEMORY );
    }

    ETraits::CopyElements( m_pData, aSrc.m_pData, aSrc.m_nSize );
}

template< typename E, class ETraits >
void SArray< E, ETraits >::FreeExtra()
{
    SASSERT_VALID(this);

    if( m_nSize != m_nMaxSize )
    {
        // shrink to desired size
#ifdef SIZE_T_MAX
        SASSUME( m_nSize <= (SIZE_T_MAX/sizeof( E )) ); // no overflow
#endif
        E* pNewData = NULL;
        if( m_nSize != 0 )
        {
            pNewData = (E*)soui_mem_wrapper::SouiCalloc( m_nSize,sizeof( E ) );
            if( pNewData == NULL )
            {
                return;
            }

            // copy new data from old
            ETraits::RelocateElements( pNewData, m_pData, m_nSize );
        }

        // get rid of old stuff (note: no destructors called)
        soui_mem_wrapper::SouiFree( m_pData );
        m_pData = pNewData;
        m_nMaxSize = m_nSize;
    }
}

template< typename E, class ETraits >
void SArray< E, ETraits >::SetAtGrow( size_t iElement, INARGTYPE element )
{
    SASSERT_VALID(this);
    size_t nOldSize;

    nOldSize = m_nSize;
    if( iElement >= m_nSize )
    {
        bool bSuccess=SetCount( iElement+1, -1 );
        if( !bSuccess )
        {
            SThrow( E_OUTOFMEMORY );
        }
    }

//     _STRY
    {
        m_pData[iElement] = element;
    }
//     _SCATCHALL()
//     {
//         if( m_nSize != nOldSize )
//         {
//             SetCount( nOldSize, -1 );
//         }
//     }
}

template< typename E, class ETraits >
void SArray< E, ETraits >::InsertAt( size_t iElement, INARGTYPE element, size_t nElements /*=1*/)
{
    SASSERT_VALID(this);
    SASSERT( nElements > 0 );     // zero size not allowed

    if( iElement >= m_nSize )
    {
        // adding after the end of the array
        bool bSuccess=SetCount( iElement+nElements, -1 );   // grow so nIndex is valid
        if( !bSuccess )
        {
            SThrow( E_OUTOFMEMORY );
        }
    }
    else
    {
        // inserting in the middle of the array
        size_t nOldSize = m_nSize;
        bool bSuccess=SetCount( m_nSize+nElements, -1 );  // grow it to new size
        if( !bSuccess )
        {
            SThrow( E_OUTOFMEMORY );
        }
        // destroy intial data before copying over it
        CallDestructors( m_pData+nOldSize, nElements );
        // shift old data up to fill gap
        ETraits::RelocateElements( m_pData+(iElement+nElements), m_pData+iElement,
                                   nOldSize-iElement );

//         _STRY
        {
            // re-init slots we copied from
            CallConstructors( m_pData+iElement, nElements );
        }
//         _SCATCHALL()
//         {
//             ETraits::RelocateElements( m_pData+iElement, m_pData+(iElement+nElements),
//                                        nOldSize-iElement );
//             SetCount( nOldSize, -1 );
//         }
    }

    // insert new value in the gap
    SASSERT( (iElement+nElements) <= m_nSize );
    for( size_t iNewElement = iElement; iNewElement < (iElement+nElements); iNewElement++ )
    {
        m_pData[iNewElement] = element;
    }
}

template< typename E, class ETraits >
void SArray< E, ETraits >::RemoveAt( size_t iElement, size_t nElements )
{
    SASSERT_VALID(this);
    SASSERT( (iElement+nElements) <= m_nSize );

    size_t newCount = iElement+nElements;
    if ((newCount < iElement) || (newCount < nElements) || (newCount > m_nSize))
        SThrow(E_INVALIDARG);

    // just remove a range
    size_t nMoveCount = m_nSize-(newCount);
    CallDestructors( m_pData+iElement, nElements );
    if( nMoveCount > 0 )
    {
        ETraits::RelocateElements( m_pData+iElement, m_pData+(newCount),
                                   nMoveCount );
    }
    m_nSize -= nElements;
}

template< typename E, class ETraits >
void SArray< E, ETraits >::InsertArrayAt( size_t iStartElement,
        const SArray< E, ETraits >* paNew )
{
    SASSERT_VALID( this );
    SENSURE( paNew != NULL );
    SASSERT_VALID( paNew );

    if( paNew->GetCount() > 0 )
    {
        InsertAt( iStartElement, paNew->GetAt( 0 ), paNew->GetCount() );
        for( size_t iElement = 0; iElement < paNew->GetCount(); iElement++ )
        {
            SetAt( iStartElement+iElement, paNew->GetAt( iElement ) );
        }
    }
}

#ifdef _DEBUG
template< typename E, class ETraits >
void SArray< E, ETraits >::AssertValid() const
{
    if( m_pData == NULL )
    {
        SASSUME( m_nSize == 0 );
        SASSUME( m_nMaxSize == 0 );
    }
    else
    {
        SASSUME( m_nSize <= m_nMaxSize );
    }
}
#endif

#pragma push_macro("new")
#undef new

template< typename E, class ETraits >
void SArray< E, ETraits >::CallConstructors( E* pElements, size_t nElements )
{
    size_t iElement = 0;

//     _STRY
    {
        for( iElement = 0; iElement < nElements; iElement++ )
        {
            ::new( pElements+iElement ) E;
        }
    }
//     _SCATCHALL()
//     {
//         while( iElement > 0 )
//         {
//             iElement--;
//             pElements[iElement].~E();
//         }
// 
//     }
}

#pragma pop_macro("new")

template< typename E, class ETraits >
void SArray< E, ETraits >::CallDestructors( E* pElements, size_t nElements )
{
    (void)pElements;

    for( size_t iElement = 0; iElement < nElements; iElement++ )
    {
        pElements[iElement].~E();
    }
}


template< typename E, class ETraits = CElementTraits< E > >
class SList
{
public:
    typedef typename ETraits::INARGTYPE INARGTYPE;

private:
    class CNode :
        public __SPOSITION
    {
    public:
        CNode()
        {
        }
        CNode( INARGTYPE element ) :
            m_element( element )
        {
        }
        ~CNode()
        {
        }

    public:
        CNode* m_pNext;
        CNode* m_pPrev;
        E m_element;

    private:
        CNode( const CNode& );
    };

public:
    SList( UINT nBlockSize = 10 );

    size_t GetCount() const;
    bool IsEmpty() const;

    E& GetHead();
    const E& GetHead() const;
    E& GetTail();
    const E& GetTail() const;

    E RemoveHead();
    E RemoveTail();
    void RemoveHeadNoReturn();
    void RemoveTailNoReturn();

    SPOSITION AddHead();
    SPOSITION AddHead( INARGTYPE element );
    void AddHeadList( const SList< E, ETraits >* plNew );
    SPOSITION AddTail();
    SPOSITION AddTail( INARGTYPE element );
    void AddTailList( const SList< E, ETraits >* plNew );

    void RemoveAll();

    SPOSITION GetHeadPosition() const;
    SPOSITION GetTailPosition() const;
    SPOSITION Next(SPOSITION pos) const;
    SPOSITION Prev(SPOSITION pos) const;
    E& GetNext( SPOSITION& pos );
    const E& GetNext( SPOSITION& pos ) const;
    E& GetPrev( SPOSITION& pos );
    const E& GetPrev( SPOSITION& pos ) const;

    E& GetAt( SPOSITION pos );
    const E& GetAt( SPOSITION pos ) const;
    void SetAt( SPOSITION pos, INARGTYPE element );
    void RemoveAt( SPOSITION pos );

    SPOSITION InsertBefore( SPOSITION pos, INARGTYPE element );
    SPOSITION InsertAfter( SPOSITION pos, INARGTYPE element );

    SPOSITION Find( INARGTYPE element, SPOSITION posStartAfter = NULL ) const;
    SPOSITION FindIndex( size_t iElement ) const;

    void MoveToHead( SPOSITION pos );
    void MoveToTail( SPOSITION pos );
    void SwapElements( SPOSITION pos1, SPOSITION pos2 );

#ifdef _DEBUG
    void AssertValid() const;
#endif  // _DEBUG

    // Implementation
private:
    CNode* m_pHead;
    CNode* m_pTail;
    size_t m_nElements;
    SPlex* m_pBlocks;
    CNode* m_pFree;
    UINT m_nBlockSize;

private:
    void GetFreeNode();
    CNode* NewNode( CNode* pPrev, CNode* pNext );
    CNode* NewNode( INARGTYPE element, CNode* pPrev, CNode* pNext );
    void FreeNode( CNode* pNode );

public:
    ~SList();

private:
    // Private to prevent use
    SList( const SList& );
    SList& operator=( const SList& );
};

template< typename E, class ETraits >
inline size_t SList< E, ETraits >::GetCount() const
{
    return( m_nElements );
}

template< typename E, class ETraits >
inline bool SList< E, ETraits >::IsEmpty() const
{
    return( m_nElements == 0 );
}

template< typename E, class ETraits >
inline E& SList< E, ETraits >::GetHead()
{
    SENSURE( m_pHead != NULL );
    return( m_pHead->m_element );
}

template< typename E, class ETraits >
inline const E& SList< E, ETraits >::GetHead() const
{
    SENSURE( m_pHead != NULL );
    return( m_pHead->m_element );
}

template< typename E, class ETraits >
inline E& SList< E, ETraits >::GetTail()
{
    SENSURE( m_pTail != NULL );
    return( m_pTail->m_element );
}

template< typename E, class ETraits >
inline const E& SList< E, ETraits >::GetTail() const
{
    SENSURE( m_pTail != NULL );
    return( m_pTail->m_element );
}

template< typename E, class ETraits >
inline SPOSITION SList< E, ETraits >::GetHeadPosition() const
{
    return( SPOSITION( m_pHead ) );
}

template< typename E, class ETraits >
inline SPOSITION SList< E, ETraits >::GetTailPosition() const
{
    return( SPOSITION( m_pTail ) );
}

template< typename E, class ETraits >
inline SPOSITION SList< E, ETraits >::Next( SPOSITION pos ) const
{
    CNode* pNode;

    SENSURE( pos != NULL );
    pNode = (CNode*)pos;
    return SPOSITION( pNode->m_pNext );
}

template< typename E, class ETraits >
inline SPOSITION SList< E, ETraits >::Prev( SPOSITION pos ) const
{
    CNode* pNode;

    SENSURE( pos != NULL );
    pNode = (CNode*)pos;
    return SPOSITION( pNode->m_pPrev );
}


template< typename E, class ETraits >
inline E& SList< E, ETraits >::GetNext( SPOSITION& pos )
{
    CNode* pNode;

    SENSURE( pos != NULL );
    pNode = (CNode*)pos;
    pos = SPOSITION( pNode->m_pNext );

    return( pNode->m_element );
}


template< typename E, class ETraits >
inline const E& SList< E, ETraits >::GetNext( SPOSITION& pos ) const
{
    CNode* pNode;

    SENSURE( pos != NULL );
    pNode = (CNode*)pos;
    pos = SPOSITION( pNode->m_pNext );

    return( pNode->m_element );
}

template< typename E, class ETraits >
inline E& SList< E, ETraits >::GetPrev( SPOSITION& pos )
{
    CNode* pNode;

    SENSURE( pos != NULL );
    pNode = (CNode*)pos;
    pos = SPOSITION( pNode->m_pPrev );

    return( pNode->m_element );
}

template< typename E, class ETraits >
inline const E& SList< E, ETraits >::GetPrev( SPOSITION& pos ) const
{
    CNode* pNode;

    SASSERT( pos != NULL );
    pNode = (CNode*)pos;
    pos = SPOSITION( pNode->m_pPrev );

    return( pNode->m_element );
}

template< typename E, class ETraits >
inline E& SList< E, ETraits >::GetAt( SPOSITION pos )
{
    SENSURE( pos != NULL );
    CNode* pNode = (CNode*)pos;
    return( pNode->m_element );
}

template< typename E, class ETraits >
inline const E& SList< E, ETraits >::GetAt( SPOSITION pos ) const
{
    SENSURE( pos != NULL );
    CNode* pNode = (CNode*)pos;
    return( pNode->m_element );
}

template< typename E, class ETraits >
inline void SList< E, ETraits >::SetAt( SPOSITION pos, INARGTYPE element )
{
    SENSURE( pos != NULL );
    CNode* pNode = (CNode*)pos;
    pNode->m_element = element;
}

template< typename E, class ETraits >
SList< E, ETraits >::SList( UINT nBlockSize ) :
    m_pHead( NULL ),
    m_pTail( NULL ),
    m_nElements(0),
    m_pBlocks( NULL ),
    m_pFree( NULL ),
    m_nBlockSize(nBlockSize)
{
    SASSERT( nBlockSize > 0 );
}

template< typename E, class ETraits >
void SList< E, ETraits >::RemoveAll()
{
    while( m_nElements > 0 )
    {
        CNode* pKill = m_pHead;
        SENSURE( pKill != NULL );

        m_pHead = m_pHead->m_pNext;
        FreeNode( pKill );
    }

    SASSUME( m_nElements == 0 );
    m_pHead = NULL;
    m_pTail = NULL;
    m_pFree = NULL;

    if( m_pBlocks != NULL )
    {
        m_pBlocks->FreeDataChain();
        m_pBlocks = NULL;
    }
}

template< typename E, class ETraits >
SList< E, ETraits >::~SList()
{
    RemoveAll();
    SASSUME( m_nElements == 0 );
}


template< typename E, class ETraits >
void SList< E, ETraits >::GetFreeNode()
{
    if( m_pFree == NULL )
    {
        SPlex* pPlex;
        CNode* pNode;

        pPlex = SPlex::Create( m_pBlocks, m_nBlockSize, sizeof( CNode ) );
        if( pPlex == NULL )
        {
            SThrow( E_OUTOFMEMORY );
        }
        pNode = (CNode*)pPlex->data();
        pNode += m_nBlockSize-1;
        for( int iBlock = m_nBlockSize-1; iBlock >= 0; iBlock-- )
        {
            pNode->m_pNext = m_pFree;
            m_pFree = pNode;
            pNode--;
        }
    }
    SASSUME( m_pFree != NULL );
}

#pragma push_macro("new")
#undef new
template< typename E, class ETraits >
typename SList< E, ETraits >::CNode* SList< E, ETraits >::NewNode( CNode* pPrev, CNode* pNext )
{
    GetFreeNode();

    CNode* pNewNode = m_pFree;
    CNode* pNextFree = m_pFree->m_pNext;

    ::new( pNewNode ) CNode;

    m_pFree = pNextFree;
    pNewNode->m_pPrev = pPrev;
    pNewNode->m_pNext = pNext;
    m_nElements++;
    SASSUME( m_nElements > 0 );

    return( pNewNode );
} 
    
template< typename E, class ETraits >
typename SList< E, ETraits >::CNode* SList< E, ETraits >::NewNode( INARGTYPE element, CNode* pPrev,
        CNode* pNext )
{
    GetFreeNode();

    CNode* pNewNode = m_pFree;
    CNode* pNextFree = m_pFree->m_pNext;

    ::new( pNewNode ) CNode( element );

    m_pFree = pNextFree;
    pNewNode->m_pPrev = pPrev;
    pNewNode->m_pNext = pNext;
    m_nElements++;
    SASSUME( m_nElements > 0 );

    return( pNewNode );
}

#pragma pop_macro("new")

template< typename E, class ETraits >
void SList< E, ETraits >::FreeNode( CNode* pNode )
{
    pNode->~CNode();
    pNode->m_pNext = m_pFree;
    m_pFree = pNode;
    SASSUME( m_nElements > 0 );
    m_nElements--;
    if( m_nElements == 0 )
    {
        RemoveAll();
    }
}

template< typename E, class ETraits >
SPOSITION SList< E, ETraits >::AddHead()
{
    CNode* pNode = NewNode( NULL, m_pHead );
    if( m_pHead != NULL )
    {
        m_pHead->m_pPrev = pNode;
    }
    else
    {
        m_pTail = pNode;
    }
    m_pHead = pNode;

    return( SPOSITION( pNode ) );
}

template< typename E, class ETraits >
SPOSITION SList< E, ETraits >::AddHead( INARGTYPE element )
{
    CNode* pNode;

    pNode = NewNode( element, NULL, m_pHead );

    if( m_pHead != NULL )
    {
        m_pHead->m_pPrev = pNode;
    }
    else
    {
        m_pTail = pNode;
    }
    m_pHead = pNode;

    return( SPOSITION( pNode ) );
}

template< typename E, class ETraits >
SPOSITION SList< E, ETraits >::AddTail()
{
    CNode* pNode = NewNode( m_pTail, NULL );
    if( m_pTail != NULL )
    {
        m_pTail->m_pNext = pNode;
    }
    else
    {
        m_pHead = pNode;
    }
    m_pTail = pNode;

    return( SPOSITION( pNode ) );
}

template< typename E, class ETraits >
SPOSITION SList< E, ETraits >::AddTail( INARGTYPE element )
{
    CNode* pNode;

    pNode = NewNode( element, m_pTail, NULL );

    if( m_pTail != NULL )
    {
        m_pTail->m_pNext = pNode;
    }
    else
    {
        m_pHead = pNode;
    }
    m_pTail = pNode;

    return( SPOSITION( pNode ) );
}

template< typename E, class ETraits >
void SList< E, ETraits >::AddHeadList( const SList< E, ETraits >* plNew )
{
    SENSURE( plNew != NULL );

    SPOSITION pos = plNew->GetTailPosition();
    while( pos != NULL )
    {
        INARGTYPE element = plNew->GetPrev( pos );
        AddHead( element );
    }
}

template< typename E, class ETraits >
void SList< E, ETraits >::AddTailList( const SList< E, ETraits >* plNew )
{
    SENSURE( plNew != NULL );

    SPOSITION pos = plNew->GetHeadPosition();
    while( pos != NULL )
    {
        INARGTYPE element = plNew->GetNext( pos );
        AddTail( element );
    }
}

template< typename E, class ETraits >
E SList< E, ETraits >::RemoveHead()
{
    SENSURE( m_pHead != NULL );

    CNode* pNode = m_pHead;
    E element( pNode->m_element );

    m_pHead = pNode->m_pNext;
    if( m_pHead != NULL )
    {
        m_pHead->m_pPrev = NULL;
    }
    else
    {
        m_pTail = NULL;
    }
    FreeNode( pNode );

    return( element );
}

template< typename E, class ETraits >
void SList< E, ETraits >::RemoveHeadNoReturn()
{
    SENSURE( m_pHead != NULL );

    CNode* pNode = m_pHead;

    m_pHead = pNode->m_pNext;
    if( m_pHead != NULL )
    {
        m_pHead->m_pPrev = NULL;
    }
    else
    {
        m_pTail = NULL;
    }
    FreeNode( pNode );
}

template< typename E, class ETraits >
E SList< E, ETraits >::RemoveTail()
{
    SENSURE( m_pTail != NULL );

    CNode* pNode = m_pTail;

    E element( pNode->m_element );

    m_pTail = pNode->m_pPrev;
    if( m_pTail != NULL )
    {
        m_pTail->m_pNext = NULL;
    }
    else
    {
        m_pHead = NULL;
    }
    FreeNode( pNode );

    return( element );
}

template< typename E, class ETraits >
void SList< E, ETraits >::RemoveTailNoReturn()
{
    SENSURE( m_pTail != NULL );

    CNode* pNode = m_pTail;

    m_pTail = pNode->m_pPrev;
    if( m_pTail != NULL )
    {
        m_pTail->m_pNext = NULL;
    }
    else
    {
        m_pHead = NULL;
    }
    FreeNode( pNode );
}

template< typename E, class ETraits >
SPOSITION SList< E, ETraits >::InsertBefore( SPOSITION pos, INARGTYPE element )
{
    SASSERT_VALID(this);

    if( pos == NULL )
        return AddHead( element ); // insert before nothing -> head of the list

    // Insert it before position
    CNode* pOldNode = (CNode*)pos;
    CNode* pNewNode = NewNode( element, pOldNode->m_pPrev, pOldNode );

    if( pOldNode->m_pPrev != NULL )
    {
        pOldNode->m_pPrev->m_pNext = pNewNode;
    }
    else
    {
        SASSERT( pOldNode == m_pHead );
        m_pHead = pNewNode;
    }
    pOldNode->m_pPrev = pNewNode;

    return( SPOSITION( pNewNode ) );
}

template< typename E, class ETraits >
SPOSITION SList< E, ETraits >::InsertAfter( SPOSITION pos, INARGTYPE element )
{
    SASSERT_VALID(this);

    if( pos == NULL )
        return AddTail( element ); // insert after nothing -> tail of the list

    // Insert it after position
    CNode* pOldNode = (CNode*)pos;
    CNode* pNewNode = NewNode( element, pOldNode, pOldNode->m_pNext );

    if( pOldNode->m_pNext != NULL )
    {
        pOldNode->m_pNext->m_pPrev = pNewNode;
    }
    else
    {
        SASSERT( pOldNode == m_pTail );
        m_pTail = pNewNode;
    }
    pOldNode->m_pNext = pNewNode;

    return( SPOSITION( pNewNode ) );
}

template< typename E, class ETraits >
void SList< E, ETraits >::RemoveAt( SPOSITION pos )
{
    SASSERT_VALID(this);
    SENSURE( pos != NULL );

    CNode* pOldNode = (CNode*)pos;

    // remove pOldNode from list
    if( pOldNode == m_pHead )
    {
        m_pHead = pOldNode->m_pNext;
    }
    else
    {
        pOldNode->m_pPrev->m_pNext = pOldNode->m_pNext;
    }
    if( pOldNode == m_pTail )
    {
        m_pTail = pOldNode->m_pPrev;
    }
    else
    {
        pOldNode->m_pNext->m_pPrev = pOldNode->m_pPrev;
    }
    FreeNode( pOldNode );
}

template< typename E, class ETraits >
SPOSITION SList< E, ETraits >::FindIndex( size_t iElement ) const
{
    SASSERT_VALID(this);

    if( iElement >= m_nElements )
        return NULL;  // went too far

    if(m_pHead == NULL)
        return NULL;

    CNode* pNode = m_pHead;
    for( size_t iSearch = 0; iSearch < iElement; iSearch++ )
    {
        pNode = pNode->m_pNext;
    }

    return( SPOSITION( pNode ) );
}

template< typename E, class ETraits >
void SList< E, ETraits >::MoveToHead( SPOSITION pos )
{
    SENSURE( pos != NULL );

    CNode* pNode = static_cast< CNode* >( pos );

    if( pNode == m_pHead )
    {
        // Already at the head
        return;
    }

    if( pNode->m_pNext == NULL )
    {
        SASSERT( pNode == m_pTail );
        m_pTail = pNode->m_pPrev;
    }
    else
    {
        pNode->m_pNext->m_pPrev = pNode->m_pPrev;
    }

    SASSERT( pNode->m_pPrev != NULL );  // This node can't be the head, since we already checked that case
    pNode->m_pPrev->m_pNext = pNode->m_pNext;

    m_pHead->m_pPrev = pNode;
    pNode->m_pNext = m_pHead;
    pNode->m_pPrev = NULL;
    m_pHead = pNode;
}

template< typename E, class ETraits >
void SList< E, ETraits >::MoveToTail( SPOSITION pos )
{
    SENSURE( pos != NULL );
    CNode* pNode = static_cast< CNode* >( pos );

    if( pNode == m_pTail )
    {
        // Already at the tail
        return;
    }

    if( pNode->m_pPrev == NULL )
    {
        SENSURE( pNode == m_pHead );
        m_pHead = pNode->m_pNext;
    }
    else
    {
        pNode->m_pPrev->m_pNext = pNode->m_pNext;
    }

    pNode->m_pNext->m_pPrev = pNode->m_pPrev;

    m_pTail->m_pNext = pNode;
    pNode->m_pPrev = m_pTail;
    pNode->m_pNext = NULL;
    m_pTail = pNode;
}

template< typename E, class ETraits >
void SList< E, ETraits >::SwapElements( SPOSITION pos1, SPOSITION pos2 )
{
    SASSERT( pos1 != NULL );
    SASSERT( pos2 != NULL );

    if( pos1 == pos2 )
    {
        // Nothing to do
        return;
    }

    CNode* pNode1 = static_cast< CNode* >( pos1 );
    CNode* pNode2 = static_cast< CNode* >( pos2 );
    if( pNode2->m_pNext == pNode1 )
    {
        // Swap pNode2 and pNode1 so that the next case works
        CNode* pNodeTemp = pNode1;
        pNode1 = pNode2;
        pNode2 = pNodeTemp;
    }
    if( pNode1->m_pNext == pNode2 )
    {
        // Node1 and Node2 are adjacent
        pNode2->m_pPrev = pNode1->m_pPrev;
        if( pNode1->m_pPrev != NULL )
        {
            pNode1->m_pPrev->m_pNext = pNode2;
        }
        else
        {
            SASSUME( m_pHead == pNode1 );
            m_pHead = pNode2;
        }
        pNode1->m_pNext = pNode2->m_pNext;
        if( pNode2->m_pNext != NULL )
        {
            pNode2->m_pNext->m_pPrev = pNode1;
        }
        else
        {
            SASSUME( m_pTail == pNode2 );
            m_pTail = pNode1;
        }
        pNode2->m_pNext = pNode1;
        pNode1->m_pPrev = pNode2;
    }
    else
    {
        // The two nodes are not adjacent
        CNode* pNodeTemp;

        pNodeTemp = pNode1->m_pPrev;
        pNode1->m_pPrev = pNode2->m_pPrev;
        pNode2->m_pPrev = pNodeTemp;

        pNodeTemp = pNode1->m_pNext;
        pNode1->m_pNext = pNode2->m_pNext;
        pNode2->m_pNext = pNodeTemp;

        if( pNode1->m_pNext != NULL )
        {
            pNode1->m_pNext->m_pPrev = pNode1;
        }
        else
        {
            SASSUME( m_pTail == pNode2 );
            m_pTail = pNode1;
        }
        if( pNode1->m_pPrev != NULL )
        {
            pNode1->m_pPrev->m_pNext = pNode1;
        }
        else
        {
            SASSUME( m_pHead == pNode2 );
            m_pHead = pNode1;
        }
        if( pNode2->m_pNext != NULL )
        {
            pNode2->m_pNext->m_pPrev = pNode2;
        }
        else
        {
            SASSUME( m_pTail == pNode1 );
            m_pTail = pNode2;
        }
        if( pNode2->m_pPrev != NULL )
        {
            pNode2->m_pPrev->m_pNext = pNode2;
        }
        else
        {
            SASSUME( m_pHead == pNode1 );
            m_pHead = pNode2;
        }
    }
}

template< typename E, class ETraits >
SPOSITION SList< E, ETraits >::Find( INARGTYPE element, SPOSITION posStartAfter ) const
{
    SASSERT_VALID(this);

    CNode* pNode = (CNode*)posStartAfter;
    if( pNode == NULL )
    {
        pNode = m_pHead;  // start at head
    }
    else
    {
        pNode = pNode->m_pNext;  // start after the one specified
    }

    for( ; pNode != NULL; pNode = pNode->m_pNext )
    {
        if( ETraits::CompareElements( pNode->m_element, element ) )
            return( SPOSITION( pNode ) );
    }

    return( NULL );
}

#ifdef _DEBUG
template< typename E, class ETraits >
void SList< E, ETraits >::AssertValid() const
{
    if( IsEmpty() )
    {
        // empty list
        SASSUME(m_pHead == NULL);
        SASSUME(m_pTail == NULL);
    }
    else
    {
        // non-empty list
    }
}
#endif

template< typename K, typename V, class KTraits = CElementTraits< K >, class VTraits = CElementTraits< V > >
class SMap
{
public:
    typedef typename KTraits::INARGTYPE KINARGTYPE;
    typedef typename KTraits::OUTARGTYPE KOUTARGTYPE;
    typedef typename VTraits::INARGTYPE VINARGTYPE;
    typedef typename VTraits::OUTARGTYPE VOUTARGTYPE;

    class CPair :
        public __SPOSITION
    {
    protected:
        CPair( KINARGTYPE key ) :
            m_key( key )
        {
        }

    public:
        const K m_key;
        V m_value;
    };

private:
    class CNode :
        public CPair
    {
    public:
        CNode( KINARGTYPE key, UINT nHash ) :
            CPair( key ),
            m_nHash( nHash )
        {
        }

    public:
        UINT GetHash() const
        {
            return( m_nHash );
        }

    public:
        CNode* m_pNext;
        UINT m_nHash;
    };

public:
    SMap( UINT nBins = 17, float fOptimalLoad = 0.75f,
             float fLoThreshold = 0.25f, float fHiThreshold = 2.25f, UINT nBlockSize = 10 );

    size_t GetCount() const;
    bool IsEmpty() const;

    bool Lookup( KINARGTYPE key, VOUTARGTYPE value ) const;
    const CPair* Lookup( KINARGTYPE key ) const;
    CPair* Lookup( KINARGTYPE key );
    V& operator[]( KINARGTYPE key ) ;

    SPOSITION SetAt( KINARGTYPE key, VINARGTYPE value );
    void SetValueAt( SPOSITION pos, VINARGTYPE value );

    bool RemoveKey( KINARGTYPE key );
    void RemoveAll();
    void RemoveAtPos( SPOSITION pos );

    SPOSITION GetStartPosition() const;
    void GetNextAssoc( SPOSITION& pos, KOUTARGTYPE key, VOUTARGTYPE value ) const;
    const CPair* GetNext( SPOSITION& pos ) const;
    CPair* GetNext( SPOSITION& pos );
    const K& GetNextKey( SPOSITION& pos ) const;
    const V& GetNextValue( SPOSITION& pos ) const;
    V& GetNextValue( SPOSITION& pos );
    void GetAt( SPOSITION pos, KOUTARGTYPE key, VOUTARGTYPE value ) const;
    CPair* GetAt( SPOSITION pos );
    const CPair* GetAt( SPOSITION pos ) const;
    const K& GetKeyAt( SPOSITION pos ) const;
    const V& GetValueAt( SPOSITION pos ) const;
    V& GetValueAt( SPOSITION pos );

    UINT GetHashTableSize() const;
    bool InitHashTable( UINT nBins, bool bAllocNow = true );
    void EnableAutoRehash();
    void DisableAutoRehash();
    void Rehash( UINT nBins = 0 );
    void SetOptimalLoad( float fOptimalLoad, float fLoThreshold, float fHiThreshold,
                         bool bRehashNow = false );

#ifdef _DEBUG
    void AssertValid() const;
#endif  // _DEBUG

    // Implementation
private:
    CNode** m_ppBins;
    size_t m_nElements;
    UINT m_nBins;
    float m_fOptimalLoad;
    float m_fLoThreshold;
    float m_fHiThreshold;
    size_t m_nHiRehashThreshold;
    size_t m_nLoRehashThreshold;
    ULONG m_nLockCount;
    UINT m_nBlockSize;
    SPlex* m_pBlocks;
    CNode* m_pFree;

private:
    bool IsLocked() const;
    UINT PickSize( size_t nElements ) const;
    CNode* NewNode( KINARGTYPE key, UINT iBin, UINT nHash );
    void FreeNode( CNode* pNode );
    void FreePlexes();
    CNode* GetNode( KINARGTYPE key, UINT& iBin, UINT& nHash, CNode*& pPrev ) const;
    CNode* CreateNode( KINARGTYPE key, UINT iBin, UINT nHash ) ;
    void RemoveNode( CNode* pNode, CNode* pPrev );
    CNode* FindNextNode( CNode* pNode ) const;
    void UpdateRehashThresholds();

public:
    ~SMap();

private:
    // Private to prevent use
    SMap( const SMap& );
    SMap& operator=( const SMap& );
};


template< typename K, typename V, class KTraits, class VTraits >
inline size_t SMap< K, V, KTraits, VTraits >::GetCount() const
{
    return( m_nElements );
}

template< typename K, typename V, class KTraits, class VTraits >
inline bool SMap< K, V, KTraits, VTraits >::IsEmpty() const
{
    return( m_nElements == 0 );
}

template< typename K, typename V, class KTraits, class VTraits >
inline V& SMap< K, V, KTraits, VTraits >::operator[]( KINARGTYPE key ) 
{
    CNode* pNode;
    UINT iBin;
    UINT nHash;
    CNode* pPrev;

    pNode = GetNode( key, iBin, nHash, pPrev );
    if( pNode == NULL )
    {
        pNode = CreateNode( key, iBin, nHash );
    }

    return( pNode->m_value );
}

template< typename K, typename V, class KTraits, class VTraits >
inline UINT SMap< K, V, KTraits, VTraits >::GetHashTableSize() const
{
    return( m_nBins );
}

template< typename K, typename V, class KTraits, class VTraits >
inline void SMap< K, V, KTraits, VTraits >::GetAt( SPOSITION pos, KOUTARGTYPE key, VOUTARGTYPE value ) const
{
    SENSURE( pos != NULL );

    CNode* pNode = static_cast< CNode* >( pos );

    key = pNode->m_key;
    value = pNode->m_value;
}

template< typename K, typename V, class KTraits, class VTraits >
inline typename SMap< K, V, KTraits, VTraits >::CPair* SMap< K, V, KTraits, VTraits >::GetAt( SPOSITION pos )
{
    SASSERT( pos != NULL );

    return( static_cast< CPair* >( pos ) );
}

template< typename K, typename V, class KTraits, class VTraits >
inline const typename SMap< K, V, KTraits, VTraits >::CPair* SMap< K, V, KTraits, VTraits >::GetAt( SPOSITION pos ) const
{
    SASSERT( pos != NULL );

    return( static_cast< const CPair* >( pos ) );
}

template< typename K, typename V, class KTraits, class VTraits >
inline const K& SMap< K, V, KTraits, VTraits >::GetKeyAt( SPOSITION pos ) const
{
    SENSURE( pos != NULL );

    CNode* pNode = (CNode*)pos;

    return( pNode->m_key );
}

template< typename K, typename V, class KTraits, class VTraits >
inline const V& SMap< K, V, KTraits, VTraits >::GetValueAt( SPOSITION pos ) const
{
    SENSURE( pos != NULL );

    CNode* pNode = (CNode*)pos;

    return( pNode->m_value );
}

template< typename K, typename V, class KTraits, class VTraits >
inline V& SMap< K, V, KTraits, VTraits >::GetValueAt( SPOSITION pos )
{
    SENSURE( pos != NULL );

    CNode* pNode = (CNode*)pos;

    return( pNode->m_value );
}

template< typename K, typename V, class KTraits, class VTraits >
inline void SMap< K, V, KTraits, VTraits >::DisableAutoRehash()
{
    m_nLockCount++;
}

template< typename K, typename V, class KTraits, class VTraits >
inline void SMap< K, V, KTraits, VTraits >::EnableAutoRehash()
{
    SASSUME( m_nLockCount > 0 );
    m_nLockCount--;
}

template< typename K, typename V, class KTraits, class VTraits >
inline bool SMap< K, V, KTraits, VTraits >::IsLocked() const
{
    return( m_nLockCount != 0 );
}

template< typename K, typename V, class KTraits, class VTraits >
UINT SMap< K, V, KTraits, VTraits >::PickSize( size_t nElements ) const
{
    // List of primes such that s_anPrimes[i] is the smallest prime greater than 2^(5+i/3)
    static const UINT s_anPrimes[] =
    {
        17, 23, 29, 37, 41, 53, 67, 83, 103, 131, 163, 211, 257, 331, 409, 521, 647, 821,
        1031, 1291, 1627, 2053, 2591, 3251, 4099, 5167, 6521, 8209, 10331,
        13007, 16411, 20663, 26017, 32771, 41299, 52021, 65537, 82571, 104033,
        131101, 165161, 208067, 262147, 330287, 416147, 524309, 660563,
        832291, 1048583, 1321139, 1664543, 2097169, 2642257, 3329023, 4194319,
        5284493, 6658049, 8388617, 10568993, 13316089, UINT_MAX
    };

    size_t nBins = (size_t)(nElements/m_fOptimalLoad);
    UINT nBinsEstimate = UINT(  UINT_MAX < nBins ? UINT_MAX : nBins );

    // Find the smallest prime greater than our estimate
    int iPrime = 0;
    while( nBinsEstimate > s_anPrimes[iPrime] )
    {
        iPrime++;
    }

    if( s_anPrimes[iPrime] == UINT_MAX )
    {
        return( nBinsEstimate );
    }
    else
    {
        return( s_anPrimes[iPrime] );
    }
}

template< typename K, typename V, class KTraits, class VTraits >
typename SMap< K, V, KTraits, VTraits >::CNode* SMap< K, V, KTraits, VTraits >::CreateNode(
    KINARGTYPE key, UINT iBin, UINT nHash ) 
{
    CNode* pNode;

    if( m_ppBins == NULL )
    {
        bool bSuccess;

        bSuccess = InitHashTable( m_nBins );
        if( !bSuccess )
        {
            SThrow( E_OUTOFMEMORY );
        }
    }

    pNode = NewNode( key, iBin, nHash );

    return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
SPOSITION SMap< K, V, KTraits, VTraits >::GetStartPosition() const
{
    if( IsEmpty() )
    {
        return( NULL );
    }

    for( UINT iBin = 0; iBin < m_nBins; iBin++ )
    {
        if( m_ppBins[iBin] != NULL )
        {
            return( SPOSITION( m_ppBins[iBin] ) );
        }
    }
    SASSERT( false );

    return( NULL );
}

template< typename K, typename V, class KTraits, class VTraits >
SPOSITION SMap< K, V, KTraits, VTraits >::SetAt( KINARGTYPE key, VINARGTYPE value )
{
    CNode* pNode;
    UINT iBin;
    UINT nHash;
    CNode* pPrev;

    pNode = GetNode( key, iBin, nHash, pPrev );
    if( pNode == NULL )
    {
        pNode = CreateNode( key, iBin, nHash );
//         _STRY
        {
            pNode->m_value = value;
        }
//         _SCATCHALL()
//         {
//             RemoveAtPos( SPOSITION( pNode ) );
//         }
    }
    else
    {
        pNode->m_value = value;
    }

    return( SPOSITION( pNode ) );
}

template< typename K, typename V, class KTraits, class VTraits >
void SMap< K, V, KTraits, VTraits >::SetValueAt( SPOSITION pos, VINARGTYPE value )
{
    SASSERT( pos != NULL );

    CNode* pNode = static_cast< CNode* >( pos );

    pNode->m_value = value;
}

template< typename K, typename V, class KTraits, class VTraits >
SMap< K, V, KTraits, VTraits >::SMap( UINT nBins, float fOptimalLoad,
        float fLoThreshold, float fHiThreshold, UINT nBlockSize ) :
    m_ppBins( NULL ),
    m_nElements( 0 ),
    m_nBins(nBins),
    m_fOptimalLoad( fOptimalLoad ),
    m_fLoThreshold( fLoThreshold ),
    m_fHiThreshold( fHiThreshold ),
    m_nHiRehashThreshold( UINT_MAX ),
    m_nLoRehashThreshold( 0 ),
    m_nLockCount(0),  // Start unlocked
    m_nBlockSize( nBlockSize ),
    m_pBlocks(NULL),
    m_pFree(NULL)
{
    SASSERT( nBins > 0 );
    SASSERT( nBlockSize > 0 );

    SetOptimalLoad( fOptimalLoad, fLoThreshold, fHiThreshold, false );
}

template< typename K, typename V, class KTraits, class VTraits >
void SMap< K, V, KTraits, VTraits >::SetOptimalLoad( float fOptimalLoad, float fLoThreshold,
        float fHiThreshold, bool bRehashNow )
{
    SASSERT( fOptimalLoad > 0 );
    SASSERT( (fLoThreshold >= 0) && (fLoThreshold < fOptimalLoad) );
    SASSERT( fHiThreshold > fOptimalLoad );

    m_fOptimalLoad = fOptimalLoad;
    m_fLoThreshold = fLoThreshold;
    m_fHiThreshold = fHiThreshold;

    UpdateRehashThresholds();

    if( bRehashNow && ((m_nElements > m_nHiRehashThreshold) ||
                       (m_nElements < m_nLoRehashThreshold)) )
    {
        Rehash( PickSize( m_nElements ) );
    }
}

template< typename K, typename V, class KTraits, class VTraits >
void SMap< K, V, KTraits, VTraits >::UpdateRehashThresholds()
{
    m_nHiRehashThreshold = size_t( m_fHiThreshold*m_nBins );
    m_nLoRehashThreshold = size_t( m_fLoThreshold*m_nBins );
    if( m_nLoRehashThreshold < 17 )
    {
        m_nLoRehashThreshold = 0;
    }
}

template< typename K, typename V, class KTraits, class VTraits >
bool SMap< K, V, KTraits, VTraits >::InitHashTable( UINT nBins, bool bAllocNow )
{
    SASSUME( m_nElements == 0 );
    SASSERT( nBins > 0 );

    if( m_ppBins != NULL )
    {
        soui_mem_wrapper::SouiFree(m_ppBins);
        m_ppBins = NULL;
    }

    if( bAllocNow )
    {
        //hjx            STRY( m_ppBins = new CNode*[nBins] );
        m_ppBins = (CNode**)soui_mem_wrapper::SouiMalloc(nBins*sizeof(CNode*));
        if( m_ppBins == NULL )
        {
            return false;
        }

        SENSURE( UINT_MAX / sizeof( CNode* ) >= nBins );
        memset( m_ppBins, 0, sizeof( CNode* )*nBins );
    }
    m_nBins = nBins;

    UpdateRehashThresholds();

    return true;
}

template< typename K, typename V, class KTraits, class VTraits >
void SMap< K, V, KTraits, VTraits >::RemoveAll()
{
    DisableAutoRehash();
    if( m_ppBins != NULL )
    {
        for( UINT iBin = 0; iBin < m_nBins; iBin++ )
        {
            CNode* pNext;

            pNext = m_ppBins[iBin];
            while( pNext != NULL )
            {
                CNode* pKill;

                pKill = pNext;
                pNext = pNext->m_pNext;
                FreeNode( pKill );
            }
        }
    }

    soui_mem_wrapper::SouiFree(m_ppBins);
    m_ppBins = NULL;
    m_nElements = 0;

    if( !IsLocked() )
    {
        InitHashTable( PickSize( m_nElements ), false );
    }

    FreePlexes();
    EnableAutoRehash();
}

template< typename K, typename V, class KTraits, class VTraits >
SMap< K, V, KTraits, VTraits >::~SMap()
{
    RemoveAll();
}

#pragma push_macro("new")
#undef new

template< typename K, typename V, class KTraits, class VTraits >
typename SMap< K, V, KTraits, VTraits >::CNode* SMap< K, V, KTraits, VTraits >::NewNode(
    KINARGTYPE key, UINT iBin, UINT nHash )
{
    CNode* pNewNode;

    if( m_pFree == NULL )
    {
        SPlex* pPlex;
        CNode* pNode;

        pPlex = SPlex::Create( m_pBlocks, m_nBlockSize, sizeof( CNode ) );
        if( pPlex == NULL )
        {
            SThrow( E_OUTOFMEMORY );
        }
        pNode = (CNode*)pPlex->data();
        pNode += m_nBlockSize-1;
        for( int iBlock = m_nBlockSize-1; iBlock >= 0; iBlock-- )
        {
            pNode->m_pNext = m_pFree;
            m_pFree = pNode;
            pNode--;
        }
    }
    SENSURE(m_pFree != NULL );
    pNewNode = m_pFree;
    m_pFree = pNewNode->m_pNext;

//     _STRY
    {
        ::new( pNewNode ) CNode( key, nHash );
    }
//     _SCATCHALL()
//     {
//         pNewNode->m_pNext = m_pFree;
//         m_pFree = pNewNode;
// 
//     }
    m_nElements++;

    pNewNode->m_pNext = m_ppBins[iBin];
    m_ppBins[iBin] = pNewNode;

    if( (m_nElements > m_nHiRehashThreshold) && !IsLocked() )
    {
        Rehash( PickSize( m_nElements ) );
    }

    return( pNewNode );
}

#pragma pop_macro("new")

template< typename K, typename V, class KTraits, class VTraits >
void SMap< K, V, KTraits, VTraits >::FreeNode( CNode* pNode )
{
    SENSURE( pNode != NULL );

    pNode->~CNode();
    pNode->m_pNext = m_pFree;
    m_pFree = pNode;

    SASSUME( m_nElements > 0 );
    m_nElements--;

    if( (m_nElements < m_nLoRehashThreshold) && !IsLocked() )
    {
        Rehash( PickSize( m_nElements ) );
    }

    if( m_nElements == 0 )
    {
        FreePlexes();
    }
}

template< typename K, typename V, class KTraits, class VTraits >
void SMap< K, V, KTraits, VTraits >::FreePlexes()
{
    m_pFree = NULL;
    if( m_pBlocks != NULL )
    {
        m_pBlocks->FreeDataChain();
        m_pBlocks = NULL;
    }
}

template< typename K, typename V, class KTraits, class VTraits >
typename SMap< K, V, KTraits, VTraits >::CNode* SMap< K, V, KTraits, VTraits >::GetNode(
    KINARGTYPE key, UINT& iBin, UINT& nHash, CNode*& pPrev ) const
{
    CNode* pFollow;

    nHash = KTraits::Hash( key );
    iBin = nHash%m_nBins;

    if( m_ppBins == NULL )
    {
        return( NULL );
    }

    pFollow = NULL;
    pPrev = NULL;
    for( CNode* pNode = m_ppBins[iBin]; pNode != NULL; pNode = pNode->m_pNext )
    {
        if( (pNode->GetHash() == nHash) && KTraits::CompareElements( pNode->m_key, key ) )
        {
            pPrev = pFollow;
            return( pNode );
        }
        pFollow = pNode;
    }

    return( NULL );
}

template< typename K, typename V, class KTraits, class VTraits >
bool SMap< K, V, KTraits, VTraits >::Lookup( KINARGTYPE key, VOUTARGTYPE value ) const
{
    UINT iBin;
    UINT nHash;
    CNode* pNode;
    CNode* pPrev;

    pNode = GetNode( key, iBin, nHash, pPrev );
    if( pNode == NULL )
    {
        return( false );
    }

    value = pNode->m_value;

    return( true );
}

template< typename K, typename V, class KTraits, class VTraits >
const typename SMap< K, V, KTraits, VTraits >::CPair* SMap< K, V, KTraits, VTraits >::Lookup( KINARGTYPE key ) const
{
    UINT iBin;
    UINT nHash;
    CNode* pNode;
    CNode* pPrev;

    pNode = GetNode( key, iBin, nHash, pPrev );

    return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
typename SMap< K, V, KTraits, VTraits >::CPair* SMap< K, V, KTraits, VTraits >::Lookup( KINARGTYPE key )
{
    UINT iBin;
    UINT nHash;
    CNode* pNode;
    CNode* pPrev;

    pNode = GetNode( key, iBin, nHash, pPrev );

    return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
bool SMap< K, V, KTraits, VTraits >::RemoveKey( KINARGTYPE key )
{
    CNode* pNode;
    UINT iBin;
    UINT nHash;
    CNode* pPrev;

    pPrev = NULL;
    pNode = GetNode( key, iBin, nHash, pPrev );
    if( pNode == NULL )
    {
        return( false );
    }

    RemoveNode( pNode, pPrev );

    return( true );
}

template< typename K, typename V, class KTraits, class VTraits >
void SMap< K, V, KTraits, VTraits >::RemoveNode( CNode* pNode, CNode* pPrev )
{
    SENSURE( pNode != NULL );

    UINT iBin = pNode->GetHash() % m_nBins;

    if( pPrev == NULL )
    {
        SASSUME( m_ppBins[iBin] == pNode );
        m_ppBins[iBin] = pNode->m_pNext;
    }
    else
    {
        SASSERT( pPrev->m_pNext == pNode );
        pPrev->m_pNext = pNode->m_pNext;
    }
    FreeNode( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
void SMap< K, V, KTraits, VTraits >::RemoveAtPos( SPOSITION pos )
{
    SENSURE( pos != NULL );

    CNode* pNode = static_cast< CNode* >( pos );
    CNode* pPrev = NULL;
    UINT iBin = pNode->GetHash() % m_nBins;

    SASSUME( m_ppBins[iBin] != NULL );
    if( pNode == m_ppBins[iBin] )
    {
        pPrev = NULL;
    }
    else
    {
        pPrev = m_ppBins[iBin];
        while( pPrev->m_pNext != pNode )
        {
            pPrev = pPrev->m_pNext;
            SASSERT( pPrev != NULL );
        }
    }
    RemoveNode( pNode, pPrev );
}

template< typename K, typename V, class KTraits, class VTraits >
void SMap< K, V, KTraits, VTraits >::Rehash( UINT nBins )
{
    CNode** ppBins = NULL;

    if( nBins == 0 )
    {
        nBins = PickSize( m_nElements );
    }

    if( nBins == m_nBins )
    {
        return;
    }

    //hjx         STRACE(atlTraceMap, 2, _T("Rehash: %u bins\n"), nBins );

    if( m_ppBins == NULL )
    {
        // Just set the new number of bins
        InitHashTable( nBins, false );
        return;
    }

    //hjx        STRY(ppBins = new CNode*[nBins]);
    ppBins = (CNode**)soui_mem_wrapper::SouiMalloc(nBins*sizeof(CNode*));
    if (ppBins == NULL)
    {
        return;
    }

    SENSURE( UINT_MAX / sizeof( CNode* ) >= nBins );
    memset( ppBins, 0, nBins*sizeof( CNode* ) );

    // Nothing gets copied.  We just rewire the old nodes
    // into the new bins.
    for( UINT iSrcBin = 0; iSrcBin < m_nBins; iSrcBin++ )
    {
        CNode* pNode;

        pNode = m_ppBins[iSrcBin];
        while( pNode != NULL )
        {
            CNode* pNext;
            UINT iDestBin;

            pNext = pNode->m_pNext;  // Save so we don't trash it
            iDestBin = pNode->GetHash()%nBins;
            pNode->m_pNext = ppBins[iDestBin];
            ppBins[iDestBin] = pNode;

            pNode = pNext;
        }
    }

    soui_mem_wrapper::SouiFree(m_ppBins);
    m_ppBins = ppBins;
    m_nBins = nBins;

    UpdateRehashThresholds();
}

template< typename K, typename V, class KTraits, class VTraits >
void SMap< K, V, KTraits, VTraits >::GetNextAssoc( SPOSITION& pos, KOUTARGTYPE key,
        VOUTARGTYPE value ) const
{
    CNode* pNode;
    CNode* pNext;

    SASSUME( m_ppBins != NULL );
    SENSURE( pos != NULL );

    pNode = (CNode*)pos;
    pNext = FindNextNode( pNode );

    pos = SPOSITION( pNext );
    key = pNode->m_key;
    value = pNode->m_value;
}

template< typename K, typename V, class KTraits, class VTraits >
const typename SMap< K, V, KTraits, VTraits >::CPair* SMap< K, V, KTraits, VTraits >::GetNext( SPOSITION& pos ) const
{
    CNode* pNode;
    CNode* pNext;

    SASSUME( m_ppBins != NULL );
    SASSERT( pos != NULL );

    pNode = (CNode*)pos;
    pNext = FindNextNode( pNode );

    pos = SPOSITION( pNext );

    return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
typename SMap< K, V, KTraits, VTraits >::CPair* SMap< K, V, KTraits, VTraits >::GetNext(
    SPOSITION& pos )
{
    SASSUME( m_ppBins != NULL );
    SASSERT( pos != NULL );

    CNode* pNode = static_cast< CNode* >( pos );
    CNode* pNext = FindNextNode( pNode );

    pos = SPOSITION( pNext );

    return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
const K& SMap< K, V, KTraits, VTraits >::GetNextKey( SPOSITION& pos ) const
{
    CNode* pNode;
    CNode* pNext;

    SASSUME( m_ppBins != NULL );
    SENSURE( pos != NULL );

    pNode = (CNode*)pos;
    pNext = FindNextNode( pNode );

    pos = SPOSITION( pNext );

    return( pNode->m_key );
}

template< typename K, typename V, class KTraits, class VTraits >
const V& SMap< K, V, KTraits, VTraits >::GetNextValue( SPOSITION& pos ) const
{
    CNode* pNode;
    CNode* pNext;

    SASSUME( m_ppBins != NULL );
    SENSURE( pos != NULL );

    pNode = (CNode*)pos;
    pNext = FindNextNode( pNode );

    pos = SPOSITION( pNext );

    return( pNode->m_value );
}

template< typename K, typename V, class KTraits, class VTraits >
V& SMap< K, V, KTraits, VTraits >::GetNextValue( SPOSITION& pos )
{
    CNode* pNode;
    CNode* pNext;

    SASSUME( m_ppBins != NULL );
    SENSURE( pos != NULL );

    pNode = (CNode*)pos;
    pNext = FindNextNode( pNode );

    pos = SPOSITION( pNext );

    return( pNode->m_value );
}

template< typename K, typename V, class KTraits, class VTraits >
typename SMap< K, V, KTraits, VTraits >::CNode* SMap< K, V, KTraits, VTraits >::FindNextNode( CNode* pNode ) const
{
    CNode* pNext;

    if(pNode == NULL)
    {
        SASSERT(FALSE);
        return NULL;
    }

    if( pNode->m_pNext != NULL )
    {
        pNext = pNode->m_pNext;
    }
    else
    {
        UINT iBin;

        pNext = NULL;
        iBin = (pNode->GetHash()%m_nBins)+1;
        while( (pNext == NULL) && (iBin < m_nBins) )
        {
            if( m_ppBins[iBin] != NULL )
            {
                pNext = m_ppBins[iBin];
            }

            iBin++;
        }
    }

    return( pNext );
}

#ifdef _DEBUG
template< typename K, typename V, class KTraits, class VTraits >
void SMap< K, V, KTraits, VTraits >::AssertValid() const
{
    SASSUME( m_nBins > 0 );
    // non-empty map should have hash table
    SASSERT( IsEmpty() || (m_ppBins != NULL) );
}
#endif

//
// The red-black tree code is based on the the descriptions in
// "Introduction to Algorithms", by Cormen, Leiserson, and Rivest
//
template< typename K, typename V, class KTraits = CElementTraits< K >, class VTraits = CElementTraits< V > >
class SRBTree
{
public:
    typedef typename KTraits::INARGTYPE KINARGTYPE;
    typedef typename KTraits::OUTARGTYPE KOUTARGTYPE;
    typedef typename VTraits::INARGTYPE VINARGTYPE;
    typedef typename VTraits::OUTARGTYPE VOUTARGTYPE;

public:
    class CPair : 
        public __SPOSITION
    {
    protected:

        CPair( KINARGTYPE key, VINARGTYPE value ) : 
             m_key( key ),
                 m_value( value )
             {
             }
             ~CPair()
             {
             }

    public:
        const K m_key;
        V m_value;
    };

private:

    class CNode : 
        public CPair
    {
    public:
        enum RB_COLOR
        {
            RB_RED, 
            RB_BLACK
        };

    public:
        RB_COLOR m_eColor;
        CNode* m_pLeft;
        CNode* m_pRight;
        CNode* m_pParent;

        CNode( KINARGTYPE key, VINARGTYPE value ) : 
        CPair( key, value ),
            m_pParent( NULL ),
            m_eColor( RB_BLACK )
        {
        }
        ~CNode() 
        {
        }
    };

private:
    CNode* m_pRoot;
    size_t m_nCount;
    CNode* m_pFree;
    SPlex* m_pBlocks;
    size_t m_nBlockSize;

    // sentinel node
    CNode *m_pNil;

    // methods
    bool IsNil(CNode *p) const ;
    void SetNil(CNode **p) ;

    CNode* NewNode( KINARGTYPE key, VINARGTYPE value );
    void FreeNode(CNode* pNode) ;
    void RemovePostOrder(CNode* pNode) ;
    CNode* LeftRotate(CNode* pNode) ;
    CNode* RightRotate(CNode* pNode) ;
    void SwapNode(CNode* pDest, CNode* pSrc) ;
    CNode* InsertImpl( KINARGTYPE key, VINARGTYPE value );
    void RBDeleteFixup(CNode* pNode) ;
    bool RBDelete(CNode* pZ) ;

#ifdef _DEBUG

    // internal debugging code to verify red-black properties of tree:
    // 1) Every node is either red or black
    // 2) Every leaf (NIL) is black
    // 3) If a node is red, both its children are black
    // 4) Every simple path from a node to a descendant leaf node contains 
    //    the same number of black nodes
private:
    void VerifyIntegrity(const CNode *pNode, int nCurrBlackDepth, int &nBlackDepth) const ;

public:
    void VerifyIntegrity() const ;

#endif // _DEBUG

protected:
    CNode* Minimum(CNode* pNode) const ;
    CNode* Maximum(CNode* pNode) const ;
    CNode* Predecessor( CNode* pNode ) const ;
    CNode* Successor(CNode* pNode) const ;
    CNode* RBInsert( KINARGTYPE key, VINARGTYPE value ) ;
    CNode* Find(KINARGTYPE key) const ;
    CNode* FindPrefix( KINARGTYPE key ) const ;

protected:
    explicit SRBTree( size_t nBlockSize = 10 ) ;  // protected to prevent instantiation

public:
    ~SRBTree() ;

    void RemoveAll() ;
    void RemoveAt(SPOSITION pos) ;

    size_t GetCount() const ;
    bool IsEmpty() const ;

    SPOSITION FindFirstKeyAfter( KINARGTYPE key ) const ;

    SPOSITION GetHeadPosition() const ;
    SPOSITION GetTailPosition() const ;
    void GetNextAssoc( SPOSITION& pos, KOUTARGTYPE key, VOUTARGTYPE value ) const;
    const CPair* GetNext(SPOSITION& pos) const ;
    CPair* GetNext(SPOSITION& pos) ;
    const CPair* GetPrev(SPOSITION& pos) const ;
    CPair* GetPrev(SPOSITION& pos) ;
    const K& GetNextKey(SPOSITION& pos) const ;
    const V& GetNextValue(SPOSITION& pos) const ;
    V& GetNextValue(SPOSITION& pos) ;

    CPair* GetAt( SPOSITION pos ) ;
    const CPair* GetAt( SPOSITION pos ) const ;
    void GetAt(SPOSITION pos, KOUTARGTYPE key, VOUTARGTYPE value) const;
    const K& GetKeyAt(SPOSITION pos) const;
    const V& GetValueAt(SPOSITION pos) const;
    V& GetValueAt(SPOSITION pos);
    void SetValueAt(SPOSITION pos, VINARGTYPE value);

private:
    // Private to prevent use
    SRBTree( const SRBTree& ) ;
    SRBTree& operator=( const SRBTree& ) ;
};

template< typename K, typename V, class KTraits, class VTraits >
inline bool SRBTree< K, V, KTraits, VTraits >::IsNil(CNode *p) const 
{
    return ( p == m_pNil );
}

template< typename K, typename V, class KTraits, class VTraits >
inline void SRBTree< K, V, KTraits, VTraits >::SetNil(CNode **p)
{
    SENSURE( p != NULL );
    *p = m_pNil;
}

template< typename K, typename V, class KTraits, class VTraits >
SRBTree< K, V, KTraits, VTraits >::SRBTree( size_t nBlockSize )  :
m_pRoot( NULL ),
m_nCount( 0 ),
m_nBlockSize( nBlockSize ),
m_pFree( NULL ),
m_pBlocks( NULL ),
m_pNil( NULL )
{
    SASSERT( nBlockSize > 0 );
}

template< typename K, typename V, class KTraits, class VTraits >
SRBTree< K, V, KTraits, VTraits >::~SRBTree() 
{
    RemoveAll();
    if (m_pNil != NULL)
    {
        soui_mem_wrapper::SouiFree(m_pNil);
    }
}

template< typename K, typename V, class KTraits, class VTraits >
void SRBTree< K, V, KTraits, VTraits >::RemoveAll() 
{
    if (!IsNil(m_pRoot))
        RemovePostOrder(m_pRoot);
    m_nCount = 0;
    m_pBlocks->FreeDataChain();
    m_pBlocks = NULL;
    m_pFree = NULL;
    m_pRoot = m_pNil;
}

template< typename K, typename V, class KTraits, class VTraits >
size_t SRBTree< K, V, KTraits, VTraits >::GetCount() const 
{
    return m_nCount;
}

template< typename K, typename V, class KTraits, class VTraits >
bool SRBTree< K, V, KTraits, VTraits >::IsEmpty() const 
{
    return( m_nCount == 0 );
}

template< typename K, typename V, class KTraits, class VTraits >
SPOSITION SRBTree< K, V, KTraits, VTraits >::FindFirstKeyAfter( KINARGTYPE key ) const 
{
    return( FindPrefix( key ) );
}

template< typename K, typename V, class KTraits, class VTraits >
void SRBTree< K, V, KTraits, VTraits >::RemoveAt(SPOSITION pos) 
{
    SASSERT(pos != NULL);
    RBDelete(static_cast<CNode*>(pos));
}

template< typename K, typename V, class KTraits, class VTraits >
SPOSITION SRBTree< K, V, KTraits, VTraits >::GetHeadPosition() const 
{
    return( Minimum( m_pRoot ) );
}

template< typename K, typename V, class KTraits, class VTraits >
SPOSITION SRBTree< K, V, KTraits, VTraits >::GetTailPosition() const 
{
    return( Maximum( m_pRoot ) );
}

template< typename K, typename V, class KTraits, class VTraits >
void SRBTree< K, V, KTraits, VTraits >::GetNextAssoc( SPOSITION& pos, KOUTARGTYPE key, VOUTARGTYPE value ) const
{
    SASSERT(pos != NULL);
    CNode* pNode = static_cast< CNode* >(pos);

    key = pNode->m_key;
    value = pNode->m_value;

    pos = Successor(pNode);
}

template< typename K, typename V, class KTraits, class VTraits >
const typename SRBTree< K, V, KTraits, VTraits >::CPair* SRBTree< K, V, KTraits, VTraits >::GetNext(SPOSITION& pos) const 
{
    SASSERT(pos != NULL);
    CNode* pNode = static_cast< CNode* >(pos);
    pos = Successor(pNode);
    return pNode;
}

template< typename K, typename V, class KTraits, class VTraits >
typename SRBTree< K, V, KTraits, VTraits >::CPair* SRBTree< K, V, KTraits, VTraits >::GetNext(SPOSITION& pos) 
{
    SASSERT(pos != NULL);
    CNode* pNode = static_cast< CNode* >(pos);
    pos = Successor(pNode);
    return pNode;
}

template< typename K, typename V, class KTraits, class VTraits >
const typename SRBTree< K, V, KTraits, VTraits >::CPair* SRBTree< K, V, KTraits, VTraits >::GetPrev(SPOSITION& pos) const 
{
    SASSERT(pos != NULL);
    CNode* pNode = static_cast< CNode* >(pos);
    pos = Predecessor(pNode);

    return pNode;
}

template< typename K, typename V, class KTraits, class VTraits >
typename SRBTree< K, V, KTraits, VTraits >::CPair* SRBTree< K, V, KTraits, VTraits >::GetPrev(SPOSITION& pos) 
{
    SASSERT(pos != NULL);
    CNode* pNode = static_cast< CNode* >(pos);
    pos = Predecessor(pNode);

    return pNode;
}

template< typename K, typename V, class KTraits, class VTraits >
const K& SRBTree< K, V, KTraits, VTraits >::GetNextKey(SPOSITION& pos) const 
{
    SASSERT(pos != NULL);
    CNode* pNode = static_cast<CNode*>(pos);
    pos = Successor(pNode);

    return pNode->m_key;
}

template< typename K, typename V, class KTraits, class VTraits >
const V& SRBTree< K, V, KTraits, VTraits >::GetNextValue(SPOSITION& pos) const 
{
    SASSERT(pos != NULL);
    CNode* pNode = static_cast<CNode*>(pos);
    pos = Successor(pNode);

    return pNode->m_value;
}

template< typename K, typename V, class KTraits, class VTraits >
V& SRBTree< K, V, KTraits, VTraits >::GetNextValue(SPOSITION& pos) 
{
    SASSERT(pos != NULL);
    CNode* pNode = static_cast<CNode*>(pos);
    pos = Successor(pNode);

    return pNode->m_value;
}

template< typename K, typename V, class KTraits, class VTraits >
typename SRBTree< K, V, KTraits, VTraits >::CPair* SRBTree< K, V, KTraits, VTraits >::GetAt( SPOSITION pos ) 
{
    SASSERT( pos != NULL );

    return( static_cast< CPair* >( pos ) );
}

template< typename K, typename V, class KTraits, class VTraits >
const typename SRBTree< K, V, KTraits, VTraits >::CPair* SRBTree< K, V, KTraits, VTraits >::GetAt( SPOSITION pos ) const 
{
    SASSERT( pos != NULL );

    return( static_cast< const CPair* >( pos ) );
}

template< typename K, typename V, class KTraits, class VTraits >
void SRBTree< K, V, KTraits, VTraits >::GetAt(SPOSITION pos, KOUTARGTYPE key, VOUTARGTYPE value) const
{
    SENSURE( pos != NULL );
    key = static_cast<CNode*>(pos)->m_key;
    value = static_cast<CNode*>(pos)->m_value;
}

template< typename K, typename V, class KTraits, class VTraits >
const K& SRBTree< K, V, KTraits, VTraits >::GetKeyAt(SPOSITION pos) const
{
    SENSURE( pos != NULL );
    return static_cast<CNode*>(pos)->m_key;
}

template< typename K, typename V, class KTraits, class VTraits >
const V& SRBTree< K, V, KTraits, VTraits >::GetValueAt(SPOSITION pos) const
{
    SENSURE( pos != NULL );
    return static_cast<CNode*>(pos)->m_value;
}

template< typename K, typename V, class KTraits, class VTraits >
V& SRBTree< K, V, KTraits, VTraits >::GetValueAt(SPOSITION pos)
{
    SENSURE( pos != NULL );
    return static_cast<CNode*>(pos)->m_value;
}

template< typename K, typename V, class KTraits, class VTraits >
void SRBTree< K, V, KTraits, VTraits >::SetValueAt(SPOSITION pos, VINARGTYPE value)
{
    SENSURE( pos != NULL );
    static_cast<CNode*>(pos)->m_value = value;
}

#pragma push_macro("new")
#undef new

template< typename K, typename V, class KTraits, class VTraits >
typename SRBTree< K, V, KTraits, VTraits >::CNode* SRBTree< K, V, KTraits, VTraits >::NewNode( KINARGTYPE key, VINARGTYPE value )
{
    if( m_pFree == NULL )
    {
        if (m_pNil == NULL)
        {
            m_pNil = reinterpret_cast<CNode *>(soui_mem_wrapper::SouiMalloc(sizeof( CNode )));
            if (m_pNil == NULL)
            {
                SThrow( E_OUTOFMEMORY );
            }
            memset(m_pNil, 0x00, sizeof(CNode));
            m_pNil->m_eColor = CNode::RB_BLACK;
            m_pNil->m_pParent = m_pNil->m_pLeft = m_pNil->m_pRight = m_pNil;
            m_pRoot = m_pNil;
        }

        SPlex* pPlex = SPlex::Create( m_pBlocks, m_nBlockSize, sizeof( CNode ) );
        if( pPlex == NULL )
        {
            SThrow( E_OUTOFMEMORY );
        }
        CNode* pNode = static_cast< CNode* >( pPlex->data() );
        pNode += m_nBlockSize-1;
        for( INT_PTR iBlock = m_nBlockSize-1; iBlock >= 0; iBlock-- )
        {
            pNode->m_pLeft = m_pFree;
            m_pFree = pNode;
            pNode--;
        }
    }
    SASSUME( m_pFree != NULL );

    CNode* pNewNode = m_pFree;
    ::new( pNewNode ) CNode( key, value );

    m_pFree = m_pFree->m_pLeft;
    pNewNode->m_eColor = CNode::RB_RED;
    SetNil(&pNewNode->m_pLeft);
    SetNil(&pNewNode->m_pRight);
    SetNil(&pNewNode->m_pParent);

    m_nCount++;
    SASSUME( m_nCount > 0 );

    return( pNewNode );
}
#pragma pop_macro("new")

template< typename K, typename V, class KTraits, class VTraits >
void SRBTree< K, V, KTraits, VTraits >::FreeNode(CNode* pNode)
{
    SENSURE( pNode != NULL );
    pNode->~CNode();
    pNode->m_pLeft = m_pFree;
    m_pFree = pNode;
    SASSUME( m_nCount > 0 );
    m_nCount--;
}

template< typename K, typename V, class KTraits, class VTraits >
void SRBTree< K, V, KTraits, VTraits >::RemovePostOrder(CNode* pNode)
{
    if (IsNil(pNode))
        return;
    RemovePostOrder(pNode->m_pLeft);
    RemovePostOrder(pNode->m_pRight);
    FreeNode( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
typename SRBTree< K, V, KTraits, VTraits >::CNode* SRBTree< K, V, KTraits, VTraits >::LeftRotate(CNode* pNode)
{
    SASSERT(pNode != NULL);
    if(pNode == NULL)
        return NULL;

    CNode* pRight = pNode->m_pRight;
    pNode->m_pRight = pRight->m_pLeft;
    if (!IsNil(pRight->m_pLeft))
        pRight->m_pLeft->m_pParent = pNode;

    pRight->m_pParent = pNode->m_pParent;
    if (IsNil(pNode->m_pParent))
        m_pRoot = pRight;
    else if (pNode == pNode->m_pParent->m_pLeft)
        pNode->m_pParent->m_pLeft = pRight;
    else 
        pNode->m_pParent->m_pRight = pRight;

    pRight->m_pLeft = pNode;
    pNode->m_pParent = pRight;
    return pNode;

}

template< typename K, typename V, class KTraits, class VTraits >
typename SRBTree< K, V, KTraits, VTraits >::CNode* SRBTree< K, V, KTraits, VTraits >::RightRotate(CNode* pNode)
{
    SASSERT(pNode != NULL);
    if(pNode == NULL)
        return NULL;

    CNode* pLeft = pNode->m_pLeft;
    pNode->m_pLeft = pLeft->m_pRight;
    if (!IsNil(pLeft->m_pRight))
        pLeft->m_pRight->m_pParent = pNode;

    pLeft->m_pParent = pNode->m_pParent;
    if (IsNil(pNode->m_pParent))
        m_pRoot = pLeft;
    else if (pNode == pNode->m_pParent->m_pRight)
        pNode->m_pParent->m_pRight = pLeft;
    else
        pNode->m_pParent->m_pLeft = pLeft;

    pLeft->m_pRight = pNode;
    pNode->m_pParent = pLeft;
    return pNode;
}

template< typename K, typename V, class KTraits, class VTraits >
typename SRBTree< K, V, KTraits, VTraits >::CNode* SRBTree< K, V, KTraits, VTraits >::Find(KINARGTYPE key) const
{
    CNode* pKey = NULL;
    CNode* pNode = m_pRoot;
    while( !IsNil(pNode) && (pKey == NULL) )
    {
        int nCompare = KTraits::CompareElementsOrdered( key, pNode->m_key );
        if( nCompare == 0 )
        {
            pKey = pNode;
        }
        else
        {
            if( nCompare < 0 )
            {
                pNode = pNode->m_pLeft;
            }
            else
            {
                pNode = pNode->m_pRight;
            }
        }
    }

    if( pKey == NULL )
    {
        return( NULL );
    }

#pragma warning(push)
#pragma warning(disable:4127)

    while( true )
    {
        CNode* pPrev = Predecessor( pKey );
        if( (pPrev != NULL) && KTraits::CompareElements( key, pPrev->m_key ) )
        {
            pKey = pPrev;
        }
        else
        {
            return( pKey );
        }
    }

#pragma warning(pop)
}

template< typename K, typename V, class KTraits, class VTraits >
typename SRBTree< K, V, KTraits, VTraits >::CNode* SRBTree< K, V, KTraits, VTraits >::FindPrefix( KINARGTYPE key ) const
{
    // First, attempt to find a node that matches the key exactly
    CNode* pParent = NULL;
    CNode* pKey = NULL;
    CNode* pNode = m_pRoot;
    while( !IsNil(pNode) && (pKey == NULL) )
    {
        pParent = pNode;
        int nCompare = KTraits::CompareElementsOrdered( key, pNode->m_key );
        if( nCompare == 0 )
        {
            pKey = pNode;
        }
        else if( nCompare < 0 )
        {
            pNode = pNode->m_pLeft;
        }
        else
        {
            pNode = pNode->m_pRight;
        }
    }

    if( pKey != NULL )
    {
        // We found a node with the exact key, so find the first node after 
        // this one with a different key 
        while( true )
        {
            CNode* pNext = Successor( pKey );
            if ((pNext != NULL) && KTraits::CompareElements( key, pNext->m_key ))
            {
                pKey = pNext;
            }
            else
            {
                return pNext;
            }
        }
    }
    else if (pParent != NULL)
    {
        // No node matched the key exactly, so pick the first node with 
        // a key greater than the given key
        int nCompare = KTraits::CompareElementsOrdered( key, pParent->m_key );
        if( nCompare < 0 )
        {
            pKey = pParent;
        }
        else
        {
            SASSERT( nCompare > 0 );
            pKey = Successor( pParent );
        }
    }

    return( pKey );
}

template< typename K, typename V, class KTraits, class VTraits >
void SRBTree< K, V, KTraits, VTraits >::SwapNode(CNode* pDest, CNode* pSrc)
{
    SENSURE( pDest != NULL );
    SENSURE( pSrc != NULL );

    pDest->m_pParent = pSrc->m_pParent;
    if (pSrc->m_pParent->m_pLeft == pSrc)
    {
        pSrc->m_pParent->m_pLeft = pDest;
    }
    else
    {
        pSrc->m_pParent->m_pRight = pDest;
    }

    pDest->m_pRight = pSrc->m_pRight;
    pDest->m_pLeft = pSrc->m_pLeft;
    pDest->m_eColor = pSrc->m_eColor;
    pDest->m_pRight->m_pParent = pDest;
    pDest->m_pLeft->m_pParent = pDest;

    if (m_pRoot == pSrc)
    {
        m_pRoot = pDest;
    }
}

template< typename K, typename V, class KTraits, class VTraits >
typename SRBTree< K, V, KTraits, VTraits >::CNode* SRBTree< K, V, KTraits, VTraits >::InsertImpl( KINARGTYPE key, VINARGTYPE value ) 
{
    CNode* pNew = NewNode( key, value );

    CNode* pY = NULL;
    CNode* pX = m_pRoot;

    while (!IsNil(pX))
    {
        pY = pX;
        if( KTraits::CompareElementsOrdered( key, pX->m_key ) <= 0 )
            pX = pX->m_pLeft;
        else
            pX = pX->m_pRight;
    }

    pNew->m_pParent = pY;
    if (pY == NULL)
    {
        m_pRoot = pNew;
    }
    else if( KTraits::CompareElementsOrdered( key, pY->m_key ) <= 0 )
        pY->m_pLeft = pNew;
    else
        pY->m_pRight = pNew;

    return pNew;
}

template< typename K, typename V, class KTraits, class VTraits >
void SRBTree< K, V, KTraits, VTraits >::RBDeleteFixup(CNode* pNode)
{
    SENSURE( pNode != NULL );

    CNode* pX = pNode;
    CNode* pW = NULL;

    while (( pX != m_pRoot ) && ( pX->m_eColor == CNode::RB_BLACK ))
    {
        if (pX == pX->m_pParent->m_pLeft)
        {
            pW = pX->m_pParent->m_pRight;
            if (pW->m_eColor == CNode::RB_RED)
            {
                pW->m_eColor = CNode::RB_BLACK;
                pW->m_pParent->m_eColor = CNode::RB_RED;
                LeftRotate(pX->m_pParent);
                pW = pX->m_pParent->m_pRight;
            }
            if (pW->m_pLeft->m_eColor == CNode::RB_BLACK && pW->m_pRight->m_eColor == CNode::RB_BLACK)
            {
                pW->m_eColor = CNode::RB_RED;
                pX = pX->m_pParent;
            }
            else
            {
                if (pW->m_pRight->m_eColor == CNode::RB_BLACK)
                {
                    pW->m_pLeft->m_eColor = CNode::RB_BLACK;
                    pW->m_eColor = CNode::RB_RED;
                    RightRotate(pW);
                    pW = pX->m_pParent->m_pRight;
                }
                pW->m_eColor = pX->m_pParent->m_eColor;
                pX->m_pParent->m_eColor = CNode::RB_BLACK;
                pW->m_pRight->m_eColor = CNode::RB_BLACK;
                LeftRotate(pX->m_pParent);
                pX = m_pRoot;
            }
        }
        else
        {
            pW = pX->m_pParent->m_pLeft;
            if (pW->m_eColor == CNode::RB_RED)
            {
                pW->m_eColor = CNode::RB_BLACK;
                pW->m_pParent->m_eColor = CNode::RB_RED;
                RightRotate(pX->m_pParent);
                pW = pX->m_pParent->m_pLeft;
            }
            if (pW->m_pRight->m_eColor == CNode::RB_BLACK && pW->m_pLeft->m_eColor == CNode::RB_BLACK)
            {
                pW->m_eColor = CNode::RB_RED;
                pX = pX->m_pParent;
            }
            else
            {
                if (pW->m_pLeft->m_eColor == CNode::RB_BLACK)
                {
                    pW->m_pRight->m_eColor = CNode::RB_BLACK;
                    pW->m_eColor = CNode::RB_RED;
                    LeftRotate(pW);
                    pW = pX->m_pParent->m_pLeft;
                }
                pW->m_eColor = pX->m_pParent->m_eColor;
                pX->m_pParent->m_eColor = CNode::RB_BLACK;
                pW->m_pLeft->m_eColor = CNode::RB_BLACK;
                RightRotate(pX->m_pParent);
                pX = m_pRoot;
            }
        }
    }

    pX->m_eColor = CNode::RB_BLACK;
}


template< typename K, typename V, class KTraits, class VTraits >
bool SRBTree< K, V, KTraits, VTraits >::RBDelete(CNode* pZ) 
{
    if (pZ == NULL)
        return false;

    CNode* pY = NULL;
    CNode* pX = NULL;
    if (IsNil(pZ->m_pLeft) || IsNil(pZ->m_pRight))
        pY = pZ;
    else
        pY = Successor(pZ);

    if (!IsNil(pY->m_pLeft))
        pX = pY->m_pLeft;
    else
        pX = pY->m_pRight;

    pX->m_pParent = pY->m_pParent;

    if (IsNil(pY->m_pParent))
        m_pRoot = pX;
    else if (pY == pY->m_pParent->m_pLeft)
        pY->m_pParent->m_pLeft = pX;
    else
        pY->m_pParent->m_pRight = pX;

    if (pY->m_eColor == CNode::RB_BLACK)
        RBDeleteFixup(pX);

    if (pY != pZ)
        SwapNode(pY, pZ);

    if (m_pRoot != NULL)
        SetNil(&m_pRoot->m_pParent);

    FreeNode( pZ );

    return true;
}

template< typename K, typename V, class KTraits, class VTraits >
typename SRBTree< K, V, KTraits, VTraits >::CNode* SRBTree< K, V, KTraits, VTraits >::Minimum(CNode* pNode) const 
{
    if (pNode == NULL || IsNil(pNode))
    {
        return NULL;
    }

    CNode* pMin = pNode;
    while (!IsNil(pMin->m_pLeft))
    {
        pMin = pMin->m_pLeft;
    }

    return pMin;
}

template< typename K, typename V, class KTraits, class VTraits >
typename SRBTree< K, V, KTraits, VTraits >::CNode* SRBTree< K, V, KTraits, VTraits >::Maximum(CNode* pNode) const 
{
    if (pNode == NULL || IsNil(pNode))
    {
        return NULL;
    }

    CNode* pMax = pNode;
    while (!IsNil(pMax->m_pRight))
    {
        pMax = pMax->m_pRight;
    }

    return pMax;
}

template< typename K, typename V, class KTraits, class VTraits >
typename SRBTree< K, V, KTraits, VTraits >::CNode* SRBTree< K, V, KTraits, VTraits >::Predecessor( CNode* pNode ) const 
{
    if( pNode == NULL )
    {
        return( NULL );
    }
    if( !IsNil(pNode->m_pLeft) )
    {
        return( Maximum( pNode->m_pLeft ) );
    }

    CNode* pParent = pNode->m_pParent;
    CNode* pLeft = pNode;
    while( !IsNil(pParent) && (pLeft == pParent->m_pLeft) )
    {
        pLeft = pParent;
        pParent = pParent->m_pParent;
    }

    if (IsNil(pParent))
    {
        pParent = NULL;
    }
    return( pParent );
}

template< typename K, typename V, class KTraits, class VTraits >
typename SRBTree< K, V, KTraits, VTraits >::CNode* SRBTree< K, V, KTraits, VTraits >::Successor(CNode* pNode) const 
{
    if ( pNode == NULL )
    {
        return NULL;
    }
    if ( !IsNil(pNode->m_pRight) )
    {
        return Minimum(pNode->m_pRight);
    }

    CNode* pParent = pNode->m_pParent;
    CNode* pRight = pNode;
    while ( !IsNil(pParent) && (pRight == pParent->m_pRight) )
    {
        pRight = pParent;
        pParent = pParent->m_pParent;
    }

    if (IsNil(pParent))
    {
        pParent = NULL;
    }
    return pParent;
}

template< typename K, typename V, class KTraits, class VTraits >
typename SRBTree< K, V, KTraits, VTraits >::CNode* SRBTree< K, V, KTraits, VTraits >::RBInsert( KINARGTYPE key, VINARGTYPE value ) 
{
    CNode* pNewNode = InsertImpl( key, value );

    CNode* pX = pNewNode;
    pX->m_eColor = CNode::RB_RED;
    CNode* pY = NULL;
    while (pX != m_pRoot && pX->m_pParent->m_eColor == CNode::RB_RED)
    {
        if (pX->m_pParent == pX->m_pParent->m_pParent->m_pLeft)
        {
            pY = pX->m_pParent->m_pParent->m_pRight;
            if (pY != NULL && pY->m_eColor == CNode::RB_RED)
            {
                pX->m_pParent->m_eColor = CNode::RB_BLACK;
                pY->m_eColor = CNode::RB_BLACK;
                pX->m_pParent->m_pParent->m_eColor = CNode::RB_RED;
                pX = pX->m_pParent->m_pParent;
            }
            else
            {
                if (pX == pX->m_pParent->m_pRight)
                {
                    pX = pX->m_pParent;
                    LeftRotate(pX);
                }
                pX->m_pParent->m_eColor = CNode::RB_BLACK;
                pX->m_pParent->m_pParent->m_eColor = CNode::RB_RED;
                RightRotate(pX->m_pParent->m_pParent);
            }
        }
        else
        {
            pY = pX->m_pParent->m_pParent->m_pLeft;
            if (pY != NULL && pY->m_eColor == CNode::RB_RED)
            {
                pX->m_pParent->m_eColor = CNode::RB_BLACK;
                pY->m_eColor = CNode::RB_BLACK;
                pX->m_pParent->m_pParent->m_eColor = CNode::RB_RED;
                pX = pX->m_pParent->m_pParent;
            }
            else
            {
                if (pX == pX->m_pParent->m_pLeft)
                {
                    pX = pX->m_pParent;
                    RightRotate(pX);
                }
                pX->m_pParent->m_eColor = CNode::RB_BLACK;
                pX->m_pParent->m_pParent->m_eColor = CNode::RB_RED;
                LeftRotate(pX->m_pParent->m_pParent);
            }
        }
    }

    m_pRoot->m_eColor = CNode::RB_BLACK;
    SetNil(&m_pRoot->m_pParent);

    return( pNewNode );
}

#ifdef _DEBUG

template< typename K, typename V, class KTraits, class VTraits >
void SRBTree< K, V, KTraits, VTraits >::VerifyIntegrity(const CNode *pNode, int nCurrBlackDepth, int &nBlackDepth) const 
{
    bool bCheckForBlack = false;
    bool bLeaf = true;

    if (pNode->m_eColor == CNode::RB_RED) 
        bCheckForBlack = true;
    else
        nCurrBlackDepth++;

    SASSERT(pNode->m_pLeft != NULL);
    if (!IsNil(pNode->m_pLeft))
    {
        bLeaf = false;
        if (bCheckForBlack)
        {
            SASSERT(pNode->m_pLeft->m_eColor == CNode::RB_BLACK);
        }

        VerifyIntegrity(pNode->m_pLeft, nCurrBlackDepth, nBlackDepth);
    }

    SASSERT(pNode->m_pRight != NULL);
    if (!IsNil(pNode->m_pRight))
    {
        bLeaf = false;
        if (bCheckForBlack)
        {
            SASSERT(pNode->m_pRight->m_eColor == CNode::RB_BLACK);
        }

        VerifyIntegrity(pNode->m_pRight, nCurrBlackDepth, nBlackDepth);
    }

    SASSERT( pNode->m_pParent != NULL );
    SASSERT( ( IsNil(pNode->m_pParent) ) ||
        ( pNode->m_pParent->m_pLeft == pNode ) ||
        ( pNode->m_pParent->m_pRight == pNode ) );

    if (bLeaf) 
    {
        if (nBlackDepth == 0)
        {
            nBlackDepth = nCurrBlackDepth;
        }
        else 
        {
            SASSERT(nBlackDepth == nCurrBlackDepth);
        }
    }
}

template< typename K, typename V, class KTraits, class VTraits >
void SRBTree< K, V, KTraits, VTraits >::VerifyIntegrity() const 
{
    if ((m_pRoot == NULL) || (IsNil(m_pRoot)))
        return;

    SASSUME(m_pRoot->m_eColor == CNode::RB_BLACK);
    int nBlackDepth = 0;
    VerifyIntegrity(m_pRoot, 0, nBlackDepth);
}

#endif // _DEBUG

template< typename K, typename V, class KTraits = CElementTraits< K >, class VTraits = CElementTraits< V > >
class SRBMap :
    public SRBTree< K, V, KTraits, VTraits >
{
public:
    explicit SRBMap( size_t nBlockSize = 10 ) ;
    ~SRBMap() ;

    bool Lookup( typename SRBTree< K, V, KTraits, VTraits >::KINARGTYPE key, typename SRBTree< K, V, KTraits, VTraits >::VOUTARGTYPE value ) const ;
    const typename SRBMap< K, V, KTraits, VTraits >::CPair* Lookup(typename SRBTree< K, V, KTraits, VTraits >::KINARGTYPE key ) const ;
    typename SRBMap< K, V, KTraits, VTraits >::CPair* Lookup(typename SRBTree< K, V, KTraits, VTraits >::KINARGTYPE key ) ;
    typename SRBMap< K, V, KTraits, VTraits >::SPOSITION SetAt(typename SRBTree< K, V, KTraits, VTraits >::KINARGTYPE key, typename SRBTree< K, V, KTraits, VTraits >::VINARGTYPE value)
    {
        CPair* pNode = Find(key);
        if (pNode == NULL)
        {
            return(RBInsert(key, value));
        }
        else
        {
            pNode->m_value = value;

            return(pNode);
        }
    }

    bool RemoveKey(typename SRBTree< K, V, KTraits, VTraits >::KINARGTYPE key ) ;

};


template< typename K, typename V, class KTraits, class VTraits >
SRBMap< K, V, KTraits, VTraits >::SRBMap( size_t nBlockSize )  :
SRBTree< K, V, KTraits, VTraits >( nBlockSize )
{
}

template< typename K, typename V, class KTraits, class VTraits >
SRBMap< K, V, KTraits, VTraits >::~SRBMap() 
{
}

template< typename K, typename V, class KTraits, class VTraits >
const typename SRBMap< K, V, KTraits, VTraits >::CPair* SRBMap< K, V, KTraits, VTraits >::Lookup( typename SRBTree< K, V, KTraits, VTraits >::KINARGTYPE key ) const 
{
    return Find(key);
}

template< typename K, typename V, class KTraits, class VTraits >
typename SRBMap< K, V, KTraits, VTraits >::CPair* SRBMap< K, V, KTraits, VTraits >::Lookup(  typename SRBTree< K, V, KTraits, VTraits >::KINARGTYPE key ) 
{
    return Find(key);
}

template< typename K, typename V, class KTraits, class VTraits >
bool SRBMap< K, V, KTraits, VTraits >::Lookup(  typename SRBTree< K, V, KTraits, VTraits >::KINARGTYPE key, typename SRBTree< K, V, KTraits, VTraits >::VOUTARGTYPE value ) const 
{
    const CPair* pLookup = Find( key );
    if( pLookup == NULL )
        return false;

    value = pLookup->m_value;
    return true;
}

template< typename K, typename V, class KTraits, class VTraits >
bool SRBMap< K, V, KTraits, VTraits >::RemoveKey( typename SRBTree< K, V, KTraits, VTraits >::KINARGTYPE key ) 
{
    SPOSITION pos = Lookup( key );
    if( pos != NULL )
    {
        RemoveAt( pos );

        return( true );
    }
    else
    {
        return( false );
    }
}

template< typename K, typename V, class KTraits = CElementTraits< K >, class VTraits = CElementTraits< V > >
class SRBMultiMap :
    public SRBTree< K, V, KTraits, VTraits >
{
public:
    explicit SRBMultiMap( size_t nBlockSize = 10 ) ;
    ~SRBMultiMap() ;

    typename SRBMap< K, V, KTraits, VTraits >::SPOSITION Insert( typename SRBTree< K, V, KTraits, VTraits >::KINARGTYPE key, typename SRBTree< K, V, KTraits, VTraits >::VINARGTYPE value ) ;
    size_t RemoveKey( typename SRBTree< K, V, KTraits, VTraits >::KINARGTYPE key ) ;

    typename SRBMultiMap< K, V, KTraits, VTraits >::SPOSITION FindFirstWithKey(typename SRBTree< K, V, KTraits, VTraits >::KINARGTYPE key) const
    {
        return(Find(key));
    }

    const typename SRBMultiMap< K, V, KTraits, VTraits >::CPair* GetNextWithKey( typename SRBMultiMap< K, V, KTraits, VTraits >::SPOSITION& pos, typename SRBTree< K, V, KTraits, VTraits >::KINARGTYPE key ) const ;
    typename SRBMultiMap< K, V, KTraits, VTraits >::CPair* GetNextWithKey(typename SRBMultiMap< K, V, KTraits, VTraits >::SPOSITION& pos, typename SRBTree< K, V, KTraits, VTraits >::KINARGTYPE key);
    const V& GetNextValueWithKey( typename SRBMultiMap< K, V, KTraits, VTraits >::SPOSITION& pos, typename SRBTree< K, V, KTraits, VTraits >::KINARGTYPE key ) const ;
    V& GetNextValueWithKey( typename SRBMultiMap< K, V, KTraits, VTraits >::SPOSITION& pos, typename SRBTree< K, V, KTraits, VTraits >::KINARGTYPE key ) ;
};

template< typename K, typename V, class KTraits, class VTraits >
SRBMultiMap< K, V, KTraits, VTraits >::SRBMultiMap( size_t nBlockSize )  :
SRBTree< K, V, KTraits, VTraits >( nBlockSize )
{
}

template< typename K, typename V, class KTraits, class VTraits >
SRBMultiMap< K, V, KTraits, VTraits >::~SRBMultiMap() 
{
}

template< typename K, typename V, class KTraits, class VTraits >
typename SRBMap< K, V, KTraits, VTraits >::SPOSITION SRBMultiMap< K, V, KTraits, VTraits >::Insert( typename SRBTree< K, V, KTraits, VTraits >::KINARGTYPE key, typename SRBTree< K, V, KTraits, VTraits >::VINARGTYPE value ) 
{
    return( RBInsert( key, value ) );
}

template< typename K, typename V, class KTraits, class VTraits >
size_t SRBMultiMap< K, V, KTraits, VTraits >::RemoveKey( typename SRBTree< K, V, KTraits, VTraits >::KINARGTYPE key ) 
{
    size_t nElementsDeleted = 0;

    SPOSITION pos = FindFirstWithKey( key );
    while( pos != NULL )
    {
        SPOSITION posDelete = pos;
        GetNextWithKey( pos, key );
        RemoveAt( posDelete );
        nElementsDeleted++;
    }

    return( nElementsDeleted );
}



template< typename K, typename V, class KTraits, class VTraits >
const typename SRBMultiMap< K, V, KTraits, VTraits >::CPair* SRBMultiMap< K, V, KTraits, VTraits >::GetNextWithKey( typename SRBMultiMap< K, V, KTraits, VTraits >::SPOSITION& pos, typename SRBTree< K, V, KTraits, VTraits >::KINARGTYPE key ) const
{
    SASSERT( pos != NULL );
    const CPair* pNode = GetNext( pos );
    if( (pos == NULL) || !KTraits::CompareElements( static_cast< CPair* >( pos )->m_key, key ) )
    {
        pos = NULL;
    }

    return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
typename SRBMultiMap< K, V, KTraits, VTraits >::CPair* SRBMultiMap< K, V, KTraits, VTraits >::GetNextWithKey(typename SRBMultiMap< K, V, KTraits, VTraits >::SPOSITION& pos, typename SRBTree< K, V, KTraits, VTraits >::KINARGTYPE key)
{
    SASSERT( pos != NULL );
    CPair* pNode = GetNext( pos );
    if( (pos == NULL) || !KTraits::CompareElements( static_cast< CPair* >( pos )->m_key, key ) )
    {
        pos = NULL;
    }

    return( pNode );
}

template< typename K, typename V, class KTraits, class VTraits >
const V& SRBMultiMap< K, V, KTraits, VTraits >::GetNextValueWithKey( typename SRBMultiMap< K, V, KTraits, VTraits >::SPOSITION& pos, typename SRBTree< K, V, KTraits, VTraits >::KINARGTYPE key ) const
{
    const CPair* pPair = GetNextWithKey( pos, key );

    return( pPair->m_value );
}

template< typename K, typename V, class KTraits, class VTraits >
V& SRBMultiMap< K, V, KTraits, VTraits >::GetNextValueWithKey( typename SRBMultiMap< K, V, KTraits, VTraits >::SPOSITION& pos, typename SRBTree< K, V, KTraits, VTraits >::KINARGTYPE key )
{
    CPair* pPair = GetNextWithKey( pos, key );

    return( pPair->m_value );
}

}; // namespace SOUI
#pragma pack(pop)

#pragma warning(pop)

#endif  // __SOUICOLL_H__

//IObjRef的实现类
#pragma  once

#ifndef SASSERT
#include <assert.h>
#define SASSERT(x) assert(x);
#endif

namespace SOUI
{

template<class T>
class TObjRefImpl :  public T
{
public:
	TObjRefImpl():m_cRef(1)
	{
	}

	virtual ~TObjRefImpl(){
	}

	//!添加引用
	/*!
	*/
	virtual long AddRef()
	{
		return InterlockedIncrement(&m_cRef);
	}

	//!释放引用
	/*!
	*/
	virtual long Release()
	{
		long lRet = InterlockedDecrement(&m_cRef);
		if(lRet==0)
		{
			OnFinalRelease();
		}
		return lRet;
	}

	//!释放对象
	/*!
	*/
    virtual void OnFinalRelease()
    {
        delete this;
    }
protected:
	volatile LONG m_cRef;
};

template<class T,class T2>
class TObjRefImpl2 :  public TObjRefImpl<T>
{
public:
    virtual void OnFinalRelease()
    {
        delete static_cast<T2*>(this);
    }
};

//CAutoRefPtr provides the basis for all other smart pointers
template <class T>
class CAutoRefPtr
{
public:
	CAutoRefPtr() throw()
	{
		p = NULL;
	}
	CAutoRefPtr(int nNull) throw()
	{
		(void)nNull;
		p = NULL;
	}
	CAutoRefPtr(T* lp) throw()
	{
		p = lp;
		if (p != NULL)
		{
			p->AddRef();
		}
	}

	CAutoRefPtr(const CAutoRefPtr & src) throw()
	{
		p=src.p;
		if(p)
		{
			p->AddRef();
		}
	}

	~CAutoRefPtr() throw()
	{
		if (p)
		{
			p->Release();
		}
	}

	T* operator->() const throw()
	{
		return p;
	}

	operator T*() const throw()
	{
		return p;
	}
	T& operator*() const
	{
		return *p;
	}
	//The assert on operator& usually indicates a bug.  If this is really
	//what is needed, however, take the address of the p member explicitly.
	T** operator&() throw()
	{
	    SASSERT(p==NULL);
		return &p;
	}
	bool operator!() const throw()
	{
		return (p == NULL);
	}
	bool operator<(T* pT) const throw()
	{
		return p < pT;
	}
	bool operator!=(T* pT) const
	{
		return !operator==(pT);
	}
	bool operator==(T* pT) const throw()
	{
		return p == pT;
	}

	T* operator=(T* lp) throw()
	{
		if(*this!=lp)
		{
			if(p)
			{
				p->Release();
			}
			p=lp;
			if(p)
			{
				p->AddRef();
			}
		}
		return *this;
	}

	T* operator=(const CAutoRefPtr<T>& lp) throw()
	{
		if(*this!=lp)
		{
			if(p)
			{
				p->Release();
			}
			p=lp;
			if(p)
			{
				p->AddRef();
			}
		}
		return *this;	
	}

	// Release the interface and set to NULL
	void Release() throw()
	{
		T* pTemp = p;
		if (pTemp)
		{
			p = NULL;
			pTemp->Release();
		}
	}

	// Attach to an existing interface (does not AddRef)
	void Attach(T* p2) throw()
	{
		if (p)
		{
			p->Release();
		}
		p = p2;
	}
	// Detach the interface (does not Release)
	T* Detach() throw()
	{
		T* pt = p;
		p = NULL;
		return pt;
	}
	HRESULT CopyTo(T** ppT) throw()
	{
		if (ppT == NULL)
			return E_POINTER;
		*ppT = p;
		if (p)
		{
			p->AddRef();
		}
		return S_OK;
	}

protected:
	T* p;
};


}//end of namespace SOUI

#pragma once
#include "SSingleton2.h"
#include <souicoll.h>
namespace SOUI
{

template<class TObj,class TKey=SStringA>
class SCmnMap
{
public:
    SCmnMap(void (*funOnKeyRemoved)(const TObj &)=NULL):m_pFunOnKeyRemoved(funOnKeyRemoved)
    {
        m_mapNamedObj=new SMap<TKey,TObj>;
    }
    virtual ~SCmnMap()
    {
        RemoveAll();
        delete m_mapNamedObj;
    }

    bool HasKey(const TKey & key) const
    {
        return m_mapNamedObj->Lookup(key)!=NULL;
    }
    bool GetKeyObject(const TKey & key,TObj & obj)
    {
        if(!HasKey(key)) return false;
        obj=(*m_mapNamedObj)[key];
        return true;
    }
    TObj & GetKeyObject(const TKey & key) const
    {
        return (*m_mapNamedObj)[key];
    }
    bool AddKeyObject(const TKey & key,const TObj & obj)
    {
        if(HasKey(key)) return false;
        (*m_mapNamedObj)[key]=obj;
        return true;
    }
    void SetKeyObject(const TKey & key,const TObj & obj)
    {
		RemoveKeyObject(key);
        (*m_mapNamedObj)[key]=obj;
    }
    bool RemoveKeyObject(const TKey & key)
    {
        if(!HasKey(key)) return false;
        if(m_pFunOnKeyRemoved)
        {
            m_pFunOnKeyRemoved((*m_mapNamedObj)[key]);
        }
        m_mapNamedObj->RemoveKey(key);
        return true;
    }
    void RemoveAll()
    {
        if(m_pFunOnKeyRemoved)
        {
            SPOSITION pos=m_mapNamedObj->GetStartPosition();
            while(pos)
            { 
				typename SMap<TKey,TObj>::CPair *p = m_mapNamedObj->GetNext(pos);
                m_pFunOnKeyRemoved(p->m_value);
            }
        }
        m_mapNamedObj->RemoveAll();
    }
    size_t GetCount()
    {
        return m_mapNamedObj->GetCount();
    }
protected:
    void (*m_pFunOnKeyRemoved)(const TObj &obj);

    SMap<TKey,TObj> *m_mapNamedObj;
};

template<class TClass,class TObj,class TKey=SStringA>
class SSingletonMap :public SSingleton2<TClass>, public SCmnMap<TObj,TKey>
{
};

}//namespace SOUI
/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       SSingleton.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/02
* 
* Describe    SOUI系统中使用的单件模块
*/

#pragma once

#include <assert.h>
#include "../SApp.h"

namespace SOUI
{

#define SINGLETON2_TYPE(x) \
public:\
	friend class SApplication;\
	static int GetType()\
	{\
		return x;\
	}

	enum {
		SINGLETON_UIDEF = 0,
		SINGLETON_SWNDMGR,
		SINGLETON_TIMER,
		SINGLETON_SCRIPTTIMER,
		SINGLETON_FONTPOOL,
		SINGLETON_STYLEPOOLMGR,
		SINGLETON_SKINPOOLMGR,
		SINGLETON_WINDOWFINDER,
		SINGLETON_TEXTSERVICEHELPER,
		SINGLETON_RICHEDITMENUDEF,
		SINGLETON_SIMPLEWNDHELPER,

		SINGLETON_COUNT,
	};

    /**
    * @class      SSingleton
    * @brief      单件模板
    * 
    * Describe    
    */
    template <typename T> 
    class SSingleton2
    {
    public:
        SSingleton2( void )
        {
        }
        virtual ~SSingleton2( void )
        {
        }
        static T& getSingleton( void )
        {
            assert(getObjPtr());
            return (*getObjPtr());
        }
        static T* getSingletonPtr( void )
        {
			assert(getObjPtr());
			return getObjPtr();
        }
    private:
		static T * getObjPtr()
		{
			return (T*)SApplication::getSingletonPtr()->GetInnerSingleton(T::GetType());
		}

        SSingleton2& operator=(const SSingleton2&)
        {
            return *this;
        }
        SSingleton2(const SSingleton2&) {}
    };

}//namespace SOUI
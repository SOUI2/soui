/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       SwndFactoryMgr.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/02
* 
* Describe    SOUI窗口类厂管理器
*/

#pragma once
#include "core/ssingletonmap.h"


namespace SOUI
{
	class SOUI_EXP SObjectInfo
	{
	public:
		SObjectInfo(const SStringW & name = L"", int type = None) :mName(name), mType(type)
		{
			mName.MakeLower();
		}

		bool operator == (const SObjectInfo & src) const
		{
			return src.mType == mType && src.mName.Compare(mName) == 0;
		}

		bool IsValid() const
		{
			return mType >= None && !mName.IsEmpty();
		}

		ULONG Hash() const
		{
			ULONG uRet = CElementTraits<SStringW>::Hash(mName);
			uRet = (uRet << 5) + mType;
			return uRet;
		}
		
		int Compare(const SObjectInfo & src) const
		{
			if (mType == src.mType)
				return mName.Compare(src.mName);
			else
				return mType - src.mType;
		}

		SStringW mName;
		int      mType;
	};

	/**
	* @class     CElementTraits< SObjectInfo >
	* @brief      SObjectInfo的Hash及比较模板
	*
	* Describe    用于实现一个SObjectInfo SObjectFactroy map
	*/
	template<>
	class CElementTraits< SObjectInfo > :
		public CElementTraitsBase<SObjectInfo >
	{
	public:
		static ULONG Hash(INARGTYPE objInfo)
		{
			return objInfo.Hash();
		}

		static bool CompareElements(INARGTYPE obj1, INARGTYPE obj2)
		{
			return obj1 == obj2;
		}

		static int CompareElementsOrdered(INARGTYPE obj1, INARGTYPE obj2)
		{
			return obj1.Compare(obj2);
		}
	};


    class SObjectFactory
    {
    public:
        virtual ~SObjectFactory() {}
        virtual IObject * NewObject() const = 0;
        virtual LPCWSTR BaseClassName() const =0;
		virtual SObjectInfo GetObjectInfo() const = 0;
        virtual SObjectFactory* Clone() const =0;
    };



    template <typename T>
    class TplSObjectFactory : public SObjectFactory
    {
    public:
        //! Default constructor.
		TplSObjectFactory()
        {
        }

        LPCWSTR BaseClassName() const {
			return T::BaseClassName();
		}

        virtual IObject* NewObject() const
        {
            return new T;
        }

        virtual TplSObjectFactory* Clone() const
        {
            return new TplSObjectFactory<T>();
        }

		// 通过 SObjectFactory 继承
		virtual SObjectInfo GetObjectInfo() const
		{
			return SObjectInfo(T::GetClassName(),T::GetClassType());
		}
	};



    typedef SObjectFactory* SObjectFactoryPtr;

    class SOUI_EXP SObjectFactoryMgr :
        public SCmnMap<SObjectFactoryPtr, SObjectInfo>
    {
    public:
		SObjectFactoryMgr(void);

        //************************************
        // Method:    RegisterFactory,注册APP自定义的窗口类
        // Access:    public
        // Returns:   bool
        // Qualifier:
        // Parameter: SObjectFactory * pWndFactory:窗口工厂指针
        // Parameter: bool bReplace:强制替换原有工厂标志
        //************************************
		bool RegisterFactory(SObjectFactory & objFactory, bool bReplace = false);

        //************************************
        // Method:    UnregisterFactor,反注册APP自定义的窗口类
        // Access:    public
        // Returns:   bool
        // Qualifier:
        // Parameter: SWindowFactory * pWndFactory
        //************************************
		bool UnregisterFactory(const SObjectInfo & objInfo);

        IObject *CreateObject(const SObjectInfo & objInfo) const;

        void SetSwndDefAttr(IObject *pObject) const;
        
        SObjectInfo BaseObjectInfoFromObjectInfo(const SObjectInfo & objInfo);

		template<class T>
		bool TplRegisterFactory()
		{
			return RegisterFactory(TplSObjectFactory<T>());
		}

		template<class T>
		bool TplUnregisterFactory()
		{
			return UnregisterFactory(SObjectInfo(T::GetClassName(), T::GetClassType()));
		}
    protected:
        static void OnFactoryRemoved(const SObjectFactoryPtr & obj);

        void AddStandardFactory();
    };

}//namespace SOUI
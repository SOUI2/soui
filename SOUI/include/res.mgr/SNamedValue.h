//////////////////////////////////////////////////////////////////////////
//   File Name: SNamedValue.h
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "helper/color.h"
#include "layout/SLayoutSize.h"

namespace SOUI
{
    #define  MAX_NAME   50
    
    template<class T>
    struct TNAMEDVALUE
    {
        wchar_t strName[MAX_NAME+1];
        T      value;
    };
        
    template<class T,class ValueParser >
    class SNamedValue
    {
    public:
        typedef TNAMEDVALUE<T> NAMEDVALUE;

        virtual BOOL Init(pugi::xml_node xmlNode)
        {
            if(!xmlNode) return FALSE;

            for (pugi::xml_node xmlStr=xmlNode.first_child(); xmlStr; xmlStr=xmlStr.next_sibling())
            {
                NAMEDVALUE namedValue;
                wcscpy_s(namedValue.strName,MAX_NAME,xmlStr.name());
                if(ValueParser::ParseValue(xmlStr.attribute(L"value").as_string(),namedValue.value))
                    m_lstNamedValue.Add(namedValue);
            }
            qsort(m_lstNamedValue.GetData(),m_lstNamedValue.GetCount(),sizeof(NAMEDVALUE),Compare);
            return TRUE;
        }
        
        const T  & String2Value(const SStringW &strName) const
        {
            NAMEDVALUE target;
            wcscpy_s(target.strName,MAX_NAME,strName);
            NAMEDVALUE *pFind = (NAMEDVALUE *)bsearch(&target,m_lstNamedValue.GetData(),m_lstNamedValue.GetCount(),sizeof(NAMEDVALUE),Compare);
            if(pFind) return pFind->value;
            static const T nullValue = ValueParser::GetNullValue();
            return nullValue;
        }
        
        const T  & GetAt(const int idx) const
        {
            SASSERT(idx>=0 && idx < (int)m_lstNamedValue.GetCount());
            return m_lstNamedValue[idx].value;
        }
        
		size_t Merge(const SNamedValue & src) {
			if (m_lstNamedValue.IsEmpty())
			{
				m_lstNamedValue.Copy(src.m_lstNamedValue);
				return src.m_lstNamedValue.GetCount();
			}
			else
			{
				SArray<NAMEDVALUE> data;
				for (size_t i = 0; i < src.m_lstNamedValue.GetCount(); i++)
				{
					NAMEDVALUE *pFind = (NAMEDVALUE *)bsearch(&src.m_lstNamedValue[i], m_lstNamedValue.GetData(), m_lstNamedValue.GetCount(), sizeof(NAMEDVALUE), Compare);
					if (!pFind)
					{
						data.Add(src.m_lstNamedValue[i]);
					}
				}
				m_lstNamedValue.Append(data);
				qsort(m_lstNamedValue.GetData(), m_lstNamedValue.GetCount(), sizeof(NAMEDVALUE), Compare);
				return data.GetCount();
			}
		}
    protected:
        
        static int Compare(const void * p1,const void * p2)
        {
            const NAMEDVALUE * pData1 = (const NAMEDVALUE*)p1;
            const NAMEDVALUE * pData2 = (const NAMEDVALUE*)p2;
            return wcscmp(pData1->strName,pData2->strName);
        }
        
        SArray<NAMEDVALUE> m_lstNamedValue;
    };
    

    class SIntParser
    {
    public:
        static bool ParseValue(const SStringW & strValue,int & value);
        
        static const int  GetNullValue();
    };
    
    class SOUI_EXP SNamedID : public SNamedValue<int,SIntParser>
    {
    public:
        //从数组里初始化命名ID列表
        //TNAMEDVALUE<int> *pValue:数组
        //int nCount:数组长度
        //BOOL bSorted:数组关键字从小到大已经有序
        void Init2(const NAMEDVALUE *pValue,int nCount,BOOL bSorted);
    }; 
    
    class SColorParser
    {
    public:
        static bool ParseValue(const SStringW & strValue, COLORREF & value);

        static const COLORREF  GetNullValue();
    };

    class SOUI_EXP SNamedColor : public SNamedValue<COLORREF,SColorParser>
    {
    public:
        //自动转换@color/namedcolor
        const COLORREF & Get(const SStringW & strValue) const;
        const COLORREF & Get(int idx) const {return GetAt(idx);}
    }; 

    class SStringParser
    {
    public:
        static bool ParseValue(const SStringW & strValue, SStringW & value);
        static const SStringW GetNullValue();
    };

    class SOUI_EXP SNamedString :public SNamedValue<SStringW,SStringParser>
    {
    public:
        //自动转换@string/namedstring
        const SStringW &  Get(const SStringW & strValue) const;
        const SStringW &  Get(int idx) const {return GetAt(idx);}
    };

	class SDimensionParser
	{
	public:
		static bool ParseValue(const SStringW & strValue, SLayoutSize & value);
		static const SLayoutSize  GetNullValue();
	};

	class SOUI_EXP SNamedDimension : public SNamedValue<SLayoutSize, SDimensionParser>
	{
	public:
		//自动转换@dim/namedDimension
		const SLayoutSize &  Get(const SStringW & strValue) const;
		const SLayoutSize &  Get(int idx) const { return GetAt(idx); }
	};
} //namespace SOUI

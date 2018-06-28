/**
* Copyright (C) 2014-2050
* All rights reserved.
*
* @file       Sobject.hpp
* @brief
* @version    v1.0
* @author     SOUI group
* @date       2014/08/01
*
* Describe    the base class used in SOUI, which provides type identify of class in runtime
*             and implement attributes dispatcher described in XML.
*/

#pragma once
#include "sobject-i.h"


namespace SOUI
{
#pragma warning(push)
#pragma warning (disable : 4231)
#pragma warning (disable : 4251)

	template<class T>
	class SObjectImpl : public T
	{
	public:

		HRESULT DefAttributeProc(const SStringW & strAttribName, const SStringW & strValue, BOOL bLoading)
		{
			UNREFERENCED_PARAMETER(strAttribName);
			UNREFERENCED_PARAMETER(strValue);
			UNREFERENCED_PARAMETER(bLoading);
			return E_FAIL;
		}

		BOOL InitFromXml(pugi::xml_node xmlNode)
		{
			if (!xmlNode) return FALSE;
#ifdef _DEBUG
			{
				pugi::xml_writer_buff writer;
				xmlNode.print(writer, L"\t", pugi::format_default, pugi::encoding_utf16);
				m_strXml = SStringW(writer.buffer(), writer.size());
			}
#endif

			//设置当前对象的属性

			for (pugi::xml_attribute attr = xmlNode.first_attribute(); attr; attr = attr.next_attribute())
			{
				if (T::IsAttributeHandled(attr)) continue;   //忽略已经被预处理的属性
				SetAttribute(attr.name(), attr.value(), TRUE);
			}
			//调用初始化完成接口
			OnInitFinished(xmlNode);
			return TRUE;
		}

		LPCWSTR GetName() const
		{
			return NULL;
		}

		int GetID() const
		{
			return 0;
		}

		HRESULT AfterAttribute(const SStringW & strAttribName, const SStringW & strValue, BOOL bLoading, HRESULT hr)
		{
			UNREFERENCED_PARAMETER(strAttribName);
			UNREFERENCED_PARAMETER(strValue);
			UNREFERENCED_PARAMETER(bLoading);
			return hr;
		}

		HRESULT SetAttribute(const SStringW & strAttribName, const SStringW & strValue, BOOL bLoading)
		{
			return DefAttributeProc(strAttribName, strValue, bLoading);
		}

		HRESULT SetAttribute(const SStringA & strAttribName, const SStringA & strValue, BOOL bLoading)
		{
			return SetAttribute(S_CA2W(strAttribName), S_CA2W(strValue), bLoading);
		}

		HRESULT SetAttribute(const char* strAttribName, const char* strValue, BOOL bLoading)
		{
			return SetAttribute(S_CA2W(strAttribName), S_CA2W(strValue), bLoading);
		}

		LPCWSTR GetObjectClass() const
		{
			return NULL;
		}

		int GetObjectType() const
		{
			return None;
		}


		BOOL IsClass(LPCWSTR lpszName) const
		{
			UNREFERENCED_PARAMETER(lpszName);
			return FALSE;
		}

		SStringW GetAttribute(const SStringW & strAttr) const{
			UNREFERENCED_PARAMETER(strAttr);
			return SStringW();
		}

		/**
		 * OnInitFinished
		 * @brief    属性初始化完成处理接口
		 * @param    pugi::xml_node xmlNode --  属性节点
		 * @return   void
		 * Describe
		 */
		void OnInitFinished(pugi::xml_node xmlNode) {
			UNREFERENCED_PARAMETER(xmlNode);
		}

#ifdef    _DEBUG
	public:
		SStringW m_strXml;  //<** XML字符串，用于在调试时观察对象*/
#endif//_DEBUG
	};

#ifdef UTILITIES_EXPORTS
#    define EXPIMP_TEMPLATE
#else
#    define EXPIMP_TEMPLATE extern
#endif


	EXPIMP_TEMPLATE template class UTILITIES_API SObjectImpl<IObject>;

	typedef SObjectImpl<IObject> SObject;

#pragma warning(pop)

}//namespace SOUI

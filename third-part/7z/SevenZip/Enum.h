#pragma once


//
// ###### To create an enum call MyType with string support: ######
//
//// MyType.h
//struct MyType
//{
//	enum _Enum
//	{
//		Unknown,
//		Foo,
//		Bar
//	};
//
//	struct _Definition : public EnumerationDefinition< _Enum, _Definition > { static StringValue Strings[]; };
//	typedef EnumerationValue< _Enum, _Definition, Unknown > _Value;
//};
//
//typedef MyType::_Value MyTypeEnum;
//
//
//// MyType.cpp
//#include "MyType.h"
//
//MyType::_Definition::StringValue MyType::_Definition::Strings[] =
//{
//	{ MyType::Foo, _T( "Foo" ) },
//	{ MyType::Bar, _T( "Bar" ) },
//	{ MyType::Unknown, NULL }
//};
//
//
// ###### To create an enum call MyType without string support: ######
//
//// MyType.h
//struct MyType
//{
//	enum _Enum
//	{
//		Unknown,
//		Foo,
//		Bar
//	};
//
//	typedef EnumerationDefinitionNoStrings _Definition;
//	typedef EnumerationValue< _Enum, _Definition, Unknown > _Value;
//};
//
//typedef MyType::_Value MyTypeEnum;
//

#include "SevenString.h"

namespace SevenZip
{
namespace intl
{
	template < typename TEnum, class DerivedDef >
	struct EnumerationDefinition
	{
		struct StringValue
		{
			TEnum			value;
			const TCHAR*	string;
		};

		static TEnum Parse( const TString& string, const TEnum defaultValue )
		{
			const StringValue* it = DerivedDef::Strings;
			for (; it->string != NULL; ++it )
			{
				if ( string.Compare( it->string ) == 0 )
				{
					return it->value;
				}
			}
			return defaultValue;
		}

		static TString Format( const TEnum& value )
		{
			const StringValue* it = DerivedDef::Strings;
			for (; it->string != NULL; ++it )
			{
				if ( value == it->value )
				{
					return it->string;
				}
			}
			return TString();
		}
	};


	struct EnumerationDefinitionNoStrings {};


	template < typename TEnum, class TEnumClass, TEnum DefaultValue >
	class EnumerationValue
	{
	private:

		typedef typename EnumerationValue< TEnum, TEnumClass, DefaultValue > ThisClass;

		TEnum m_value;

	public:

		typedef typename TEnum Enum;

		EnumerationValue():
			m_value( DefaultValue )
		{}

		EnumerationValue( const TEnum& value ):
			m_value( value )
		{}

		static ThisClass Parse( const TString& string )
		{
			return ThisClass( TEnumClass::Parse( string, DefaultValue ) );
		}

		const TEnum& GetValue() const
		{
			return m_value;
		}

		TString GetString() const
		{
			return TEnumClass::Format( m_value );
		}
	
		operator TEnum() const
		{
			return m_value;
		}
	
		ThisClass& operator=( const TEnum& value )
		{
			m_value = value;
			return *this;
		}

		bool operator==( const ThisClass& that ) const
		{
			return that.m_value == m_value;
		}

		bool operator!=( const ThisClass& that ) const
		{
			return !operator==( that );
		}

		bool operator==( const TEnum& value ) const
		{
			return value == m_value;
		}

		bool operator!=( const TEnum& value ) const
		{
			return !operator==( value );
		}

		bool operator< ( const ThisClass& that ) const
		{
			return m_value < that.m_value;
		}

		// Bit field members
		void AddFlag( const TEnum& value )
		{
			*reinterpret_cast< int* >( &m_value ) |= static_cast< int >( value );
		}

		void RemoveFlag( const TEnum& value )
		{
			*reinterpret_cast< int* >( &m_value ) &= ~static_cast< int >( value );
		}

		bool HasFlag( const TEnum& value ) const
		{
			return ( m_value & value ) == value;
		}

		bool HasAnyFlag( const TEnum& valueCombo ) const
		{
			return ( m_value & valueCombo ) != 0;
		}
	};
}
}

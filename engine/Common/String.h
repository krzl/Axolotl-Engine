#pragma once

#include "Collections/Array.h"

namespace axlt {
	class String {

	public:

		static const uint32_t indexNotFound = 0xFFFFFFFF;

		String();
		// ReSharper disable once CppNonExplicitConvertingConstructor
		String( const char* array );
		explicit String( const char* array, uint32_t length );
		String( const String& other );
		String( String&& other ) noexcept;

		~String() = default;

		String& operator=( const String& other ) = default;
		String& operator=( String&& other ) noexcept;
		String& operator=( const char* cstr );

		String operator+( const String& other ) const;
		String operator+( const char* cstr ) const;
		String operator+( char c ) const;
		String& operator+=( const String& other );
		String& operator+=( const char* cstr );
		String& operator+=( char c );

		bool operator==( const String& other ) const;
		bool operator!=( const String& other ) const;

		char& operator[]( uint32_t index );
		char operator[]( uint32_t index ) const;

		char* GetData();
		const char* GetData() const;

		uint32_t Length() const;
		uint32_t FirstIndexOf( char character ) const;
		uint32_t LastIndexOf( char character ) const;
		
		String Substring( uint32_t startIndex, uint32_t length ) const;

	private:

		Array<char, ExactHeapArrayAllocator> data;
	};
	
	uint32_t StrLen( const char* array );
}

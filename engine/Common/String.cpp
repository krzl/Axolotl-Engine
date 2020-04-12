#include "stdafx.h"
#include "String.h"

namespace axlt {

	String::String() : String( "" ) {}

	String::String( const char* array ) : String( array, StrLen( array ) ) {}

	String::String( const char* array, const uint32_t length ) {
		data.AddEmpty( length + 1 );
		CopyElements( data.GetData(), array, length );
		data[length] = '\0';
	}

	String::String( const String& other ) {
		operator=( other );
	}

	String::String( String&& other ) noexcept {
		operator=( other );
	}

	String& String::operator=( String&& other ) noexcept {
		data = Move( other.data );
		return *this;
	}

	String& String::operator=( const char* cstr ) {
		data.Clear();
		const uint32_t length = StrLen( cstr );
		data.AddEmpty( length + 1 );
		CopyElements( data.GetData(), cstr, length );
		data[length] = '\0';
		return *this;
	}

	String String::operator+( const String& other ) const {
		String string;
		string.data.AddEmpty( data.GetSize() + other.data.GetSize() - 2 );

		CopyElements( string.GetData(), GetData(), data.GetSize() - 1 );
		CopyElements( &string[data.GetSize() - 1], other.GetData(), other.data.GetSize() );

		return string;
	}

	String String::operator+( const char* cstr ) const {

		const uint32_t length = StrLen( cstr );

		String string;
		string.data.AddEmpty( data.GetSize() + length - 1 );

		CopyElements( string.GetData(), GetData(), data.GetSize() - 1 );
		CopyElements( &string[data.GetSize() - 1], cstr, length );

		string.data[string.data.GetSize() - 1] = '\0';

		return string;
	}

	String String::operator+( const char c ) const {
		String string;

		string.data.AddEmpty( data.GetSize() );

		CopyElements( string.GetData(), GetData(), data.GetSize() - 1 );

		string[string.data.GetSize() - 2] = c;
		string[string.data.GetSize() - 1] = '\0';

		return string;
	}

	String& String::operator+=( const String& other ) {
		const uint32_t oldSize = data.GetSize() - 1;
		data.AddEmpty( other.data.GetSize() - 1 );
		CopyElements( &data[oldSize], other.GetData(), other.data.GetSize() );
		return *this;
	}

	String& String::operator+=( const char* cstr ) {
		const uint32_t length = StrLen( cstr );
		const uint32_t oldSize = data.GetSize() - 1;
		data.AddEmpty( length );
		CopyElements( &data[oldSize], cstr, length );
		return *this;
	}

	String& String::operator+=( const char c ) {
		data.AddEmpty( 1 );
		data[data.GetSize() - 2] = c;
		data[data.GetSize() - 1] = '\0';
		return *this;
	}

	bool String::operator==( const String& other ) const {
		return !( operator!=( other ) );
	}

	bool String::operator!=( const String& other ) const {
		if( this == &other ) {
			return false;
		}
		if( Length() != other.Length() ) {
			return true;
		}
		for( uint32_t i = 0; i < Length(); i++ ) {
			if( operator[]( i ) != other[i] ) {
				return true;
			}
		}
		return false;
	}

	char& String::operator[]( const uint32_t index ) {
		return data[index];
	}

	char String::operator[]( const uint32_t index ) const {
		return data[index];
	}

	char* String::GetData() {
		return data.GetData();
	}

	const char* String::GetData() const {
		return data.GetData();
	}

	uint32_t String::Length() const {
		return data.GetSize() - 1;
	}

	uint32_t String::FirstIndexOf( const char character ) const {
		for( uint32_t i = 0; i < Length(); i++ ) {
			if( data[i] == character ) {
				return i;
			}
		}
		return indexNotFound;
	}

	uint32_t String::LastIndexOf( const char character ) const {
		for( uint32_t i = Length(); i > 0; i-- ) {
			if( data[i - 1] == character ) {
				return i - 1;
			}
		}
		return indexNotFound;
	}

	String String::Substring( const uint32_t startIndex, const uint32_t length ) const {
		return String( &data[startIndex], length );
	}

	uint32_t StrLen( const char* array ) {
		uint32_t i = 0;
		while( array[i] != '\0' ) i++;
		return i;
	}
}

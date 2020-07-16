#pragma once

namespace axlt {
	class Serializable;

	class File;

	class Serializer {

		class SerializerEnd {};

	public:

		inline static SerializerEnd end;

		explicit Serializer( const char* filePath, const char* accessFlags );
		
		explicit Serializer( const File& file, const char* accessFlags );

		template<typename T>
		void Write( const T& value ) {
			fwrite( &value, sizeof T, 1, fp );
		}

		template<typename T, typename N>
		void Write( const T* data, N count ) {
			fwrite( data, sizeof T, count, fp );
		}

		template<typename T>
		void Read( T& value ) {
			fread( &value, sizeof T, 1, fp );
		}

		template<typename T, typename N>
		void Read( T* data, N count ) {
			fread( data, sizeof T, count, fp );
		}

		template<typename T>
		typename EnableIf<IsBaseOf<Serializable, T>::Value, Serializer&>::Type operator<<( T& value );

		template<typename T>
		typename EnableIf<!IsBaseOf<Serializable, T>::Value, Serializer&>::Type operator<<( T& value ) {
			Write( value );
			return *this;
		}

		void operator<<( SerializerEnd end );

		template<typename T>
		typename EnableIf<IsBaseOf<Serializable, T>::Value, Serializer&>::Type operator>>( T& value );

		template<typename T>
		typename EnableIf<!IsBaseOf<Serializable, T>::Value, Serializer&>::Type operator>>( T& value ) {
			Read( value );
			return *this;
		}

		void operator>>( SerializerEnd end );

	private:

		FILE* fp;
	};

	void WriteSerializable( Serializer& s, Serializable& serializable );
	void ReadSerializable( Serializer& s, Serializable& serializable );
	
	template<typename T>
	typename EnableIf<IsBaseOf<Serializable, T>::Value, Serializer&>::Type Serializer::operator<<( T& value ) {
		WriteSerializable( *this, value );
		return *this;
	}

	template<typename T>
	typename EnableIf<IsBaseOf<Serializable, T>::Value, Serializer&>::Type Serializer::operator>>( T& value ) {
		ReadSerializable( *this, value );
		return *this;
	}

	template<typename T>
	void Serialize( Serializer& serializer, T* value ) {
		serializer << *value;
	}

	template<typename T>
	void Deserialize( Serializer& serializer, T* value ) {
		serializer >> *value;
	}
}

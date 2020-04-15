#pragma once

namespace axlt {
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
		Serializer& operator<<( T& value ) {
			Write( value );
			return *this;
		}

		void operator<<( SerializerEnd end );

		template<typename T>
		Serializer& operator>>( T& value ) {
			Read( value );
			return *this;
		}

		void operator>>( SerializerEnd end );

	private:

		FILE* fp;
	};
}

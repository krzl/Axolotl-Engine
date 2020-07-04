#pragma once
#include "FieldGetter.h"

namespace axlt {
	class Serializable;
	
	class FieldInfo {

	public:

		template<typename T, typename C>
		explicit FieldInfo( T C::* ptr, const uint32_t byteSize, const uint32_t arraySize = 1 ) :
			fieldGetter( FieldGetter<T, C>( ptr ) ),
			typeHash( GetTypeHash<T>() ),
			isDerivedFromSerializable( IsBaseOf<Serializable, T>::Value ),
			arraySize( arraySize ),
			byteSize( byteSize ) {

			serializationFunction = (Serializer& (*)(Serializer&, void*)) axlt::Serialize<T>;
			deserializationFunction = (Serializer& (*)(Serializer&, void*)) axlt::Deserialize<T>;
		}
		
		template<typename T, typename C>
		T& GetValue( C& owner ) const {
			return fieldGetter.GetValue<T, C>( owner );
		}

		template<typename C>
		void* GetVoidPtr( C& owner ) const {
			return fieldGetter.GetVoidPtr( owner );
		}

		template<typename T, typename C>
		T& GetArrayValue( C& owner, const uint32_t index ) const {
			AXLT_ASSERT( IsArray(), "Field is not array" );
			AXLT_ASSERT( arraySize > index, "Index out of bounds" );
			return fieldGetter.GetValue<T*, C>( owner )[ index ];
		}

		template<typename C>
		void* GetArrayVoidPtr( C& owner, const uint32_t index ) const {
			AXLT_ASSERT( IsArray(), "Field is not array" );
			AXLT_ASSERT( arraySize > index, "Index out of bounds" );
			return ( void* ) ( (uint64_t) ( fieldGetter.GetVoidPtr( owner ) ) + index * byteSize );
		}

		uint32_t GetStoredTypeHash() const {
			return typeHash;
		}

		bool IsDerivedFromSerializable() const {
			return isDerivedFromSerializable;
		}

		bool IsArray() const {
			return arraySize != 1;
		}

		uint32_t GetArraySize() const {
			return arraySize;
		}

		void Serialize( Serializer& s, Serializable& owner ) const {
			if( IsArray() ) {
				for( uint32_t i = 0 ; i < arraySize; i++ ) {
					serializationFunction( s, GetArrayVoidPtr<Serializable>( owner, i ) );
				}
			} else {
				serializationFunction( s, GetVoidPtr<Serializable>( owner ) );
			}
		}

		void Deserialize( Serializer& s, Serializable& owner ) const {
			if (IsArray()) {
				for (uint32_t i = 0; i < arraySize; i++) {
					deserializationFunction( s, GetArrayVoidPtr<Serializable>( owner, i ) );
				}
			}
			else {
				deserializationFunction( s, GetVoidPtr<Serializable>( owner ) );
			}
		}
		
	private:

		BaseFieldGetter fieldGetter;
		uint32_t typeHash;
		bool isDerivedFromSerializable;
		uint32_t arraySize;
		uint32_t byteSize;

		Serializer& (*serializationFunction)(Serializer&, void*);
		Serializer& (*deserializationFunction)(Serializer&, void*);
	};
}

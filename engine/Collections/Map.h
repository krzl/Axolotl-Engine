#pragma once

#include "Allocators/SetAllocators.h"
#include "Tuple.h"
#include "Set.h"

namespace axlt {

	template<typename KeyType, typename ElementType>
	struct MapKeyFunctions {

		typedef typename TypeTraits<KeyType>::ParamType KeyParamType;
		typedef typename TypeTraits<KeyType>::ConstParamType ConstKeyParamType;
		typedef typename TypeTraits<ElementType>::ParamType ElementParamType;

		static uint32_t GetHashFromKey( ConstKeyParamType key ) {
			return GetHash( key );
		}
		
		static uint32_t GetSetHash( ElementParamType elem ) {
			return GetHash( elem.key );
		}
	};

	template<typename KeyType, typename ValueType, typename SetAllocator = DefaultSetAllocator>
	class Map {

		template<typename, typename, typename>
		friend class BiMap;
		
		typedef Pair<KeyType, ValueType> SetElement;
		typedef Set<SetElement, SetAllocator, MapKeyFunctions<KeyType, SetElement>> SetType;
		typedef Map<KeyType, ValueType, SetAllocator> MapType;

		typedef MapKeyFunctions<KeyType, ValueType> KeyFunctions;
		typedef typename KeyFunctions::KeyParamType KeyParamType;
		typedef typename KeyFunctions::ConstKeyParamType ConstKeyParamType;
		typedef typename KeyFunctions::ElementParamType ValueParamType;

		typedef typename SetType::Iterator Iterator;
		typedef typename SetType::ConstIterator ConstIterator;

	public:

		Map() = default;

		Map( const MapType& other ) {
			*this = other;
		}

		Map& operator=( const MapType& other ) {  // NOLINT(cert-oop54-cpp)
			if( this != &other ) {
				set = other.set;
			}
			return *this;
		}

		Map( MapType&& other ) noexcept {
			*this = Move( other );
		}

		Map& operator=( MapType&& other ) noexcept {
			if( this != &other ) {
				set = Move( other.set );
			}
			return *this;
		}

		~Map() = default;

		ValueType& Add( const KeyType& key, const ValueType& value ) {
			return Emplace( key, value );
		}

		ValueType& Add( const KeyType& key, ValueType&& value ) {
			return Emplace( key, Move( value ) );
		}

		ValueType& Add( KeyType&& key, const ValueType& value ) {
			return Emplace( Move( key ), value );
		}

		ValueType& Add( KeyType&& key, ValueType&& value ) {
			return Emplace( Move( key ), Move( value ) );
		}

		template< typename KeyInitType, typename ValueInitType>
		ValueType& Emplace( KeyInitType&& key, ValueInitType&& value ) {
			SetElement& element = set.Emplace( Forward<KeyInitType>( key ), Forward<ValueInitType>( value ) );
			return element.value;
		}

		template< typename KeyInitType, typename ValueInitType>
		SetElement& EmplaceReturnTuple( KeyInitType&& key, ValueInitType&& value ) {
			SetElement& element = set.Emplace( Forward<KeyInitType>( key ), Forward<ValueInitType>( value ) );
			return element;
		}

		bool Remove( ConstKeyParamType key ) {
			return set.Remove( key );
		}

		ValueType* Find( ConstKeyParamType key ) {
			SetElement* elem = set.Find( key );
			if( elem == nullptr ) {
				return nullptr;
			}
			return &(elem->value);
		}

		const ValueType* Find( ConstKeyParamType key ) const {
			const SetElement* elem = set.Find( key );
			if( elem == nullptr ) {
				return nullptr;
			}
			return &(elem->value);
		}

		ValueType& operator[]( ConstKeyParamType key ) {
			return *Find( key );
		}

		const ValueType& operator[]( ConstKeyParamType key ) const {
			return *Find( key );
		}

		uint32_t GetSize() const {
			return set.GetSize();
		}

		uint32_t GetDataArraySize() const {
			return set.GetDataArraySize();
		}

		uint32_t GetFirstUsedElementIndex() const {
			return set.GetFirstUsedElementIndex();
		}

		void Clear() {
			set.Clear();
		}

		// ReSharper disable CppInconsistentNaming

		Iterator begin() {
			return Iterator( set, GetFirstUsedElementIndex() );
		}

		Iterator end() {
			return Iterator( set, GetDataArraySize() );
		}

		ConstIterator begin() const {
			return ConstIterator( set, GetFirstUsedElementIndex() );
		}

		ConstIterator end() const {
			return ConstIterator( set, GetDataArraySize() );
		}

		// ReSharper restore CppInconsistentNaming

	private:

		SetType set;
	};

	template<typename KeyType, typename ValueType, typename SetAllocator>
	Serializer& operator<<( Serializer& s, Map<KeyType, ValueType, SetAllocator>& map ) {
		s.Write( map.GetSize() );
		for( auto& pair : map ) {
			s << pair.key;
			s << pair.value;
		}
		return s;
	}

	template<typename KeyType, typename ValueType, typename SetAllocator>
	Serializer& operator>>( Serializer& s, Map<KeyType, ValueType, SetAllocator>& map ) {
		uint32_t mapSize = 0;
		s.Read( mapSize );
		for( uint32_t i = 0; i < mapSize; i++ ) {
			KeyType key;
			ValueType value;
			s >> key;
			s >> value;
			map.Emplace( key, value );
		}
		return s;
	}
}

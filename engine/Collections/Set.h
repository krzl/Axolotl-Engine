#pragma once

#include "Allocators/SetAllocators.h"

namespace axlt {

	template<typename ElementType>
	struct BaseKeyFunctions {

		typedef typename TypeTraits<ElementType>::ParamType KeyParamType;
		typedef typename TypeTraits<ElementType>::ConstParamType ConstKeyParamType;
		typedef typename TypeTraits<ElementType>::ParamType ElementParamType;

		static uint32_t GetHashFromKey( ConstKeyParamType key ) {
			return GetHash( key );
		}

		static uint32_t GetSetHash( ElementParamType elem ) {
			return GetHash( elem );
		}
	};

	template<typename ElementType>
	class SetElement {

	public:

		ElementType element;
		uint32_t hashBucketId = -1;
		uint32_t nextElementId = -1;


		template<typename... ArgsType>
		explicit SetElement( ArgsType&&... args ) : element( Forward<ArgsType>( args )... ) {}

		SetElement() {}
		SetElement( const SetElement& ) = default;
		SetElement( SetElement&& ) = default;
		SetElement& operator=( const SetElement& ) = default;
		SetElement& operator=( SetElement&& ) = default;
		~SetElement() = default;
	};

	template<typename ElementType, typename _SetAllocator = DefaultSetAllocator, typename _KeyFunctions = BaseKeyFunctions<ElementType>>
	class Set {

		template<typename, typename, typename>
		friend class Map;

		class Iterator;
		class ConstIterator;

		friend class Iterator;
		friend class ConstIterator;

		typedef _KeyFunctions KeyFunctions;
		typedef _SetAllocator SetAllocator;
		typedef Set<ElementType, SetAllocator, KeyFunctions> SetType;
		typedef SetElement<ElementType> SetElementType;
		typedef SparseArray<SetElementType, typename SetAllocator::ElementArrayAllocator> ElementArrayType;
		typedef typename SetAllocator::HashAllocator::template AllocatorInstance<uint32_t> HashAllocator;
		typedef typename KeyFunctions::KeyParamType KeyParamType;
		typedef typename KeyFunctions::ConstKeyParamType ConstKeyParamType;
		typedef typename KeyFunctions::ElementParamType ValueParamType;

		const uint32_t UNUSED_ELEMENT = 0xFFFFFFFF;

	public:

		Set() = default;

		Set( const SetType& other ) {
			*this = other;
		}

		Set& operator=( const SetType& other ) {  // NOLINT(cert-oop54-cpp)
			if( this != &other ) {
				m_hashAllocator.ResizeAllocation( m_hashSize, other.m_hashSize );
				CopyElements( m_hashAllocator.GetAllocation(), other.m_hashAllocator.GetAllocation(), other.m_hashSize );
				m_hashSize = other.m_hashSize;
				m_elements = other.m_elements;
			}

			return *this;
		}

		Set( SetType&& other ) noexcept {
			*this = other;
		}

		Set& operator=( SetType&& other ) noexcept {  // NOLINT(cert-oop54-cpp)
			if( this != &other ) {
				m_hashAllocator.ResizeAllocation( m_hashSize, other.m_hashSize );
				MoveElements( m_hashAllocator.GetAllocation(), other.m_hashAllocator.GetAllocation(), other.m_hashSize );
				m_hashSize = other.m_hashSize;
				m_elements = Move( other.m_elements );
			}

			return *this;
		}

		~Set() = default;

		ElementType& Add( const ElementType& element ) {
			return Emplace( element );
		}

		ElementType& Add( ElementType&& element ) {
			return Emplace( Move( element ) );
		}

		template <typename... ArgsType>
		ElementType& Emplace( ArgsType&&... args ) {
			SparseArrayAllocationInfo allocationInfo = m_elements.Emplace( Forward<ArgsType>( args )... );
			SetElementType& element = *(SetElementType*) allocationInfo.element;
			const uint32_t hash = KeyFunctions::GetSetHash( element.element );

			if( !ConditionalRehash() ) {
				LinkElement( element, allocationInfo.index, hash );
			}

			return element.element;
		}

		bool Remove( ConstKeyParamType element ) {
			const uint32_t hash = KeyFunctions::GetHashFromKey( element );

			uint32_t& bucketElementId = HashToElementId( hash );

			SetElementType* currentElement = &m_elements[bucketElementId];
			if( hash == KeyFunctions::GetSetHash( currentElement->element ) ) {
				const uint32_t idToRemove = bucketElementId;
				bucketElementId = currentElement->nextElementId;
				m_elements.Remove( idToRemove );
				return true;
			}

			uint32_t currentElementId = currentElement->nextElementId;
			SetElementType& previousElement = *currentElement;

			while( currentElementId != UNUSED_ELEMENT ) {
				previousElement = *currentElement;
				currentElement = &m_elements[currentElementId];

				if( hash == KeyFunctions::GetSetHash( currentElement->element ) ) {
					previousElement.nextElementId = currentElement->nextElementId;
					m_elements.Remove( currentElementId );
					return true;
				}
				currentElementId = currentElement->nextElementId;
			}

			return false;
		}

		ElementType* Find( ConstKeyParamType key ) {
			if( m_hashSize == 0 ) return nullptr;
			
			const uint32_t hash = KeyFunctions::GetHashFromKey( key );

			uint32_t elementId = HashToElementId( hash );
			
			while( elementId != UNUSED_ELEMENT ) {
				SetElementType& currentElement = m_elements[elementId];
				const uint32_t otherHash = KeyFunctions::GetSetHash( currentElement.element );
				if( hash == otherHash ) {
					return &(currentElement.element);
				}
				elementId = currentElement.nextElementId;
			}

			return nullptr;
		}

		const ElementType* Find( ConstKeyParamType key ) const {
			if( m_hashSize == 0 ) return nullptr;
			
			const uint32_t hash = KeyFunctions::GetHashFromKey( key );

			uint32_t elementId = HashToElementId( hash );

			while( elementId != UNUSED_ELEMENT ) {
				const SetElementType& currentElement = m_elements[elementId];
				const uint32_t otherHash = KeyFunctions::GetSetHash( currentElement.element );
				if( hash == otherHash ) {
					return &(currentElement.element);
				}
				elementId = currentElement.nextElementId;
			}

			return nullptr;
		}

		bool Contains( ConstKeyParamType key ) const {
			return Find( key ) != nullptr;
		}

		uint32_t GetSize() const {
			return m_elements.GetSize();
		}

		uint32_t GetDataArraySize() const {
			return m_elements.GetDataArraySize();
		}

		uint32_t GetFirstUsedElementIndex() const {
			return m_elements.GetFirstUsedElementIndex();
		}

		void Clear() {
			m_elements.Clear();
			Rehash( 0 );
		}

	private:

		bool ConditionalRehash( const bool allowShrink = false ) {
			const uint32_t expectedHashBucketCount = SetAllocator::CalculateNumberOfHashBuckets( m_elements.GetSize() );
			if( m_elements.GetSize() > 0 &&
				( m_hashSize < expectedHashBucketCount ||
				( m_hashSize > expectedHashBucketCount && allowShrink ) ) ) {

				Rehash( expectedHashBucketCount );

				return true;
			}
			return false;
		}

		void Rehash( const uint32_t expectedHashSize ) {
			m_hashAllocator.ResizeAllocation( m_hashSize, expectedHashSize );
			m_hashSize = expectedHashSize;
			memset( m_hashAllocator.GetAllocation(), UNUSED_ELEMENT, sizeof( uint32_t ) * m_hashSize );

			for( uint32_t i = 0; i < m_elements.GetSize(); i++ ) {
				if( m_elements.IsFree( i ) ) continue;

				SetElementType& element = m_elements[i];
				LinkElement( element, i, KeyFunctions::GetSetHash( element.element ) );
			}
		}

		void LinkElement( SetElementType& element, const uint32_t elementIndex, const uint32_t hash ) {
			element.nextElementId = HashToElementId( hash );
			element.hashBucketId = hash & ( m_hashSize - 1 );

			HashToElementId( hash ) = elementIndex;
		}

		uint32_t& HashToElementId( const uint32_t hash ) {
			return m_hashAllocator.GetAllocation()[hash & ( m_hashSize - 1 )];
		}

		uint32_t HashToElementId( const uint32_t hash ) const {
			return m_hashAllocator.GetAllocation()[hash & ( m_hashSize - 1 )];
		}

	public:

		class Iterator {

		public:

			explicit Iterator( SetType& set, const uint32_t index ) :
				m_set( set ), m_index( index ) {}

			explicit operator ElementType&( ) const {
				return m_set.m_elements[m_index].element;
			}

			ElementType& operator*() const {
				return m_set.m_elements[m_index].element;
			}

			Iterator& operator=( const ElementType& value ) {
				m_set.m_elements[m_index] = value;
				return *this;
			}

			Iterator& operator=( ElementType&& value ) {
				m_set.m_elements[m_index] = Move( value );
				return *this;
			}

			Iterator& operator++() {
				do {
					++m_index;
				} while( m_index != m_set.m_elements.GetDataArraySize() && !IsValid() );
				return *this;
			}

			bool operator==( const Iterator& other ) const {
				return &m_set == &other.m_set && m_index == other.m_index;
			}

			bool operator!=( const Iterator& other ) const {
				return &m_set != &other.m_set || m_index != other.m_index;
			}

		private:

			bool IsValid() {
				return !m_set.m_elements.IsFree( m_index );
			}

			SetType& m_set;
			uint32_t m_index;
		};

		class ConstIterator {

		public:

			explicit ConstIterator( const SetType& set, const uint32_t index ) :
				m_set( set ), m_index( index ) {}

			explicit operator ElementType&( ) const {
				return m_set.m_elements[m_index].element;
			}

			const ElementType& operator*() const {
				return m_set.m_elements[m_index].element;
			}

			ConstIterator& operator++() {
				do {
					m_index++;
				} while( m_index != m_set.m_elements.GetDataArraySize() && !IsValid() );
				return *this;
			}

			bool operator==( const ConstIterator& other ) const {
				return &m_set == &other.m_set && m_index == other.m_index;
			}

			bool operator!=( const ConstIterator& other ) const {
				return &m_set != &other.m_set || m_index != other.m_index;
			}

		private:

			bool IsValid() {
				return !m_set.m_elements.IsFree( m_index );
			}

			const SetType& m_set;
			uint32_t m_index;
		};

		// ReSharper disable CppInconsistentNaming

		Iterator begin() {
			return Iterator( *this, GetSize() == 0 ? m_elements.GetDataArraySize() : GetFirstUsedElementIndex() );
		}

		Iterator end() {
			return Iterator( *this, GetDataArraySize() );
		}

		ConstIterator begin() const {
			return ConstIterator( *this, GetSize() == 0 ? m_elements.GetDataArraySize() : GetFirstUsedElementIndex() );
		}

		ConstIterator end() const {
			return ConstIterator( *this, GetDataArraySize() );
		}

		// ReSharper restore CppInconsistentNaming

	private:

		ElementArrayType m_elements;
		HashAllocator m_hashAllocator{};
		uint32_t m_hashSize = 0;
	};
}
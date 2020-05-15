#pragma once

#include "Allocators/ArrayAllocators.h"
#include "Array.h"
#include "BitArray.h"

namespace axlt {
	template<typename _ArrayAllocator = DefaultArrayAllocator, typename _BitAllocator = DefaultArrayAllocator>
	class SparseArrayAllocator {

	public:

		typedef _ArrayAllocator ArrayAllocator;
		typedef _BitAllocator BitAllocator;
	};

	typedef SparseArrayAllocator<> DefaultSparseArrayAllocator;

	template<typename ElementType>
	union SparseArrayElement {

		TypeCompatibleBytes<ElementType> element;

		struct {
			int32_t previousFreeIndex;
			int32_t nextFreeIndex;
		};
	};

	class SparseArrayAllocationInfo {
	public:

		uint32_t index;
		void* element;

		SparseArrayAllocationInfo( void* element, const uint32_t index ) :
			index( index ),
			element( element ) {}
	};

	template<typename ElementType, typename AllocatorClass = DefaultSparseArrayAllocator>
	class SparseArray {

	public:

		typedef SparseArray<ElementType, AllocatorClass> ArrayType;
		typedef SparseArrayElement<ElementType> ArrayElement;
		typedef Array<ArrayElement, typename AllocatorClass::ArrayAllocator> DataArrayType;
		typedef BitArray<typename AllocatorClass::BitAllocator> BitArrayType;
		typedef SparseArrayAllocationInfo AllocationInfo;

		explicit SparseArray()
			: m_size( 0 ) {}

		SparseArray( ElementType* elementType, const uint32_t count )
			: m_data( (ArrayElement*) elementType, count ) // TODO: Won't work for ElementType that has sizeof less than ArrayElement
			, m_size( count ) {
			m_allocationInfo.AddEmpty( m_size );
			for( uint32_t i = 0; i < m_size; i++ ) {
				m_allocationInfo[i] = true;
			}
		}

		SparseArray( std::initializer_list<ElementType> initializerList )
			: SparseArray( initializerList.begin(), initializerList.size() ) {}

		SparseArray( const ArrayType& other ) {
			operator=( other );
		}

		SparseArray<ElementType, AllocatorClass>& operator=( const ArrayType& other ) {  // NOLINT(cert-oop54-cpp)
			if( this == &other ) return *this;
			
			m_data = other.m_data;
			m_allocationInfo = other.m_allocationInfo;
			m_lastFreeIndex = other.m_lastFreeIndex;
			m_size = other.m_size;

			return *this;
		}

		SparseArray( ArrayType&& other ) noexcept {
			operator=( Move( other ) );
		}

		SparseArray<ElementType, AllocatorClass>& operator=( ArrayType&& other ) noexcept {
			if( this == &other ) return *this;

			m_data = Move( other.m_data );
			m_allocationInfo = Move( other.m_allocationInfo );
			m_lastFreeIndex = other.m_lastFreeIndex;
			m_size = other.m_size;

			other.m_size = 0;
			other.m_lastFreeIndex = -1;
			
			return *this;
		}

		~SparseArray() {  // NOLINT(bugprone-exception-escape)
			Clear();
		}

		AllocationInfo Add( const ElementType& element ) {
			return Emplace( element );
		}

		AllocationInfo Add( ElementType&& element ) {
			return Emplace( Move( element ) );
		}

		template<typename... ArgsType>
		AllocationInfo Emplace( ArgsType&&...  args ) {
			const AllocationInfo allocationInfo = AddSingleEmpty();
			// ReSharper disable once CppNonReclaimedResourceAcquisition
			new( allocationInfo.element ) ElementType( Forward<ArgsType>( args )... );
			return allocationInfo;
		}

	private:

		AllocationInfo AddSingleEmpty() {
			m_size += 1;
			if( m_lastFreeIndex == -1 ) {
				const uint32_t index = m_data.AddEmpty( 1 );
				const AllocationInfo allocation( (ElementType*) &m_data[index], index );
				m_allocationInfo.Add( true );
				return allocation;
			} else {
				const uint32_t index = m_lastFreeIndex;
				AXLT_ASSERT( !m_allocationInfo[index], "Element should not be allocated" );
				ArrayElement& allocatedElement = (ArrayElement&) m_data[index];
				m_lastFreeIndex = allocatedElement.previousFreeIndex;
				m_allocationInfo[index] = true;
				return AllocationInfo( (ElementType*) &allocatedElement.element, index );
			}
		}

	public:

		ArrayType& operator+=( const ElementType& element ) {
			Emplace( element );
			return *this;
		}

		ArrayType& operator+=( ElementType&& element ) {
			Emplace( Move( element ) );
			return *this;
		}

		AllocationInfo Insert( const ElementType& element, const uint32_t index ) {
			AllocationInfo allocationInfo = InsertSingleEmpty( index );
			// ReSharper disable once CppNonReclaimedResourceAcquisition
			new( allocationInfo.element ) ElementType( element );
			return allocationInfo;
		}

		AllocationInfo Insert( ElementType&& element, const uint32_t index ) {
			AllocationInfo allocationInfo = InsertSingleEmpty( index );
			// ReSharper disable once CppNonReclaimedResourceAcquisition
			new( allocationInfo.element ) ElementType( Move( element ) );
			return allocationInfo;
		}

		AllocationInfo InsertSingleEmpty( const uint32_t index ) {
			if( index >= m_data.GetSize() ) {
				Grow( index + 1 );
			} else {
				AXLT_ASSERT( !m_allocationInfo[index], "Element is already allocated" );
			}

			m_size += 1;
			m_allocationInfo[index] = true;
			ArrayElement& element = m_data[index];
			if( element.previousFreeIndex != -1 ) {
				m_data[element.previousFreeIndex].nextFreeIndex = element.nextFreeIndex;
			}
			if( element.nextFreeIndex != -1 ) {
				m_data[element.nextFreeIndex].previousFreeIndex = element.previousFreeIndex;
			}
			if( m_lastFreeIndex == index ) {
				m_lastFreeIndex = element.previousFreeIndex;
			}

			return AllocationInfo( &element.element, index );
		}

		ElementType& operator[]( uint32_t index ) {
			return *m_data[index].element;
		}

		const ElementType& operator[]( uint32_t index ) const {
			return *m_data[index].element;
		}

		ArrayElement* GetData() {
			return m_data.GetData();
		}

		const ArrayElement* GetData() const {
			return m_data.GetData();
		}

		bool IsFree( const uint32_t index ) const {
			return !m_allocationInfo[index];
		}

		void Grow( const uint32_t size ) {
			AXLT_ASSERT( size >= m_data.GetSize(), "Parameter must be bigger than size of the allocated data" );

			const uint32_t additionalElements = size - m_data.GetSize();
			m_data.AddEmpty( additionalElements );
			m_allocationInfo.AddEmpty( additionalElements );
			for( uint32_t i = m_data.GetSize(); i < size; ++i ) {
				if( m_lastFreeIndex != -1 ) {
					m_data[m_lastFreeIndex].nextFreeIndex = i;
				}
				m_data[i].previousFreeIndex = m_lastFreeIndex;
				m_lastFreeIndex = i;

				m_allocationInfo[i] = false;
			}
		}

		void Shrink() {
			const uint32_t lastIndex = GetLastUsedElementIndex();

			for( uint32_t i = lastIndex + 1; i < m_data.GetSize(); ++i ) {
				if( m_lastFreeIndex == i ) {
					m_lastFreeIndex = m_data[i].previousFreeIndex;
				}
				if( m_data[i].previousFreeIndex != -1 ) {
					m_data[m_data[i].previousFreeIndex].nextFreeIndex = m_data[i].nextFreeIndex;
				}
				if( m_data[i].nextFreeIndex != -1 ) {
					m_data[m_data[i].nextFreeIndex].previousFreeIndex = m_data[i].previousFreeIndex;
				}
			}

			m_data.RemoveRange( lastIndex + 1, m_data.GetSize() - lastIndex - 1 );
			m_allocationInfo.RemoveRange( lastIndex + 1, m_data.GetSize() - lastIndex - 1 );
		}

		uint32_t GetLastUsedElementIndex() {
			if( m_size == 0 ) {
				return 0;
			}

			uint32_t elementsSeen = 0;
			for( uint32_t i = 0; i < m_data.GetSize(); i++ ) {
				if( m_allocationInfo[i] == true && ++elementsSeen == m_size ) {
					return i;
				}
			}

			AXLT_ASSERT( false, "Should never get here" );
		}

		void Remove( const uint32_t index ) {
			AXLT_ASSERT( index < m_size, "Remove is used outside of Array length" );
			AXLT_ASSERT( m_allocationInfo[index], "Element was not allocated" );

			if( m_lastFreeIndex != -1 ) {
				m_data[m_lastFreeIndex].nextFreeIndex = index;
			}
			m_data[index].previousFreeIndex = m_lastFreeIndex;
			m_data[index].nextFreeIndex = -1;
			m_lastFreeIndex = index;

			m_allocationInfo[index] = false;
			m_size -= 1;
		}

		void RemoveRange( const uint32_t startIndex, const uint32_t count ) {
			if( count == 0 ) {
				return;
			}
			AXLT_ASSERT( startIndex + count <= m_size, "Remove is used outside of Array length" );

			if( m_lastFreeIndex != -1 ) {
				m_data[m_lastFreeIndex].nextFreeIndex = startIndex;
			}
			for( uint32_t i = 0; i < count; ++i ) {
				AXLT_ASSERT( m_allocationInfo[startIndex + i], "Element was not allocated" );
				m_data[startIndex + i].previousFreeIndex = i == 0 ? m_lastFreeIndex : startIndex + i - 1;
				m_allocationInfo[startIndex + i] = false;
				if( i != 0 ) {
					m_data[startIndex + i - 1].nextFreeIndex = startIndex + i;
				}
			}

			m_data[startIndex + count - 1].nextFreeIndex = -1;
			m_lastFreeIndex = startIndex + count - 1;

			m_size -= count;
		}

		uint32_t GetSize() const {
			return m_size;
		}

		uint32_t GetDataArraySize() const {
			return m_data.GetSize();
		}

		uint32_t GetFirstUsedElementIndex() const {
			typename BitArrayType::ConstIterator it = m_allocationInfo.begin();
			for( uint32_t i = 0; i < m_size; i++ ) {
				if ( *it ) {
					return i;
				}
				++it;
			}
			return m_size;
		}

		void Clear() {
			for( ElementType& elem : *this ) {
				elem.ElementType::~ElementType();
			}
			m_data.Clear();
			m_allocationInfo.Clear();
			m_size = 0;
			m_lastFreeIndex = -1;
		}

		class Iterator {

		public:

			explicit Iterator( ArrayType& array, const uint32_t index ) :
				m_array( array ), m_index( index ) {}

			explicit operator ElementType&() const {
				return m_array[m_index];
			}

			ElementType& operator*() const {
				return m_array[m_index];
			}

			Iterator& operator=( const ElementType& value ) {
				m_array[m_index] = value;
				return *this;
			}

			Iterator& operator=( ElementType&& value ) {
				m_array[m_index] = Move( value );
				return *this;
			}

			Iterator& operator++() {
				do {
					++m_index;
				} while( m_index != m_array.m_data.GetSize() && !IsValid() );
				return *this;
			}

			bool operator==( const Iterator& other ) const {
				return m_array == other.m_array && m_index == other.m_index;
			}

			bool operator!=( const Iterator& other ) const {
				return &m_array != &other.m_array || m_index != other.m_index;
			}

		private:

			bool IsValid() {
				return !m_array.IsFree( m_index );
			}

			ArrayType& m_array;
			uint32_t m_index;
		};

		class ConstIterator {

		public:

			explicit ConstIterator( const ArrayType& array, const uint32_t index ) :
				m_array( array ), m_index( index ) {}

			explicit operator ElementType&() const {
				return m_array[m_index];
			}

			const ElementType& operator*() const {
				return m_array[m_index];
			}

			ConstIterator& operator++() {
				do {
					m_index++;
				} while( m_index != m_array.m_data.GetSize() && !IsValid() );
				return *this;
			}

			bool operator==( const ConstIterator& other ) const {
				return m_array == other.m_array && m_index == other.m_index;
			}

			bool operator!=( const ConstIterator& other ) const {
				return m_array != other.m_array && m_index != other.m_index;
			}

		private:

			bool IsValid() {
				return !m_array.IsFree( m_index );
			}

			const ArrayType& m_array;
			uint32_t m_index;
		};

		// ReSharper disable CppInconsistentNaming

		Iterator begin() {
			return Iterator( *this, GetFirstUsedElementIndex() );
		}

		Iterator end() {
			return Iterator( *this, GetDataArraySize() );
		}

		ConstIterator begin() const {
			return ConstIterator( *this, GetFirstUsedElementIndex() );
		}

		ConstIterator end() const {
			return ConstIterator( *this, GetDataArraySize() );
		}

		// ReSharper restore CppInconsistentNaming

	private:

		DataArrayType m_data;
		BitArrayType m_allocationInfo;
		int32_t m_lastFreeIndex = -1;
		uint32_t m_size = 0;
	};
}
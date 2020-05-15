#pragma once

#include "Allocators/ArrayAllocators.h"
#include "Memory/MemoryFunctions.h"

namespace axlt {
	class BitReference {

	public:

		BitReference( uint8_t& value, const uint8_t mask ) :
			m_value( value ), m_mask( mask ) {}

		operator bool() const {
			return ( m_value & m_mask ) > 0;
		}

		bool operator*() const {
			return ( m_value & m_mask ) > 0;
		}

		BitReference& operator=( const bool value ) {
			m_value = ( m_mask & ( value ? 0b11111111 : 0b00000000 ) ) + ( ~m_mask & m_value );
			return *this;
		}

	private:

		uint8_t& m_value;
		uint8_t m_mask;
	};

	template<typename AllocatorClass = DefaultArrayAllocator>
	class BitArray {

	public:

		typedef BitArray<AllocatorClass> ArrayType;
		typedef typename AllocatorClass::template AllocatorInstance<uint8_t> Allocator;

		explicit BitArray()
			: m_size( 0 ) {}

		BitArray( std::initializer_list<bool> initializerList ) : m_size( initializerList.size() ) {
			uint32_t byteSize = BitsToBytes( initializerList.size() );
			m_allocator.ExpandAllocation( 0, byteSize );
			int i = 0;
			for( auto val : initializerList ) {
				operator[]( i++ ) = val;
			}
		}

		BitArray( const ArrayType& other ) : m_size( other.m_size ) {
			m_allocator.CopyToEmpty( other.m_allocator );
		}

		BitArray<AllocatorClass>& operator=( const ArrayType& other ) {  // NOLINT(cert-oop54-cpp)
			if( this == &other ) return *this;
			Clear();
			m_allocator.CopyToEmpty( other.m_allocator );
			return *this;
		}

		BitArray( ArrayType&& other ) noexcept : m_size( other.m_size ) {
			m_allocator.MoveOrCopyToEmpty( other.m_allocator );
		}

		BitArray<AllocatorClass>& operator=( ArrayType&& other ) noexcept {
			if( this == &other ) return *this;
			Clear();
			m_allocator.MoveOrCopyToEmpty( other.m_allocator );
			return *this;
		}

		template<typename OtherAllocatorClass>
		explicit BitArray( const BitArray<OtherAllocatorClass>& other ) : m_size( other.m_size ) {
			uint32_t byteSize = BitsToBytes( other.m_size );
			m_allocator.ExpandAllocation( 0, byteSize );
			CopyElements( GetData(), other.GetData(), byteSize );
		}

		~BitArray() = default;

	private:

		static uint32_t BitsToBytes( const uint32_t bitCount ) {
			return ( bitCount >> 3 ) - ( ( bitCount & 0b111 ) == 0 ) + 1;
		}

	public:

		BitReference Add( const bool value ) {
			const uint32_t index = AddEmpty( 1 );
			BitReference ref = operator[]( index );
			ref = value;
			return ref;
		}

		uint32_t AddEmpty( const uint32_t count ) {
			const uint32_t oldByteSize = BitsToBytes( m_size );
			const uint32_t newByteSize = BitsToBytes( m_size + count );
			const uint32_t oldSize = m_size;

			if( oldByteSize != newByteSize ) {
				m_allocator.ExpandAllocation( oldByteSize, newByteSize );
			}
			m_size += count;
			return oldSize;
		}

		ArrayType& operator+=( const bool value ) {
			const uint32_t index = AddEmpty( 1 );
			operator[]( index ) = value;
			return *this;
		}

		void AddRange( const bool* data, const uint32_t count ) {
			const uint32_t startIndex = AddEmpty( count );
			for( int i = 0; i < count; i++ ) {
				operator[]( startIndex + i ) = data[i];
			}
		}

		template<typename OtherAllocatorClass>
		void AddRange( const BitArray<OtherAllocatorClass>& other ) {
			AddRange( other.GetData(), other.GetSize() );
		}

		template<typename OtherAllocatorClass>
		ArrayType& operator+=( const BitArray<OtherAllocatorClass>& other ) {
			AddRange( other.GetData(), other.GetSize() );
			return *this;
		}

		BitReference Insert( const bool element, const uint32_t index ) {
			InsertEmpty( index, 1 );
			BitReference ref = operator[]( index );
			ref = element;
			return ref;
		}

		void InsertEmpty( const uint32_t index, const uint32_t count ) {
			AXLT_ASSERT( index < m_size, "Insert is used outside of Array length" );
			
			const uint32_t oldSize = m_size;
			m_size += count;

			const uint32_t oldByteSize = BitsToBytes( oldSize );
			const uint32_t newByteSize = BitsToBytes( m_size );

			if( oldByteSize != newByteSize ) {
				m_allocator.ExpandAllocation( oldSize, m_size );
			}

			for( int i = oldSize - 1; i >= index; i-- ) {
				operator[]( i + count ) = *operator[]( i );
			}
		}

		BitReference operator[]( const uint32_t index ) {
			return BitReference( GetData()[index >> 3], 1 << ( index % 8 ) );
		}

		bool operator[]( const uint32_t index ) const {
			return ( ( GetData()[index >> 3] )& ( 1 << ( index % 8 ) ) ) > 0;
		}

		uint8_t* GetData() {
			return m_allocator.GetAllocation();
		}

		const uint8_t* GetData() const {
			return m_allocator.GetAllocation();
		}

		void Remove( const uint32_t index ) {
			AXLT_ASSERT( index < m_size, "Remove is used outside of Array length" );

			const uint32_t previousSize = m_size;
			m_size -= 1;

			for( uint32_t i = index; i < m_size; i++ ) {
				operator[]( i ) = *operator[]( i + 1 );
			}

			m_allocator.ShrinkAllocation( BitsToBytes( previousSize ), BitsToBytes( m_size ) );
		}

		void RemoveRange( const uint32_t startIndex, const uint32_t count ) {
			AXLT_ASSERT( startIndex + count <= m_size, "RemoveRange is used outside of Array length" );

			const uint32_t previousSize = m_size;
			m_size -= count;

			for( uint32_t i = startIndex; i < m_size; i++ ) {
				operator[]( i ) = *operator[]( i + count );
			}

			m_allocator.ShrinkAllocation( BitsToBytes( previousSize ), BitsToBytes( m_size ) );
		}

		uint32_t GetSize() const {
			return m_size;
		}

		void Clear() {
			m_allocator.ShrinkAllocation( BitsToBytes( m_size ), 0 );
			m_size = 0;
		}

		class Iterator {

		public:

			explicit Iterator( ArrayType& array, const uint32_t index ) :
				m_array( array ), m_index( index ) {}

			operator bool() const {
				return m_array[m_index];
			}

			bool operator*() const {
				return m_array[m_index];
			}

			Iterator& operator=( bool value ) {
				m_array[m_index] = value;
				return *this;
			}

			Iterator& operator++() {
				m_index++;
				return *this;
			}

			bool operator==( const Iterator& other ) const {
				return m_array == other.m_array && m_index == other.m_index;
			}

			bool operator!=( const Iterator& other ) const {
				return &m_array != &other.m_array || m_index != other.m_index;
			}

		private:

			ArrayType& m_array;
			uint32_t m_index;
		};

		class ConstIterator {

		public:

			explicit ConstIterator( const ArrayType& array, const uint32_t index ) :
				m_array( array ), m_index( index ) {}

			operator bool() const {
				return m_array[m_index];
			}

			bool operator*() const {
				return m_array[m_index];
			}

			ConstIterator& operator++() {
				m_index++;
				return *this;
			}

			bool operator==( const ConstIterator& other ) const {
				return m_array == other.m_array && m_index == other.m_index;
			}

			bool operator!=( const ConstIterator& other ) const {
				return m_array != other.m_array && m_index != other.m_index;
			}

		private:

			const ArrayType& m_array;
			uint32_t m_index;
		};

		// ReSharper disable CppInconsistentNaming

		Iterator begin() {
			return Iterator( *this, 0 );
		}

		Iterator end() {
			return Iterator( *this, m_size );
		}

		ConstIterator begin() const {
			return ConstIterator( *this, 0 );
		}

		ConstIterator end() const {
			return ConstIterator( *this, m_size );
		}
		// ReSharper restore CppInconsistentNaming

	private:

		uint32_t m_size;
		Allocator m_allocator;
	};
}

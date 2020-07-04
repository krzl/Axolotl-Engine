#pragma once

#include "Allocators/ArrayAllocators.h"
#include "Memory/MemoryFunctions.h"
#include "Resources/Serialization/Serializer.h"

namespace axlt {
	template<typename _ElementType, typename _AllocatorClass = DefaultArrayAllocator>
	class Array {

		template<typename, typename>
		friend class Array;

	public:

		typedef _ElementType ElementType;
		typedef _AllocatorClass AllocatorClass;
		typedef Array<ElementType, AllocatorClass> ArrayType;
		typedef typename AllocatorClass::template AllocatorInstance<ElementType> Allocator;

		explicit Array()
			: m_size( 0 ), m_allocator() {}

		explicit Array( const uint32_t initialSize ) :
			m_size( 0 ) {
			AddEmpty( initialSize );
		}

		Array( std::initializer_list<ElementType> initializerList ) : m_size( 0 ) {
			m_allocator.ExpandAllocation( 0, (uint32_t) initializerList.size() );
			CopyElements( m_allocator.GetAllocation(), initializerList.begin(), (uint32_t) initializerList.size() );
			m_size = (uint32_t) initializerList.size();
		}

		Array( ElementType* data, const uint32_t count ) : m_size( 0 ) {
			m_allocator.ExpandAllocation( 0, count );
			CopyElements( m_allocator.GetAllocation(), data, count );
			m_size = count;
		}

		Array( const ArrayType& other ) : m_size( other.GetSize() ) {
			m_allocator.CopyToEmpty( other.m_allocator );
		}

		Array<ElementType, AllocatorClass>& operator=( const ArrayType& other ) {  // NOLINT(cert-oop54-cpp)
			if( this == &other ) return *this;
			Clear();
			m_allocator.CopyToEmpty( other.m_allocator );
			m_size = other.m_size;
			return *this;
		}

		template<typename OtherAllocatorClass>
		Array<ElementType, AllocatorClass>& operator=( const Array<ElementType, OtherAllocatorClass>& other ) {
			Clear();
			m_allocator.ExpandAllocation( 0, other.m_size );
			CopyElements( m_allocator.GetAllocation(), other.GetData(), other.m_size );
			m_size = other.m_size;
			return *this;
		}

		template<typename OtherAllocatorClass>
		Array( const Array<ElementType, OtherAllocatorClass>& other ) : m_size( 0 ) {
			m_allocator.ExpandAllocation( 0, other.m_size );
			CopyElements( m_allocator.GetAllocation(), other.GetData(), other.m_size );
			m_size = other.m_size;
		}

		template<typename OtherElementType, typename OtherAllocatorClass>
		explicit Array( const Array<OtherElementType, OtherAllocatorClass>& other ) : m_size( 0 ) {
			AddEmpty( other.m_size );
			for( uint32_t i = 0; i < other.m_size; i++ ) {
				operator[]( i ) = (ElementType) other[i];
			}
		}

		Array( ArrayType&& other ) noexcept : m_size( 0 ) {
			m_allocator.MoveOrCopyToEmpty( other.m_allocator );
			m_size = other.m_size;
			other.m_size = 0;
		}

		Array<ElementType, AllocatorClass>& operator=( ArrayType&& other ) noexcept {
			if( this == &other ) return *this;
			Clear();
			m_allocator.MoveOrCopyToEmpty( other.m_allocator );
			m_size = other.m_size;
			other.m_size = 0;
			return *this;
		}
		
		template<typename OtherElementType, typename OtherAllocatorClass>
		Array<ElementType, AllocatorClass>& operator=( const Array<OtherElementType, OtherAllocatorClass>& other ) {
			Clear();
			AddEmpty( other.m_size );
			for( uint32_t i = 0; i < other.m_size; i++ ) {
				operator[]( i ) = (ElementType) other[i];
			}
			return *this;
		}

		~Array() {
			for( uint32_t i = 0; i < m_size; i++ ) {
				DestroyElement( i );
			}
		}

		ElementType& Add( const ElementType& element ) {
			return Emplace( element );
		}

		ElementType& Add( ElementType&& element ) {
			return Emplace( Move( element ) );
		}

		template <typename... ArgsType>
		ElementType& Emplace( ArgsType&&... args ) {
			uint32_t index = AddEmpty( 1 );
			// ReSharper disable once CppNonReclaimedResourceAcquisition
			new( GetData() + index ) ElementType( Forward<ArgsType>( args )... );
			return operator[]( index );
		}

		uint32_t AddEmpty( const uint32_t count ) {
			const uint32_t previousSize = m_size;
			m_size += count;
			m_allocator.ExpandAllocation( previousSize, m_size );
			return previousSize;
		}

		ArrayType& operator+=( ElementType& element ) {
			Emplace( element );
			return *this;
		}

		ArrayType& operator+=( ElementType&& element ) {
			Emplace( Move( element ) );
			return *this;
		}

		void AddRange( const ElementType* data, const uint32_t count ) {
			m_allocator.ExpandAllocation( m_size, m_size + count );
			uint32_t oldSize = m_size;
			m_size += count;
			CopyElements( GetData() + oldSize, data, count );
		}

		template<typename OtherAllocatorClass>
		void AddRange( const Array<ElementType, OtherAllocatorClass>& other ) {
			m_allocator.ExpandAllocation( m_size, m_size + other.GetSize() );
			uint32_t oldSize = m_size;
			m_size += other.GetSize();
			CopyElements( GetData() + oldSize, other.GetData(), other.GetSize() );
		}

		template<typename OtherAllocatorClass>
		ArrayType& operator+=( const Array<ElementType, OtherAllocatorClass>& other ) {
			m_allocator.ExpandAllocation( m_size, m_size + other.GetSize() );
			uint32_t oldSize = m_size;
			m_size += other.GetSize();
			CopyElements( GetData() + oldSize, other.GetData(), other.GetSize() );
			return *this;
		}

		ElementType& Insert( const ElementType& element, const uint32_t index ) {
			return InsertImpl( index, element );
		}

		ElementType& Insert( ElementType&& element, const uint32_t index ) {
			return InsertImpl( index, Move( element ) );
		}

	private:

		template <typename... ArgsType>
		ElementType& InsertImpl( const uint32_t index, ArgsType&&... args ) {
			InsertEmpty( index, 1 );
			// ReSharper disable once CppNonReclaimedResourceAcquisition
			new( GetData() + index ) ElementType( Forward<ArgsType>( args )... );
			return operator[]( index );
		}

	public:

		void InsertEmpty( const uint32_t index, const uint32_t count ) {
			AXLT_ASSERT( index <= m_size, "Insert is used outside of Array length" );
			uint32_t previousSize = m_size;
			m_size += count;
			m_allocator.ExpandAllocation( previousSize, m_size );
			if( index != previousSize ) {
				//TODO: If ElementType is not relocatable, it won't work because of data overriding itself
				MoveElements( GetData() + index + count, GetData() + index, previousSize - index );
			}
		}

		ElementType& operator[]( uint32_t index ) {
			return GetData()[index];
		}

		const ElementType& operator[]( uint32_t index ) const {
			return GetData()[index];
		}

		ElementType* GetData() {
			return GetSize() == 0 ? nullptr : m_allocator.GetAllocation();
		}

		const ElementType* GetData() const {
			return GetSize() == 0 ? nullptr : m_allocator.GetAllocation();
		}

		void Remove( const uint32_t index ) {
			AXLT_ASSERT( index < m_size, "Remove is used outside of Array length" );

			DestroyElement( index );

			uint32_t previousSize = m_size;
			m_size -= 1;

			if( index != previousSize - 1 ) {
				MoveElements( GetData() + index, GetData() + index + 1, previousSize - index - 1 );
			}

			m_allocator.ShrinkAllocation( previousSize, m_size );
		}

		void RemoveRange( const uint32_t startIndex, const uint32_t count ) {
			AXLT_ASSERT( startIndex + count - 1 < m_size, "RemoveRange is used outside of Array length" );
			if( count == 0 ) {
				return;
			}

			for( uint32_t i = startIndex; i < startIndex + count; i++ ) {
				DestroyElement( i );
			}

			uint32_t previousSize = m_size;
			m_size -= count;

			if( startIndex != previousSize - count ) {
				MoveElements( GetData() + startIndex, GetData() + startIndex + count, previousSize - startIndex - count );
			}

			m_allocator.ShrinkAllocation( previousSize, m_size );
		}

		void DestroyElement( const uint32_t index ) {
			operator[]( index )._ElementType::~_ElementType();
		}

		uint32_t GetSize() const {
			return m_size;
		}

		void Clear() {
			for( uint32_t i = 0; i < m_size; i++ ) {
				DestroyElement( i );
			}
			m_allocator.ShrinkAllocation( m_size, 0 );
			m_size = 0;
		}


		// ReSharper disable CppInconsistentNaming
		ElementType* begin() {
			return &operator[]( 0 );
		}

		ElementType* end() {
			return &operator[]( m_size );
		}

		const ElementType* begin() const {
			return &operator[]( 0 );
		}

		const ElementType* end() const {
			return &operator[]( m_size );
		}
		// ReSharper restore CppInconsistentNaming

	private:

		uint32_t m_size;
		Allocator m_allocator{};
	};

	template<typename ElementType, int Count>
	using FixedArray = Array<ElementType, FixedArrayAllocator<Count>>;

	template<typename ElementType>
	using ExactArray = Array<ElementType, ExactHeapArrayAllocator>;

	template<typename ElementType, typename AllocatorType>
	typename EnableIf<!IsBaseOf<Serializable, ElementType>::Value, Serializer&>::Type operator<<( Serializer& s, Array<ElementType, AllocatorType>& array ) {
		s.Write( array.GetSize() );
		s.Write( array.GetData(), array.GetSize() );
		return s;
	}

	template<typename ElementType, typename AllocatorType>
	typename EnableIf<IsBaseOf<Serializable, ElementType>::Value, Serializer&>::Type operator<<( Serializer& s, Array<ElementType, AllocatorType>& array ) {
		s.Write( array.GetSize() );
		for( ElementType& element : array ) {
			s << element;
		}
		return s;
	}

	template<typename ElementType, typename AllocatorType, typename OtherAllocatorType>
	Serializer& operator<<( Serializer& s, Array<Array<ElementType, AllocatorType>, OtherAllocatorType>& arrayOfArrays ) {
		s.Write( arrayOfArrays.GetSize() );
		for( Array<ElementType, AllocatorType>& array : arrayOfArrays ) {
			s << array;
		}
		return s;
	}

	template<typename ElementType, typename AllocatorType>
	typename EnableIf<!IsBaseOf<Serializable, ElementType>::Value, Serializer&>::Type operator>>( Serializer& s, Array<ElementType, AllocatorType>& array ) {
		uint32_t arraySize = 0;
		s.Read( arraySize );
		array.AddEmpty( arraySize );
		s.Read( array.GetData(), array.GetSize() );
		return s;
	}

	template<typename ElementType, typename AllocatorType>
	typename EnableIf<IsBaseOf<Serializable, ElementType>::Value, Serializer&>::Type operator>>( Serializer& s, Array<ElementType, AllocatorType>& array ) {
		uint32_t arraySize = 0;
		s.Read( arraySize );
		for( uint32_t i = 0; i < arraySize; i++ ) {
			ElementType& element = array.Emplace();
			s >> element;
		}
		return s;
	}

	template<typename ElementType, typename AllocatorType, typename OtherAllocatorType>
	Serializer& operator>>( Serializer& s, Array<Array<ElementType, AllocatorType>, OtherAllocatorType>& arrayOfArrays ) {
		uint32_t arraySize = 0;
		s.Read( arraySize );
		arrayOfArrays.AddEmpty( arraySize );
		for( Array<ElementType, AllocatorType>& array : arrayOfArrays ) {
			s >> array;
		}
		return s;
	}
}

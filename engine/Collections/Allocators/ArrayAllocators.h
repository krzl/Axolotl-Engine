#pragma once

#include "Debug/Assertions.h"
#include "Templates/TemplateFunctions.h"
#include "Memory/MemoryFunctions.h"

namespace axlt {

	template<uint32_t ElementCount>
	class FixedArrayAllocator {

	public:

		template<typename ElementType>
		class AllocatorInstance {

		public:

			void MoveOrCopyToEmpty( AllocatorInstance& other ) {
				MoveElements<ElementType>( (ElementType*) m_elements, (ElementType*) other.m_elements, ElementCount );
			}

			void CopyToEmpty( const AllocatorInstance& other ) {
				CopyElements<ElementType>( (ElementType*) m_elements, (ElementType*) other.m_elements, ElementCount );
			}

			ElementType* GetAllocation() {
				return (ElementType*) m_elements;
			}

			const ElementType* GetAllocation() const {
				return (const ElementType*) m_elements;
			}

			// ReSharper disable once CppMemberFunctionMayBeStatic
			void ResizeAllocation( const uint32_t oldSize, const uint32_t newSize ) {

			}

			void ExpandAllocation( const uint32_t oldSize, const uint32_t newSize ) {
				FLXASSERT( newSize <= ElementCount, "Too much elements allocated" );
			}

			// ReSharper disable once CppMemberFunctionMayBeStatic
			void ShrinkAllocation( const uint32_t oldSize, const uint32_t newSize ) {

			}

		private:

			TypeCompatibleBytes<ElementType> m_elements[ElementCount];
		};
	};

	class HeapArrayAllocator {

	public:

		template<typename ElementType>
		class AllocatorInstance {  // NOLINT(hicpp-special-member-functions, cppcoreguidelines-special-member-functions)

			typedef TypeCompatibleBytes<ElementType> AllocationType;

		public:

			void MoveOrCopyToEmpty( AllocatorInstance& other ) {
				ExpandAllocation( 0, other.m_count );
				m_elements = other.m_elements;
				m_allocationSize = other.m_allocationSize;
				m_count = other.m_count;

				other.m_elements = nullptr;
				other.m_allocationSize = 0;
				other.m_count = 0;
			}

			void CopyToEmpty( const AllocatorInstance& other ) {
				ExpandAllocation( 0, other.m_count );
				CopyElements<ElementType>( (ElementType*) m_elements, (ElementType*) other.m_elements, other.m_count );
				m_allocationSize = other.m_allocationSize;
				m_count = other.m_count;
			}

			ElementType* GetAllocation() {
				return (ElementType*) m_elements;
			}

			const ElementType* GetAllocation() const {
				return (const ElementType*) m_elements;
			}

			void ResizeAllocation( const uint32_t oldSize, const uint32_t newSize ) {
				if( oldSize < newSize ) {
					ExpandAllocation( oldSize, newSize );
				} else if( oldSize > newSize ) {
					ShrinkAllocation( oldSize, newSize );
				}
			}

			void ExpandAllocation( const uint32_t oldSize, const uint32_t newSize ) {
				if( m_allocationSize < newSize ) {
					AllocationType* oldElements = m_elements;

					constexpr uint32_t initialSize = 4;
					constexpr uint32_t numerator = 11;
					constexpr uint32_t denominator = 8;
					constexpr uint32_t constant = 16;

					uint32_t newAllocationSize = m_allocationSize;

					while( newAllocationSize < newSize ) {
						if( oldElements == nullptr && newSize <= initialSize ) {
							newAllocationSize = initialSize;
						} else {
							newAllocationSize = numerator * newAllocationSize / denominator + constant;
						}
					}

					m_elements = new AllocationType[newAllocationSize];
					m_allocationSize = newAllocationSize;

					if( oldElements != nullptr ) {
						MoveElements( m_elements, oldElements, oldSize );
					}

					delete[] oldElements;
				}
				m_count = newSize;
			}

			void ShrinkAllocation( const uint32_t oldSize, const uint32_t newSize ) {
				if( oldSize == 0 ) return;
				const uint32_t unusedElements = m_allocationSize - newSize;
				const uint32_t unusedBytes = sizeof( ElementType ) * unusedElements;

				const uint32_t unusedElementsLimit = 2 * m_allocationSize / 3;
				constexpr uint32_t unusedBytesLimit = 1024 * 16;

				if( unusedElements >= unusedElementsLimit || unusedBytes >= unusedBytesLimit ) {
					AllocationType* oldElements = m_elements;

					if( newSize != 0 ) {
						m_elements = new AllocationType[newSize];
						MoveElements( m_elements, oldElements, newSize );
					} else {
						m_elements = nullptr;
					}
					m_allocationSize = newSize;

					delete[] oldElements;
				}
				m_count = newSize;
			}

			~AllocatorInstance() {
				delete[] m_elements;
			}

		private:

			AllocationType* m_elements = nullptr;
			uint32_t m_allocationSize = 0;
			uint32_t m_count = 0;
		};
	};

	class ExactHeapArrayAllocator {

	public:

		template<typename ElementType>
		class AllocatorInstance {  // NOLINT(hicpp-special-member-functions, cppcoreguidelines-special-member-functions)

			typedef TypeCompatibleBytes<ElementType> AllocationType;

		public:

			void MoveOrCopyToEmpty( AllocatorInstance& other ) {
				ExpandAllocation( 0, other.m_count );
				m_elements = other.m_elements;
				m_count = other.m_count;

				other.m_elements = nullptr;
				other.m_count = 0;
			}

			void CopyToEmpty( const AllocatorInstance& other ) {
				ExpandAllocation( 0, other.m_count );
				CopyElements<ElementType>( (ElementType*) m_elements, (ElementType*) other.m_elements, other.m_count );
				m_count = other.m_count;
			}

			ElementType* GetAllocation() {
				return (ElementType*) m_elements;
			}

			const ElementType* GetAllocation() const {
				return (const ElementType*) m_elements;
			}

			void ResizeAllocation( const uint32_t oldSize, const uint32_t newSize ) {
				if( oldSize < newSize ) {
					ExpandAllocation( oldSize, newSize );
				} else if( oldSize > newSize ) {
					ShrinkAllocation( oldSize, newSize );
				}
			}

			void ExpandAllocation( const uint32_t oldSize, const uint32_t newSize ) {
				AllocationType* oldElements = m_elements;

				m_elements = new AllocationType[newSize];
				m_count = newSize;

				if( oldElements != nullptr ) {
					MoveElements( m_elements, oldElements, oldSize );
				}

				delete[] oldElements;
			}

			void ShrinkAllocation( const uint32_t oldSize, const uint32_t newSize ) {
				AllocationType* oldElements = m_elements;

				m_elements = new AllocationType[newSize];
				m_count = newSize;

				MoveElements( m_elements, oldElements, newSize );

				delete[] oldElements;
			}

			~AllocatorInstance() {
				delete[] m_elements;
			}

		private:

			AllocationType* m_elements = nullptr;
			uint32_t m_count = 0;
		};
	};

	template<uint32_t ElementCount, typename SecondaryAllocator = HeapArrayAllocator>
	class MixedArrayAllocator {

	public:

		template<typename ElementType>
		class AllocatorInstance {

			typedef typename SecondaryAllocator::template AllocatorInstance<ElementType> SecondaryAllocatorInstance;

		public:

			void MoveOrCopyToEmpty( AllocatorInstance& other ) {
				ExpandAllocation( 0, other.m_count );
				if( other.m_count > ElementCount ) {
					MoveElements<ElementType>( (ElementType*) m_elements, (ElementType*) other.m_elements, ElementCount );
				} else {
					m_secondaryAllocator.MoveOrCopyToEmpty( other.m_secondaryAllocator );
				}
				m_count = other.m_count;
				other.m_count = 0;
			}

			void CopyToEmpty( const AllocatorInstance& other ) {
				ExpandAllocation( 0, other.m_count );
				if( other.m_count > ElementCount ) {
					CopyElements<ElementType>( (ElementType*) m_elements, (ElementType*) other.m_elements, ElementCount );
				} else {
					CopyElements<ElementType>( (ElementType*) m_elements, (ElementType*) other.m_elements, ElementCount );
				}
				m_count = other.m_count;
			}

			ElementType* GetAllocation() {
				return ( m_count > ElementCount ? (ElementType*) m_secondaryAllocator.GetAllocation() : (ElementType*) m_elements );
			}

			const ElementType* GetAllocation() const {
				return ( m_count > ElementCount ? (const ElementType*) m_secondaryAllocator.GetAllocation() : (const ElementType*) m_elements );
			}

			void ResizeAllocation( const uint32_t oldSize, const uint32_t newSize ) {
				if( oldSize < newSize ) {
					ExpandAllocation( oldSize, newSize );
				} else if( oldSize > newSize ) {
					ShrinkAllocation( oldSize, newSize );
				}
			}

			void ExpandAllocation( const uint32_t oldSize, const uint32_t newSize ) {
				if( oldSize <= ElementCount && newSize > ElementCount ) {
					m_secondaryAllocator.ExpandAllocation( 0, newSize );
					MoveElements( m_secondaryAllocator.GetAllocation(), GetAllocation(), oldSize );
				} else if( oldSize > ElementCount ) {
					m_secondaryAllocator.ExpandAllocation( oldSize, newSize );
				}
				m_count = newSize;
			}

			void ShrinkAllocation( const uint32_t oldSize, const uint32_t newSize ) {
				if( oldSize > ElementCount && newSize <= ElementCount ) {
					MoveElements( GetAllocation(), m_secondaryAllocator.GetAllocation(), newSize );
					m_secondaryAllocator.ShrinkAllocation( oldSize, 0 );
				} else if( newSize > ElementCount ) {
					m_secondaryAllocator.ShrinkAllocation( oldSize, newSize );
				}
				m_count = newSize;
			}

		private:

			TypeCompatibleBytes<ElementType> m_elements[ElementCount];
			SecondaryAllocatorInstance m_secondaryAllocator;
			uint32_t m_count = 0;
		};
	};

	// Defaults

	typedef HeapArrayAllocator DefaultArrayAllocator;
}

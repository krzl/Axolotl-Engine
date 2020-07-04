#pragma once

namespace axlt {
	struct ReferenceData {
		void* ptr = nullptr;
		uint32_t sharedReferenceCount = 0;
		uint32_t weakReferenceCount = 0;
	};

	template<typename T>
	class ReferenceCounter {

	public:

		void AssignSharedReference( T* ptr ) {
			AXLT_ASSERT( refData == nullptr, "ReferenceCounter was not reset before assigning" );
			refData = new ReferenceData{
				ptr,
				1,
				0
			};
		}

		void AssignSharedReference( const ReferenceCounter<T>& counter ) {
			AXLT_ASSERT( this->refData == nullptr, "ReferenceCounter was not reset before assigning" );
			this->refData = const_cast<ReferenceCounter<T>&>(counter).refData;
			++this->refData->sharedReferenceCount;
		}

		void AssignWeakReference( const ReferenceCounter<T>& counter ) {
			AXLT_ASSERT( this->refData == nullptr, "ReferenceCounter was not reset before assigning" );
			this->refData = const_cast<ReferenceCounter<T>&>(counter).refData;
			++this->refData->weakReferenceCount;
		}

		void FreeSharedReference() {
			if (refData == nullptr) return;
			AXLT_ASSERT( refData->sharedReferenceCount != 0, "ReferenceCounter was already set to zero before freeing" );
			--refData->sharedReferenceCount;
			if ( refData->sharedReferenceCount == 0) {
				T* ptr = (T*)refData->ptr;
				delete ptr;
				refData->ptr = nullptr;
				if ( refData->weakReferenceCount == 0 ) {
					delete refData;
				}
			}
			refData = nullptr;
		}

		void FreeWeakReference() {
			if (refData == nullptr) return;
			AXLT_ASSERT( refData->weakReferenceCount != 0, "ReferenceCounter was already set to zero before freeing" );
			--refData->weakReferenceCount;
			if (refData->sharedReferenceCount == 0 && refData->weakReferenceCount == 0) {
				delete refData;
			}
			refData = nullptr;
		}

		bool IsValid() const {
			return refData != nullptr && refData->ptr != nullptr;
		}
		
		ReferenceData* refData = nullptr;
	};
}
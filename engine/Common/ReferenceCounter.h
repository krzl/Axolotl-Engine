#pragma once

namespace axlt {
	struct ReferenceData {
		void* ptr = nullptr;
		uint32_t sharedReferenceCount = 0;
		uint32_t weakReferenceCount = 0;
	};

	template<typename Type>
	class ReferenceCounter {

		template<typename>
		friend class SharedPtr;

	public:

		void AssignSharedReference( Type* ptr ) {
			AXLT_ASSERT( refData == nullptr, "ReferenceCounter was not reset before assigning" );
			refData = new ReferenceData{
				ptr,
				1,
				0
			};
		}

		void AssignSharedReference( const ReferenceCounter<Type>& counter ) {
			AXLT_ASSERT( this->refData == nullptr, "ReferenceCounter was not reset before assigning" );
			this->refData = const_cast<ReferenceCounter<Type>&>(counter).refData;
			++this->refData->sharedReferenceCount;
		}

		void AssignWeakReference( const ReferenceCounter<Type>& counter ) {
			AXLT_ASSERT( this->refData == nullptr, "ReferenceCounter was not reset before assigning" );
			this->refData = const_cast<ReferenceCounter<Type>&>(counter).refData;
			++this->refData->weakReferenceCount;
		}

		void FreeSharedReference() {
			if (refData == nullptr) return;
			AXLT_ASSERT( this->refData->sharedReferenceCount != 0, "ReferenceCounter was already set to zero before freeing" );
			if (--this->refData->sharedReferenceCount == 0) {
				delete (Type*)refData->ptr;
				delete refData;
			}
			refData = nullptr;
		}

		void FreeWeakReference() {
			if (refData == nullptr) return;
			AXLT_ASSERT( this->refData->weakReferenceCount != 0, "ReferenceCounter was already set to zero before freeing" );
			--this->refData->weakReferenceCount;
			refData = nullptr;
		}

		bool IsValid() const {
			return refData != nullptr;
		}

	private:

		ReferenceData* refData = nullptr;
	};
}
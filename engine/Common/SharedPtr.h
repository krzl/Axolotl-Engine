#pragma once
#include "ReferenceCounter.h"

namespace axlt {
	template<typename Type>
	class SharedPtr {

	public:

		SharedPtr() : SharedPtr( nullptr ) {}

		explicit SharedPtr( Type* ptr ) {
			counter.AssignSharedReference( ptr );
		}

		SharedPtr( const SharedPtr& ptr ) {
			counter.AssignSharedReference( ptr.counter );
		}

		SharedPtr( SharedPtr&& ptr ) noexcept {
			counter.AssignSharedReference( ptr.counter );
			ptr.counter.FreeSharedReference();
		}

		~SharedPtr() {
			counter.FreeSharedReference();
		}

		SharedPtr& operator=( Type* ptr ) {
			counter.FreeSharedReference();
			counter.AssignSharedReference( ptr );
			return *this;
		}

		SharedPtr& operator=( const SharedPtr& ptr ) {  // NOLINT(cert-oop54-cpp)
			if (&ptr == nullptr) return *this;
			counter.FreeSharedReference();
			counter.AssignSharedReference( ptr );
			return *this;
		}

		SharedPtr& operator=( SharedPtr&& ptr ) noexcept {
			if (&ptr == nullptr) return *this;
			counter.FreeSharedReference();
			counter.AssignSharedReference( ptr );
			ptr.counter.FreeSharedReference();
			return *this;
		}

		bool IsValid() const {
			return counter.IsValid();
		}

	private:

		ReferenceCounter<Type> counter;
	};
}

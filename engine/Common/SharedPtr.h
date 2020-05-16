#pragma once
#include "ReferenceCounter.h"
#include "WeakPtr.h"

namespace axlt {

	template<typename T> class UniquePtr;
	template<typename T> class WeakPtr;
	
	template<typename T>
	class SharedPtr {

		friend class WeakPtr<T>;
		friend class UniquePtr<T>;

	public:

		SharedPtr() : SharedPtr( nullptr ) {}

		explicit SharedPtr( T* ptr ) {
			counter.AssignSharedReference( ptr );
		}

		SharedPtr( const SharedPtr<T>& ptr ) {
			counter.AssignSharedReference( ptr.counter );
		}

		SharedPtr( SharedPtr<T>&& ptr ) noexcept {
			counter.AssignSharedReference( ptr.counter );
			ptr.counter.FreeSharedReference();
		}

		explicit SharedPtr( const WeakPtr<T>& ptr ) {
			counter.AssignSharedReference( ptr.counter );
		}

		~SharedPtr() {
			counter.FreeSharedReference();
		}

		SharedPtr& operator=( T* ptr ) {
			counter.FreeSharedReference();
			counter.AssignSharedReference( ptr );
			return *this;
		}

		SharedPtr<T>& operator=( const SharedPtr<T>& ptr ) {  // NOLINT(cert-oop54-cpp)
			if (&ptr == nullptr) return *this;
			counter.FreeSharedReference();
			counter.AssignSharedReference( ptr.counter );
			return *this;
		}

		SharedPtr<T>& operator=( const WeakPtr<T>& ptr ) {
			counter.FreeSharedReference();
			counter.AssignSharedReference( ptr.counter );
			return *this;
		}

		SharedPtr<T>& operator=( SharedPtr<T>&& ptr ) noexcept {
			if (&ptr == nullptr) return *this;
			counter.FreeSharedReference();
			counter.AssignSharedReference( ptr.counter );
			return *this;
		}

		bool IsValid() const {
			return counter.IsValid();
		}

		T& operator*() {
			return *(T*)counter.refData->ptr;
		}

		const T& operator*() const {
			return *(T*)counter.refData->ptr;
		}

		T* operator->() {
			return counter.refData != nullptr ? (T*)counter.refData->ptr : nullptr;
		}

		const T* operator->() const {
			return counter.refData != nullptr ? (T*)counter.refData->ptr : nullptr;
		}

		T* GetData() {
			return counter.refData != nullptr ? (T*) counter.refData->ptr : nullptr;
		}

		const T* GetData() const {
			return counter.refData != nullptr ? (T*)counter.refData->ptr : nullptr;
		}
		
	private:

		ReferenceCounter<T> counter;
	};

	template<typename T>
	uint32_t GetHash( const SharedPtr<T>& ptr ) {
		return GetHash( (void*)ptr.GetData() );
	}
}

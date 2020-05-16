#pragma once
#include "ReferenceCounter.h"

namespace axlt {

	template<typename T> class WeakPtr;
	template<typename T> class SharedPtr;
	
	template<typename T>
	class UniquePtr {

		friend class WeakPtr<T>;
		friend class SharedPtr<T>;

	public:

		UniquePtr() : UniquePtr( nullptr ) {}

		explicit UniquePtr( T* ptr ) {
			counter.AssignSharedReference( ptr );
		}

		UniquePtr( UniquePtr<T>&& ptr ) noexcept {
			counter.AssignSharedReference( ptr.counter );
			ptr.counter.FreeSharedReference();
		}

		~UniquePtr() {
			counter.FreeSharedReference();
		}

		UniquePtr<T>& operator=( T* ptr ) {
			counter.FreeSharedReference();
			counter.AssignSharedReference( ptr );
			return *this;
		}

		UniquePtr<T>& operator=( UniquePtr<T>&& ptr ) noexcept {
			if (&ptr == nullptr) return *this;
			counter.FreeSharedReference();
			counter.AssignSharedReference( ptr.counter );
			ptr.counter.FreeSharedReference();
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
			return counter.refData != nullptr ? (T*) counter.refData->ptr : nullptr;
		}

		const T* operator->() const {
			return counter.refData != nullptr ? (T*) counter.refData->ptr : nullptr;
		}

		T* GetData() {
			return counter.refData != nullptr ? (T*) counter.refData->ptr : nullptr;
		}

		const T* GetData() const {
			return counter.refData != nullptr ? (T*) counter.refData->ptr : nullptr;
		}

	private:

		ReferenceCounter<T> counter;
	};

	template<typename T>
	uint32_t GetHash( const UniquePtr<T>& ptr ) {
		return GetHash( (void*)ptr.GetData() );
	}
}

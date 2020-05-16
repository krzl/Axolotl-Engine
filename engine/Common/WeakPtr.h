// ReSharper disable CppNonExplicitConvertingConstructor
#pragma once
#include "ReferenceCounter.h"
#include "UniquePtr.h"

namespace axlt {

	template<typename T> class SharedPtr;
	template<typename T> class UniquePtr;
	
	template<typename T>
	class WeakPtr {

		friend class SharedPtr<T>;
		friend class UniquePtr<T>;

	public:

		WeakPtr() {}

		WeakPtr( const WeakPtr<T>& ptr ) {
			counter.AssignWeakReference( ptr.counter );
		}

		WeakPtr( WeakPtr<T>&& ptr ) noexcept {
			counter.AssignWeakReference( ptr.counter );
			ptr.counter.FreeWeakReference();
		}

		WeakPtr( const SharedPtr<T>& ptr ) {
			counter.AssignWeakReference( ptr.counter );
		}

		WeakPtr( const UniquePtr<T>& ptr ) {
			counter.AssignWeakReference( ptr.counter );
		}

		~WeakPtr() {
			counter.FreeWeakReference();
		}

		WeakPtr<T>& operator=( const WeakPtr<T>& ptr ) {  // NOLINT(cert-oop54-cpp)
			if (&ptr == nullptr) return *this;
			counter.FreeWeakReference();
			counter.AssignWeakReference( ptr.counter );
			return *this;
		}

		WeakPtr<T>& operator=( WeakPtr<T>&& ptr ) noexcept {
			if (&ptr == nullptr) return *this;
			counter.FreeWeakReference();
			counter.AssignWeakReference( ptr.counter );
			ptr.counter.FreeWeakReference();
			return *this;
		}

		WeakPtr<T>& operator=( const SharedPtr<T>& ptr ) {  // NOLINT(cert-oop54-cpp)
			if (&ptr == nullptr) return *this;
			counter.FreeWeakReference();
			counter.AssignWeakReference( ptr.counter );
			return *this;
		}

		WeakPtr<T>& operator=( const UniquePtr<T>& ptr ) {  // NOLINT(cert-oop54-cpp)
			if (&ptr == nullptr) return *this;
			counter.FreeWeakReference();
			counter.AssignWeakReference( ptr.counter );
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
	uint32_t GetHash( const WeakPtr<T>& ptr ) {
		return GetHash( (void*) ptr.GetData() );
	}
}

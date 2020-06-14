#pragma once

namespace axlt {

	class Serializable;
	
	class BaseFieldGetter {

	protected:

		BaseFieldGetter() = default;
		
	public:

		template<typename T, typename C>
		T& GetValue( C& owner ) const;

		size_t offset;
	};

	template<typename T, typename C>
	class FieldGetter final : public BaseFieldGetter {

		friend class BaseFieldGetter;

	public:
		
		// ReSharper disable once CppNonExplicitConvertingConstructor
		FieldGetter( T C::* ptr ) {
			this->ptr = ptr;

			C* zero = nullptr;
			this->offset = (size_t)(&(zero->*ptr));
		}

		T C::* ptr;
	};
	
	template<typename T, typename C>
	T& BaseFieldGetter::GetValue( C& owner ) const {
		const FieldGetter<T, C>* typed = reinterpret_cast<const FieldGetter<T, C>*>( this );
		T C::* ptr = (T C::*) typed->ptr;
		return owner.*ptr;
	}

	template<>
	inline void*& BaseFieldGetter::GetValue( Serializable& owner ) const {
		void* ptr = (uint8_t*)(&owner) + offset;
		return ptr;
	}
}
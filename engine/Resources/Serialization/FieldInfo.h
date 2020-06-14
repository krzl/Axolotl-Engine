#pragma once
#include "FieldGetter.h"

namespace axlt {
	class Serializable;
	
	class FieldInfo {

	public:

		template<typename T, typename C>
		explicit FieldInfo( T C::* ptr ) :
			fieldGetter( FieldGetter<T, C>( ptr ) ),
			typeHash( GetTypeHash<T>() ),
			isDerivedFromSerializable( IsBaseOf<Serializable, T>::Value ) {}

		template<typename T, typename C>
		T& GetValue( C& owner ) const {
			return fieldGetter.GetValue<T, C>( owner );
		}

		uint32_t GetStoredTypeHash() const {
			return typeHash;
		}

		bool IsDerivedFromSerializable() const {
			return isDerivedFromSerializable;
		}

	private:

		BaseFieldGetter fieldGetter;
		uint32_t typeHash;
		bool isDerivedFromSerializable;
	};
}

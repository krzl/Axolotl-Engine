#pragma once

#include "SerializationInfo.h"

namespace axlt {
	template<typename C>
	class SerializationInfoBuilder {

	public:

		explicit SerializationInfoBuilder( String&& name ) :
			name( Move( name ) ) {}

		template<typename T>
		SerializationInfoBuilder<C>& AddField( String name, T C::* ptr ) {
			const FieldInfo fieldInfo( ptr );
			fields.Add( Move( name ), fieldInfo );
			return *this;
		}

		SerializationInfo Build() {
			return SerializationInfo( Move( name ), Move( fields ) );
		}

	private:

		String name;
		Map<String, FieldInfo> fields;
	};
}
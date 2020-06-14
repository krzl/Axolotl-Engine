#pragma once
#include "Collections/Map.h"
#include "FieldInfo.h"

namespace axlt {
	class SerializationInfo {

		template<typename C>
		friend class SerializationInfoBuilder;

	public:

		SerializationInfo() = default;

		const FieldInfo* GetField( const String& name ) const {
			return fields.Find( name );
		}

		const Map<String, FieldInfo>& GetAllFields() const {
			return fields;
		}

		const String& GetName() const {
			return name;
		}

	private:
		
		explicit SerializationInfo( String&& name, Map<String, FieldInfo>&& fields ) :
			name( Move( name ) ),
			fields( Move( fields ) ) {}

		String name;
		Map<String, FieldInfo> fields;
	};
}

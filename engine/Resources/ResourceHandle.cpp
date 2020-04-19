// ReSharper disable CppClangTidyBugproneMacroParentheses
#include "stdafx.h"
#include "ResourceHandle.h"

#include "BinaryResource.h"
#include "ModelResource.h"
#include "Graphics/TechniqueResource.h"
#include "Graphics/MaterialResource.h"

namespace axlt::resourceHandleInner {
	void SerializeResource( Serializer& serializer, const uint32_t type, void* data ) {
		if( data == nullptr ) return;
		//TODO: Cache type hashes
	#define RESOURCE_TYPE( Type )						\
			if( GetHash( String( #Type ) ) == type ) {	\
				serializer << *(Type*)data;				\
				return;									\
			}

	#include "ResourceTypes.inl"
	}

	void* DeserializeResource( Serializer& serializer, const uint32_t type ) {
		//TODO: Cache type hashes
	#define RESOURCE_TYPE( Type )						\
			if( GetHash( String( #Type ) ) == type ) {	\
				Type* ptr = new Type();					\
				serializer >> *ptr;						\
				return (void*) ptr;						\
			}

	#include "ResourceTypes.inl"

		return nullptr;
	}
}
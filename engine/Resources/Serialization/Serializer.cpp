#include "stdafx.h"
#include "Serializer.h"

#include "FileSystem/File.h"
#include "FieldInfo.h"
#include "Collections/Map.h"
#include "Serializable.h"

namespace axlt {

	Serializer::Serializer( const char* filePath, const char* accessFlags ) : fp( nullptr ) {
		fopen_s( &fp, filePath, accessFlags );
	}

	Serializer::Serializer( const File& file, const char* accessFlags ) : fp( nullptr ) {
		fopen_s( &fp, file.AbsolutePath().GetData(), accessFlags );
	}

	void WriteSerializable( Serializer& s, Serializable& serializable ) {
		const Map<String, FieldInfo>& serializationFields = serializable.GetSerializationData().GetAllFields();
		for (const auto& kvp : serializationFields) {
			const FieldInfo& fieldInfo = kvp.value;
			fieldInfo.Serialize( s, serializable );
		}
	}

	void ReadSerializable( Serializer& s, Serializable& serializable ) {
		const Map<String, FieldInfo>& serializationFields = serializable.GetSerializationData().GetAllFields();
		for (const auto& kvp : serializationFields) {
			const FieldInfo& fieldInfo = kvp.value;
			fieldInfo.Deserialize( s, serializable );
		}
		serializable.OnPostDeserialization();
	}

	// ReSharper disable once CppMemberFunctionMayBeConst
	void Serializer::operator<<( SerializerEnd end ) {
		fclose( fp );
	}

	// ReSharper disable once CppMemberFunctionMayBeConst
	void Serializer::operator>>( SerializerEnd end ) {
		fclose( fp );
	}
}

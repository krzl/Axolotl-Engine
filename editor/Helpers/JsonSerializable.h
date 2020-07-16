#pragma once

#include <rapidjson/document.h>
#include <Resources/Serialization/Serializable.h>

namespace axlt {
	class Guid;
}

namespace axlt::editor {
	namespace Hidden_createSerializable {
		void FillSerializableFromJson( Serializable& serializable, rapidjson::Value& json );
		void FillSerializableFromJsonWithGuid( Serializable& serializable, rapidjson::Value& json, Guid& guid );
		rapidjson::Value& SaveSerializable( Serializable& serializable, rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator );;
	}
	
	template<typename T>
	T CreateSerializableFromJson( rapidjson::Document& json ) {
		T serializable;
		Hidden_createSerializable::FillSerializableFromJson( serializable, json );
		return serializable;
	}

	template<typename T>
	T CreateSerializableFromJsonWithGuid( rapidjson::Document& json, Guid& guid ) {
		T serializable;
		Hidden_createSerializable::FillSerializableFromJsonWithGuid( serializable, json, guid );
		return serializable;
	}

	void SaveSerializableToJson( Serializable& serializable, File& file );
	void SaveSerializableToJsonWithGuid( Serializable& serializable, File& file, const Guid& guid );
}

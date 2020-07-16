#include "stdafx.h"
#include "JsonSerializable.h"

#include <FileSystem/File.h>
#include <FileSystem/Guid.h>

namespace axlt::editor::Hidden_createSerializable {

	typedef void (*SetValueFunction)(Serializable&, const FieldInfo&, rapidjson::Value&);

	void SetStringValue( Serializable& owner, const FieldInfo& fieldInfo, rapidjson::Value& value ) {
		fieldInfo.GetValue<String, Serializable>( owner ) = value.GetString();
	}

	template<typename T>
	void SetSignedIntegerValue( Serializable& owner, const FieldInfo& fieldInfo, rapidjson::Value& value ) {
		fieldInfo.GetValue<T, Serializable>( owner ) = (T) value.GetInt64();
	}

	template<typename T>
	void SetUnsignedIntegerValue( Serializable& owner, const FieldInfo& fieldInfo, rapidjson::Value& value ) {
		fieldInfo.GetValue<T, Serializable>( owner ) = (T) value.GetUint64();
	}

	template<typename T>
	void SetDecimalValue( Serializable& owner, const FieldInfo& fieldInfo, rapidjson::Value& value ) {
		fieldInfo.GetValue<T, Serializable>( owner ) = (T) value.GetDouble();
	}

	template<typename T, int Size>
	void SetVectorValue( Serializable& owner, const FieldInfo& fieldInfo, rapidjson::Value& value ) {
		T val = fieldInfo.GetValue<T, Serializable>( owner );
		float* xPtr = &val.x;
		const auto valueArray = value.GetArray();
		for( uint32_t i = 0; i < Size; i++ ) {
			xPtr[i] = valueArray[i].GetFloat();
		}
	}

	template<typename T, int Size>
	void SetVectorIntValue( Serializable& owner, const FieldInfo& fieldInfo, rapidjson::Value& value ) {
		T val = fieldInfo.GetValue<T, Serializable>( owner );
		int32_t* xPtr = &val.x;
		const auto valueArray = value.GetArray();
		for (uint32_t i = 0; i < Size; i++) {
			xPtr[ i ] = valueArray[ i ].GetInt();
		}
	}

	void SetStringArrayValue( Serializable& owner, const FieldInfo& fieldInfo, rapidjson::Value& value ) {
		Array<String>& val = fieldInfo.GetValue<Array<String>, Serializable>( owner );
		for( auto& arrayElement : value.GetArray() ) {
			val.Add( arrayElement.GetString() );
		}
	}

	Map<uint32_t, SetValueFunction> CreateSetValueFunctionMap() {
		Map<uint32_t, SetValueFunction> map;
		map.Add( GetTypeHash<String>(), SetStringValue );
		map.Add( GetTypeHash<uint8_t>(), SetUnsignedIntegerValue<uint8_t> );
		map.Add( GetTypeHash<uint16_t>(), SetUnsignedIntegerValue<uint16_t> );
		map.Add( GetTypeHash<uint32_t>(), SetUnsignedIntegerValue<uint32_t> );
		map.Add( GetTypeHash<uint64_t>(), SetUnsignedIntegerValue<uint64_t> );
		map.Add( GetTypeHash<int8_t>(), SetSignedIntegerValue<int8_t> );
		map.Add( GetTypeHash<int16_t>(), SetSignedIntegerValue<int16_t> );
		map.Add( GetTypeHash<int32_t>(), SetSignedIntegerValue<int32_t> );
		map.Add( GetTypeHash<int64_t>(), SetSignedIntegerValue<int64_t> );
		map.Add( GetTypeHash<float>(), SetDecimalValue<float> );
		map.Add( GetTypeHash<double>(), SetDecimalValue<double> );
		map.Add( GetTypeHash<Vector2>(), SetVectorValue<Vector2, 4> );
		map.Add( GetTypeHash<Vector3>(), SetVectorValue<Vector3, 4> );
		map.Add( GetTypeHash<Vector4>(), SetVectorValue<Vector4, 4> );
		map.Add( GetTypeHash<Vector2Int>(), SetVectorIntValue<Vector2Int, 2> );
		map.Add( GetTypeHash<Quaternion>(), SetVectorValue<Quaternion, 4> );
		map.Add( GetTypeHash<Array<String>>(), SetStringArrayValue ); //TODO: Add other array types

		return map;
	}

	void TrySetValue( Serializable& serializable, const FieldInfo& fieldInfo, rapidjson::Value& value ) {
		static Map<uint32_t, SetValueFunction> functionMap = CreateSetValueFunctionMap();

		if (fieldInfo.IsDerivedFromSerializable()) {
			FillSerializableFromJson( fieldInfo.GetValue<Serializable, Serializable>( serializable ), value );
		} else {
			SetValueFunction* funcPtr = functionMap.Find( fieldInfo.GetStoredTypeHash() );
			if (funcPtr != nullptr) {
				(*funcPtr)( serializable, fieldInfo, value );
			}
		}
	}
	
	void FillSerializableFromJson( Serializable& serializable, rapidjson::Value& json ) {
		for( const Pair<String, FieldInfo>& fieldPair : serializable.GetSerializationData().GetAllFields() ) {
			if( json.HasMember( fieldPair.key.GetData() ) ) {
				TrySetValue( serializable, fieldPair.value, json[ fieldPair.key.GetData() ] );
			}
		}
	}

	void FillSerializableFromJsonWithGuid( Serializable& serializable, rapidjson::Value& json, Guid& guid ) {
		if( json.HasMember( "guid" ) ) {
			guid = Guid::FromString( json[ "guid" ].GetString() );
		}
		for( const Pair<String, FieldInfo>& fieldPair : serializable.GetSerializationData().GetAllFields() ) {
			if( json.HasMember( fieldPair.key.GetData() ) ) {
				TrySetValue( serializable, fieldPair.value, json[ fieldPair.key.GetData() ] );
			}
		}
	}
}

namespace axlt::editor {

	typedef void( *SaveValueFunction )( Serializable&, const FieldInfo&, rapidjson::Value& );

	rapidjson::Document::AllocatorType* allocator;

	namespace Hidden_createSerializable {
		void SaveStringValue( Serializable& owner, const FieldInfo& fieldInfo, rapidjson::Value& value ) {
			const String& str = fieldInfo.GetValue<String, Serializable>( owner );
			value.SetString( str.GetData(), str.Length() );
		}

		template<typename T>
		void SaveSignedIntegerValue( Serializable& owner, const FieldInfo& fieldInfo, rapidjson::Value& value ) {
			value.SetInt64( fieldInfo.GetValue<T, Serializable>( owner ) );
		}

		template<typename T>
		void SaveUnsignedIntegerValue( Serializable& owner, const FieldInfo& fieldInfo, rapidjson::Value& value ) {
			value.SetUint64( fieldInfo.GetValue<T, Serializable>( owner ) );
		}

		template<typename T>
		void SaveDecimalValue( Serializable& owner, const FieldInfo& fieldInfo, rapidjson::Value& value ) {
			value.SetDouble( fieldInfo.GetValue<T, Serializable>( owner ) );
		}

		template<typename T, int Size>
		void SaveVectorValue( Serializable& owner, const FieldInfo& fieldInfo, rapidjson::Value& value ) {
			T val = fieldInfo.GetValue<T, Serializable>( owner );
			float* xPtr = &val.x;
			const auto valueArray = value.GetArray();
			for (uint32_t i = 0; i < Size; i++) {
				valueArray[ i ].SetFloat( xPtr[ i ] );
			}
		}

		template<typename T, int Size>
		void SaveVectorIntValue( Serializable& owner, const FieldInfo& fieldInfo, rapidjson::Value& value ) {
			T val = fieldInfo.GetValue<T, Serializable>( owner );
			int32_t* xPtr = &val.x;
			const auto valueArray = value.GetArray();
			for (uint32_t i = 0; i < Size; i++) {
				valueArray[ i ].SetInt( xPtr[ i ] );
			}
		}

		void SaveStringArrayValue( Serializable& owner, const FieldInfo& fieldInfo, rapidjson::Value& value ) {
			const Array<String>& val = fieldInfo.GetValue<Array<String>, Serializable>( owner );
			rapidjson::Value& array = value.SetArray();
			for( const String& string : val ) {
				rapidjson::Value arrayElement( string.GetData(), string.Length(), *allocator );
				array.PushBack( arrayElement, *allocator );
			}
		}

		Map<uint32_t, SaveValueFunction> CreateSaveValueFunctionMap() {
			Map<uint32_t, SaveValueFunction> map;
			map.Add( GetTypeHash<String>(), SaveStringValue );
			map.Add( GetTypeHash<uint8_t>(), SaveUnsignedIntegerValue<uint8_t> );
			map.Add( GetTypeHash<uint16_t>(), SaveUnsignedIntegerValue<uint16_t> );
			map.Add( GetTypeHash<uint32_t>(), SaveUnsignedIntegerValue<uint32_t> );
			map.Add( GetTypeHash<uint64_t>(), SaveUnsignedIntegerValue<uint64_t> );
			map.Add( GetTypeHash<int8_t>(), SaveSignedIntegerValue<int8_t> );
			map.Add( GetTypeHash<int16_t>(), SaveSignedIntegerValue<int16_t> );
			map.Add( GetTypeHash<int32_t>(), SaveSignedIntegerValue<int32_t> );
			map.Add( GetTypeHash<int64_t>(), SaveSignedIntegerValue<int64_t> );
			map.Add( GetTypeHash<float>(), SaveDecimalValue<float> );
			map.Add( GetTypeHash<double>(), SaveDecimalValue<double> );
			map.Add( GetTypeHash<Vector2>(), SaveVectorValue<Vector2, 4> );
			map.Add( GetTypeHash<Vector3>(), SaveVectorValue<Vector3, 4> );
			map.Add( GetTypeHash<Vector4>(), SaveVectorValue<Vector4, 4> );
			map.Add( GetTypeHash<Vector2Int>(), SaveVectorIntValue<Vector2Int, 2> );
			map.Add( GetTypeHash<Quaternion>(), SaveVectorValue<Quaternion, 4> );
			map.Add( GetTypeHash<Array<String>>(), SaveStringArrayValue ); //TODO: Add other array types

			return map;
		}

		void TrySaveValue( Serializable& serializable, const FieldInfo& fieldInfo, rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator ) {
			static Map<uint32_t, SaveValueFunction> functionMap = CreateSaveValueFunctionMap();

			if( fieldInfo.IsDerivedFromSerializable() ) {
				SaveSerializable( fieldInfo.GetValue<Serializable, Serializable>( serializable ), value, allocator );
			} else {
				SaveValueFunction* funcPtr = functionMap.Find( fieldInfo.GetStoredTypeHash() );
				if( funcPtr != nullptr ) {
					return (*funcPtr)( serializable, fieldInfo, value );
				}
			}
		}

		rapidjson::Value& SaveSerializable( Serializable& serializable, rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator ) {
			for( const Pair<String, FieldInfo>& fieldPair : serializable.GetSerializationData().GetAllFields() ) {
				rapidjson::Value fieldValue;
				TrySaveValue( serializable, fieldPair.value, fieldValue, allocator );
				rapidjson::Value name( fieldPair.key.GetData(), fieldPair.key.Length(), allocator );
				value.AddMember( name, fieldValue, allocator );
			}
			return value;
		}
	}

	void SaveSerializableToJson( Serializable& serializable, File& file ) {
		rapidjson::Document json;
		json.SetObject();
		allocator = &json.GetAllocator();
		Hidden_createSerializable::SaveSerializable( serializable, json, json.GetAllocator() );
		file.FromJson( json );
		allocator = nullptr;
	}

	void SaveSerializableToJsonWithGuid( Serializable& serializable, File& file, const Guid& guid ) {
		rapidjson::Document json;
		json.SetObject();
		String guidString = guid.ToString();
		allocator = &json.GetAllocator();
		rapidjson::Value guidValue( guidString.GetData(), guidString.Length(), json.GetAllocator() );
		json.AddMember( "guid", guidValue, json.GetAllocator() );
		Hidden_createSerializable::SaveSerializable( serializable, json, json.GetAllocator() );
		file.FromJson( json );
		allocator = nullptr;
	}
}

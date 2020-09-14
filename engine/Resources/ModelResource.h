#pragma once
#include "Serialization/Serializer.h"
#include "Serialization/Serializable.h"
#include "Resource.h"
#include "Entities/ComponentHandle.h"

namespace axlt {
	
	class RendererComponent;

	enum class MeshChannels {
		NORMAL = 1,
		TANGENT = 2,
		BITANGENT = 4,
		COLOR = 8,
		UV0 = 16,
		UV1 = 32,
		UV2 = 64,
		UV3 = 128
	};

#define MAX_COLOR_CHANNELS 1
#define MAX_UV_CHANNELS 4

	namespace vk {
		class MeshBufferBinding;
		class ModelBufferBinding;
		
		void UpdateBindings( ComponentHandle<RendererComponent>& renderer );
	}

	class ModelResource;
	
	namespace editor {
		class ModelImport;
		ModelResource* FbxImporter( const File&, editor::ModelImport&, Array<Guid>& );
	}

	/*
	 * MeshLayout Structure Info
	 *
	 * Positions are always included in mesh, they aren't represented in MeshLayout
	 * 
	 * 0 bit		- does mesh contains normals
	 * 1 bit		- does mesh contains tangents
	 * 2 bit		- does mesh contains bitangents
	 * 3 bit		- does mesh contains color channel
	 * 4-6 bits		- amount of texCoord0 channels in mesh
	 * 7-9 bits		- amount of texCoord1 channels in mesh
	 * 10-12 bits	- amount of texCoord2 channels in mesh
	 * 13-15 bits	- amount of texCoord3 channels in mesh
	 *
	 */

	class Mesh final : public Serializable {

		friend class vk::MeshBufferBinding;
		friend class vk::ModelBufferBinding;
		
		friend ModelResource* editor::FbxImporter( const File&, editor::ModelImport&, Array<Guid>& );
		
	public:

		explicit Mesh( uint16_t meshLayout = 0 );
		
		const SerializationInfo& GetSerializationData() const override;
		uint32_t GetTypeHash() const override;

		uint16_t GetMeshLayout() const;

		static uint16_t GetAttributeByteSize( uint16_t index );
		static uint8_t GetAttributeVectorSize( uint8_t location, uint16_t meshLayout );
		static uint16_t GetAttributeOffset( uint8_t location, uint16_t meshLayout, uint8_t startingLocation = 0 );
		static uint16_t CreateMeshLayoutMask( uint8_t location, uint8_t vectorSize );
		static uint16_t GetMeshStride( uint16_t meshLayout );
		
		Array<uint32_t, ExactHeapArrayAllocator> indices;

		Array<Vector3, ExactHeapArrayAllocator> vertices;
		Array<Vector3, ExactHeapArrayAllocator> normals;
		Array<Vector3, ExactHeapArrayAllocator> tangents;
		Array<Vector3, ExactHeapArrayAllocator> bitangents;
		Array<Color, ExactHeapArrayAllocator> colorChannels[MAX_COLOR_CHANNELS];
		Array<float, ExactHeapArrayAllocator> texCoordChannels[MAX_UV_CHANNELS];

		Array<float, ExactHeapArrayAllocator> vertexData;

		uint8_t usedMeshChannels = 0;
		uint8_t uvChannelsSize = 0;

	private:
		
		uint16_t meshLayout = 0;
	};
	
	class ModelResource final : public Resource {

		friend class vk::MeshBufferBinding;
		friend class vk::ModelBufferBinding;
		friend void vk::UpdateBindings( ComponentHandle<RendererComponent>& );

	public:

		ModelResource() = default;
		explicit ModelResource( bool isDynamic );

		void Flush();

		const SerializationInfo& GetSerializationData() const override;
		uint32_t GetTypeHash() const override;

		Array<Mesh, ExactHeapArrayAllocator> meshes;

		bool storeMeshData = false;
		
	private:

		bool isDynamic = false;
		bool isDirty = true;
	};
}

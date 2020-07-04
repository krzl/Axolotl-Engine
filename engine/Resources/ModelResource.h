#pragma once
#include "Serialization/Serializer.h"
#include "Serialization/Serializable.h"
#include "Resource.h"

namespace axlt {

#define MAX_COLOR_CHANNELS 1
#define MAX_UV_CHANNELS 4

	class Mesh final : public Serializable {

	public:
		
		const SerializationInfo& GetSerializationData() const override;
		uint32_t GetTypeHash() const override;

		Array<uint32_t, ExactHeapArrayAllocator> indices;
		Array<Vector3, ExactHeapArrayAllocator> vertices;
		Array<Vector3, ExactHeapArrayAllocator> normals;
		Array<Vector3, ExactHeapArrayAllocator> tangents;
		Array<Vector3, ExactHeapArrayAllocator> bitangents;
		Array<Color, ExactHeapArrayAllocator> colorChannels[MAX_COLOR_CHANNELS];
		Array<float, ExactHeapArrayAllocator> texCoordChannels[MAX_UV_CHANNELS];
	};

	namespace vk {
		void BindResources();
	}
	
	class ModelResource final : public Resource {

		friend void vk::BindResources();

	public:

		Array<Mesh, ExactHeapArrayAllocator> meshes;

		void Flush();

		const SerializationInfo& GetSerializationData() const override;
		uint32_t GetTypeHash() const override;

	private:

		bool isDirty = true;
	};
}

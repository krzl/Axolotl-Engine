#pragma once
#include "Resources/ModelResource.h"
#include "Entities/Component.h"
#include "Resources/ResourceHandle.h"
#include "MaterialResource.h"

namespace axlt {
	class RendererComponent final : public BaseComponent<RendererComponent> {

	public:

		const SerializationInfo& GetSerializationData() const override {
			static SerializationInfo serializationInfo = SerializationInfoBuilder<RendererComponent>( "RendererComponent" )
				.AddField( "model", &RendererComponent::model )
				.AddField( "material", &RendererComponent::material )
				.Build();
			return serializationInfo;
		}

		ResourceHandle<ModelResource> model;
		ResourceHandle<MaterialResource> material;
	};
}

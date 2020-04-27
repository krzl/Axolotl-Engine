#pragma once
#include "Entities/System.h"
#include "RendererComponent.h"
#include "Entities/TransformComponent.h"

namespace axlt {
	class RenderSystem final : public System<TransformComponent, RendererComponent> {

		friend void vk::Update();

		void Update( Tuple<ComponentHandle<TransformComponent>, ComponentHandle<RendererComponent>> tuple ) override {

			auto&[transform, renderer] = tuple;

			if( renderer->model.guid != Guid::invalidGuid ) {
				const Matrix4& worldMatrix = transform->LocalToWorldMatrix();
			}
		}
	};
	DEFINE_SYSTEM( RenderSystem )
}

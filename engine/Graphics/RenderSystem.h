#pragma once
#include "Entities/System.h"
#include "RendererComponent.h"
#include "Entities/TransformComponent.h"

namespace axlt {
	class RenderSystem final : public System<TransformComponent, RendererComponent> {

		friend void vk::BindResources();
		friend void vk::Draw();

		void Update( Tuple<ComponentHandle<TransformComponent>, ComponentHandle<RendererComponent>> tuple ) override {

			auto&[transform, renderer] = tuple;

			if( renderer->model.GetData() != nullptr ) {
				const Matrix4& worldMatrix = transform->LocalToWorldMatrix();
			}
		}
	};
	DEFINE_SYSTEM( RenderSystem )
}

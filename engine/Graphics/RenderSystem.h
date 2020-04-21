#pragma once
#include "Entities/System.h"
#include "RendererComponent.h"
#include "Entities/TransformComponent.h"

namespace axlt {
	class RenderSystem final : public System<TransformComponent, RendererComponent> {

		void Update( Tuple<ComponentHandle<TransformComponent>, ComponentHandle<RendererComponent>> tuple ) override {

			auto&[transform, renderer] = tuple;

			if( renderer->model != nullptr ) {
				const Matrix4& worldMatrix = transform->LocalToWorldMatrix();

				// renderer->model.Draw( worldMatrix );
			}
		}

	};
	DEFINE_SYSTEM( RenderSystem )
}

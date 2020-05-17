#include "stdafx.h"
#include "RenderSystem.h"

namespace axlt {

	DEFINE_SYSTEM( RenderSystem, 0 )
	
	void RenderSystem::Update( Tuple<ComponentHandle<TransformComponent>, ComponentHandle<RendererComponent>> tuple ) {

		auto&[ transform, renderer ] = tuple;

		if (renderer->model.GetData() != nullptr) {
			const Matrix4& worldMatrix = transform->LocalToWorldMatrix();
		}
	}

	Map<Entity*, Tuple<ComponentHandle<TransformComponent>, ComponentHandle<RendererComponent>>>& RenderSystem::GetTuples() {
		return m_componentTuples;
	};

	RenderSystem* RenderSystem::GetSystem() {
		return g_RenderSystem_instance;
	}
}

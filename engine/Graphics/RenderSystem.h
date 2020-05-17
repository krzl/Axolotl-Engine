#pragma once
#include "Entities/System.h"
#include "RendererComponent.h"
#include "Entities/TransformComponent.h"

namespace axlt {
	class RenderSystem final : public System<TransformComponent, RendererComponent> {

		void Update( Tuple<ComponentHandle<TransformComponent>, ComponentHandle<RendererComponent>> tuple ) override;

	public:

		Map<Entity*, Tuple<ComponentHandle<TransformComponent>, ComponentHandle<RendererComponent>>>& GetTuples();
		static RenderSystem* GetSystem();
	};
}

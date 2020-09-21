#include <Game.h>
#include <Entities/Entity.h>
#include <Entities/TransformComponent.h>
#include "SceneViewComponent.h"
#include <Graphics/RendererComponent.h>
#include "EditorFileManager.h"
#include "Import/ImportManager.h"

using namespace axlt;
using namespace axlt::editor;

int main() {
	EditorFileManager::Init();
	ImportManager::ImportAll();
	
	//ComponentHandle<RendererComponent> renderer = child->AddComponent<RendererComponent>();
	//renderer->material = ResourceHandle<MaterialResource>::Load( Guid::FromString( "0d97a3502e2b09a41b37ea756b27fce0" ) );
	//renderer->model = ResourceHandle<ModelResource>::Load( Guid::FromString( "659b81fdd681befc2bb4eec221253744" ) );

	GameInstance.Run();
}

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

	Entity* rooot = new Entity( "Root" );
	Entity* child = new Entity( "Child" );

	ComponentHandle<TransformComponent> rooot_t = rooot->AddComponent<TransformComponent>();
	ComponentHandle<TransformComponent> child_t = child->AddComponent<TransformComponent>();

	child_t->SetParent( rooot_t );
	child_t->SetLocalPosition( Vector3( 0.0f, 10.0f, 0.0f ) );

	ComponentHandle<SceneViewComponent> camera = rooot->AddComponent<SceneViewComponent>();
	
	//ComponentHandle<RendererComponent> renderer = child->AddComponent<RendererComponent>();
	//renderer->material = ResourceHandle<MaterialResource>::Load( Guid::FromString( "5ff581b52472a6c2b71b8d6f868206e0" ) );
	//renderer->model = ResourceHandle<ModelResource>::Load( Guid::FromString( "c6346a51f768aa43903cf4058c8fde77" ) );

	GameInstance.Run();
}

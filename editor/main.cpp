#include <Game.h>
#include <Entities/Entity.h>
#include <Graphics/RendererComponent.h>
#include "EditorFileManager.h"
#include "Import/ImportManager.h"
#include <Entities/TransformComponent.h>

using namespace axlt;
using namespace axlt::editor;

int main() {
	EditorFileManager::Init();
	ImportManager::ImportAll();

	Entity* rendererEntity = new Entity( "3D Model" );

	ComponentHandle<TransformComponent> transform = rendererEntity->AddComponent<TransformComponent>();
	transform->SetPosition( Vector3( 0.0f, 0.0f, -0.8f ) );
	transform->SetRotation( Quaternion( 90.0f, 0.0f, 0.0f ) );
	transform->SetScale( Vector3( 0.2f, 0.2f, 0.2f ) );
	
	ComponentHandle<RendererComponent> renderer = rendererEntity->AddComponent<RendererComponent>();
	renderer->material = ResourceHandle<MaterialResource>::Load( Guid::FromString( "0d97a3502e2b09a41b37ea756b27fce0" ) );
	renderer->model = ResourceHandle<ModelResource>::Load( Guid::FromString( "0c5605029ff861509368dc0472452b48" ) );

	GameInstance.Run();
}

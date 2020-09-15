#include "AxolotlEngine.h"

#include <Game.h>
#include <Entities/Entity.h>
#include <FileSystem/FileSystem.h>
#include <Entities/TransformComponent.h>
#include <Resources/ResourceHandle.h>
#include <Resources/ModelResource.h>
#include <Graphics/MaterialResource.h>
#include <Graphics/RendererComponent.h>

#include "SceneViewComponent.h"
#include "../editor/Import/ImportManager.h"
#include "../editor/EditorFileManager.h"

using namespace axlt;

int main() {
	FileSystem fileSystem( "../ImportedFiles" );
	GameInstance.SetImportFileSystem( fileSystem );
	
	Quaternion q( 20.0f, 40.0f, 80.0f );

	Entity* rooot = new Entity();
	Entity* child = new Entity();
	
	ComponentHandle<TransformComponent> rooot_t = rooot->AddComponent<TransformComponent>();
	ComponentHandle<TransformComponent> child_t = child->AddComponent<TransformComponent>();

	child_t->SetLocalPosition( Vector3( 0.0f, 10.0f, 0.0f ) );

	ComponentHandle<editor::SceneViewComponent> camera = rooot->AddComponent<editor::SceneViewComponent>();
	ComponentHandle<RendererComponent> renderer = child->AddComponent<RendererComponent>();

	Vector3 test = child_t->GetLocalPosition();
	
	renderer->material = ResourceHandle<MaterialResource>::Load( Guid::FromString( "0d97a3502e2b09a41b37ea756b27fce0" ) );
	renderer->model = ResourceHandle<ModelResource>::Load( Guid::FromString( "659b81fdd681befc2bb4eec221253744" ) );
	
	GameInstance.Run();
}
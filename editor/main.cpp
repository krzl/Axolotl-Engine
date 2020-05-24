#include "ResourceDatabase.h"
#include "Globals.h"

#include <Game.h>
#include <Entities/Entity.h>
#include <Entities/TransformComponent.h>
#include "../test_env/SceneViewComponent.h"
#include <Graphics/RendererComponent.h>

using namespace axlt;

int main() {
	axlt::editor::resourceDatabase.ImportAll();

	Quaternion q( 20.0f, 40.0f, 80.0f );

	Entity* rooot = new Entity( "Root" );
	Entity* child = new Entity( "Child" );

	ComponentHandle<TransformComponent> rooot_t = rooot->AddComponent<TransformComponent>();
	ComponentHandle<TransformComponent> child_t = child->AddComponent<TransformComponent>();

	child_t->SetParent( rooot_t );

	child_t->SetLocalPosition( Vector3( 0.0f, 10.0f, 0.0f ) );

	ComponentHandle<editor::SceneViewComponent> camera = rooot->AddComponent<editor::SceneViewComponent>();
	//ComponentHandle<RendererComponent> renderer = child->AddComponent<RendererComponent>();

	Vector3 test = child_t->GetLocalPosition();

	FileSystem fileSystem( "../ImportedFiles" );
	g_importFilesystem = &fileSystem;

	//renderer->material = ResourceHandle<MaterialResource>::Load( Guid::FromString( "5ff581b52472a6c2b71b8d6f868206e0" ) );
	//renderer->model = ResourceHandle<ModelResource>::Load( Guid::FromString( "c6346a51f768aa43903cf4058c8fde77" ) );
	
	axlt::GameInstance.Run();
}

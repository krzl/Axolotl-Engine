#include "AxolotlEngine.h"

#include <Game.h>
#include <Entities/Entity.h>
#include <Entities/System.h>
#include <FileSystem/FileSystem.h>
#include <Entities/TransformComponent.h>
#include <Resources/ResourceHandle.h>
#include <Resources/ModelResource.h>
#include <Graphics/MaterialResource.h>

using namespace axlt;

int main() {

	Game game = axlt::GameInstance;
	
	Entity* rooot = new Entity();
	Entity* child = new Entity();
	
	ComponentHandle<TransformComponent> rooot_t = rooot->AddComponent<TransformComponent>();
	ComponentHandle<TransformComponent> child_t = child->AddComponent<TransformComponent>();

	rooot_t->SetLocalPosition( Vector3( 1.0f, 0.0f, 0.0f ) );
	rooot_t->SetLocalRotation( Quaternion( 0.70710678118f, 0.0f, 0.0f, 0.70710678118f ) );
	child_t->SetLocalPosition( Vector3( 0.0f, 1.0f, 0.0f ) );
	child_t->SetParent( rooot_t );

	Vector3 test = child_t->GetLocalPosition();

	FileSystem fileSystem( "../ImportedFiles" );
	g_importFilesystem = &fileSystem;
	
	ResourceHandle<MaterialResource> resource = ResourceHandle<MaterialResource>::Load( "5ff581b52472a6c2b71b8d6f868206e0" );

	fileSystem.PrintContent();

	game.Run();
}
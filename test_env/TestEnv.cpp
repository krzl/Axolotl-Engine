#include "AxolotlEngine.h"

#include <Game.h>
#include <Entities/Entity.h>
#include <Entities/System.h>
#include <Entities/Component.h>
#include <FileSystem/FileSystem.h>
#include <Entities/TransformComponent.h>

using namespace axlt;

int main() {

	axlt::Game game = axlt::GameInstance;
	
	axlt::Entity* rooot = new Entity();
	axlt::Entity* child = new Entity();
	
	ComponentHandle<TransformComponent> rooot_t = rooot->AddComponent<TransformComponent>();
	ComponentHandle<TransformComponent> child_t = child->AddComponent<TransformComponent>();

	rooot_t->SetLocalPosition( Vector3( 1.0f, 0.0f, 0.0f ) );
	rooot_t->SetLocalRotation( Quaternion( 0.70710678118f, 0.0f, 0.0f, 0.70710678118f ) );
	child_t->SetLocalPosition( Vector3( 0.0f, 1.0f, 0.0f ) );
	child_t->SetParent( rooot_t );

	Vector3 test = child_t->GetPosition();

	const axlt::FileSystem fileSystem( "Test" );

	fileSystem.PrintContent();

	game.Run();
}
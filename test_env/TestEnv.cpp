#include "AxolotlEngine.h"

#include <Game.h>
#include <Entities/Entity.h>
#include <Entities/System.h>
#include <Entities/Component.h>
#include <FileSystem/FileSystem.h>

using namespace axlt;

class ExampleComponent {
	DEFINE_COMPONENT( ExampleComponent )

public:

	ExampleComponent( int testValue ) : testValue( testValue ) {}

	int testValue;
};

class Example2Component {
	DEFINE_COMPONENT( Example2Component )

public:

	Example2Component( int testValue ) : testValue( testValue ) {}

	int testValue;
};

class ExampleSystem : public System<ExampleComponent> {


	void Update( Tuple<ComponentHandle<ExampleComponent>> tuple ) override {

		auto& [exComp] = tuple;

		exComp->testValue++;

		printf("%d\n", exComp->testValue );
	}
};

DEFINE_SYSTEM(ExampleSystem)

class Example2System : public System<ExampleComponent, Example2Component> {

	void Update( Tuple<ComponentHandle<ExampleComponent>, ComponentHandle<Example2Component>> tuple ) override {

		auto& [ exComp, ex2Comp ] = tuple;

		exComp->testValue++;
		ex2Comp->testValue+=100;

		printf("%d %d\n", exComp->testValue, ex2Comp->testValue );
	}
};

DEFINE_SYSTEM(Example2System)


int main() {
	
	axlt::Game game = axlt::GameInstance;

	/*
	axlt::Entity* entity = new Entity();
	axlt::Entity* entity2 = new Entity();
	axlt::Entity* entity3 = new Entity();
	axlt::Entity* entity4 = new Entity();
	axlt::Entity* entity5 = new Entity();

	entity->AddComponent<ExampleComponent>( 32 );
	entity2->AddComponent<ExampleComponent>( 342 );
	entity3->AddComponent<ExampleComponent>( 65 );
	entity4->AddComponent<ExampleComponent>( 465394 );
	entity5->AddComponent<ExampleComponent>( 0 );

	entity3->AddComponent<Example2Component>( 1 );

	entity5->RemoveComponent<ExampleComponent>();
	*/
	const axlt::FileSystem fileSystem( "Test" );

	fileSystem.PrintContent();
	
	game.Run();
}
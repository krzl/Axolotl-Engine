#include "AxolotlEngine.h"

#include <Game.h>
#include <Entities/Entity.h>
#include <Entities/System.h>
#include <FileSystem/FileSystem.h>
#include <Entities/TransformComponent.h>
#include <Resources/ResourceHandle.h>
#include <Resources/ModelResource.h>
#include <Graphics/MaterialResource.h>
#include <Graphics/CameraComponent.h>
#include <Graphics/RendererComponent.h>



#include "SceneViewSystem.h"

using namespace axlt;

int main() {

	editor::SceneViewSystem sceneViewSystem;

	Quaternion q( 20.0f, 40.0f, 80.0f );

	Entity* rooot = new Entity();
	Entity* child = new Entity();
	
	ComponentHandle<TransformComponent> rooot_t = rooot->AddComponent<TransformComponent>();
	ComponentHandle<TransformComponent> child_t = child->AddComponent<TransformComponent>();

	child_t->SetLocalPosition( Vector3( 0.0f, 10.0f, 0.0f ) );

	ComponentHandle<editor::SceneViewComponent> camera = rooot->AddComponent<editor::SceneViewComponent>();
	ComponentHandle<RendererComponent> renderer = child->AddComponent<RendererComponent>();

	Vector3 test = child_t->GetLocalPosition();

	FileSystem fileSystem( "../ImportedFiles" );
	g_importFilesystem = &fileSystem;
	
	renderer->material = ResourceHandle<MaterialResource>::Load( "5ff581b52472a6c2b71b8d6f868206e0" );
	renderer->model = ResourceHandle<ModelResource>::Load( "c6346a51f768aa43903cf4058c8fde77" );
	
	GameInstance.Run();
}
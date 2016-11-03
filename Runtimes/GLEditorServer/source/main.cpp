// main.cpp

#include <iostream>
#include <Resource/Archives/JsonArchive.h>
#include <Engine/Scene.h>
#include <EditorServerSystem/EditorServerSystem.h>

int main()
{
	// Create a scene
	sge::Scene scene;
	sge::register_builtin_components(scene);

	// Load the scene
	sge::JsonArchive archive;
	archive.load("scene.json");
	archive.deserialize_root(scene);

	// Create an editor server
	sge::EditorServerSystem editorServer{ scene, 1995 };
	editorServer.serve();
}

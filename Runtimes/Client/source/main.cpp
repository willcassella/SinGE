// main.cpp

#include <iostream>
#include <Core/Interfaces/IToString.h>
#include <Engine/Scene.h>
#include <Engine/Components/Transform2D.h>
#include <Engine/Components/StaticMesh.h>

int main()
{
	singe::Scene scene;
	auto entity = scene.new_entity();
	scene.new_component(entity, singe::Transform2D());
	scene.new_component(entity, singe::StaticMesh());

	auto components = scene.enumerate_components<singe::Transform2D>();
	auto test = *components.begin();

	auto& sceneType = singe::get_type<singe::Scene>();
}

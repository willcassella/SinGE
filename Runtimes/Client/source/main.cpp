// main.cpp

#include <iostream>
#include <Core/Interfaces/IToString.h>
#include <Engine/Scene.h>
#include <Engine/Components/Transform2D.h>
#include <Engine/Components/Display/StaticMesh.h>

int main()
{
	sge::Scene scene;
	auto entity = scene.new_entity();
	scene.new_component(entity, sge::CTransform2D());
	scene.new_component(entity, sge::CStaticMesh());
}

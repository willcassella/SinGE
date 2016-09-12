// main.cpp

#include <iostream>
#include <Core/Interfaces/IToString.h>
#include <Engine/Scene.h>
#include <Engine/Components/Transform2D.h>
#include <Engine/Components/Display/StaticMesh.h>

struct System
{
	void func(
		sge::Scene& scene, 
		sge::EntityID entity,
		sge::comp::With<sge::CTransform2D> tranform, 
		sge::comp::Without<sge::CStaticMesh> mesh)
	{
		std::cout << "Entity " << entity << " meets the requirements." << std::endl;
	}
};

int main()
{
	sge::Scene scene;
	auto entity1 = scene.new_entity();
	auto entity2 = scene.new_entity();
	scene.new_component(entity1, sge::CTransform2D());
	scene.new_component(entity2, sge::CTransform2D());
	scene.new_component(entity2, sge::CStaticMesh());

	System s;
	scene.run_system(s, &System::func);
	std::cin.get();
}

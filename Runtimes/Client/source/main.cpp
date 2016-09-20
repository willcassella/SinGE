// main.cpp

#include <iostream>
#include <Core/Interfaces/IToString.h>
#include <Engine/Scene.h>
#include <Engine/Components/Transform3D.h>
#include <Engine/Components/Display/StaticMesh.h>

struct System
{
	void func(
		sge::Scene& scene,
		sge::EntityID entity,
		sge::comp::With<sge::CTransform3D> tranform,
		sge::comp::Without<sge::CStaticMesh> mesh)
	{
		std::cout << "Entity " << entity << " meets the requirements." << std::endl;
	}
};

void lookAtVec3(sge::Any vec)
{

}

void testReflection()
{
	sge::CTransform3D transform;
	sge::ComponentContext ctx{ sge::ComponentIdentity{ 5, sge::WORLD_ENTITY } };

	sge::get_type(transform).get_property("local_position", &transform, &ctx, lookAtVec3);
}

int main()
{
	sge::Scene scene;
	auto entity1 = scene.new_entity();
	auto entity2 = scene.new_entity();
	scene.new_component(entity1, sge::CTransform3D());
	scene.new_component(entity2, sge::CTransform3D());
	scene.new_component(entity2, sge::CStaticMesh());

	testReflection();

	System s;
	scene.run_system(s, &System::func);
	std::cin.get();
}

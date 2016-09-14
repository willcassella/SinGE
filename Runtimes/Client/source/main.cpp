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
	sge::ComponentContext ctx;
	ctx.entity = sge::WORLD_ENTITY;
	ctx.id = 5;
	ctx.scene = nullptr;

	sge::get_type(transform).properties.find("local_position")->second.getter(&transform, &ctx, lookAtVec3);
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

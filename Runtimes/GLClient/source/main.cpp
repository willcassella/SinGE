// main.cpp

#include <iostream>
#include <GLFW/glfw3.h>
#include <Engine/Scene.h>
#include <Engine/Components/CTransform3D.h>
#include <Engine/Components/Display/CCamera.h>
#include <Engine/Components/Display/CStaticMesh.h>
#include <GLRender/GLRenderSystem.h>

constexpr sge::uint32 window_width = 1280;
constexpr sge::uint32 window_height = 720;

int main()
{
	// Initialize GLFW3
	if (!glfwInit())
	{
		std::cerr << "GLCLient: Could not initialize GLFW3." << std::endl;
		return EXIT_FAILURE;
	}

	// Create a windowed mode window and its OpenGL context
	auto* window = glfwCreateWindow(window_width, window_height, "SinGE GLClient", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "GLClient: Could not create a window." << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	// Make the window's OpenGL context current
	glfwMakeContextCurrent(window);

	// Create a scene
	sge::Scene scene;
	sge::Frame initFrame{ scene, 0 };

	// Create a mesh object
	auto meshEnt = scene.new_entity();
	scene.new_component<sge::CTransform3D>(meshEnt);
	auto meshComp = scene.new_component<sge::CStaticMesh>(meshEnt);
	sge::CStaticMesh::set_mesh(meshComp, initFrame, "game_content/spawn_pad.wmesh");

	// Create a camera object
	auto cameraEnt = scene.new_entity();
	scene.new_component<sge::CPerspectiveCamera>(cameraEnt);
	auto cameraPos = scene.new_component<sge::CTransform3D>(cameraEnt);
	sge::CTransform3D::set_local_position(cameraPos, initFrame, sge::Vec3{ 0, 0, 0 });

	// Create a render system
	sge::GLRenderSystem renderSystem{ window_width, window_height };

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		// Draw the scene
		renderSystem.render_frame(initFrame);

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	glfwTerminate();
}
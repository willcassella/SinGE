// main.cpp

#include <iostream>
#include <GLFW/glfw3.h>
#include <Core/Math/Vec4.h>
#include <Resource/Archives/JsonArchive.h>
#include <Engine/Scene.h>
#include <Engine/Components/CTransform3D.h>
#include <Engine/Components/Display/CCamera.h>
#include <Engine/Components/Display/CStaticMesh.h>
#include <JavaScript/JavaScriptEngine.h>
#include <GLRender/GLRenderSystem.h>

constexpr sge::uint32 window_width = 1920;
constexpr sge::uint32 window_height = 1080;

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
	sge::register_builtin_components(scene);

	sge::JsonArchive archive;
	archive.from_file("scene.json");
	archive.deserialize_root(scene);

	// Create a render system
	sge::GLRenderSystem renderSystem{ window_width, window_height };

	// Create a JavaScript system
	sge::JavaScriptEngine jsEngine{ scene };
	jsEngine.register_type(sge::Vec2::type_info);
	jsEngine.register_type(sge::Vec3::type_info);
	jsEngine.register_type(sge::Vec4::type_info);
	jsEngine.register_type(sge::CTransform3D::type_info);
	jsEngine.register_type(sge::CPerspectiveCamera::type_info);
	jsEngine.register_type(sge::CStaticMesh::type_info);
	jsEngine.load_script("main.js");

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		// Draw the scene
		renderSystem.render_scene(scene);

		scene.process_entities_mut([](sge::ProcessingFrame&, sge::EntityId, sge::CTransform3D& transform, sge::CPerspectiveCamera&)
		{
			transform.set_local_position(transform.get_local_position() + sge::Vec3{ 0, 0, 0.01 });
		});

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	glfwTerminate();
}

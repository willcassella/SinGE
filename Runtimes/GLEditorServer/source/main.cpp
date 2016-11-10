// main.cpp

#include <iostream>
#include <GLFW/glfw3.h>
#include <Resource/Archives/JsonArchive.h>
#include <Engine/Scene.h>
#include <GLRender/GLRenderSystem.h>
#include <EditorServerSystem/EditorServerSystem.h>

constexpr sge::uint32 window_width = 1920;
constexpr sge::uint32 window_height = 1080;

int main()
{
	// Initialize GLFW3
	if (!glfwInit())
	{
		std::cerr << "GLEditorServer: Could not initialize GLFW3." << std::endl;
		return EXIT_FAILURE;
	}

	// Create a windowed mode window and its OpenGL context
	auto* window = glfwCreateWindow(window_width, window_height, "SinGE GLEditorServer", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "GLEditorServer: Could not create a window." << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	// Make the window's OpenGL context current
	glfwMakeContextCurrent(window);

	// Create a scene
	sge::Scene scene;
	sge::register_builtin_components(scene);

	// Load the scene
	sge::JsonArchive archive;
	archive.from_file("scene.json");
	archive.deserialize_root(scene);

	// Create a render system
	sge::GLRenderSystem renderSystem{ window_width, window_height };

	// Create an editor server
	sge::EditorServerSystem editorServer{ scene, 1995 };

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		// Draw the scene
		renderSystem.render_scene(scene);

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();

		// Run the server
		editorServer.serve(4);
	}

	glfwTerminate();
}

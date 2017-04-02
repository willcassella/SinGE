// main.cpp

#include <iostream>
#include <GLFW/glfw3.h>
#include <Core/Reflection/TypeDB.h>
#include <Core/Math/Quat.h>
#include <Core/Math/Vec3.h>
#include <Core/Interfaces/IFromArchive.h>
#include <Resource/Archives/JsonArchive.h>
#include <Resource/Resources/StaticMesh.h>
#include <Resource/Resources/Material.h>
#include <Resource/Resources/Texture.h>
#include <Engine/Scene.h>
#include <Engine/UpdatePipeline.h>
#include <GLRender/GLRenderSystem.h>
#include <GLRender/Config.h>
#include <GLWindow/GLEventWindow.h>
#include <EditorServerSystem/EditorServerSystem.h>

int main(int argc, char* argv[])
{
	// Make sure we have a config file
	assert(argc == 2);
	sge::JsonArchive config;
	auto loaded_config = config.from_file(argv[1]);
	assert(loaded_config);
	auto* config_reader = config.read_root();

	// Initialize GLFW3
	if (!glfwInit())
	{
		std::cerr << "GLEditorServer: Could not initialize GLFW3." << std::endl;
		return EXIT_FAILURE;
	}

	// Get the window width and height
	int window_width = 1920, window_height = 1080;
	config_reader->object_member("window_width", window_width);
	config_reader->object_member("window_height", window_height);

	// Create a windowed mode window and its OpenGL context
    auto* window = sge::create_sge_opengl_window("SinGE Editor Server", window_width, window_height);
	if (!window)
	{
		std::cerr << "GLEditorServer: Could not create a window." << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	// Make the window's OpenGL context current
	glfwMakeContextCurrent(window);

    // Create an event window
    sge::GLEventWindow event_window{ window };
    sge::InputBindings input_bindings;
    config_reader->object_member("input_bindings", input_bindings);
    event_window.set_bindings(std::move(input_bindings));

	// Create a type database
	sge::TypeDB type_db;
	type_db.new_type<sge::Angle>();
	type_db.new_type<sge::Vec3>();
	type_db.new_type<sge::Quat>();
	type_db.new_type<sge::StaticMesh>();
	type_db.new_type<sge::Material>();
	type_db.new_type<sge::Texture>();

	// Create a scene
	sge::Scene scene{ type_db };
	sge::register_builtin_components(scene);

	// Load the scene
	std::string scene_path;
	if (config_reader->object_member("scene", scene_path))
	{
		sge::JsonArchive scene_archive;
		scene_archive.from_file(scene_path.c_str());
		scene_archive.deserialize_root(scene);
	}

    // Create a pipeline
    sge::UpdatePipeline pipeline;

    // Register the input window
    event_window.register_pipeline(pipeline);

	// Create a render system
	sge::gl_render::Config render_config;
	render_config.viewport_width = window_width;
	render_config.viewport_height = window_height;

	if (!config_reader->object_member("gl_render", render_config))
	{
		assert(false /*Could not load render config from the config file.*/);
	}

	sge::gl_render::GLRenderSystem render_system{ render_config };
	render_system.pipeline_register(pipeline);

	// Create an editor server
	sge::EditorServerSystem editorServer{ 1995 };
	editorServer.set_serve_time(12);
    editorServer.register_pipeline(pipeline);

    // Load the pipeline config
    if (config_reader->pull_object_member("update_pipeline"))
    {
        pipeline.configure_pipeline(*config_reader);
        config_reader->pop();
    }
    else
    {
        assert(false /*Engine update pipeline not specified*/);
    }

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		// Update
		scene.update(pipeline, 0.016);

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	glfwTerminate();
}

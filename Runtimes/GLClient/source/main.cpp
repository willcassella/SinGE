// main.cpp

#include <iostream>
#include <GLFW/glfw3.h>
#include <Core/Math/Vec4.h>
#include <Core/Reflection/TypeDB.h>
#include <Resource/Archives/JsonArchive.h>
#include <Engine/Scene.h>
#include <Engine/SystemFrame.h>
#include <Engine/Components/CTransform3D.h>
#include <Engine/Components/Display/CCamera.h>
#include <Engine/Components/Display/CStaticMesh.h>
#include <JavaScript/JavaScriptEngine.h>
#include <GLRender/GLRenderSystem.h>
#include <GLRender/Config.h>

void mut_process_test(sge::SystemFrameMut& frame, float current_time, float dt)
{
	frame.process_entities_mut([](sge::ProcessingFrame&, sge::EntityId, sge::CTransform3D& transform, sge::CPerspectiveCamera&)
	{
		transform.set_local_position(transform.get_local_position() + sge::Vec3{ 0.01, 0, 0.01 });
	});
}

int main(int argc, char* argv[])
{
	/// Make sure we have a config file
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
	auto* window = glfwCreateWindow(window_width, window_height, "SinGE GLEditorServer", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "GLEditorServer: Could not create a window." << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	// Make the window's OpenGL context current
	glfwMakeContextCurrent(window);

	// Create a type database
	sge::TypeDB type_db;
	type_db.new_type<sge::Vec3>();
	type_db.new_type<sge::Quat>();
	type_db.new_type<float>();

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

	// Create a render system
	sge::gl_render::Config render_config;
	render_config.viewport_width = window_width;
	render_config.viewport_height = window_height;

	if (!config_reader->object_member("gl_render", render_config))
	{
		assert(false /*Could not load render config from the config file.*/);
	}

	sge::gl_render::GLRenderSystem renderSystem{ render_config };
	renderSystem.register_with_scene(scene);

	// Create a JavaScript system
	sge::JavaScriptEngine jsEngine{ scene };
	jsEngine.register_type(sge::Vec2::type_info);
	jsEngine.register_type(sge::Vec3::type_info);
	jsEngine.register_type(sge::Vec4::type_info);
	jsEngine.register_type(sge::CTransform3D::type_info);
	jsEngine.register_type(sge::CPerspectiveCamera::type_info);
	jsEngine.register_type(sge::CStaticMesh::type_info);
	//jsEngine.load_script("Content/JavaScript/main.js");

	// Create the test mutation system
	auto system_test_token = scene.register_system_mut_fn(&mut_process_test);

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		scene.update(0.016);

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	// Unregister systems
	scene.unregister_system_fn(system_test_token);
	renderSystem.unregister_with_scene(scene);

	glfwTerminate();
}

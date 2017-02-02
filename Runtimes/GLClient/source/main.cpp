// main.cpp

#include <chrono>
#include <iostream>
#include <GLFW/glfw3.h>
#include <Core/Math/Quat.h>
#include <Core/Reflection/TypeDB.h>
#include <Resource/Archives/JsonArchive.h>
#include <Engine/Scene.h>
#include <Engine/SystemFrame.h>
#include <Engine/UpdatePipeline.h>
#include <Engine/Components/CTransform3D.h>
#include <Engine/Components/Gameplay/CInput.h>
#include <Engine/Components/Gameplay/CCharacterController.h>
#include <GLRender/GLRenderSystem.h>
#include <GLRender/Config.h>
#include <BulletPhysics/BulletPhysicsSystem.h>
#include <BulletPhysics/Config.h>
#include <GLWindow/GLEventWindow.h>

namespace sge
{
    void input_test(
        SystemFrame& frame,
        const CInput::FActionEvent* tags,
        const EntityId* comps,
        std::size_t num)
    {
        frame.process_entities_mut(comps, num, [tags](
            ProcessingFrame& pframe,
            CCharacterController& controller,
            CTransform3D& transform)
        {
            auto input = tags[pframe.iteration_index()];

            if (input.name == "forward")
            {
                controller.walk({ 0, 1 });
            }
            else if (input.name == "backward")
            {
                controller.walk({ 0, -1 });
            }
            else if (input.name == "strafe_left")
            {
                controller.walk({ -1, 0 });
            }
            else if (input.name == "strafe_right")
            {
                controller.walk({ 1, 0 });
            }
            else if (input.name == "jump")
            {
                controller.jump();
            }
            else if (input.name == "turn_left")
            {
                auto rot = transform.get_local_rotation();
                rot.rotate_by_axis_angle({ 0, 1, 0 }, degrees(1), false);
                transform.set_local_rotation(rot);
            }
            else if (input.name == "turn_right")
            {
                auto rot = transform.get_local_rotation();
                rot.rotate_by_axis_angle({ 0, 1, 0 }, degrees(-1), false);
                transform.set_local_rotation(rot);
            }
        });
    }
}

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
    auto* window = sge::create_sge_opengl_window("SinGE GLClient", window_width, window_height);
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

    // Create a pipeline
    sge::UpdatePipeline pipeline;
    pipeline.register_tag_callback<sge::CInput>(
        sge::UpdatePipeline::NO_SYSTEM,
        sge::UpdatePipeline::FULLY_ASYNC,
        sge::TCO_NONE,
        sge::input_test);

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

	sge::gl_render::GLRenderSystem renderSystem{ render_config };
    renderSystem.pipeline_register(pipeline);

    // Create a bullet physics system
    sge::bullet_physics::Config physics_config;
    config_reader->object_member("bullet_physics", physics_config);
    sge::bullet_physics::BulletPhysicsSystem physics_system{ physics_config };
    physics_system.register_pipeline(pipeline);

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

    // Store the last time we printed out frame time
    auto last_printout = std::chrono::steady_clock::now();

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
        const auto start = std::chrono::high_resolution_clock::now();
		scene.update(pipeline, 0.016);
        const auto end = std::chrono::high_resolution_clock::now();
        const auto duration = std::chrono::duration<double, std::milli>{ end - start }.count();

        if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - last_printout).count() >= 1)
        {
    	    std::cout << "Scene update took " << duration << " milliseconds" << std::endl;
            last_printout = std::chrono::steady_clock::now();
        }

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	glfwTerminate();
}

// main.cpp

#include <chrono>
#include <iostream>
#include <GLFW/glfw3.h>
#include <Core/Math/Quat.h>
#include <Core/Reflection/TypeDB.h>
#include <Resource/Archives/JsonArchive.h>
#include <Resource/Resources/StaticMesh.h>
#include <Engine/Scene.h>
#include <Engine/SystemFrame.h>
#include <Engine/UpdatePipeline.h>
#include <Engine/Components/Gameplay/CInput.h>
#include <Engine/Components/Gameplay/CCharacterController.h>
#include <Engine/Components/Display/CCamera.h>
#include <GLRender/GLRenderSystem.h>
#include <GLRender/Config.h>
#include <BulletPhysics/BulletPhysicsSystem.h>
#include <BulletPhysics/Config.h>
#include <GLWindow/GLEventWindow.h>

static void action_input_response(
	sge::EventChannel& action_event,
	sge::EventChannel::SubscriberId subscriber,
	sge::ComponentContainer& character_component)
{
	sge::CInput::EAction events[8];
	sge::int32 num_events;

	while (action_event.consume(subscriber, sizeof(sge::CInput::EAction), 8, events, &num_events))
	{
		for (sge::int32 i = 0; i < num_events; ++i)
		{
			// Access character component for this event
			sge::CCharacterController* character;
			character_component.get_instances(&events[i].input_node, 1, &character);
			if (!character)
			{
				continue;
			}

			const float speed = 0.1;
			if (events[i].name == "jump")
			{
				// Make it jump
				character->jump();
			}
			else if (events[i].name == "forward")
			{
				character->walk(sge::Vec2{ 0, speed });
			}
			else if (events[i].name == "backward")
			{
				character->walk(sge::Vec2{ 0, -speed });
			}
			else if (events[i].name == "strafe_left")
			{
				character->walk(sge::Vec2{ -speed, 0 });
			}
			else if (events[i].name == "strafe_right")
			{
				character->walk(sge::Vec2{ speed, 0 });
			}
		}
	}
}

static void axis_input_response(
	sge::EventChannel& axis_event,
	sge::EventChannel::SubscriberId subscriber_id,
	sge::Scene& scene,
	sge::ComponentContainer& character_components)
{
	sge::CInput::EAxis events[8];
	sge::int32 num_events;

	while (axis_event.consume(subscriber_id, sizeof(sge::CInput::EAxis), 8, events, &num_events))
	{
		for (sge::int32 i = 0; i < num_events; ++i)
		{
			if (events[i].name == "turn")
			{
				// Access the node instance for this event
				sge::Node* node;
				scene.get_nodes(&events[i].input_node, 1, &node);

				// Make it turn
				auto rot = node->get_local_rotation();
				rot.rotate_by_axis_angle(sge::Vec3::up(), sge::degrees(events[i].value) / 5, true);
				node->set_local_rotation(rot);
			}
		}
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
    event_window.capture_mouse(true);

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

    // Register the input window
    event_window.register_pipeline(pipeline);

	// Create a render system
	sge::gl_render::Config render_config;
    glfwGetFramebufferSize(window, &render_config.viewport_width, &render_config.viewport_height);
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

	// Get the input component action event channel
	auto* const action_channel = scene.get_event_channel(sge::CInput::type_info, "action_event");
	auto* const axis_channel = scene.get_event_channel(sge::CInput::type_info, "axis_event");
	const auto action_subscriber = action_channel->subscribe();
	const auto axis_subscriber = axis_channel->subscribe();
	auto* const character_component = scene.get_component_container(sge::CCharacterController::type_info);

	// Create an input response system
	pipeline.register_system_fn("input_response", [=](sge::Scene& scene, sge::SystemFrame& /*frame*/)
	{
		action_input_response(*action_channel, action_subscriber, *character_component);
		axis_input_response(*axis_channel, axis_subscriber, scene, *character_component);
	});

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

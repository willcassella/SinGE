#include <chrono>
#include <iostream>

#include <GLFW/glfw3.h>

#include "lib/base/math/quat.h"
#include "lib/base/reflection/type_db.h"
#include "lib/bullet_physics/bullet_physics_system.h"
#include "lib/bullet_physics/config.h"
#include "lib/engine/components/gameplay/character_controller.h"
#include "lib/engine/components/gameplay/input.h"
#include "lib/engine/scene.h"
#include "lib/engine/system_frame.h"
#include "lib/engine/systems/animation_system.h"
#include "lib/engine/systems/change_level_system.h"
#include "lib/engine/update_pipeline.h"
#include "lib/gl_render/config.h"
#include "lib/gl_render/gl_render_system.h"
#include "lib/gl_window/gl_event_window.h"
#include "lib/resource/archives/json_archive.h"

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
    sge::Scene& scene)
{
    sge::CInput::EAxis events[8];
    sge::int32 num_events;

    while (axis_event.consume(subscriber_id, sizeof(sge::CInput::EAxis), 8, events, &num_events))
    {
        for (sge::int32 i = 0; i < num_events; ++i)
        {
            // Access the node instance for this event
            sge::Node* node;
            scene.get_nodes(&events[i].input_node, 1, &node);

            if (events[i].name == "turn")
            {
                // Make it turn
                auto rot = node->get_local_rotation();
                rot.rotate_by_axis_angle(sge::Vec3::up(), sge::degrees(events[i].value) / 5, true);
                node->set_local_rotation(rot);
            }
            if (events[i].name == "look")
            {
                // Get first child of this object
                sge::NodeId child_id = sge::NodeId::null_id();
                std::size_t num_children;
                node->get_children(0, 1, &num_children, &child_id);

                if (num_children == 0)
                {
                    continue;
                }

                sge::Node* child;
                scene.get_nodes(&child_id, 1, &child);

                // Rotate the child
                auto rot = child->get_local_rotation();
                rot.rotate_by_axis_angle(sge::Vec3::right(), sge::degrees(events[i].value) / 5, true);
                child->set_local_rotation(rot);
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
        std::cerr << "Could not initialize GLFW3." << std::endl;
        return EXIT_FAILURE;
    }

    // Get the window width and height
    int window_width = 1920, window_height = 1080;
    bool fullscreen = false;
    config_reader->object_member("window_width", window_width);
    config_reader->object_member("window_height", window_height);
    config_reader->object_member("fullscreen", fullscreen);

    // Create a windowed mode window and its OpenGL context
    auto* window = sge::create_sge_opengl_window("SinGE GLClient", window_width, window_height, fullscreen);
    if (!window)
    {
        std::cerr << "Could not create a window." << std::endl;
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
    renderSystem.initialize_subscriptions(scene);

    // Create a bullet physics system
    sge::bullet_physics::Config physics_config;
    config_reader->object_member("bullet_physics", physics_config);
    sge::bullet_physics::BulletPhysicsSystem physics_system{ physics_config };
    physics_system.register_pipeline(pipeline);
    physics_system.initialize_subscriptions(scene);

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
        axis_input_response(*axis_channel, axis_subscriber, scene);
    });

    // Create a change level system
    sge::ChangeLevelSystem change_level_system;
    change_level_system.pipeline_register(pipeline);
    change_level_system.initialize_subscriptions(scene);

    // Create an animation system
    sge::AnimationSystem anim_system;
    anim_system.register_pipeline(pipeline);

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

    // Load the scene
    std::string scene_path;
    if (config_reader->object_member("scene", scene_path))
    {
        sge::JsonArchive scene_archive;
        scene_archive.from_file(scene_path.c_str());
        scene_archive.deserialize_root(scene);
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

        if (event_window.quit_requested())
        {
            break;
        }

        if (change_level_system.requires_change_level())
        {
            const auto change_level_target = change_level_system.change_level_target();
            if (change_level_target == "EXIT")
            {
                break;
            }

            // Reset systems
            physics_system.reset();
            renderSystem.reset();
            change_level_system.reset();

            // Load new scene
            sge::JsonArchive scene_archive;
            scene_archive.from_file(change_level_target.c_str());
            scene_archive.deserialize_root(scene);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}

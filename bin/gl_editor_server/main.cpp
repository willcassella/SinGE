#include <assert.h>
#include <iostream>

#include <GLFW/glfw3.h>

#include "lib/base/math/quat.h"
#include "lib/base/math/vec3.h"
#include "lib/base/reflection/type_db.h"
#include "lib/bullet_physics/bullet_physics_system.h"
#include "lib/bullet_physics/config.h"
#include "lib/editor_server/editor_server_system.h"
#include "lib/engine/components/display/spot_light.h"
#include "lib/engine/components/display/static_mesh.h"
#include "lib/engine/scene.h"
#include "lib/engine/update_pipeline.h"
#include "lib/gl_render/config.h"
#include "lib/gl_render/gl_render_system.h"
#include "lib/gl_window/gl_event_window.h"
#include "lib/resource/archives/json_archive.h"
#include "lib/resource/resources/material.h"
#include "lib/resource/resources/static_mesh.h"
#include "lib/resource/resources/texture.h"

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
    auto* window = sge::create_sge_opengl_window("SinGE Editor Server", window_width, window_height, false);
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
    type_db.new_type<sge::Vec2>();
    type_db.new_type<sge::Vec3>();
    type_db.new_type<sge::Quat>();
    type_db.new_type<sge::StaticMesh>();
    type_db.new_type<sge::Material>();
    type_db.new_type<sge::Texture>();
    type_db.new_type<sge::CStaticMesh::LightmaskMode>();
    type_db.new_type<sge::CSpotlight::Shape>();
    type_db.new_type<sge::color::RGBF32>();

    // Create a scene
    sge::Scene scene{ type_db };
    sge::register_builtin_components(scene);

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
    render_system.initialize_subscriptions(scene);

    sge::bullet_physics::Config physics_config;
    sge::bullet_physics::BulletPhysicsSystem physics_system{ physics_config };
    physics_system.register_pipeline(pipeline);
    physics_system.initialize_subscriptions(scene);

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

    // Load the scene
    std::string scene_path;
    if (config_reader->object_member("scene", scene_path))
    {
        sge::JsonArchive scene_archive;
        scene_archive.from_file(scene_path.c_str());
        scene_archive.deserialize_root(scene);
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

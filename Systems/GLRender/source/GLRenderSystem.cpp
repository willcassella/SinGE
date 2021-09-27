// GLRenderSystem.cpp

#include <iostream>
#include <Core/Reflection/ReflectionBuilder.h>
#include <Resource/Archives/JsonArchive.h>
#include <Resource/Archives/BinaryArchive.h>
#include <Resource/Misc/LightmaskVolume.h>
#include <Engine/Components/Display/CStaticMesh.h>
#include <Engine/Components/Display/CSpotlight.h>
#include <Engine/Components/Display/CCamera.h>
#include <Engine/Scene.h>
#include <Engine/SystemFrame.h>
#include <Engine/UpdatePipeline.h>
#include <Engine/Util/DebugDraw.h>
#include "../include/GLRender/Config.h"
#include "../private/GLRenderSystemState.h"
#include "../private/DebugLine.h"
#include "../private/Util.h"

SGE_REFLECT_TYPE(sge::gl_render::GLRenderSystem);

namespace sge
{
    namespace gl_render
    {
        static void on_node_transform_update(
            EventChannel& node_transform_update_channel,
            EventChannel::SubscriberId subscriber_id,
            RenderScene_Commands& commands)
        {
            // Get events
            ENodeTransformChanged events[16];
            int32 num_events;
            while (node_transform_update_channel.consume(subscriber_id, events, &num_events))
            {
                // Get ids and transforms
                NodeId nodes_ids[16];
                Mat4 world_transforms[16];
                for (int32 i = 0; i < num_events; ++i)
                {
                    nodes_ids[i] = events[i].node->get_id();
                    world_transforms[i] = events[i].node->get_world_matrix();
                }

                // Update render scene
                RenderScene_update_matrices(
                    commands,
                    nodes_ids,
                    world_transforms,
                    num_events);
            }
        }

        static void on_static_mesh_new(
            Scene& scene,
            EventChannel& new_static_mesh_channel,
            EventChannel::SubscriberId subscriber_id,
            RenderResource& resources,
            RenderScene_Commands& commands)
        {
            // Get events
            ENewComponent events[8];
            int32 num_events;
            while (new_static_mesh_channel.consume(subscriber_id, events, &num_events))
            {
                NodeId node_ids[8];
                const CStaticMesh* components[8];
                for (int32 i = 0; i < num_events; ++i)
                {
                    node_ids[i] = events[i].node;
                    components[i] = (const CStaticMesh*)events[i].instance;
                }

                // Get nodes
                const Node* nodes[8];
                scene.get_nodes(node_ids, num_events, nodes);

                // Update render scene
                RenderScene_insert_static_mesh_commands(
                    commands,
                    resources,
                    nodes,
                    components,
                    num_events);
            }
        }

        static void on_static_mesh_destroy(
            EventChannel& destroyed_static_mesh_channel,
            EventChannel::SubscriberId subscriber_id,
            RenderScene_Commands& commands)
        {
            // Get events
            EDestroyedComponent events[16];
            int32 num_events;
            while (destroyed_static_mesh_channel.consume(subscriber_id, events, &num_events))
            {
                NodeId node_ids[16];
                for (int32 i = 0; i < num_events; ++i)
                {
                    node_ids[i] = events[i].node;
                }

                // Update render scene
                RenderScene_remove_static_mesh_commands(
                    commands,
                    node_ids,
                    num_events);
            }
        }

        static void on_spotlight_new(
            Scene& scene,
            EventChannel& new_spotlight_channel,
            EventChannel::SubscriberId subscriber_id,
            RenderResource& resources,
            RenderScene_Commands& commands)
        {
            // Get events
            ENewComponent events[8];
            int32 num_events;
            while (new_spotlight_channel.consume(subscriber_id, events, & num_events))
            {
                NodeId node_ids[8];
                const CSpotlight* components[8];
                for (int32 i = 0; i < num_events; ++i)
                {
                    node_ids[i] = events[i].node;
                    components[i] = (const CSpotlight*)events[i].instance;
                }

                // Get nodes
                const Node* nodes[8];
                scene.get_nodes(node_ids, num_events, nodes);

                // Update render scene
                RenderScene_insert_spotlight_commands(
                    commands,
                    resources,
                    nodes,
                    components,
                    num_events);
            }
        }

        static void on_spotlight_modified(
            Scene& scene,
            EventChannel& modified_spotlight_event_channel,
            EventChannel::SubscriberId subscriber_id,
            RenderResource& resources,
            RenderScene_Commands& commands)
        {
            // Get events
            EModifiedComponent events[8];
            int32 num_events;
            while (modified_spotlight_event_channel.consume(subscriber_id, events, &num_events))
            {
                NodeId node_ids[8];
                const CSpotlight* components[8];
                for (int32 i = 0; i < num_events; ++i)
                {
                    node_ids[i] = events[i].node;
                    components[i] = (const CSpotlight*)events[i].instance;
                }

                // Get nodes
                const Node* nodes[8];
                scene.get_nodes(node_ids, num_events, nodes);

                // Update render scene
                RenderScene_update_spotlight_commands(
                    commands,
                    resources,
                    nodes,
                    components,
                    num_events);
            }
        }

        static void on_spotlight_destroy(
            EventChannel& destroyed_spotlight_event_channel,
            EventChannel::SubscriberId subscriber_id,
            RenderScene_Commands& commands)
        {
            // Get events
            EDestroyedComponent events[16];
            int32 num_events;
            while (destroyed_spotlight_event_channel.consume(subscriber_id, events, &num_events))
            {
                NodeId node_ids[16];
                for (int32 i = 0; i < num_events; ++i)
                {
                    node_ids[i] = events[i].node;
                }

                // Update render scene
                RenderScene_remove_spotlight_commands(
                    commands,
                    node_ids,
                    num_events);
            }
        }

        static void initialize_render_scene(
            RenderScene_Commands& render_scene,
            Scene& scene)
        {
            // Load the lightmap object
            if (!scene.get_raw_scene_data().lightmap_data_path.empty())
            {
                BinaryArchive lightmap_archive;
                lightmap_archive.from_file(scene.get_raw_scene_data().lightmap_data_path.c_str());

                SceneLightmap scene_lightmap;
                lightmap_archive.deserialize_root(scene_lightmap);

                // Get the light direction and color
                render_scene.light_dir = scene_lightmap.light_direction;
                render_scene.light_intensity = scene_lightmap.light_intensity;

                // Load all lightmap components into textures
                for (const auto& element : scene_lightmap.lightmap_elements)
                {
                    RenderScene_Lightmap lightmap;
                    lightmap.x_basis_tex = create_texture(
                        element.second.width,
                        element.second.height,
                        element.second.basis_x_radiance.data(),
                        GL_RGB32F,
                        GL_RGB,
                        GL_FLOAT);
                    lightmap.y_basis_tex = create_texture(
                        element.second.width,
                        element.second.height,
                        element.second.basis_y_radiance.data(),
                        GL_RGB32F,
                        GL_RGB,
                        GL_FLOAT);
                    lightmap.z_basis_tex = create_texture(
                        element.second.width,
                        element.second.height,
                        element.second.basis_z_radiance.data(),
                        GL_RGB32F,
                        GL_RGB,
                        GL_FLOAT);
                    lightmap.direct_mask_tex = create_texture(
                        element.second.width,
                        element.second.height,
                        element.second.direct_mask.data(),
                        GL_R8,
                        GL_RED,
                        GL_UNSIGNED_BYTE);
                    render_scene.node_lightmaps.insert(std::make_pair(element.first, lightmap));
                }
            }
        }

        static void set_render_target_params()
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }

        static void upload_render_target_data(
            GLsizei width,
            GLsizei height,
            GLenum internal_format,
            GLenum upload_format,
            GLenum upload_type)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, upload_format, upload_type, nullptr);
        }

        static void attach_framebuffer_layer(
            GLuint layer,
            GLenum attachment)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, layer, 0);
        }

        static GLuint create_viewport_program(
            GLuint v_shader,
            GLuint f_shader)
        {
            // Create the program and attach shaders
            const auto program = glCreateProgram();
            glAttachShader(program, v_shader);
            glAttachShader(program, f_shader);

            // Bind vertex attributes
            glBindAttribLocation(program, gl_material::POSITION_ATTRIB_LOCATION, gl_material::POSITION_ATTRIB_NAME);
            glBindAttribLocation(program, gl_material::MATERIAL_TEXCOORD_ATTRIB_LOCATION, gl_material::MATERIAL_TEXCOORD_ATTRIB_NAME);

            // Link the program and detach shaders
            glLinkProgram(program);
            glDetachShader(program, v_shader);
            glDetachShader(program, f_shader);

            // Make sure the program compiled linked
            debug_program_status(program, GLDebugOutputMode::ONLY_ERROR);

            // Set uniforms
            glUseProgram(program);
            glUniform1i(glGetUniformLocation(program, "depth_buffer"), 0);
            glUniform1i(glGetUniformLocation(program, "position_buffer"), 1);
            glUniform1i(glGetUniformLocation(program, "normal_buffer"), 2);
            glUniform1i(glGetUniformLocation(program, "albedo_buffer"), 3);
            glUniform1i(glGetUniformLocation(program, "roughness_metallic_buffer"), 4);
            glUniform1i(glGetUniformLocation(program, "irradiance_buffer"), 5);

            // Return it
            return program;
        }

        ////////////////////////
        ///   Constructors   ///

        GLRenderSystem::GLRenderSystem(const Config& config)
        {
            assert(config.validate() /*The given GLRenderSystem config object is not valid*/);

            // Create the internal state object
            _state = std::make_unique<GLRenderSystem::State>();
            _state->width = config.viewport_width;
            _state->height = config.viewport_height;

            // Initialize GLEW
            glewExperimental = GL_TRUE;
            if (glewInit() != GLEW_OK)
            {
                printf("ERROR: GLRenderSystem: Could not initialize GLEW\n");
            }
            glGetError(); // Sometimes GLEW initialization generates an error, pop it off the stack.

            // Create default mesh resource
            _state->resources.missing_mesh = RenderResource_get_static_mesh_resource(
                _state->resources,
                config.missing_mesh.c_str());

            // Create the default material resource
            _state->resources.missing_material = RenderResource_get_material_resource(
                _state->resources,
                config.missing_material.c_str());

            // Initialize lightmap volume resources
            _state->resources.lightmask_volume_material = _state->resources.missing_material;

            // Create the frustum volume EBO
            uint32 frustum_elems[NUM_FRUSTUM_ELEMS];
            create_lightmask_volume_frustum_elems(frustum_elems);
            glGenBuffers(1, &_state->resources.frustum_ebo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _state->resources.frustum_ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(frustum_elems), frustum_elems, GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            // Initialize OpenGL
            glLineWidth(1.f);
            glClearColor(0, 0, 0, 1);
            glClearDepth(1.f);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glEnable(GL_STENCIL_TEST);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);

            // Get the default framebuffer
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_state->default_framebuffer);

            // Create a framebuffer for deferred rendering (GBuffer)
            glGenFramebuffers(1, &_state->gbuffer_framebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, _state->gbuffer_framebuffer);
            glGenTextures(GBufferLayer::NUM_LAYERS, _state->gbuffer_layers.data());

            // Create gbuffer depth layer
            glBindTexture(GL_TEXTURE_2D, _state->gbuffer_layers[GBufferLayer::DEPTH_STENCIL]);
            upload_render_target_data(
                _state->width,
                _state->height,
                GBUFFER_DEPTH_STENCIL_INTERNAL_FORMAT,
                GBUFFER_DEPTH_STENCIL_UPLOAD_FORMAT,
                GBUFFER_DEPTH_STENCIL_UPLOAD_TYPE);
            set_render_target_params();
            attach_framebuffer_layer(
                _state->gbuffer_layers[GBufferLayer::DEPTH_STENCIL],
                GBUFFER_DEPTH_STENCIL_ATTACHMENT);

            // Create gbuffer position layer
            glBindTexture(GL_TEXTURE_2D, _state->gbuffer_layers[GBufferLayer::POSITION]);
            upload_render_target_data(
                _state->width,
                _state->height,
                GBUFFER_POSITION_INTERNAL_FORMAT,
                GBUFFER_POSITION_UPLOAD_FORMAT,
                GBUFFER_POSITION_UPLOAD_TYPE);
            set_render_target_params();
            attach_framebuffer_layer(
                _state->gbuffer_layers[GBufferLayer::POSITION],
                GBUFFER_POSITION_ATTACHMENT);

            // Create gbuffer normal layer
            glBindTexture(GL_TEXTURE_2D, _state->gbuffer_layers[GBufferLayer::NORMAL]);
            upload_render_target_data(
                _state->width,
                _state->height,
                GBUFFER_NORMAL_INTERNAL_FORMAT,
                GBUFFER_NORMAL_UPLOAD_FORMAT,
                GBUFFER_NORMAL_UPLOAD_TYPE);
            set_render_target_params();
            attach_framebuffer_layer(
                _state->gbuffer_layers[GBufferLayer::NORMAL],
                GBUFFER_NORMAL_ATTACHMENT);

            // Create gbuffer albedo layer
            glBindTexture(GL_TEXTURE_2D, _state->gbuffer_layers[GBufferLayer::ALBEDO]);
            upload_render_target_data(
                _state->width,
                _state->height,
                GBUFFER_ALBEDO_INTERNAL_FORMAT,
                GBUFFER_ALBEDO_UPLOAD_FORMAT,
                GBUFFER_ALBEDO_UPLOAD_TYPE);
            set_render_target_params();
            attach_framebuffer_layer(
                _state->gbuffer_layers[GBufferLayer::ALBEDO],
                GBUFFER_ALBEDO_ATTACHMENT);

            // Create gbuffer roughness/metallic layer
            glBindTexture(GL_TEXTURE_2D, _state->gbuffer_layers[GBufferLayer::ROUGHNESS_METALLIC]);
            upload_render_target_data(
                _state->width,
                _state->height,
                GBUFFER_ROUGHNESS_METALLIC_INTERNAL_FORMAT,
                GBUFFER_ROUGHNESS_METALLIC_UPLOAD_FORMAT,
                GBUFFER_ROUGHNESS_METALLIC_UPLOAD_TYPE);
            set_render_target_params();
            attach_framebuffer_layer(
                _state->gbuffer_layers[GBufferLayer::ROUGHNESS_METALLIC],
                GBUFFER_ROUGHNESS_METALLIC_ATTACHMENT);

            // Create gbuffer irradiance layer
            glBindTexture(GL_TEXTURE_2D, _state->gbuffer_layers[GBufferLayer::IRRADIANCE]);
            upload_render_target_data(
                _state->width,
                _state->height,
                GBUFFER_IRRADIANCE_INTERNAL_FORMAT,
                GBUFFER_IRRADIANCE_UPLOAD_FORMAT,
                GBUFFER_IRRADIANCE_UPLOAD_TYPE);
            set_render_target_params();
            attach_framebuffer_layer(
                _state->gbuffer_layers[GBufferLayer::IRRADIANCE],
                GBUFFER_IRRADIANCE_ATTACHMENT);

            constexpr GLenum GBUFFER_DRAW_BUFFERS[] = {
                GBUFFER_POSITION_ATTACHMENT,
                GBUFFER_NORMAL_ATTACHMENT,
                GBUFFER_ALBEDO_ATTACHMENT,
                GBUFFER_ROUGHNESS_METALLIC_ATTACHMENT,
                GBUFFER_IRRADIANCE_ATTACHMENT };
            constexpr GLsizei NUM_GBUFFER_DRAW_BUFFERS = sizeof(GBUFFER_DRAW_BUFFERS) / sizeof(GLenum);
            glDrawBuffers(NUM_GBUFFER_DRAW_BUFFERS, GBUFFER_DRAW_BUFFERS);

            // Make sure the GBuffer was constructed successfully
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                std::cerr << "GLRenderSystem: Error creating GBuffer." << std::endl;
            }

            // Create a framebuffer for post-processing
            glGenFramebuffers(1, &_state->post_framebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, _state->post_framebuffer);
            glGenTextures(1, &_state->post_buffer_hdr);

            // Create HDR buffer
            glBindTexture(GL_TEXTURE_2D, _state->post_buffer_hdr);
            upload_render_target_data(
                _state->width,
                _state->height,
                POST_BUFFER_HDR_INTERNAL_FORMAT,
                POST_BUFFER_HDR_UPLOAD_FORMAT,
                POST_BUFFER_HDR_UPLOAD_TYPE);
            set_render_target_params();
            attach_framebuffer_layer(
                _state->post_buffer_hdr,
                POST_BUFFER_HDR_ATTACHMENT);

            // Make sure the post-processing buffer was constructed successfully
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                std::cerr << "GLRenderSystem: Error creating post-processing framebuffer." << std::endl;
            }

            constexpr float QUAD_VERTEX_DATA[] = {
                -1.f, 1.f, 0.f, 1.f,    // Top-left point
                -1.f, -1.f, 0.f, 0.f,   // Bottom-left point
                1.f, -1.f, 1.f, 0.f,    // Bottom-right point
                1.f, 1.f, 1.f, 1.f,     // Top-right point
            };

            // Create a VAO for sprite
            glGenVertexArrays(1, &_state->sprite_vao);
            glBindVertexArray(_state->sprite_vao);

            // Create a VBO for the sprite quad and upload data
            glGenBuffers(1, &_state->sprite_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, _state->sprite_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD_VERTEX_DATA), QUAD_VERTEX_DATA, GL_STATIC_DRAW);

            // Sprite vertex specification
            glEnableVertexAttribArray(gl_material::POSITION_ATTRIB_LOCATION);
            glEnableVertexAttribArray(gl_material::MATERIAL_TEXCOORD_ATTRIB_LOCATION);
            glVertexAttribPointer(gl_material::POSITION_ATTRIB_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, nullptr);
            glVertexAttribPointer(gl_material::MATERIAL_TEXCOORD_ATTRIB_LOCATION, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));

            // Create screen shaders
            const GLuint viewport_v_shader = load_shader(GL_VERTEX_SHADER, config.viewport_vert_shader.c_str());
            const GLuint scene_f_shader = load_shader(GL_FRAGMENT_SHADER, config.scene_shader.c_str());
            const GLuint post_f_shader = load_shader(GL_FRAGMENT_SHADER, config.post_shader.c_str());

            // Create the scene shader program
            _state->scene_shader_program = create_viewport_program(viewport_v_shader, scene_f_shader);
            _state->scene_program_view_uniform = glGetUniformLocation(_state->scene_shader_program, "view");
            _state->scene_program_light_dir_uniform = glGetUniformLocation(_state->scene_shader_program, "light_dir");
            _state->scene_program_light_intensity_uniform = glGetUniformLocation(_state->scene_shader_program, "light_intensity");

            // Create the post-processing shader program
            _state->post_shader_program = create_viewport_program(viewport_v_shader, post_f_shader);
            glProgramUniform1i(_state->post_shader_program, glGetUniformLocation(_state->post_shader_program, "hdr_buffer"), 5);
            _state->post_program_gamma_uniform = glGetUniformLocation(_state->post_shader_program, "gamma");
            _state->post_program_brightness_uniform = glGetUniformLocation(_state->post_shader_program, "brightness_boost");

            // Create a VAO for debug lines
            glGenVertexArrays(1, &_state->debug_line_vao);
            glBindVertexArray(_state->debug_line_vao);

            // Create a vbo for lines
            glGenBuffers(1, &_state->debug_line_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, _state->debug_line_vbo);

            // Line vertex specification
            glEnableVertexAttribArray(DEBUG_LINE_POSITION_ATTRIB_LOCATION);
            glEnableVertexAttribArray(DEBUG_LINE_COLOR_ATTRIB_LOCATION);
            glVertexAttribPointer(
                DEBUG_LINE_POSITION_ATTRIB_LOCATION,
                3,
                GL_FLOAT,
                GL_FALSE,
                sizeof(DebugLineVert),
                nullptr);
            glVertexAttribPointer(
                DEBUG_LINE_COLOR_ATTRIB_LOCATION,
                3,
                GL_FLOAT,
                GL_FALSE,
                sizeof(DebugLineVert),
                (void*)offsetof(DebugLineVert, color_rgb));

            // Create debug line program
            const GLuint debug_line_v_shader = load_shader(GL_VERTEX_SHADER, config.debug_line_vert_shader.c_str());
            const GLuint debug_line_f_shader = load_shader(GL_FRAGMENT_SHADER, config.debug_line_frag_shader.c_str());
            _state->debug_line_program = glCreateProgram();
            glAttachShader(_state->debug_line_program, debug_line_v_shader);
            glAttachShader(_state->debug_line_program, debug_line_f_shader);

            // Bind vertex attributes
            glBindAttribLocation(_state->debug_line_program, DEBUG_LINE_POSITION_ATTRIB_LOCATION, DEBUG_LINE_POSITION_ATTRIB_NAME);
            glBindAttribLocation(_state->debug_line_program, DEBUG_LINE_COLOR_ATTRIB_LOCATION, DEBUG_LINE_COLOR_ATTRIB_NAME);

            // Link the program and detach shaders
            glLinkProgram(_state->debug_line_program);
            glDetachShader(_state->debug_line_program, debug_line_v_shader);
            glDetachShader(_state->debug_line_program, debug_line_f_shader);

            // Make sure the program linked sucessfully
            debug_program_status(_state->debug_line_program, GLDebugOutputMode::ONLY_ERROR);

            // Get uniforms
            _state->debug_line_view_uniform = glGetUniformLocation(_state->debug_line_program, "view");
            _state->debug_line_proj_uniform = glGetUniformLocation(_state->debug_line_program, "projection");

            const auto error = glGetError();
            if (error != GL_NO_ERROR)
            {
                std::cerr << "GLRenderSystem: An error occurred during startup - " << error << std::endl;
            }
        }

        GLRenderSystem::~GLRenderSystem()
        {
            // Todo
        }

        ///////////////////
        ///   Methods   ///

        void GLRenderSystem::pipeline_register(UpdatePipeline& pipeline)
        {
            pipeline.register_system_fn(
                "gl_render",
                this,
                &GLRenderSystem::render_scene);
        }

        void GLRenderSystem::initialize_subscriptions(Scene& scene)
        {
            _new_static_mesh_channel = scene.get_event_channel(CStaticMesh::type_info, "new");
            _modified_static_mesh_channel = scene.get_event_channel(CStaticMesh::type_info, "prop_mod");
            _destroyed_static_mesh_channel = scene.get_event_channel(CStaticMesh::type_info, "destroy");
            _new_spotlight_channel = scene.get_event_channel(CSpotlight::type_info, "new");
            _modified_spotlight_channel = scene.get_event_channel(CSpotlight::type_info, "prop_mod");
            _destroyed_spotlight_channel = scene.get_event_channel(CSpotlight::type_info, "destroy");
            _debug_draw_line_channel = scene.get_debug_draw_line_channel();
            _modified_node_transform_channel = scene.get_node_world_transform_changed_channel();
            _new_static_mesh_sid = _new_static_mesh_channel->subscribe();
            _modified_static_mesh_sid = _modified_static_mesh_channel->subscribe();
            _destroyed_static_mesh_sid = _destroyed_static_mesh_channel->subscribe();
            _new_spotlight_sid = _new_spotlight_channel->subscribe();
            _modified_spotlight_sid = _modified_spotlight_channel->subscribe();
            _destroyed_spotlight_sid = _destroyed_spotlight_channel->subscribe();
            _debug_draw_line_sid = _debug_draw_line_channel->subscribe();
            _modified_node_transform_sid = _modified_node_transform_channel->subscribe();
        }

        void GLRenderSystem::set_viewport(int width, int height)
        {
            _state->width = width;
            _state->height = height;

            // Resize depth layer
            glBindTexture(GL_TEXTURE_2D, _state->gbuffer_layers[GBufferLayer::DEPTH_STENCIL]);
            upload_render_target_data(
                width,
                height,
                GBUFFER_DEPTH_STENCIL_INTERNAL_FORMAT,
                GBUFFER_DEPTH_STENCIL_UPLOAD_FORMAT,
                GBUFFER_DEPTH_STENCIL_UPLOAD_TYPE);

            // Resize position layer
            glBindTexture(GL_TEXTURE_2D, _state->gbuffer_layers[GBufferLayer::POSITION]);
            upload_render_target_data(
                width,
                height,
                GBUFFER_POSITION_INTERNAL_FORMAT,
                GBUFFER_POSITION_UPLOAD_FORMAT,
                GBUFFER_POSITION_UPLOAD_TYPE);

            // Resize normal layer
            glBindTexture(GL_TEXTURE_2D, _state->gbuffer_layers[GBufferLayer::NORMAL]);
            upload_render_target_data(
                width,
                height,
                GBUFFER_NORMAL_INTERNAL_FORMAT,
                GBUFFER_NORMAL_UPLOAD_FORMAT,
                GBUFFER_NORMAL_UPLOAD_TYPE);

            // Resize albedo layer
            glBindTexture(GL_TEXTURE_2D, _state->gbuffer_layers[GBufferLayer::ALBEDO]);
            upload_render_target_data(
                width,
                height,
                GBUFFER_ALBEDO_INTERNAL_FORMAT,
                GBUFFER_ALBEDO_UPLOAD_FORMAT,
                GBUFFER_ALBEDO_UPLOAD_TYPE);

            // Resize rougness/metallic layer
            glBindTexture(GL_TEXTURE_2D, _state->gbuffer_layers[GBufferLayer::ROUGHNESS_METALLIC]);
            upload_render_target_data(
                width,
                height,
                GBUFFER_ROUGHNESS_METALLIC_INTERNAL_FORMAT,
                GBUFFER_ROUGHNESS_METALLIC_UPLOAD_FORMAT,
                GBUFFER_ROUGHNESS_METALLIC_UPLOAD_TYPE);

            // Resize irradiance layer
            glBindTexture(GL_TEXTURE_2D, _state->gbuffer_layers[GBufferLayer::IRRADIANCE]);
            upload_render_target_data(
                width,
                height,
                GBUFFER_IRRADIANCE_INTERNAL_FORMAT,
                GBUFFER_IRRADIANCE_UPLOAD_FORMAT,
                GBUFFER_IRRADIANCE_UPLOAD_TYPE);

            // Resize post buffer
            glBindTexture(GL_TEXTURE_2D, _state->post_buffer_hdr);
            upload_render_target_data(
                width,
                height,
                POST_BUFFER_HDR_INTERNAL_FORMAT,
                POST_BUFFER_HDR_UPLOAD_FORMAT,
                POST_BUFFER_HDR_UPLOAD_TYPE);
        }

        void GLRenderSystem::reset()
        {
            RenderScene_clear(_state->render_scene);
            _state->initialized_render_scene = false;
        }

        void GLRenderSystem::render_scene(Scene& scene, SystemFrame& /*frame*/)
        {
            // Initialize the render scene data structure, if we haven't already
            if (!_state->initialized_render_scene)
            {
                initialize_render_scene(_state->render_scene, scene);
                _state->initialized_render_scene = true;
            }

            _state->gather_debug_lines(*_debug_draw_line_channel, _debug_draw_line_sid);

            // Consume events
            on_static_mesh_new(scene, *_new_static_mesh_channel, _new_static_mesh_sid, _state->resources, _state->render_scene);
            on_static_mesh_destroy(*_destroyed_static_mesh_channel, _destroyed_static_mesh_sid, _state->render_scene);
            on_spotlight_new(scene, *_new_spotlight_channel, _new_spotlight_sid, _state->resources, _state->render_scene);
            on_spotlight_modified(scene, *_modified_spotlight_channel, _modified_spotlight_sid, _state->resources, _state->render_scene);
            on_spotlight_destroy(*_destroyed_spotlight_channel, _destroyed_spotlight_sid, _state->render_scene);
            on_node_transform_update(*_modified_node_transform_channel, _modified_node_transform_sid, _state->render_scene);

            // Create camera matrices
            NodeId cam_node;
            CPerspectiveCamera* cam_instance;
            Node* cam_node_instance;
            std::size_t num_cameras;

            auto* cam_component = scene.get_component_container(CPerspectiveCamera::type_info);
            cam_component->get_instance_nodes(0, 1, &num_cameras, &cam_node);

            // If no camera was found, return
            if (num_cameras == 0)
            {
                return;
            }

            // Access the camera
            cam_component->get_instances(&cam_node, 1, &cam_instance);
            scene.get_nodes(&cam_node, 1, &cam_node_instance);
            const Mat4 view = cam_node_instance->get_world_matrix().inverse();
            const Mat4 proj = cam_instance->get_projection_matrix((float)this->_state->width / this->_state->height);

            // Render the scene
            RenderScene_render(
                _state->render_scene,
                _state->resources,
                _state->gbuffer_framebuffer,
                _state->width,
                _state->height,
                view,
                proj);

            // Draw debug lines (only allow irradiance output)
            glDisable(GL_STENCIL_TEST);
            glDisable(GL_DEPTH_TEST);
            std::array<GLenum, 5> draw_buffers = {
                GL_NONE,
                GL_NONE,
                GL_NONE,
                GL_NONE,
                GBUFFER_IRRADIANCE_ATTACHMENT };
            glDrawBuffers((GLsizei)draw_buffers.size(), draw_buffers.data());

            if (!_state->frame_debug_lines.empty())
            {
                glBindVertexArray(_state->debug_line_vao);
                glUseProgram(_state->debug_line_program);
                glBindBuffer(GL_ARRAY_BUFFER, _state->debug_line_vbo);
                glBufferData(
                    GL_ARRAY_BUFFER,
                    _state->frame_debug_lines.size() * sizeof(DebugLineVert),
                    _state->frame_debug_lines.data(),
                    GL_DYNAMIC_DRAW);
                glUniformMatrix4fv(_state->debug_line_view_uniform, 1, GL_FALSE, view.vec());
                glUniformMatrix4fv(_state->debug_line_proj_uniform, 1, GL_FALSE, proj.vec());
                glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(_state->frame_debug_lines.size()));
                _state->frame_debug_lines.clear();
            }

            // Reset output buffers
            draw_buffers = {
                GBUFFER_POSITION_ATTACHMENT,
                GBUFFER_NORMAL_ATTACHMENT,
                GBUFFER_ALBEDO_ATTACHMENT,
                GBUFFER_ROUGHNESS_METALLIC_ATTACHMENT,
                GBUFFER_IRRADIANCE_ATTACHMENT };
            glDrawBuffers((GLsizei)draw_buffers.size(), draw_buffers.data());

            render_scene_shade_hdr(_state->post_framebuffer, *_state, view);

            /*---------------------------*/
            /*---   POST-PROCESSING   ---*/

            // Bind the post-buffer for reading
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, _state->post_buffer_hdr);

            // Bind the default framebuffer for drawing
            glBindFramebuffer(GL_FRAMEBUFFER, _state->default_framebuffer);

            // Bind the post-processing shader program
            glUseProgram(_state->post_shader_program);

            // Upload gamma and brightness uniforms
            glProgramUniform1f(_state->post_shader_program, _state->post_program_gamma_uniform, scene.get_raw_scene_data().scene_gamma);
            glProgramUniform1f(_state->post_shader_program, _state->post_program_brightness_uniform, scene.get_raw_scene_data().scene_brightness_boost);

            // Draw the screen quad
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }
    }
}

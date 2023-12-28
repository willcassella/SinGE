#include <chrono>
#include <iostream>

#include "lib/editor_server/editor_ops.h"
#include "lib/editor_server/session.h"
#include "lib/engine/scene.h"
#include "lib/engine/system_frame.h"
#include "lib/resource/archives/json_archive.h"

namespace sge
{
    namespace editor_server
    {
        SystemFrame& Session::get_frame()
        {
            return *_system_data->frame;
        }

        Scene& Session::get_scene()
        {
            return *_system_data->scene;
        }

        void Session::enequeue_message(Packet* packet)
        {
            // Add the message to the queue
            _outgoing_packets.push(packet);

            // If the packet we just added is the only packet in the queue, we need to send it
            if (_outgoing_packets.size() == 1)
            {
                async_send_message();
            }
        }

        void Session::async_send_message()
        {
            // If there are no messages, return
            if (_outgoing_packets.empty())
            {
                return;
            }

            // Send it
            asio::async_write(socket, asio::buffer(_outgoing_packets.front(), _outgoing_packets.front()->packet_size()),
                [self = shared_from_this()](const std::error_code& error, size_t /*bytes*/)
            {
                if (error)
                {
                    report_error(error);
                }

                // Get rid of the packet
                Packet::free(self->_outgoing_packets.front());
                self->_outgoing_packets.pop();

                // Go again
                self->async_send_message();
            });
        }

        void Session::async_receive_message()
        {
            asio::async_read(socket, asio::buffer(_in_header),
                [self = shared_from_this()](const std::error_code& error, size_t /*len*/)
            {
                if (error)
                {
                    report_error(error);
                    return;
                }

                // Allocate space for content
                self->_in_content.assign(Packet::content_length(self->_in_header), 0);

                // Read content
                asio::read(self->socket, asio::buffer(&self->_in_content[0], self->_in_content.size()));

                // Log query time
                const auto message_time = std::chrono::system_clock::now();
                std::cout << "Received query at " << std::chrono::system_clock::to_time_t(message_time) << std::endl;

                // Deserialze the query json string
                JsonArchive query_archive;
                query_archive.from_string(self->_in_content.c_str());
                auto* const query_reader = query_archive.read_root();

                // Create a global client output archive
                JsonArchive global_response_archive;
                auto* const global_response_writer = global_response_archive.write_root();

                // Run write operations
                const auto wrote_global_output = self->write_ops(*query_reader, *global_response_writer);

                // Allow scene to update
                self->get_frame().yield();

                // Create an archive for the sender-exclusivce response
                JsonArchive sender_response_archive;
                auto* const sender_response_writer = sender_response_archive.write_root();

                // Run post-write operations
                const auto wrote_sender_output = self->post_write_ops(*query_reader, *sender_response_writer);

                // Close reader and writer
                query_reader->pop();
                global_response_writer->pop();
                sender_response_writer->pop();

                // Log completion time
                auto completed_time = std::chrono::system_clock::now();
                std::cout << "Completed query at " << std::chrono::system_clock::to_time_t(completed_time) << std::endl;
                std::cout << std::endl;

                // Write sender-exclusive result
                if (wrote_sender_output)
                {
                    std::string result = sender_response_archive.to_string();
                    auto* packet = Packet::encode_packet(
                        Packet::sequence_number(self->_in_header),
                        result.c_str(),
                        static_cast<Packet::ContentLength_t>(result.size()));
                    self->enequeue_message(packet);
                }

                // Write global result
                if (wrote_global_output)
                {
                    std::string result = global_response_archive.to_string();
                    auto* packet = Packet::encode_packet(
                        Packet::sequence_number(self->_in_header),
                        result.c_str(),
                        static_cast<Packet::ContentLength_t>(result.size()));
                    self->enequeue_message(packet);
                }

                // Wait for a new message
                self->async_receive_message();
            });
        }

        bool Session::write_ops(ArchiveReader& query_reader, ArchiveWriter& global_response_writer)
        {
            bool wrote_output = false;

            // Handle a query to create a node
            if (query_reader.pull_object_member("new_node"))
            {
                global_response_writer.push_object_member("new_node");
                ops::new_node_query(get_scene(), query_reader, global_response_writer);
                global_response_writer.pop(); // "new_node"
                query_reader.pop();
                wrote_output = true;
            }

            // Handle a query to destroy a node
            if (query_reader.pull_object_member("destroy_node"))
            {
                global_response_writer.push_object_member("destroy_node");
                ops::destroy_node_query(get_scene(), query_reader, global_response_writer);
                global_response_writer.pop(); // "destroy_node"
                query_reader.pop();
                wrote_output = true;
            }

            // Handle a query to set a node's name
            if (query_reader.pull_object_member("node_name_update"))
            {
                global_response_writer.push_object_member("node_name_update");
                ops::node_name_update_query(get_scene(), query_reader, global_response_writer);
                global_response_writer.pop(); // "node_name_update"
                query_reader.pop();
                wrote_output = true;
            }

            // Handle a query to set a node's root
            if (query_reader.pull_object_member("node_root_update"))
            {
                global_response_writer.push_object_member("node_root_update");
                ops::node_root_update_query(get_scene(), query_reader, global_response_writer);
                global_response_writer.pop(); // "node_root_update"
                query_reader.pop();
                wrote_output = true;
            }

            // Handle a query to set a node's transform
            if (query_reader.pull_object_member("node_local_transform_update"))
            {
                global_response_writer.push_object_member("node_local_transform_update");
                ops::node_local_transform_update_query(get_scene(), query_reader, global_response_writer);
                global_response_writer.pop(); // "node_local_transform_update"
                query_reader.pop();
                wrote_output = true;
            }

            // Handle a query to create a component
            if (query_reader.pull_object_member("new_component"))
            {
                global_response_writer.push_object_member("new_component");
                ops::new_component_query(get_scene(), query_reader, global_response_writer);
                global_response_writer.pop(); // "new_component"
                query_reader.pop();
                wrote_output = true;
            }

            // Handle a query to destroy a component
            if (query_reader.pull_object_member("destroy_component"))
            {
                global_response_writer.push_object_member("destroy_component");
                ops::destroy_component_query(get_scene(), query_reader, global_response_writer);
                global_response_writer.pop(); // "destroy_component"
                query_reader.pop();
                wrote_output = true;
            }

            // Handle a query to set the properties on a component
            if (query_reader.pull_object_member("component_property_update"))
            {
                global_response_writer.push_object_member("component_property_update");
                ops::component_property_update_query(get_scene(), query_reader, global_response_writer);
                global_response_writer.pop(); // "component_property_update"
                query_reader.pop();
                wrote_output = true;
            }

            return wrote_output;
        }

        bool Session::post_write_ops(ArchiveReader& query_reader, ArchiveWriter& sender_response_writer)
        {
            bool wrote_output = false;

            // Handle a query for all component types
            if (query_reader.pull_object_member("get_component_types"))
            {
                sender_response_writer.push_object_member("get_component_types");
                ops::get_component_types_query(get_scene(), sender_response_writer);
                sender_response_writer.pop(); // "get_component_types"
                query_reader.pop();
                wrote_output = true;
            }

            // Handle a property info query
            if (query_reader.pull_object_member("get_type_info"))
            {
                sender_response_writer.push_object_member("get_type_info");
                ops::get_type_info_query(get_scene().get_type_db(), query_reader, sender_response_writer);
                sender_response_writer.pop(); // "get_type_info"
                query_reader.pop();
                wrote_output = true;
            }

            // Handle a query for acessing the scene structure
            if (query_reader.pull_object_member("get_scene"))
            {
                sender_response_writer.push_object_member("get_scene");
                ops::get_scene_query(get_scene(), sender_response_writer);
                sender_response_writer.pop(); // "get_scene"
                query_reader.pop();
                wrote_output = true;
            }

            // Handle a resource query
            if (query_reader.pull_object_member("get_resource"))
            {
                sender_response_writer.push_object_member("get_resource");
                ops::get_resource_query(get_scene(), query_reader, sender_response_writer);
                sender_response_writer.pop(); // "get_resource"
                query_reader.pop();
                wrote_output = true;
            }

            // Handle a lightmap query
            /* if (query_reader.pull_object_member("gen_lightmaps")) */
            /* { */
            /*     std::cout << "Generating lightmaps..." << std::endl; */

            /*     // Generate lightmaps, with timer */
            /*     const auto start = std::chrono::high_resolution_clock::now(); */
            /*     ops::generate_lightmaps(get_scene(), query_reader); */
            /*     const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start); */
            /*     sender_response_writer.object_member("gen_lightmaps", duration.count()); */
            /*     query_reader.pop(); */
            /*     std::cout << "Lightmaps generated." << std::endl; */
            /*     wrote_output = true; */
            /* } */

            // Handle a save query
            if (query_reader.pull_object_member("save_scene"))
            {
                ops::save_scene_query(get_scene(), query_reader);
                query_reader.pop();
            }

            return wrote_output;
        }

        void Session::report_error(std::error_code error)
        {
            std::cout << "Error occurred: '" << error << "', this may be expected." << std::endl;
        }
    }
}

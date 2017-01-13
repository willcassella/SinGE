// Session.cpp

#include <iostream>
#include <Resource/Archives/JsonArchive.h>
#include <Engine/Scene.h>
#include <Engine/SystemFrame.h>
#include "../private/Session.h"
#include "../private/EditorOps.h"

namespace sge
{
	namespace editor_server
	{
		SystemFrameMut& Session::get_frame()
		{
			return *_system_data->frame;
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
			asio::async_write(socket, asio::buffer(_outgoing_packets.front(), _outgoing_packets.front()->size()),
				[self = shared_from_this()](const std::error_code& error, std::size_t /*bytes*/)
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

		void Session::async_receive_client_message_header()
		{
			asio::async_read(socket, asio::buffer(&_in_content_length, sizeof(Packet::ContentLength_t)),
				[self = shared_from_this()](const std::error_code& error, std::size_t /*len*/)
			{
				if (!error)
				{
					// Allocate space for content
					self->_in_content.assign(self->_in_content_length, 0);

					// Read content
					self->async_receive_message();
				}
				else
				{
					report_error(error);
				}
			});
		}

		void Session::async_receive_message()
		{
			asio::async_read(socket, asio::buffer(&_in_content[0], _in_content.size()),
				[self = shared_from_this()](const std::error_code& error, std::size_t /*len*/)
			{
				if (!error)
				{
					// Log query time
					const auto message_time = std::chrono::system_clock::now();
					std::cout << "Received query at " << std::chrono::system_clock::to_time_t(message_time) << std::endl;

					// Deserialze the string
					JsonArchive archive;
					archive.from_string(self->_in_content.c_str());
					auto* in_reader = archive.read_root();

					// Handle a query to create an entity
					if (in_reader->pull_object_member("new_entity"))
					{
						ops::new_entity_query(self->get_frame(), *in_reader);
						in_reader->pop();
					}

					// Handle a query to set an entity's name
					if (in_reader->pull_object_member("set_entity_name"))
					{
						ops::set_entity_name_query(self->get_frame(), *in_reader);
						in_reader->pop();
					}

					// Handle a query to set an entity's parent
					if (in_reader->pull_object_member("set_entity_parent"))
					{
						ops::set_entity_parent_query(self->get_frame(), *in_reader);
						in_reader->pop();
					}

					// Handle a query to destroy a component
					if (in_reader->pull_object_member("destroy_component"))
					{
						ops::destroy_component_query(self->get_frame(), *in_reader);
						in_reader->pop();
					}

					// Handle a query to destroy an entity
					if (in_reader->pull_object_member("destroy_entity"))
					{
						ops::destroy_entity_query(self->get_frame(), *in_reader);
						in_reader->pop();
					}

					// Handle a query to create a component
					if (in_reader->pull_object_member("new_component"))
					{
						ops::new_component_query(self->get_frame(), *in_reader);
						in_reader->pop();
					}

					// Synchronize the frame, so we can see changes that were made
					self->get_frame().sync();

					// Handle a query to set the properties on a component
					if (in_reader->pull_object_member("set_component"))
					{
						ops::set_component_query(self->get_frame(), *in_reader);
						in_reader->pop();
					}

					// Create an output archive
					JsonArchive out;
					auto* out_writer = out.write_root();
					bool wrote_output = false;

					// Handle a query for all component types
					if (in_reader->pull_object_member("get_component_types"))
					{
						out_writer->push_object_member("get_component_types");
						ops::get_component_types_query(self->get_frame().get_scene(), *out_writer);
						out_writer->pop();
						in_reader->pop();
						wrote_output = true;
					}

					// Handle a property info query
					if (in_reader->pull_object_member("get_type_info"))
					{
						out_writer->push_object_member("get_type_info");
						ops::get_type_info_query(self->get_frame().get_scene().get_type_db(), *in_reader, *out_writer);
						out_writer->pop();
						in_reader->pop();
						wrote_output = true;
					}

					// Handle a query for acessing the scene structure
					if (in_reader->pull_object_member("get_scene"))
					{
						out_writer->push_object_member("get_scene");
						ops::get_scene_query(self->get_frame(), *out_writer);
						out_writer->pop();
						in_reader->pop();
						wrote_output = true;
					}

					// Handle an object property query
					if (in_reader->pull_object_member("get_component"))
					{
						out_writer->push_object_member("get_component");
						ops::get_component_query(self->get_frame(), *in_reader, *out_writer);
						in_reader->pop();
						out_writer->pop();
						wrote_output = true;
					}

					// Handle a resource query
					if (in_reader->pull_object_member("get_resource"))
					{
						out_writer->push_object_member("get_resource");
						ops::get_resource_query(self->get_frame().get_scene(), *in_reader, *out_writer);
						in_reader->pop();
						out_writer->pop();
						wrote_output = true;
					}

					// Handle a save query
					if (in_reader->pull_object_member("save_scene"))
					{
						ops::save_scene_query(self->get_frame().get_scene(), *in_reader);
						in_reader->pop();
					}

					// Close reader and writer
					in_reader->pop();
					out_writer->pop();

					// Log completion time
					auto completed_time = std::chrono::system_clock::now();
					std::cout << "Completed query at " << std::chrono::system_clock::to_time_t(completed_time) << std::endl;
					std::cout << std::endl;

					// If we wrote something, send it
					if (wrote_output)
					{
						// Write result
						std::string result = out.to_string();
						auto* packet = Packet::encode_packet(Packet::FLAG_NONE, result.c_str(), result.size());
						self->enequeue_message(packet);
					}

					// Wait for a new message
					self->async_receive_client_message_header();
				}
				else
				{
					report_error(error);
				}
			});
		}

		void Session::report_error(std::error_code error)
		{
			std::cout << "Error occurred: '" << error << "', this may be expected." << std::endl;
		}
	}
}

// EditorServerSystemData.h
#pragma once

#include <queue>
#include <iostream>
#include <asio.hpp>
#include <Resource/Archives/JsonArchive.h>
#include "../include/EditorServerSystem/EditorServerSystem.h"
#include "EditorPacket.h"
#include "EditorOps.h"

namespace sge
{
	using asio::ip::tcp;

	struct EditorServerSession
	: std::enable_shared_from_this<EditorServerSession>
	{
		////////////////////////
		///   Constructors   ///
	public:

		EditorServerSession(asio::io_service& io, Scene& scene)
			: socket(io),
			_scene(&scene),
			_in_content_length(0)
		{
		}
		~EditorServerSession()
		{
			socket.close();
			std::cout << "Session successfully closed." << std::endl;
		}

		//////////////////
		///   Events   ///
	public:

		/* Sends the entire state of the scene. */
		void async_send_scene()
		{
			// Create a Json archive for the scene
			JsonArchive archive;
			archive.serialize_root(*_scene);

			// Create a packet and add it to the queue
			auto content = archive.to_string();
			auto* message = EditorPacket::encode_packet(content.c_str(), content.size());

			// Send it
			enequeue_message(message);
		}

		void enequeue_message(EditorPacket* packet)
		{
			// Add the message to the queue
			_outgoing_packets.push(packet);

			// If the packet we just added is the only packet in the queue, we need to send it
			if (_outgoing_packets.size() == 1)
			{
				async_send_message();
			}
		}

		/* Sends a message to the client. */
		void async_send_message()
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
				EditorPacket::free(self->_outgoing_packets.front());
				self->_outgoing_packets.pop();

				// Go again
				self->async_send_message();
			});
		}

		/* Handles a message from the client, reads the number of bytes to receive. */
		void async_receive_client_message_header()
		{
			asio::async_read(socket, asio::buffer(&_in_content_length, sizeof(EditorPacket::ContentLength_t)),
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

		/* Handles a change received from the client, reads the JSON body of the packet. */
		void async_receive_message()
		{
			asio::async_read(socket, asio::buffer(&_in_content[0], _in_content.size()),
				[self = shared_from_this()](const std::error_code& error, std::size_t /*len*/)
			{
				if (!error)
				{
					// Deserialze the string
					JsonArchive archive;
					archive.from_string(self->_in_content.c_str());
					auto* in_reader = archive.read_root();

					// Handle a query to create an entity
					if (in_reader->pull_object_member("new_entity"))
					{
						editor_ops::new_entity_query(*self->_scene, *in_reader);
						in_reader->pop();
					}

					// Handle a query to set an entity's name
					if (in_reader->pull_object_member("set_entity_name"))
					{
						editor_ops::set_entity_name_query(*self->_scene, *in_reader);
						in_reader->pop();
					}

					// Handle a query to set an entity's parent
					if (in_reader->pull_object_member("set_entity_parent"))
					{
						editor_ops::set_entity_parent_query(*self->_scene, *in_reader);
						in_reader->pop();
					}

					// Handle a query to create a component
					if (in_reader->pull_object_member("new_component"))
					{
						editor_ops::new_component_query(*self->_scene, *in_reader);
						in_reader->pop();
					}

					// Handle a query to set the properties on a component
					if (in_reader->pull_object_member("set_component"))
					{
						editor_ops::set_component_query(*self->_scene, *in_reader);
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
						editor_ops::get_component_types_query(*self->_scene, *out_writer);
						out_writer->pop();
						in_reader->pop();
						wrote_output = true;
					}

					// Handle a property info query
					if (in_reader->pull_object_member("get_type_info"))
					{
						out_writer->push_object_member("get_type_info");
						editor_ops::get_type_info_query(*self->_scene, *in_reader, *out_writer);
						out_writer->pop();
						in_reader->pop();
						wrote_output = true;
					}

					// Handle a query for acessing the scene structure
					if (in_reader->pull_object_member("get_scene"))
					{
						out_writer->push_object_member("get_scene");
						editor_ops::get_scene_query(*self->_scene, *out_writer);
						out_writer->pop();
						in_reader->pop();
						wrote_output = true;
					}

					// Handle an object property query
					if (in_reader->pull_object_member("get_component"))
					{
						out_writer->push_object_member("get_component");
						editor_ops::get_component_query(*self->_scene, *in_reader, *out_writer);
						in_reader->pop();
						out_writer->pop();
						wrote_output = true;
					}

					// Handle a resource query
					if (in_reader->pull_object_member("get_resource"))
					{
						out_writer->push_object_member("get_resource");
						editor_ops::get_resource(*self->_scene, *in_reader, *out_writer);
						in_reader->pop();
						out_writer->pop();
						wrote_output = true;
					}

					// Close reader and writer
					in_reader->pop();
					out_writer->pop();

					// If we wrote something, send it
					if (wrote_output)
					{
						// Write result
						std::string result = out.to_string();
						auto* packet = EditorPacket::encode_packet(result.c_str(), result.size());
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

		static void report_error(std::error_code error)
		{
			std::cout << "Error occurred: '" << error << "', this may be expected." << std::endl;
		}

		//////////////////
		///   Fields   ///
	public:

		tcp::socket socket;

	private:

		Scene* _scene;

		// Outgoing data
		std::queue<EditorPacket*> _outgoing_packets;

		// Incoming content data
		std::string _in_content;
		EditorPacket::ContentLength_t _in_content_length;
	};

	struct EditorServerSystem::Data
	{
		////////////////////////
		///   Constructors   ///
	public:

		Data(Scene& scene, uint16 port)
			: io(),
			_scene(&scene),
			_acceptor(io, tcp::endpoint{ tcp::v4(), port })
		{
			io.post([this]()
			{
				this->async_connection();
			});
		}

		//////////////////
		///   Events   ///
	private:

		void async_connection()
		{
			// Create a session for the connection
			auto session = std::make_shared<EditorServerSession>(io, *_scene);
			_acceptor.async_accept(session->socket, [this, session](const std::error_code& /*error*/)
			{
				// Prepare for a new connection
				this->async_connection();

				std::cout << "Session opened with " << session->socket.remote_endpoint() << std::endl;

				// Start the session
				session->async_receive_client_message_header();
			});
		}

		//////////////////
		///   Fields   ///
	public:

		asio::io_service io;

	public:

		Scene* _scene;
		tcp::acceptor _acceptor;
	};
}

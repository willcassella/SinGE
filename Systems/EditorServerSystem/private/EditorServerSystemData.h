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
			_out_change_queue.push(EditorPacket::encode_packet(content.c_str(), content.size()));

			// Send it
			async_send_message();
		}

		/* Sends a message to the client. */
		void async_send_message()
		{
			asio::async_write(socket, asio::buffer(_out_change_queue.front(), _out_change_queue.front()->size()),
				[self = shared_from_this()](const std::error_code& error, std::size_t /*bytes*/)
			{
				if (!error)
				{
					// Get rid of the packet
					EditorPacket::free(self->_out_change_queue.front());
					self->_out_change_queue.pop();

					// If we have more to send, go again
					if (!self->_out_change_queue.empty())
					{
						self->async_send_message();
					}
				}
				else
				{
					self->close_session();
				}
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
					self->close_session();
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

					if (in_reader->pull_object_member("set_component"))
					{
						editor_ops::set_component_query(*self->_scene, *in_reader);
						in_reader->pop();
					}

					// Create an output archive
					JsonArchive out;
					auto* out_writer = out.write_root();

					// Handle a query for all component types
					if (in_reader->pull_object_member("get_component_types"))
					{
						out_writer->push_object_member("get_component_types");
						editor_ops::get_component_types_query(*self->_scene, *out_writer);
						out_writer->pop();
						in_reader->pop();
					}

					// Handle a property info query
					if (in_reader->pull_object_member("get_type_info"))
					{
						out_writer->push_object_member("get_type_info");
						editor_ops::get_type_info_query(*self->_scene, *in_reader, *out_writer);
						out_writer->pop();
						in_reader->pop();
					}

					// Handle a query for acessing the scene structure
					if (in_reader->pull_object_member("get_scene"))
					{
						out_writer->push_object_member("get_scene");
						editor_ops::get_scene_query(*self->_scene, *out_writer);
						out_writer->pop();
						in_reader->pop();
					}

					// Handle an object property query
					if (in_reader->pull_object_member("get_component"))
					{
						out_writer->push_object_member("get_component");
						editor_ops::get_component_query(*self->_scene, *in_reader, *out_writer);
						in_reader->pop();
						out_writer->pop();
					}

					// Close reader and writer
					in_reader->pop();
					out_writer->pop();

					// Write result
					std::string result = out.to_string();
					self->_out_change_queue.push(EditorPacket::encode_packet(result.c_str(), result.size()));
					self->async_send_message();

					// Prepare for new changes
					self->async_receive_client_message_header();
				}
				else
				{
					self->close_session();
				}
			});
		}

		/* Closes a session with the client. */
		void close_session()
		{
			std::cout << "Closing session with " << socket.remote_endpoint() << std::endl;
			socket.close();
		}

		//////////////////
		///   Fields   ///
	public:

		tcp::socket socket;

	private:

		Scene* _scene;

		// Incoming content data
		std::string _in_content;
		EditorPacket::ContentLength_t _in_content_length;

		// Outgoing content
		std::queue<EditorPacket*> _out_change_queue;
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

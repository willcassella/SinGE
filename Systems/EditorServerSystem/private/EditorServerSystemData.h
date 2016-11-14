// EditorServerSystemData.h
#pragma once

#include <queue>
#include <iostream>
#include <asio.hpp>
#include <Resource/Archives/JsonArchive.h>
#include "../include/EditorServerSystem/EditorServerSystem.h"
#include "EditorPacket.h"

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
			async_send_change();
		}

		/* Sends a change to the client. */
		void async_send_change()
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
						self->async_send_change();
					}
				}
				else
				{
					self->close_session();
				}
			});
		}

		/* Handles a change received from the client, reads the number of bytes to receive. */
		void async_receive_change_header()
		{
			asio::async_read(socket, asio::buffer(&_in_content_length, sizeof(EditorPacket::ContentLength_t)),
				[self = shared_from_this()](const std::error_code& error, std::size_t /*len*/)
			{
				if (!error)
				{
					// Allocate space for content
					self->_in_content.assign(self->_in_content_length, 0);

					// Read content
					self->async_receive_change();
				}
				else
				{
					self->close_session();
				}
			});
		}

		/* Handles a change received from the client, reads the JSON body of the packet. */
		void async_receive_change()
		{
			asio::async_read(socket, asio::buffer(&_in_content[0], _in_content.size()),
				[self = shared_from_this()](const std::error_code& error, std::size_t /*len*/)
			{
				if (!error)
				{
					// Deserialze the string
					JsonArchive archive;
					archive.from_string(self->_in_content.c_str());

					// Read the change
					auto* scene = self->_scene; // MSVC BUG WORKAROUND?
					archive.get_root([scene](const ArchiveReader& reader)
					{
						// Handle component modifications
						reader.object_member("component_mod", [scene](const ArchiveReader& modReader)
						{
							// Enumerate types of components changed
							modReader.enumerate_object_members([scene](const char* typeName, const ArchiveReader& typeReader)
							{
								auto* type = scene->get_component_type(typeName);
								if (!type)
								{
									return;
								}

								// Enumerate the EntityIds of components changed
								typeReader.enumerate_object_members([=](const char* entityId, const ArchiveReader& instanceReader)
								{
									// Get the component Id
									ComponentId id{ std::strtoull(entityId, nullptr, 10), *type };

									// Process the component and deserialize it
									scene->process_single_mut(id.entity(), &type, 1,
										[&instanceReader](ProcessingFrame&, EntityId, auto comp)
									{
										comp[0]->from_archive(instanceReader);
									});
								});
							});
						});
					});

					// Prepare for new changes
					self->async_receive_change_header();
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
			_acceptor.async_accept(session->socket,
				[this, session](const std::error_code& /*error*/)
			{
				// Prepare for a new connection
				this->async_connection();

				std::cout << "Session opened with " << session->socket.remote_endpoint() << std::endl;

				// Start the session
				session->async_send_scene();
				session->async_receive_change();
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

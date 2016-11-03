// EditorServerSystemData.h
#pragma once

#include <queue>
#include <asio.hpp>
#include <Core/Memory/Functions.h>
#include <Resource/Archives/JsonArchive.h>
#include "../include/EditorServerSystem/EditorServerSystem.h"
#include "EditorPacket.h"

namespace sge
{
	using asio::ip::tcp;

	struct EditorServerSession
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

		//////////////////
		///   Events   ///
	public:

		/* Sends the entire state of the scene. */
		void do_send_scene()
		{
			// Create a Json archive for the scene
			JsonArchive archive;
			archive.serialize_root(*_scene);

			// Create a packet and add it to the queue
			auto content = archive.dump_string();
			_out_change_queue.push(EditorPacket::encode_packet(content.c_str(), content.size()));

			// Send it
			do_send_change();
		}

		/* Sends a change to the client. */
		void do_send_change()
		{
			asio::async_write(socket, asio::buffer(_out_change_queue.front(), _out_change_queue.front()->size()),
				[this](const std::error_code& error, std::size_t /*bytes*/)
			{
				if (!error)
				{
					// Get rid of the packet
					EditorPacket::free(this->_out_change_queue.front());
					this->_out_change_queue.pop();

					// If we have more to send, go again
					if (!this->_out_change_queue.empty())
					{
						this->do_send_change();
					}
				}
				else
				{
					this->socket.close();
				}
			});
		}

		/* Handles a change received from the client. */
		void do_receive_change_header()
		{
			asio::async_read(socket, asio::buffer(&_in_content_length, sizeof(EditorPacket::ContentLength_t)),
				[this](const std::error_code& error, std::size_t len)
			{
				// Allocate space for content
				this->_in_content.assign(_in_content_length, 0);

				// Read content
				this->do_receive_change();
			});
		}

		void do_receive_change()
		{
			asio::async_read(socket, asio::buffer(_in_content),
				[this](const std::error_code& error, std::size_t len)
			{
				// Deserialze the string
				JsonArchive archive;
				archive.parse_string(_in_content.c_str());

				// Apply changes to scene
				archive.deserialize_root(*this->_scene);

				// Prepare for new changes
				this->do_receive_change_header();
			});
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
			asio::post(io, [this]()
			{
				this->do_connection();
			});
		}

		//////////////////
		///   Events   ///
	private:

		void do_connection()
		{
			// Create a session for the connection
			_pending_session = std::make_unique<EditorServerSession>(io, *_scene);
			_acceptor.async_accept(_pending_session->socket, [this](const std::error_code& error)
			{
				// Set this as an active session
				auto* session = this->_pending_session.get();
				this->_active_sessions.push_back(std::move(this->_pending_session));

				// Prepare for a new connection
				this->do_connection();

				// Start the session
				session->do_send_scene();
				//session->do_receive_change();
			});
		}

		//////////////////
		///   Fields   ///
	public:

		asio::io_service io;

	public:

		Scene* _scene;
		tcp::acceptor _acceptor;
		std::unique_ptr<EditorServerSession> _pending_session;
		std::vector<std::unique_ptr<EditorServerSession>> _active_sessions;
	};
}

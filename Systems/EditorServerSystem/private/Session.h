// Session.h
#pragma once

#include <queue>
#include <memory>
#include <asio.hpp>
#include <Resource/Archives/JsonArchive.h>
#include "EditorServerSystemData.h"
#include "Packet.h"

namespace sge
{
	namespace editor_server
	{
		struct Session : std::enable_shared_from_this<Session>
		{
			////////////////////////
			///   Constructors   ///
		public:

			Session(asio::io_service& io, EditorServerSystem::Data& system_data)
				: socket(io),
				_system_data(&system_data)
			{
			}
			~Session()
			{
				socket.close();
				std::cout << "Session successfully closed." << std::endl;
			}

			///////////////////
			///   Methods   ///
		public:

			SystemFrame& get_frame();

			void enequeue_message(Packet* packet);

			/**
			 * \brief Queues up the job to send a message to the client.
			 * This will automatically loop as long as there are messages to be sent.
			 */
			void async_send_message();

			/* Receives a message from the client. */
			void async_receive_message();

            void write_ops();

            void async_post_write_ops();

			static void report_error(std::error_code error);

			//////////////////
			///   Fields   ///
		public:

			tcp::socket socket;

		private:

			/* Session object needs access to system data (to access scene data). */
			EditorServerSystem::Data* _system_data;

			/* Outgoing messages */
			std::queue<Packet*> _outgoing_packets;

			/* Incoming message */
			Packet::Header_t _in_header;
			std::string _in_content;
            JsonArchive _in_archive;
		};
	}
}

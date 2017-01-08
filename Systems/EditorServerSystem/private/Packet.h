// Packet.h
#pragma once

#include <cstring>
#include <Core/Memory/Functions.h>

namespace sge
{
	namespace editor_server
	{
		struct Packet
		{
			using ContentLength_t = uint32;

			////////////////////////
			///   Constructors   ///
		public:

			static Packet* encode_packet(const char* str, std::size_t len)
			{
				// Create the packet header
				auto* packet = static_cast<Packet*>(sge::malloc(sizeof(Packet) + len));
				packet->_length = static_cast<ContentLength_t>(len);

				// Copy the string into the packet
				std::memcpy(packet + 1, str, len);

				return packet;
			}

			static void free(Packet* packet)
			{
				sge::free(packet);
			}

			///////////////////
			///   Methods   ///
		public:

			/* Returns the size of the entire packet. */
			std::size_t size() const
			{
				return sizeof(Packet) + _length;
			}

			/* Returns the length of the content in this packet. */
			std::size_t content_length() const
			{
				return _length;
			}

			/* Returns a pointer to the content in this packet. */
			const char* content() const
			{
				return reinterpret_cast<const char*>(this + 1);
			}

			//////////////////
			///   Fields   ///
		private:

			ContentLength_t _length = 0;
		};
	}
}

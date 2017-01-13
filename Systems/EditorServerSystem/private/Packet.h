// Packet.h
#pragma once

#include <cstring>
#include <Core/Memory/Functions.h>

namespace sge
{
	namespace editor_server
	{
		struct alignas(char) Packet
		{
			/**
			 * \brief The flag is the first element of the packet, and indicates things about it.
			 */
			using Flag_t = char;

			/**
			 * \brief This message has no special flag.
			 */
			static constexpr Flag_t FLAG_NONE = ' ';

			/**
			 * \brief This packet is a global editor packet, and should not be ignored.
			 */
			static constexpr Flag_t FLAG_GLOBAL = 'G';

			/**
			 * \brief The content length proceeds the message.
			 */
			using ContentLength_t = uint32;

			////////////////////////
			///   Constructors   ///
		public:

			static Packet* encode_packet(Flag_t flag, const char* str, std::size_t len)
			{
				// Create the packet buffer
				auto* packet = (Packet*)sge::malloc(sizeof(Flag_t) + sizeof(ContentLength_t) + len);

				// Insert the flag
				new (packet) Flag_t(flag);

				// Insert the content length
				new (packet + sizeof(Flag_t)) ContentLength_t(len);

				// Copy the content into the packet
				std::memcpy(packet + sizeof(Flag_t) + sizeof(ContentLength_t), str, len);

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
				return sizeof(Flag_t) + sizeof(ContentLength_t) + content_length();
			}

			Flag_t flag() const
			{
				return *reinterpret_cast<const Flag_t*>(this);
			}

			/* Returns the length of the content in this packet. */
			std::size_t content_length() const
			{
				return *reinterpret_cast<const ContentLength_t*>(this + sizeof(Flag_t));
			}

			/* Returns a pointer to the content in this packet. */
			const char* content() const
			{
				return reinterpret_cast<const char*>(this + sizeof(Flag_t) + sizeof(ContentLength_t));
			}
		};
	}
}

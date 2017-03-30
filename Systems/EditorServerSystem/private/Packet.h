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
			 * \brief Represents the sequence number of the packet (global packets always have sequence number 0).
			 */
			using SequenceNumber_t = uint32;

			/**
			 * \brief Sequence number used when there is no corresponding sequence number to send.
			 */
			static constexpr SequenceNumber_t NULL_SEQUENCE_NUMBER = 0;

			/**
			 * \brief Offset of the sequence number object in the header.
			 */
			static constexpr std::size_t HEADER_SEQUENCE_NUMBER_OFFSET = 0;

			/**
			 * \brief The content length proceeds the message.
			 */
			using ContentLength_t = uint32;

			/**
			 * \brief Offset of the content length object in the header.
			 */
			static constexpr std::size_t HEADER_CONTENT_LENGTH_OFFSET = HEADER_SEQUENCE_NUMBER_OFFSET + sizeof(SequenceNumber_t);

			/**
			 * \brief Size of header data (everything but content).
			 */
			static constexpr std::size_t HEADER_SIZE = HEADER_CONTENT_LENGTH_OFFSET + sizeof(ContentLength_t);

			/**
			 * \brief Offset of the content in the packet buffer.
			 */
			static constexpr std::size_t PACKET_CONTENT_OFFSET = HEADER_SIZE;

			/**
			 * \brief Type used to represent a packet header.
			 */
			using Header_t = byte[HEADER_SIZE];

			////////////////////////
			///   Constructors   ///
		public:

			static Packet* encode_packet(SequenceNumber_t sequence_number, const char* str, ContentLength_t len)
			{
				// Create the packet buffer
				auto* packet = (Packet*)sge::malloc(HEADER_SIZE + len);

				// Insert the sequence number
				new (packet + HEADER_SEQUENCE_NUMBER_OFFSET) SequenceNumber_t(sequence_number);

				// Insert the content length
				new (packet + HEADER_CONTENT_LENGTH_OFFSET) ContentLength_t(len);

				// Copy the content into the packet
				std::memcpy(packet + PACKET_CONTENT_OFFSET, str, len);

				return packet;
			}

			static void free(Packet* packet)
			{
				sge::free(packet);
			}

			///////////////////
			///   Methods   ///
		public:

			/* Returns the sequence number within the given header. */
			static SequenceNumber_t sequence_number(const Header_t& header)
			{
				return *reinterpret_cast<const SequenceNumber_t*>(&header[0] + HEADER_SEQUENCE_NUMBER_OFFSET);
			}

			/* Returns the length of the content in this packet. */
			static ContentLength_t content_length(const Header_t& header)
			{
				return *reinterpret_cast<const ContentLength_t*>(&header[0] + HEADER_CONTENT_LENGTH_OFFSET);
			}

			/**
			 * \brief Returns the total size of this packet.
			 */
			std::size_t packet_size() const
			{
				return HEADER_SIZE + content_length(packet_header());
			}

			/**
			 * \brief Accesses the header object within this packet.
			 */
			const Header_t& packet_header() const
			{
				return *reinterpret_cast<const Header_t*>(this);
			}

			/**
			 * \brief Accesses the message inside this packet (NOT null-terminated).
			 */
			const char* packet_content() const
			{
				return reinterpret_cast<const char*>(this + PACKET_CONTENT_OFFSET);
			}
		};
	}
}

#pragma once

#include <memory>
#include <queue>

#include <asio.hpp>

#include "lib/editor_server/editor_server_system_data.h"
#include "lib/editor_server/packet.h"
#include "lib/resource/archives/json_archive.h"

namespace sge {
namespace editor_server {
struct Session : std::enable_shared_from_this<Session> {
  Session(asio::io_service& io, EditorServerSystem::Data& system_data)
      : socket(io), _system_data(&system_data) {}
  ~Session() {
    socket.close();
    std::cout << "Session successfully closed." << std::endl;
  }

  SystemFrame& get_frame();

  Scene& get_scene();

  void enequeue_message(Packet* packet);

  /**
   * \brief Queues up the job to send a message to the client.
   * This will automatically loop as long as there are messages to be sent.
   */
  void async_send_message();

  /* Receives a message from the client. */
  void async_receive_message();

  bool write_ops(ArchiveReader& query_reader, ArchiveWriter& global_response_writer);

  bool post_write_ops(ArchiveReader& query_reader, ArchiveWriter& sender_response_writer);

  static void report_error(std::error_code error);

  tcp::socket socket;

 private:
  /* Session object needs access to system data (to access scene data). */
  EditorServerSystem::Data* _system_data;

  /* Outgoing messages */
  std::queue<Packet*> _outgoing_packets;

  /* Incoming message */
  Packet::Header_t _in_header;
  std::string _in_content;
};
}  // namespace editor_server
}  // namespace sge

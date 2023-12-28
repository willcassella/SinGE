#include <stdint.h>
#include <iostream>

#include "lib/base/reflection/reflection_builder.h"
#include "lib/editor_server/editor_server_system_data.h"
#include "lib/editor_server/session.h"

SGE_REFLECT_TYPE(sge::EditorServerSystem);

namespace sge {
EditorServerSystem::Data::Data(uint16_t port)
    : frame(nullptr), scene(nullptr), _acceptor(io, tcp::endpoint{tcp::v4(), port}) {
  io.post([this]() { this->async_connection(); });
}

void EditorServerSystem::Data::async_connection() {
  // Create a session for the connection
  auto session = std::make_shared<editor_server::Session>(io, *this);
  _acceptor.async_accept(session->socket, [this, session](const std::error_code& /*error*/) {
    // Prepare for a new connection
    this->async_connection();

    std::cout << "Session opened with " << session->socket.remote_endpoint() << std::endl;

    // Start the session
    session->async_receive_message();
  });
}
}  // namespace sge

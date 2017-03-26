// EditorServerSystemData.cpp

#include <iostream>
#include <Core/Reflection/ReflectionBuilder.h>
#include "../private/EditorServerSystemData.h"
#include "../private/Session.h"

SGE_REFLECT_TYPE(sge::EditorServerSystem);

namespace sge
{
	EditorServerSystem::Data::Data(uint16 port)
		: frame(nullptr),
		scene(nullptr),
		_acceptor(io, tcp::endpoint{ tcp::v4(), port })
	{
		io.post([this]()
		{
			this->async_connection();
		});
	}

	void EditorServerSystem::Data::async_connection()
	{
		// Create a session for the connection
		auto session = std::make_shared<editor_server::Session>(io, *this);
		_acceptor.async_accept(session->socket, [this, session](const std::error_code& /*error*/)
		{
			// Prepare for a new connection
			this->async_connection();

			std::cout << "Session opened with " << session->socket.remote_endpoint() << std::endl;

			// Start the session
			session->async_receive_message();
		});
	}
}

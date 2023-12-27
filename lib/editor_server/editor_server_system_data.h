#pragma once

#include <asio.hpp>

#include "lib/editor_server/editor_server_system.h"

namespace sge
{
    using asio::ip::tcp;

    struct EditorServerSystem::Data
    {
        Data(uint16 port);

    private:
        void async_connection();

    public:
        SystemFrame* frame;
        Scene* scene;
        asio::io_service io;

    private:
        tcp::acceptor _acceptor;
    };
}

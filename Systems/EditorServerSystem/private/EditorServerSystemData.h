// EditorServerSystemData.h
#pragma once

#include <asio.hpp>
#include "../include/EditorServerSystem/EditorServerSystem.h"

namespace sge
{
	using asio::ip::tcp;

	struct EditorServerSystem::Data
	{
		////////////////////////
		///   Constructors   ///
	public:

		Data(uint16 port);

		///////////////////
		///   Methods   ///
	private:

		void async_connection();

		//////////////////
		///   Fields   ///
	public:

		SystemFrame* frame;
		asio::io_service io;

	private:

		tcp::acceptor _acceptor;
	};
}

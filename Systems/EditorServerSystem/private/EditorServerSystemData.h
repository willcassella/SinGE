// EditorServerSystemData.h
#pragma once

#include <asio.hpp>
#include "../include/EditorServerSystem/EditorServerSystem.h"

namespace sge
{
	class SystemFrameMut;
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

		asio::io_service io;
		SystemFrameMut* frame;

	private:

		tcp::acceptor _acceptor;
	};
}

// ResourceId.h
#pragma once

#include <Core/Reflection/ReflectionBuilder.h>
#include "build.h"

namespace sge
{
	template <typename T>
	struct ResourceId
	{
        SGE_REFLECTED_TYPE;

		////////////////////////
		///   Constructors   ///
	public:

		ResourceId()
			: _uri()
		{
		}

		ResourceId(std::string uri)
			: _uri(std::move(uri))
		{
		}

		///////////////////
		///   Methods   ///
	public:

		void uri(std::string uri)
		{
			_uri = std::move(uri);
		}

		const std::string& uri() const
		{
			return _uri;
		}

		bool is_null() const
		{
			return _uri.empty();
		}

		//////////////////
		///   Fields   ///
	private:

		std::string _uri;
	};
}

template <typename T>
SGE_REFLECT_TYPE_TEMPLATE(sge::ResourceId, T);

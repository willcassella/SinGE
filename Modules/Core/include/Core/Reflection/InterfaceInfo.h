// InterfaceInfo.h
#pragma once

#include <unordered_map>
#include <string>

namespace sge
{
	struct TypeInfo;

	template <class I>
	struct InterfaceInfoBuilder;

	struct InterfaceInfo
	{
		struct Data
		{
			////////////////////////
			///   Constructors   ///
		public:

			Data() = default;

			//////////////////
			///   Fields   ///
		public:

			std::string name;
			std::unordered_map<const TypeInfo*, const void*> implementations;
		};

		////////////////////////
		///   Constructors   ///
	public:

		InterfaceInfo(Data data)
			: _data(std::move(data))
		{
		}

		template <class I>
		InterfaceInfo(InterfaceInfoBuilder<I>&& builder)
			: InterfaceInfo(std::move(builder).interface_data)
		{
		}

		///////////////////
		///   Methods   ///
	public:

		const std::string& name() const
		{
			return _data.name;
		}

		const void* get_implementation(const TypeInfo& type) const
		{
			auto iter = _data.implementations.find(&type);
			return iter == _data.implementations.end() ? nullptr : iter->second;
		}

		//////////////////
		///   Fields   ///
	private:

		Data _data;
	};
}

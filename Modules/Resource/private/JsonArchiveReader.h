// JsonArchiveReader.h
#pragma once

#include <rapidjson/document.h>
#include <Core/IO/ArchiveReader.h>
#include "Core/Memory/Functions.h"
#include <vector>

namespace sge
{
	class JsonArchiveReader final : public ArchiveReader
	{
		////////////////////////
		///   Constructors   ///
	public:

		JsonArchiveReader(const rapidjson::Value& node)
			: _node(node)
		{
		}

		///////////////////
		///   Methods   ///
	public:

		bool null() const override
		{
			return _node.IsNull();
		}

		bool value(bool& out) const override
		{
			return impl_value<bool, &rapidjson::Value::IsBool, &rapidjson::Value::GetBool>(out);
		}

		bool value(int8& out) const override
		{
			return impl_value<int, &rapidjson::Value::IsInt, &rapidjson::Value::GetInt>(out);
		}

		bool value(uint8& out) const override
		{
			return impl_value<unsigned int, &rapidjson::Value::IsUint, &rapidjson::Value::GetUint>(out);
		}

		bool value(int16& out) const override
		{
			return impl_value<int, &rapidjson::Value::IsInt, &rapidjson::Value::GetInt>(out);
		}

		bool value(uint16& out) const override
		{
			return impl_value<unsigned int, &rapidjson::Value::IsUint, &rapidjson::Value::GetUint>(out);
		}

		bool value(int32& out) const override
		{
			return impl_value<int, &rapidjson::Value::IsInt, &rapidjson::Value::GetInt>(out);
		}

		bool value(uint32& out) const override
		{
			return impl_value<unsigned int, &rapidjson::Value::IsUint, &rapidjson::Value::GetUint>(out);
		}

		bool value(int64& out) const override
		{
			return impl_value<int64_t, &rapidjson::Value::IsInt64, &rapidjson::Value::GetInt64>(out);
		}

		bool value(uint64& out) const override
		{
			return impl_value<uint64_t, &rapidjson::Value::IsUint64, &rapidjson::Value::GetUint64>(out);
		}

		bool value(float& out) const override
		{
			return impl_value<float, &rapidjson::Value::IsFloat, &rapidjson::Value::GetFloat>(out);
		}

		bool value(double& out) const override
		{
			return impl_value<double, &rapidjson::Value::IsDouble, &rapidjson::Value::GetDouble>(out);
		}

		bool value(const char*& outStr, std::size_t& outLen) const override
		{
			if (!_node.IsString())
			{
				return false;
			}

			outStr = _node.GetString();
			outLen = _node.GetStringLength();
			return true;
		}

		bool array_size(std::size_t& out) const override
		{
			if (!_node.IsArray())
			{
				return false;
			}

			out = _node.GetArray().Size();
			return true;
		}

		std::size_t typed_array(bool* out, std::size_t size) const override
		{
			return impl_typed_array<bool, &rapidjson::Value::IsBool, &rapidjson::Value::GetBool>(out, size);
		}

		std::size_t typed_array(int8* out, std::size_t size) const override
		{
			return impl_typed_array<int, &rapidjson::Value::IsInt, &rapidjson::Value::GetInt>(out, size);
		}

		std::size_t typed_array(uint8* out, std::size_t size) const override
		{
			return impl_typed_array<unsigned int, &rapidjson::Value::IsUint, &rapidjson::Value::GetUint>(out, size);
		}

		std::size_t typed_array(int16* out, std::size_t size) const override
		{
			return impl_typed_array<int, &rapidjson::Value::IsInt, &rapidjson::Value::GetInt>(out, size);
		}

		std::size_t typed_array(uint16* out, std::size_t size) const override
		{
			return impl_typed_array<unsigned int, &rapidjson::Value::IsUint, &rapidjson::Value::GetUint>(out, size);
		}

		std::size_t typed_array(int32* out, std::size_t size) const override
		{
			return impl_typed_array<int, &rapidjson::Value::IsInt, &rapidjson::Value::GetInt>(out, size);
		}

		std::size_t typed_array(uint32* out, std::size_t size) const override
		{
			return impl_typed_array<unsigned int, &rapidjson::Value::IsUint, &rapidjson::Value::GetUint>(out, size);
		}

		std::size_t typed_array(int64* out, std::size_t size) const override
		{
			return impl_typed_array<int64_t, &rapidjson::Value::IsInt64, &rapidjson::Value::GetInt64>(out, size);
		}

		std::size_t typed_array(uint64* out, std::size_t size) const override
		{
			return impl_typed_array<uint64_t, &rapidjson::Value::IsUint64, &rapidjson::Value::GetUint64>(out, size);
		}

		std::size_t typed_array(float* out, std::size_t size) const override
		{
			return impl_typed_array<float, &rapidjson::Value::IsFloat, &rapidjson::Value::GetFloat>(out, size);
		}

		std::size_t typed_array(double* out, std::size_t size) const override
		{
			return impl_typed_array<double, &rapidjson::Value::IsDouble, &rapidjson::Value::GetDouble>(out, size);
		}

		void enumerate_array_elements(FunctionView<void(std::size_t i, const ArchiveReader& elementReader)> enumerator) const override
		{
			if (!_node.IsArray())
			{
				return;
			}

			const auto& array = _node.GetArray();
			for (rapidjson::SizeType i = 0; i < array.Size(); ++i)
			{
				// Create a reader for the element
				JsonArchiveReader elementReader{ array[i] };

				// Call the enumerator
				enumerator(i, elementReader);
			}
		}

		bool array_element(std::size_t i, FunctionView<void(const ArchiveReader& elementReader)> func) const override
		{
			// We can't get the element if this node doesn't hold an array
			if (!_node.IsArray())
			{
				return false;
			}

			// Get the node as an array
			const auto& array = _node.GetArray();
			auto index = static_cast<rapidjson::SizeType>(i);

			// We can't get the element if its out of bounds
			if (array.Size() <= index)
			{
				return false;
			}

			// Create a reader for the element
			JsonArchiveReader elementReader{ array[index] };
			func(elementReader);

			return true;
		}

		bool object_num_members(std::size_t& out) const override
		{
			if (!_node.IsObject())
			{
				return false;
			}

			out = _node.MemberCount();
			return true;
		}

		void enumerate_object_members(FunctionView<void(const char* name, const ArchiveReader& memberReader)> enumerator) const override
		{
			if (!_node.IsObject())
			{
				return;
			}

			for (const auto& member : _node.GetObject())
			{
				// Create a reader for the member
				JsonArchiveReader memberReader{ member.value };

				// Call the enumerator
				enumerator(member.name.GetString(), memberReader);
			}
		}

		bool object_member(const char* name, FunctionView<void(const ArchiveReader& memberReader)> func) const override
		{
			// We can't get the member if this node doesn't hold an object
			if (!_node.IsObject())
			{
				return false;
			}

			// Get the node as an object
			const auto& object = _node.GetObject();

			// Try to get the member
			auto iter = object.FindMember(name);
			if (iter == object.MemberEnd())
			{
				return false;
			}

			// Create a reader for the member
			JsonArchiveReader memberReader{ iter->value };
			func(memberReader);

			return false;
		}

	private:

		template <typename RetT, bool(rapidjson::Value::*CheckerFn)() const, RetT(rapidjson::Value::*GetterFn)() const, typename T>
		bool impl_value(T& out) const
		{
			if (!(_node.*CheckerFn)())
			{
				return false;
			}

			out = static_cast<T>((_node.*GetterFn)());
			return true;
		}

		template <typename RetT, bool(rapidjson::Value::*CheckerFn)() const, RetT(rapidjson::Value::*GetterFn)() const, typename T>
		std::size_t impl_typed_array(T* out, std::size_t size) const
		{
			if (!_node.IsArray())
			{
				return 0;
			}

			const auto& array = _node.GetArray();
			rapidjson::SizeType i = 0;
			for (; i < size && i < array.Size(); ++i)
			{
				if (!(array[i].*CheckerFn)())
				{
					break;
				}

				out[i] = static_cast<T>((array[i].*GetterFn)());
			}

			return i;
		}

		//////////////////
		///   Fields   ///
	private:

		const rapidjson::Value& _node;
	};
}

// JsonArchiveWriter.h
#pragma once

#include <rapidjson/Document.h>
#include <Core/IO/ArchiveWriter.h>

namespace sge
{
	class JsonArchiveWriter final : public ArchiveWriter
	{
		using Allocator = rapidjson::MemoryPoolAllocator<>;

		////////////////////////
		///   Constructors   ///
	public:

		JsonArchiveWriter(rapidjson::Value& node, rapidjson::MemoryPoolAllocator<>& allocator)
			: _node(node),
			_allocator(allocator)
		{
		}

		///////////////////
		///   Methods   ///
	public:

		void null() override
		{
			_node.SetNull();
		}

		void boolean(bool value) override
		{
			_node.SetBool(value);
		}

		void value(int8 value) override
		{
			_node.SetInt(value);
		}

		void value(uint8 value) override
		{
			_node.SetUint(value);
		}

		void value(int16 value) override
		{
			_node.SetInt(value);
		}

		void value(uint16 value) override
		{
			_node.SetUint(value);
		}

		void value(int32 value) override
		{
			_node.SetInt(value);
		}

		void value(uint32 value) override
		{
			_node.SetUint(value);
		}

		void value(int64 value) override
		{
			_node.SetInt64(value);
		}

		void value(uint64 value) override
		{
			_node.SetUint64(value);
		}

		void value(float value) override
		{
			_node.SetFloat(value);
		}

		void value(double value) override
		{
			_node.SetDouble(value);
		}

		void string(const char* str, std::size_t len) override
		{
			_node.SetString(str, static_cast<rapidjson::SizeType>(len), _allocator);
		}

		void typed_array(const bool* array, std::size_t size) override
		{
			impl_typed_array(array, size);
		}

		void typed_array(const int8* array, std::size_t size) override
		{
			impl_typed_array(array, size);
		}

		void typed_array(const uint8* array, std::size_t size) override
		{
			impl_typed_array(array, size);
		}

		void typed_array(const int16* array, std::size_t size) override
		{
			impl_typed_array(array, size);
		}

		void typed_array(const uint16* array, std::size_t size) override
		{
			impl_typed_array(array, size);
		}

		void typed_array(const int32* array, std::size_t size) override
		{
			impl_typed_array(array, size);
		}

		void typed_array(const uint32* array, std::size_t size) override
		{
			impl_typed_array(array, size);
		}

		void typed_array(const int64* array, std::size_t size) override
		{
			impl_typed_array(array, size);
		}

		void typed_array(const uint64* array, std::size_t size) override
		{
			impl_typed_array(array, size);
		}

		void typed_array(const float* array, std::size_t size) override
		{
			impl_typed_array(array, size);
		}

		void typed_array(const double* array, std::size_t size) override
		{
			impl_typed_array(array, size);
		}

		void add_array_element(FunctionView<void(ArchiveWriter& elementWriter)> func) override
		{
			// Set this node as an array, if it isn't already
			if (!_node.IsArray())
			{
				_node.SetArray();
			}

			// Create a new node for the array element
			rapidjson::Value elementNode;

			// Operate on the element
			JsonArchiveWriter elementWriter{ elementNode, _allocator };
			func(elementWriter);

			// Add the element node as an element of this node
			_node.GetArray().PushBack(elementNode, _allocator);
		}

		void add_object_member(const char* name, FunctionView<void(ArchiveWriter& memberWriter)> func) override
		{
			// Set this node as an object, if it isn't already
			if (!_node.IsObject())
			{
				_node.SetObject();
			}

			// Create a new node for the object member
			rapidjson::Value memberNode;

			// Operate on the member
			JsonArchiveWriter memberWriter{ memberNode, _allocator };
			func(memberWriter);

			// Add the member node to this node
			rapidjson::Value memberName{ name, _allocator };
			_node.AddMember(memberName, memberNode, _allocator);
		}

	private:

		template <typename T>
		void impl_typed_array(const T* array, std::size_t size)
		{
			_node.SetArray();
			_node.GetArray().Reserve(static_cast<rapidjson::SizeType>(size), _allocator);

			for (std::size_t i = 0; i < size; ++i)
			{
				_node.GetArray().PushBack(array[i], _allocator);
			}
		}

		//////////////////
		///   Fields   ///
	private:

		rapidjson::Value& _node;
		rapidjson::MemoryPoolAllocator<>& _allocator;
	};
}

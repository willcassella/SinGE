// JsonArchiveWriter.h
#pragma once

#include <stack>
#include <rapidjson/document.h>
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
			: _head(&node),
			_allocator(&allocator)
		{
		}

		///////////////////
		///   Methods   ///
	public:

		void pop() override
		{
			// If we've reached the end of this stack
			if (_parents.empty())
			{
				delete this;
				return;
			}

			_head = _parents.top();
			_parents.pop();
		}

		void null() override
		{
			_head->SetNull();
		}

		void boolean(bool value) override
		{
			_head->SetBool(value);
		}

		void number(int8 value) override
		{
			_head->SetInt(value);
		}

		void number(uint8 value) override
		{
			_head->SetUint(value);
		}

		void number(int16 value) override
		{
			_head->SetInt(value);
		}

		void number(uint16 value) override
		{
			_head->SetUint(value);
		}

		void number(int32 value) override
		{
			_head->SetInt(value);
		}

		void number(uint32 value) override
		{
			_head->SetUint(value);
		}

		void number(int64 value) override
		{
			_head->SetInt64(value);
		}

		void number(uint64 value) override
		{
			_head->SetUint64(value);
		}

		void number(float value) override
		{
			_head->SetDouble(value);
		}

		void number(double value) override
		{
			_head->SetDouble(value);
		}

		void string(const char* str, std::size_t len) override
		{
			_head->SetString(str, static_cast<rapidjson::SizeType>(len), *_allocator);
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

		void as_object() override
		{
			if (!_head->IsObject())
			{
				_head->SetObject();
			}
		}

		void push_object_member(const char* name) override
		{
			// Set this node as an object, if it isn't already
			if (!_head->IsObject())
			{
				_head->SetObject();
			}

			// Push the head as a parent
			_parents.push(_head);

			// Add the member node to this node
			rapidjson::Value memberName{ name, *_allocator };
			rapidjson::Value memberNode;
			_head->AddMember(memberName, memberNode, *_allocator);

			// Get the member node out of the object
			_head = &(*_head)[name];
		}

		void push_array_element() override
		{
			// Set this node as an array, if it isn't already
			if (!_head->IsArray())
			{
				_head->SetArray();
			}

			// Push the head onto the stack
			_parents.push(_head);

			// Add the element node to this node
			rapidjson::Value element;
			_head->PushBack(element, *_allocator);

			// Get the element node
			_head = &(*_head)[_head->Size() - 1];
		}

	private:

		template <typename T>
		void impl_typed_array(const T* array, std::size_t size)
		{
			_head->SetArray();
			_head->Reserve(static_cast<rapidjson::SizeType>(size), *_allocator);

			for (std::size_t i = 0; i < size; ++i)
			{
				_head->PushBack(array[i], *_allocator);
			}
		}

		//////////////////
		///   Fields   ///
	private:

		rapidjson::Value* _head;
		std::stack<rapidjson::Value*> _parents;
		rapidjson::MemoryPoolAllocator<>* _allocator;
	};
}

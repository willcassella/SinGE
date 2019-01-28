// JsonArchiveReader.h
#pragma once

#include <stack>
#include <algorithm>
#include <rapidjson/document.h>
#include <Core/IO/ArchiveReader.h>
#include "Core/Memory/Functions.h"

namespace sge
{
    class JsonArchiveReader final : public ArchiveReader
    {
        ////////////////////////
        ///   Constructors   ///
    public:

        JsonArchiveReader(const rapidjson::Value& node)
            : _head(&node)
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

        bool null() const override
        {
            return _head->IsNull();
        }

        bool is_boolean() const override
        {
            return _head->IsBool();
        }

        bool boolean(bool& out) const override
        {
            return impl_value<bool, &rapidjson::Value::IsBool, &rapidjson::Value::GetBool>(out);
        }

        bool is_number() const override
        {
            return _head->IsNumber();
        }

        bool number(int8& out) const override
        {
            return impl_value<int, &rapidjson::Value::IsInt, &rapidjson::Value::GetInt>(out);
        }

        bool number(uint8& out) const override
        {
            return impl_value<unsigned int, &rapidjson::Value::IsUint, &rapidjson::Value::GetUint>(out);
        }

        bool number(int16& out) const override
        {
            return impl_value<int, &rapidjson::Value::IsInt, &rapidjson::Value::GetInt>(out);
        }

        bool number(uint16& out) const override
        {
            return impl_value<unsigned int, &rapidjson::Value::IsUint, &rapidjson::Value::GetUint>(out);
        }

        bool number(int32& out) const override
        {
            return impl_value<int, &rapidjson::Value::IsInt, &rapidjson::Value::GetInt>(out);
        }

        bool number(uint32& out) const override
        {
            return impl_value<unsigned int, &rapidjson::Value::IsUint, &rapidjson::Value::GetUint>(out);
        }

        bool number(int64& out) const override
        {
            return impl_value<int64_t, &rapidjson::Value::IsInt64, &rapidjson::Value::GetInt64>(out);
        }

        bool number(uint64& out) const override
        {
            return impl_value<uint64_t, &rapidjson::Value::IsUint64, &rapidjson::Value::GetUint64>(out);
        }

        bool number(float& out) const override
        {
            return impl_value<double, &rapidjson::Value::IsDouble, &rapidjson::Value::GetDouble>(out);
        }

        bool number(double& out) const override
        {
            return impl_value<double, &rapidjson::Value::IsDouble, &rapidjson::Value::GetDouble>(out);
        }

        bool is_string() const override
        {
            return _head->IsString();
        }

        bool string_size(std::size_t& out) const override
        {
            if (!_head->IsString())
            {
                return false;
            }

            out = _head->GetStringLength();
            return true;
        }

        std::size_t string(char* out, std::size_t len) const override
        {
            if (!_head->IsString())
            {
                return false;
            }

            // Copy the string
            const std::size_t copy_length = std::min(len, (std::size_t)_head->GetStringLength());
            std::memcpy(out, _head->GetString(), copy_length);
            return copy_length;
        }

        bool is_array() const override
        {
            return _head->IsArray();
        }

        bool array_size(std::size_t& out) const override
        {
            if (!_head->IsArray())
            {
                return false;
            }

            out = _head->Size();
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
            return impl_typed_array<double, &rapidjson::Value::IsDouble, &rapidjson::Value::GetDouble>(out, size);
        }

        std::size_t typed_array(double* out, std::size_t size) const override
        {
            return impl_typed_array<double, &rapidjson::Value::IsDouble, &rapidjson::Value::GetDouble>(out, size);
        }

        bool is_object() const override
        {
            return _head->IsObject();
        }

        bool object_size(std::size_t& out) const override
        {
            if (!_head->IsObject())
            {
                return false;
            }

            out = _head->MemberCount();
            return true;
        }

        void enumerate_object_members(FunctionView<void(const char* name)> enumerator) override
        {
            if (!_head->IsObject())
            {
                return;
            }

            // Push the head onto the stack
            _parents.push(_head);

            // For each member of the object
            auto const end = _head->MemberEnd();
            for (auto iter = _head->MemberBegin(); iter != end; ++iter)
            {
                _head = &iter->value;

                // Call the enumerator with the name of the member
                enumerator(iter->name.GetString());
            }

            // Pop the head off the stack
            _head = _parents.top();
            _parents.pop();
        }

        bool pull_object_member(const char* name) override
        {
            // We can't get the member if this node doesn't hold an object
            if (!_head->IsObject())
            {
                return false;
            }

            // Try to get the member
            auto iter = _head->FindMember(name);
            if (iter == _head->MemberEnd())
            {
                return false;
            }

            // Push the member as the head
            _parents.push(_head);
            _head = &iter->value;

            return true;
        }

        void enumerate_array_elements(FunctionView<void(std::size_t i)> enumerator) override
        {
            if (!_head->IsArray())
            {
                return;
            }

            // Push the head onto the stack
            _parents.push(_head);

            // For each element of the array
            std::size_t index = 0;
            auto const end = _head->End();
            for (auto iter = _head->Begin(); iter != end; ++iter)
            {
                _head = &*iter;

                // Call the enumerator with the index
                enumerator(index);
                index += 1;
            }

            // Pop the head off the stack
            _head = _parents.top();
            _parents.pop();
        }

        bool pull_array_element(std::size_t const i) override
        {
            if (!_head->IsArray() || _head->Size() <= i)
            {
                return false;
            }

            // Push the head onto the stack
            _parents.push(_head);
            _head = &(*_head)[static_cast<rapidjson::SizeType>(i)];

            return true;
        }

    private:

        template <typename RetT, bool(rapidjson::Value::*CheckerFn)() const, RetT(rapidjson::Value::*GetterFn)() const, typename T>
        bool impl_value(T& out) const
        {
            if (!(_head->*CheckerFn)())
            {
                return false;
            }

            out = static_cast<T>((_head->*GetterFn)());
            return true;
        }

        template <typename RetT, bool(rapidjson::Value::*CheckerFn)() const, RetT(rapidjson::Value::*GetterFn)() const, typename T>
        std::size_t impl_typed_array(T* const out, std::size_t const size) const
        {
            if (!_head->IsArray())
            {
                return 0;
            }

            auto const end = _head->End();
            std::size_t index = 0;
            for (auto iter = _head->Begin(); iter != end && index < size; ++iter)
            {
                if (!((*iter).*CheckerFn)())
                {
                    break;
                }

                out[index] = static_cast<T>(((*iter).*GetterFn)());
                index += 1;
            }

            return index;
        }

        //////////////////
        ///   Fields   ///
    private:

        const rapidjson::Value* _head;
        std::stack<const rapidjson::Value*> _parents;
    };
}

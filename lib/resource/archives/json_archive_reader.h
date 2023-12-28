#pragma once

#include <stdint.h>
#include <algorithm>
#include <stack>

#include <rapidjson/document.h>

#include "lib/base/io/archive_reader.h"
#include "lib/base/memory/functions.h"

namespace sge {
class JsonArchiveReader final : public ArchiveReader {
 public:
  JsonArchiveReader(const rapidjson::Value& node) : _head(&node) {}

  void pop() override {
    // If we've reached the end of this stack
    if (_parents.empty()) {
      delete this;
      return;
    }

    _head = _parents.top();
    _parents.pop();
  }

  bool null() const override { return _head->IsNull(); }

  bool is_boolean() const override { return _head->IsBool(); }

  bool boolean(bool& out) const override {
    return impl_value<bool, &rapidjson::Value::IsBool, &rapidjson::Value::GetBool>(out);
  }

  bool is_number() const override { return _head->IsNumber(); }

  bool number(int8_t& out) const override {
    return impl_value<int, &rapidjson::Value::IsInt, &rapidjson::Value::GetInt>(out);
  }

  bool number(uint8_t& out) const override {
    return impl_value<unsigned int, &rapidjson::Value::IsUint, &rapidjson::Value::GetUint>(out);
  }

  bool number(int16_t& out) const override {
    return impl_value<int, &rapidjson::Value::IsInt, &rapidjson::Value::GetInt>(out);
  }

  bool number(uint16_t& out) const override {
    return impl_value<unsigned int, &rapidjson::Value::IsUint, &rapidjson::Value::GetUint>(out);
  }

  bool number(int32_t& out) const override {
    return impl_value<int, &rapidjson::Value::IsInt, &rapidjson::Value::GetInt>(out);
  }

  bool number(uint32_t& out) const override {
    return impl_value<unsigned int, &rapidjson::Value::IsUint, &rapidjson::Value::GetUint>(out);
  }

  bool number(int64_t& out) const override {
    return impl_value<int64_t, &rapidjson::Value::IsInt64, &rapidjson::Value::GetInt64>(out);
  }

  bool number(uint64_t& out) const override {
    return impl_value<uint64_t, &rapidjson::Value::IsUint64, &rapidjson::Value::GetUint64>(out);
  }

  bool number(float& out) const override {
    return impl_value<double, &rapidjson::Value::IsDouble, &rapidjson::Value::GetDouble>(out);
  }

  bool number(double& out) const override {
    return impl_value<double, &rapidjson::Value::IsDouble, &rapidjson::Value::GetDouble>(out);
  }

  bool is_string() const override { return _head->IsString(); }

  bool string_size(size_t& out) const override {
    if (!_head->IsString()) {
      return false;
    }

    out = _head->GetStringLength();
    return true;
  }

  size_t string(char* out, size_t len) const override {
    if (!_head->IsString()) {
      return false;
    }

    // Copy the string
    const size_t copy_length = std::min(len, (size_t)_head->GetStringLength());
    memcpy(out, _head->GetString(), copy_length);
    return copy_length;
  }

  bool is_array() const override { return _head->IsArray(); }

  bool array_size(size_t& out) const override {
    if (!_head->IsArray()) {
      return false;
    }

    out = _head->Size();
    return true;
  }

  size_t typed_array(bool* out, size_t size) const override {
    return impl_typed_array<bool, &rapidjson::Value::IsBool, &rapidjson::Value::GetBool>(out, size);
  }

  size_t typed_array(int8_t* out, size_t size) const override {
    return impl_typed_array<int, &rapidjson::Value::IsInt, &rapidjson::Value::GetInt>(out, size);
  }

  size_t typed_array(uint8_t* out, size_t size) const override {
    return impl_typed_array<unsigned int, &rapidjson::Value::IsUint, &rapidjson::Value::GetUint>(out, size);
  }

  size_t typed_array(int16_t* out, size_t size) const override {
    return impl_typed_array<int, &rapidjson::Value::IsInt, &rapidjson::Value::GetInt>(out, size);
  }

  size_t typed_array(uint16_t* out, size_t size) const override {
    return impl_typed_array<unsigned int, &rapidjson::Value::IsUint, &rapidjson::Value::GetUint>(out, size);
  }

  size_t typed_array(int32_t* out, size_t size) const override {
    return impl_typed_array<int, &rapidjson::Value::IsInt, &rapidjson::Value::GetInt>(out, size);
  }

  size_t typed_array(uint32_t* out, size_t size) const override {
    return impl_typed_array<unsigned int, &rapidjson::Value::IsUint, &rapidjson::Value::GetUint>(out, size);
  }

  size_t typed_array(int64_t* out, size_t size) const override {
    return impl_typed_array<int64_t, &rapidjson::Value::IsInt64, &rapidjson::Value::GetInt64>(out, size);
  }

  size_t typed_array(uint64_t* out, size_t size) const override {
    return impl_typed_array<uint64_t, &rapidjson::Value::IsUint64, &rapidjson::Value::GetUint64>(out, size);
  }

  size_t typed_array(float* out, size_t size) const override {
    return impl_typed_array<double, &rapidjson::Value::IsDouble, &rapidjson::Value::GetDouble>(out, size);
  }

  size_t typed_array(double* out, size_t size) const override {
    return impl_typed_array<double, &rapidjson::Value::IsDouble, &rapidjson::Value::GetDouble>(out, size);
  }

  bool is_object() const override { return _head->IsObject(); }

  bool object_size(size_t& out) const override {
    if (!_head->IsObject()) {
      return false;
    }

    out = _head->MemberCount();
    return true;
  }

  void enumerate_object_members(FunctionView<void(const char* name)> enumerator) override {
    if (!_head->IsObject()) {
      return;
    }

    // Push the head onto the stack
    _parents.push(_head);

    // For each member of the object
    auto const end = _head->MemberEnd();
    for (auto iter = _head->MemberBegin(); iter != end; ++iter) {
      _head = &iter->value;

      // Call the enumerator with the name of the member
      enumerator(iter->name.GetString());
    }

    // Pop the head off the stack
    _head = _parents.top();
    _parents.pop();
  }

  bool pull_object_member(const char* name) override {
    // We can't get the member if this node doesn't hold an object
    if (!_head->IsObject()) {
      return false;
    }

    // Try to get the member
    auto iter = _head->FindMember(name);
    if (iter == _head->MemberEnd()) {
      return false;
    }

    // Push the member as the head
    _parents.push(_head);
    _head = &iter->value;

    return true;
  }

  void enumerate_array_elements(FunctionView<void(size_t i)> enumerator) override {
    if (!_head->IsArray()) {
      return;
    }

    // Push the head onto the stack
    _parents.push(_head);

    // For each element of the array
    size_t index = 0;
    auto const end = _head->End();
    for (auto iter = _head->Begin(); iter != end; ++iter) {
      _head = &*iter;

      // Call the enumerator with the index
      enumerator(index);
      index += 1;
    }

    // Pop the head off the stack
    _head = _parents.top();
    _parents.pop();
  }

  bool pull_array_element(size_t const i) override {
    if (!_head->IsArray() || _head->Size() <= i) {
      return false;
    }

    // Push the head onto the stack
    _parents.push(_head);
    _head = &(*_head)[static_cast<rapidjson::SizeType>(i)];

    return true;
  }

 private:
  template <
      typename RetT,
      bool (rapidjson::Value::*CheckerFn)() const,
      RetT (rapidjson::Value::*GetterFn)() const,
      typename T>
  bool impl_value(T& out) const {
    if (!(_head->*CheckerFn)()) {
      return false;
    }

    out = static_cast<T>((_head->*GetterFn)());
    return true;
  }

  template <
      typename RetT,
      bool (rapidjson::Value::*CheckerFn)() const,
      RetT (rapidjson::Value::*GetterFn)() const,
      typename T>
  size_t impl_typed_array(T* const out, size_t const size) const {
    if (!_head->IsArray()) {
      return 0;
    }

    auto const end = _head->End();
    size_t index = 0;
    for (auto iter = _head->Begin(); iter != end && index < size; ++iter) {
      if (!((*iter).*CheckerFn)()) {
        break;
      }

      out[index] = static_cast<T>(((*iter).*GetterFn)());
      index += 1;
    }

    return index;
  }

  const rapidjson::Value* _head;
  std::stack<const rapidjson::Value*> _parents;
};
}  // namespace sge

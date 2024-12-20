#pragma once

#include <stdint.h>
#include <stack>

#include "lib/third_party/rapidjson/document.h"

#include "lib/base/io/archive_writer.h"

namespace sge {
class JsonArchiveWriter final : public ArchiveWriter {
  using Allocator = rapidjson::MemoryPoolAllocator<>;

 public:
  JsonArchiveWriter(rapidjson::Value& node, rapidjson::MemoryPoolAllocator<>& allocator)
      : _head(&node), _allocator(&allocator) {}

  void pop() override {
    // If we've reached the end of this stack
    if (_parents.empty()) {
      delete this;
      return;
    }

    _head = _parents.top();
    _parents.pop();
  }

  void null() override { _head->SetNull(); }

  void boolean(bool value) override { _head->SetBool(value); }

  void number(int8_t value) override { _head->SetInt(value); }

  void number(uint8_t value) override { _head->SetUint(value); }

  void number(int16_t value) override { _head->SetInt(value); }

  void number(uint16_t value) override { _head->SetUint(value); }

  void number(int32_t value) override { _head->SetInt(value); }

  void number(uint32_t value) override { _head->SetUint(value); }

  void number(int64_t value) override { _head->SetInt64(value); }

  void number(uint64_t value) override { _head->SetUint64(value); }

  void number(float value) override { _head->SetDouble(value); }

  void number(double value) override { _head->SetDouble(value); }

  void string(const char* str, size_t len) override {
    _head->SetString(str, static_cast<rapidjson::SizeType>(len), *_allocator);
  }

  void typed_array(const bool* array, size_t size) override { impl_typed_array(array, size); }

  void typed_array(const int8_t* array, size_t size) override { impl_typed_array(array, size); }

  void typed_array(const uint8_t* array, size_t size) override { impl_typed_array(array, size); }

  void typed_array(const int16_t* array, size_t size) override { impl_typed_array(array, size); }

  void typed_array(const uint16_t* array, size_t size) override { impl_typed_array(array, size); }

  void typed_array(const int32_t* array, size_t size) override { impl_typed_array(array, size); }

  void typed_array(const uint32_t* array, size_t size) override { impl_typed_array(array, size); }

  void typed_array(const int64_t* array, size_t size) override { impl_typed_array(array, size); }

  void typed_array(const uint64_t* array, size_t size) override { impl_typed_array(array, size); }

  void typed_array(const float* array, size_t size) override { impl_typed_array(array, size); }

  void typed_array(const double* array, size_t size) override { impl_typed_array(array, size); }

  void as_object() override {
    if (!_head->IsObject()) {
      _head->SetObject();
    }
  }

  void push_object_member(const char* name) override {
    // Set this node as an object, if it isn't already
    if (!_head->IsObject()) {
      _head->SetObject();
    }

    // Push the head as a parent
    _parents.push(_head);

    // Add the member node to this node
    rapidjson::Value memberName{name, *_allocator};
    rapidjson::Value memberNode;
    _head->AddMember(memberName, memberNode, *_allocator);

    // Get the member node out of the object
    _head = &(*_head)[name];
  }

  void push_array_element() override {
    // Set this node as an array, if it isn't already
    if (!_head->IsArray()) {
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
  void impl_typed_array(const T* array, size_t size) {
    _head->SetArray();
    _head->Reserve(static_cast<rapidjson::SizeType>(size), *_allocator);

    for (size_t i = 0; i < size; ++i) {
      _head->PushBack(array[i], *_allocator);
    }
  }

  rapidjson::Value* _head;
  std::stack<rapidjson::Value*> _parents;
  rapidjson::MemoryPoolAllocator<>* _allocator;
};
}  // namespace sge

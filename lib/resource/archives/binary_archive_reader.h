#pragma once

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <algorithm>
#include <stack>
#include <vector>

#include "lib/base/io/archive_reader.h"
#include "lib/resource/archives/binary_archive_node.h"

namespace sge {
class BinaryArchiveReader final : public ArchiveReader {
  struct Cursor {
    const uint8_t* node_value;

    BinaryArchiveNode node_type;

    bool in_enumeration;
  };

 public:
  BinaryArchiveReader(const std::vector<uint8_t>& buffer) {
    cursor.node_value = buffer.data() + 1;
    cursor.node_type = static_cast<BinaryArchiveNode>(buffer.front());
    cursor.in_enumeration = false;
  }

  void pop() override {
    // Make sure we're not in an enumeration (popping is not allowed during an enumeration)
    assert(!cursor.in_enumeration);

    // If there are no parents, we're done
    if (cursor_stack.empty()) {
      delete this;
      return;
    }

    // Restore the parent
    cursor = cursor_stack.top();
    cursor_stack.pop();
  }

  bool null() const override { return cursor.node_type == BAN_NULL; }

  bool is_boolean() const override { return cursor.node_type == BAN_BOOLEAN; }

  bool boolean(bool& out) const override {
    BinaryArchiveBool_t value;
    if (impl_value(BAN_BOOLEAN, value)) {
      out = (value == BAN_TRUE);
      return true;
    }

    return false;
  }

  bool is_number() const override { return cursor.node_type >= BAN_INT8 && cursor.node_type <= BAN_DOUBLE; }

  bool number(int8_t& out) const override { return impl_value(BAN_INT8, out); }

  bool number(uint8_t& out) const override { return impl_value(BAN_UINT8, out); }

  bool number(int16_t& out) const override { return impl_value(BAN_INT16, out); }

  bool number(uint16_t& out) const override { return impl_value(BAN_UINT16, out); }

  bool number(int32_t& out) const override { return impl_value(BAN_INT32, out); }

  bool number(uint32_t& out) const override { return impl_value(BAN_UINT32, out); }

  bool number(int64_t& out) const override { return impl_value(BAN_INT64, out); }

  bool number(uint64_t& out) const override { return impl_value(BAN_UINT64, out); }

  bool number(float& out) const override { return impl_value(BAN_FLOAT, out); }

  bool number(double& out) const override { return impl_value(BAN_DOUBLE, out); }

  bool is_string() const override { return cursor.node_type == BAN_STRING; }

  bool string_size(size_t& out) const override {
    BinaryArchiveSize_t size = 0;
    if (impl_value(BAN_STRING, size)) {
      out = static_cast<BinaryArchiveSize_t>(size);
      return true;
    }

    return false;
  }

  size_t string(char* out, size_t len) const override {
    size_t str_len;
    if (!string_size(str_len)) {
      return 0;
    }

    const size_t copy_len = std::min(str_len, len);
    memcpy(out, cursor.node_value + sizeof(BinaryArchiveSize_t), copy_len);
    return copy_len;
  }

  bool is_array() const override {
    return (cursor.node_type >= BAN_ARRAY_INT8 && cursor.node_type <= BAN_ARRAY_FLOAT) ||
           cursor.node_type == BAN_ARRAY_GENERIC;
  }

  bool array_size(size_t& out) const override {
    if (!is_array()) {
      return false;
    }

    const auto size = buffer_read<BinaryArchiveSize_t>(cursor.node_value);
    out = static_cast<size_t>(size);
    return true;
  }

  size_t typed_array(bool* out, size_t out_size) const override {
    if (cursor.node_type != BAN_ARRAY_BOOLEAN) {
      return 0;
    }

    // Get the array size and the amount to read
    const auto size = static_cast<size_t>(buffer_read<BinaryArchiveSize_t>(cursor.node_value));
    const auto read_size = std::min(size, out_size);

    // Create the array pointer
    const auto arr = reinterpret_cast<const BinaryArchiveBool_t*>(cursor.node_value);

    // Copy the data
    for (size_t i = 0; i < read_size; ++i) {
      out[i] = (arr[i] == 1);
    }

    return read_size;
  }

  size_t typed_array(int8_t* out, size_t size) const override {
    return impl_typed_array(BAN_ARRAY_INT8, out, size);
  }

  size_t typed_array(uint8_t* out, size_t size) const override {
    return impl_typed_array(BAN_ARRAY_UINT8, out, size);
  }

  size_t typed_array(int16_t* out, size_t size) const override {
    return impl_typed_array(BAN_ARRAY_INT16, out, size);
  }

  size_t typed_array(uint16_t* out, size_t size) const override {
    return impl_typed_array(BAN_ARRAY_UINT16, out, size);
  }

  size_t typed_array(int32_t* out, size_t size) const override {
    return impl_typed_array(BAN_ARRAY_INT32, out, size);
  }

  size_t typed_array(uint32_t* out, size_t size) const override {
    return impl_typed_array(BAN_ARRAY_UINT32, out, size);
  }

  size_t typed_array(int64_t* out, size_t size) const override {
    return impl_typed_array(BAN_ARRAY_INT64, out, size);
  }

  size_t typed_array(uint64_t* out, size_t size) const override {
    return impl_typed_array(BAN_ARRAY_UINT64, out, size);
  }

  size_t typed_array(float* out, size_t size) const override {
    return impl_typed_array(BAN_ARRAY_FLOAT, out, size);
  }

  size_t typed_array(double* out, size_t size) const override {
    return impl_typed_array(BAN_ARRAY_DOUBLE, out, size);
  }

  void enumerate_array_elements(FunctionView<void(size_t i)> enumerator) override {
    // Get the size of the array
    size_t size = 0;
    if (!array_size(size)) {
      return;
    }

    // If it's a typed array, handle that seperately
    if (cursor.node_type != BAN_ARRAY_GENERIC) {
      enumerate_typed_array_elements(size, enumerator);
      return;
    }

    // Back up the cursor
    const auto cursor_backup = cursor;

    // Move the cursor forward to account for size and span
    cursor.node_value += sizeof(BinaryArchiveSize_t) * 2;
    cursor.in_enumeration = true;

    // Loop through elements
    for (size_t i = 0; i < size; ++i) {
      // Get the node type and advance the cursor past the indicator
      cursor.node_type = static_cast<BinaryArchiveNode>(*cursor.node_value);
      cursor.node_value += 1;

      // Call the enumerator function
      enumerator(i);

      // Advance the cursor to the start of the next node
      cursor.node_value += get_cursor_advancement();
    }

    // Restore the cursor
    cursor = cursor_backup;
  }

  void enumerate_typed_array_elements(size_t size, FunctionView<void(size_t i)> enumerator) {
    // Back up the cursor
    const auto cursor_backup = cursor;

    // Move the cursor forward to account for size
    cursor.node_value += sizeof(BinaryArchiveSize_t);
    cursor.in_enumeration = true;

    // Amount to advance the cursor by each enumeration
    size_t advance = 0;

    // Figure out which node to mark as the node type
    switch (cursor.node_type) {
      case BAN_ARRAY_BOOLEAN:
        cursor.node_type = BAN_BOOLEAN;
        advance = sizeof(BinaryArchiveBool_t);
        break;

      case BAN_ARRAY_INT8:
        cursor.node_type = BAN_INT8;
        advance = sizeof(int8_t);
        break;

      case BAN_ARRAY_UINT8:
        cursor.node_type = BAN_UINT8;
        advance = sizeof(uint8_t);
        break;

      case BAN_ARRAY_INT16:
        cursor.node_type = BAN_INT16;
        advance = sizeof(int16_t);
        break;

      case BAN_ARRAY_UINT16:
        cursor.node_type = BAN_UINT16;
        advance = sizeof(uint16_t);
        break;

      case BAN_ARRAY_INT32:
        cursor.node_type = BAN_INT32;
        advance = sizeof(int32_t);
        break;

      case BAN_ARRAY_UINT32:
        cursor.node_type = BAN_UINT32;
        advance = sizeof(uint32_t);
        break;

      case BAN_ARRAY_INT64:
        cursor.node_type = BAN_INT64;
        advance = sizeof(int64_t);
        break;

      case BAN_ARRAY_UINT64:
        cursor.node_type = BAN_UINT64;
        advance = sizeof(uint64_t);
        break;

      case BAN_ARRAY_FLOAT:
        cursor.node_type = BAN_FLOAT;
        advance = sizeof(float);
        break;

      case BAN_ARRAY_DOUBLE:
        cursor.node_type = BAN_DOUBLE;
        advance = sizeof(double);
        break;

      default:
        assert(false);
    }

    // For each element
    for (size_t i = 0; i < size; ++i) {
      // Call the enumerator
      enumerator(i);

      // Advance the cursor
      cursor.node_value += advance;
    }

    // Restore the cursor
    cursor = cursor_backup;
  }

  bool pull_array_element(size_t i) override {
    size_t size;
    if (!array_size(size)) {
      return false;
    }

    if (i >= size) {
      return false;
    }

    // Push the cursor onto the stack
    cursor_stack.push(cursor);
    cursor.in_enumeration = false;

    // Handle generic array
    if (cursor.node_type == BAN_ARRAY_GENERIC) {
      // Move the cursor forward to the first element
      cursor.node_type = static_cast<BinaryArchiveNode>(cursor.node_value[sizeof(BinaryArchiveSize_t) * 2]);
      cursor.node_value += sizeof(BinaryArchiveSize_t) * 2 + 1;

      // For each step until the index
      for (; i > 0; --i) {
        // Advance the cursor
        const auto advance = get_cursor_advancement();
        cursor.node_type = static_cast<BinaryArchiveNode>(cursor.node_value[advance]);
        cursor.node_value += advance + 1;
      }

      return true;
    }

    // Move the cursor past the size object
    cursor.node_value += sizeof(BinaryArchiveSize_t);

    // Handle typed arrays
    switch (cursor.node_type) {
      case BAN_ARRAY_BOOLEAN:
        cursor.node_value += sizeof(BinaryArchiveBool_t) * i;
        cursor.node_type = BAN_BOOLEAN;
        break;

      case BAN_ARRAY_INT8:
        cursor.node_value += sizeof(int8_t) * i;
        cursor.node_type = BAN_INT8;
        break;

      case BAN_ARRAY_UINT8:
        cursor.node_value += sizeof(uint8_t) * i;
        cursor.node_type = BAN_UINT8;
        break;

      case BAN_ARRAY_INT16:
        cursor.node_value += sizeof(int16_t) * i;
        cursor.node_type = BAN_INT16;
        break;

      case BAN_ARRAY_UINT16:
        cursor.node_value += sizeof(uint16_t) * i;
        cursor.node_type = BAN_UINT16;
        break;

      case BAN_ARRAY_INT32:
        cursor.node_value += sizeof(int32_t) * i;
        cursor.node_type = BAN_INT32;
        break;

      case BAN_ARRAY_UINT32:
        cursor.node_value += sizeof(uint32_t) * i;
        cursor.node_type = BAN_UINT32;
        break;

      case BAN_ARRAY_INT64:
        cursor.node_value += sizeof(int64_t) * i;
        cursor.node_type = BAN_INT64;
        break;

      case BAN_ARRAY_UINT64:
        cursor.node_value += sizeof(uint64_t) * i;
        cursor.node_type = BAN_UINT64;
        break;

      default:
        assert(false);
        pop();
        return false;
    }

    return true;
  }

  bool is_object() const override { return cursor.node_type == BAN_OBJECT; }

  bool object_size(size_t& out) const override {
    if (!is_object()) {
      return false;
    }

    // Get the size from the archive
    out = static_cast<size_t>(buffer_read<BinaryArchiveSize_t>(cursor.node_value));
    return true;
  }

  void enumerate_object_members(FunctionView<void(const char* name)> enumerator) override {
    // Get the object size
    size_t size = 0;
    if (!object_size(size)) {
      return;
    }

    // Back up the cursor
    const auto cursor_back_up = cursor;
    cursor.in_enumeration = true;

    // Advance the cursor to the first element
    cursor.node_value += sizeof(BinaryArchiveSize_t) * 2;

    // Enumerate over members
    for (size_t i = 0; i < size; ++i) {
      // Get the name
      const char* name = reinterpret_cast<const char*>(cursor.node_value);
      cursor.node_value += strlen(name) + 1;

      // Get the type
      cursor.node_type = static_cast<BinaryArchiveNode>(*cursor.node_value);
      cursor.node_value += 1;

      // Call the enumerator
      enumerator(name);

      // Advance the cursor
      cursor.node_value += get_cursor_advancement();
    }

    // Restore the cursor
    cursor = cursor_back_up;
  }

  bool pull_object_member(const char* name) override {
    // Get the object size
    size_t size = 0;
    if (!object_size(size)) {
      return false;
    }

    // Back up the cursor
    auto cursor_backup = cursor;
    cursor.in_enumeration = false;

    // Advance the cursor to the first element
    cursor.node_value += sizeof(BinaryArchiveSize_t) * 2;

    // Enumerate over members
    for (size_t i = 0; i < size; ++i) {
      // Get the member's name name
      const char* mem_name = reinterpret_cast<const char*>(cursor.node_value);
      cursor.node_value += strlen(mem_name) + 1;

      // Get the type
      cursor.node_type = static_cast<BinaryArchiveNode>(*cursor.node_value);
      cursor.node_value += 1;

      // See if this is the node we're looking for
      if (strcmp(name, mem_name) == 0) {
        // Push the cursor onto the stack
        cursor_stack.push(cursor_backup);
        return true;
      }

      // Advance the cursor
      cursor.node_value += get_cursor_advancement();
    }

    // Restore the cursor
    cursor = cursor_backup;
    return false;
  }

 private:
  template <typename T>
  bool impl_value(BinaryArchiveNode required_type, T& out) const {
    if (cursor.node_type == required_type) {
      out = buffer_read<T>(cursor.node_value);
      return true;
    }

    return false;
  }

  template <typename T>
  size_t impl_typed_array(BinaryArchiveNode required_type, T* out, size_t out_size) const {
    if (cursor.node_type != required_type) {
      return 0;
    }

    // Get the array size and the amount to read
    const auto size = static_cast<size_t>(buffer_read<BinaryArchiveSize_t>(cursor.node_value));
    const auto read_size = std::min(size, out_size);

    // Copy the data
    memcpy(out, cursor.node_value + sizeof(BinaryArchiveSize_t), read_size * sizeof(T));
    return read_size;
  }

  template <typename T>
  static T buffer_read(const uint8_t* buffer) {
    return *reinterpret_cast<const T*>(buffer);
  }

  // Figure out how much to advance the cursor by.
  // (NOTE: This does not account for indicators, because the cursor has already advanced passed the current
  // indicator)
  size_t get_cursor_advancement() const {
    // If this node is a generic array or object
    if (cursor.node_type == BAN_ARRAY_GENERIC || cursor.node_type == BAN_OBJECT) {
      // Get the span (includes indicator byte, which we've passed)
      return static_cast<size_t>(
                 buffer_read<BinaryArchiveSize_t>(cursor.node_value + sizeof(BinaryArchiveSize_t))
             ) -
             1;
    }
    // If the node contains an array
    else if (is_array()) {
      // Get the length
      const size_t len = static_cast<size_t>(buffer_read<BinaryArchiveSize_t>(cursor.node_value));

      switch (cursor.node_type) {
        case BAN_ARRAY_BOOLEAN:
          return sizeof(BinaryArchiveSize_t) + sizeof(BinaryArchiveBool_t) * len;

        case BAN_ARRAY_INT8:
          return sizeof(BinaryArchiveSize_t) + sizeof(int8_t) * len;

        case BAN_ARRAY_UINT8:
          return sizeof(BinaryArchiveSize_t) + sizeof(uint8_t) * len;

        case BAN_ARRAY_INT16:
          return sizeof(BinaryArchiveSize_t) + sizeof(int16_t) * len;

        case BAN_ARRAY_UINT16:
          return sizeof(BinaryArchiveSize_t) + sizeof(uint16_t) * len;

        case BAN_ARRAY_INT32:
          return sizeof(BinaryArchiveSize_t) + sizeof(int32_t) * len;

        case BAN_ARRAY_UINT32:
          return sizeof(BinaryArchiveSize_t) + sizeof(uint32_t) * len;

        case BAN_ARRAY_INT64:
          return sizeof(BinaryArchiveSize_t) + sizeof(int64_t) * len;

        case BAN_ARRAY_UINT64:
          return sizeof(BinaryArchiveSize_t) + sizeof(uint64_t) * len;

        case BAN_ARRAY_FLOAT:
          return sizeof(BinaryArchiveSize_t) + sizeof(float) * len;

        case BAN_ARRAY_DOUBLE:
          return sizeof(BinaryArchiveSize_t) + sizeof(double) * len;

        default:
          assert(false);
          return 0;
      }
    }

    // Handle value types
    switch (cursor.node_type) {
      case BAN_NULL:
        return 0;

      case BAN_BOOLEAN:
        return sizeof(BinaryArchiveBool_t);

      case BAN_INT8:
        return sizeof(int8_t);

      case BAN_UINT8:
        return sizeof(uint8_t);

      case BAN_INT16:
        return sizeof(int16_t);

      case BAN_UINT16:
        return sizeof(uint16_t);

      case BAN_INT32:
        return sizeof(int32_t);

      case BAN_UINT32:
        return sizeof(uint32_t);

      case BAN_INT64:
        return sizeof(int64_t);

      case BAN_UINT64:
        return sizeof(uint64_t);

      case BAN_FLOAT:
        return sizeof(float);

      case BAN_DOUBLE:
        return sizeof(double);

      case BAN_STRING:
        return sizeof(BinaryArchiveSize_t) +
               static_cast<size_t>(buffer_read<BinaryArchiveSize_t>(cursor.node_value));

      default:
        assert(false);
        return 0;
    }
  }

 private:
  Cursor cursor;
  std::stack<Cursor> cursor_stack;
};
}  // namespace sge

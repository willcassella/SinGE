#pragma once

#include <stdint.h>

#include "lib/base/build.h"
#include "lib/base/functional/function_view.h"

namespace sge {
class ArchiveReader;

template <typename T>
void from_archive(T& value, ArchiveReader& reader);

class SGE_BASE_EXPORT ArchiveReader {
 public:
  /**
   * \brief Moves this node to the previous node in the read stack.
   */
  virtual void pop() = 0;

  /**
   * \brief Returns whether this node contains 'null'.
   */
  virtual bool null() const = 0;

  /**
   * \brief Returns whether this node contains a boolean value.
   */
  virtual bool is_boolean() const = 0;

  /**
   * \brief Trys to get a boolean value out of this node.
   * \param out The value to assign to, if this node holds a value of type bool.
   * \return Whether the given value was assigned to.
   */
  virtual bool boolean(bool& out) const = 0;

  /**
   * \brief Returns whether this node contains a numeric value.
   */
  virtual bool is_number() const = 0;

  /**
   * \brief Trys to get a numeric value of type int8_t out of this node.
   * \param out The value to assign to, if this node holds a numeric value of type int8_t.
   * \return Whether the given value as assigned to.
   */
  virtual bool number(int8_t& out) const = 0;

  /**
   * \brief Trys to get a numeric value of type uint8_t out of this node.
   * \param out The value to assign to, if this node holds a value of type uint8_t.
   * \return Whether the given value was assigned to.
   */
  virtual bool number(uint8_t& out) const = 0;

  /**
   * \brief Trys to get a numeric value of type int16_t out of this node.
   * \param out The value to assign to, if this node holds a value of type int16_t.
   * \return Whether the given value was assigned to.
   */
  virtual bool number(int16_t& out) const = 0;

  /**
   * \brief Trys to get a numeric value of type uint16_t out of this node.
   * \param out The value to assign to, if this node holds a value of type uint16_t.
   * \return Whether the given value was assigned to.
   */
  virtual bool number(uint16_t& out) const = 0;

  /**
   * \brief Trys to get a numeric value of type int32_t out of this node.
   * \param out The value to assign to, if this node holds a value of type int32_t.
   * \return Whether the given value was assigned to.
   */
  virtual bool number(int32_t& out) const = 0;

  /**
   * \brief Trys to get a numeric value of type uint32_t out of this node.
   * \param out The value to assign to, if this node holds a value of type uint32_t.
   * \return Whether the given value was assigned to.
   */
  virtual bool number(uint32_t& out) const = 0;

  /**
   * \brief Trys to get a numeric value of type int64_t out of this node.
   * \param out The value to assign to, if this node holds a value of type int64_t.
   * \return Whether the given value was assigned to.
   */
  virtual bool number(int64_t& out) const = 0;

  /**
   * \brief Trys to get a numeric value of type uint64_t out of this node.
   * \param out The value to assign to, if this node holds a value of type uint64_t.
   * \return Whether the given value was assigned to.
   */
  virtual bool number(uint64_t& out) const = 0;

  /**
   * \brief Trys to get a numeric value of type float out of this node.
   * \param out The value to assign to, if this node holds a value of type float.
   * \return Whether the given value was assigned to.
   */
  virtual bool number(float& out) const = 0;

  /**
   * \brief Trys to get a numeric value of type double out of this node.
   * \param out The value to assign to, if this node holds a value of type double.
   * \return Whether the given value was assigned to.
   */
  virtual bool number(double& out) const = 0;

  virtual bool is_string() const = 0;

  virtual bool string_size(size_t& outLen) const = 0;

  /**
   * \brief Trys to get a string value out of this node.
   * \param outStr A pointer to assign the address of the first character of the string to, if this node
   * contains a string. \param outLen A value to assign the length of the string to, if this node contains a
   * string. \return Whether the given values were assigned to.
   */
  virtual size_t string(char* out, size_t len) const = 0;

  virtual bool is_array() const = 0;

  /**
   * \brief Trys to get the size of the array this node holds.
   * \param out The value to assign the size of the array to, if this node holds an array.
   * \return Whether the given value was assigned to.
   */
  virtual bool array_size(size_t& out) const = 0;

  /**
   * \brief Trys to get an array of all type bool out of this node.
   * \param out A pointer to the array to fill.
   * \param size The number of elements requested to fill into the array.
   * \return The number of elements filled into the array.
   */
  virtual size_t typed_array(bool* out, size_t size) const = 0;

  /**
   * \brief Trys to get an array of all type int8_t out of this node.
   * \param out A pointer to the array to fill.
   * \param size The number of elements requested to fill into the array.
   * \return The number of elements filled into the array.
   */
  virtual size_t typed_array(int8_t* out, size_t size) const = 0;

  /**
   * \brief Trys to get an array of all type uint8_t out of this node.
   * \param out A pointer to the array to fill.
   * \param size The number of elements requested to fill into the array.
   * \return The number of elements filled into the array.
   */
  virtual size_t typed_array(uint8_t* out, size_t size) const = 0;

  /**
   * \brief Trys to get an array of all type int16_t out of this node.
   * \param out A pointer to the array to fill.
   * \param size The number of elements requested to fill into the array.
   * \return The number of elements filled into the array.
   */
  virtual size_t typed_array(int16_t* out, size_t size) const = 0;

  /**
   * \brief Trys to get an array of all type uint16_t out of this node.
   * \param out A pointer to the array to fill.
   * \param size The number of elements requested to fill into the array.
   * \return The number of elements filled into the array.
   */
  virtual size_t typed_array(uint16_t* out, size_t size) const = 0;

  /**
   * \brief Trys to get an array of all type int32_t out of this node.
   * \param out A pointer to the array to fill.
   * \param size The number of elements requested to fill into the array.
   * \return The number of elements filled into the array.
   */
  virtual size_t typed_array(int32_t* out, size_t size) const = 0;

  /**
   * \brief Trys to get an array of all type uint32_t out of this node.
   * \param out A pointer to the array to fill.
   * \param size The number of elements requested to fill into the array.
   * \return The number of elements filled into the array.
   */
  virtual size_t typed_array(uint32_t* out, size_t size) const = 0;

  /**
   * \brief Trys to get an array of all type int64_t out of this node.
   * \param out A pointer to the array to fill.
   * \param size The number of elements requested to fill into the array.
   * \return The number of elements filled into the array.
   */
  virtual size_t typed_array(int64_t* out, size_t size) const = 0;

  /**
   * \brief Trys to get an array of all type uint64_t out of this node.
   * \param out A pointer to the array to fill.
   * \param size The number of elements requested to fill into the array.
   * \return The number of elements filled into the array.
   */
  virtual size_t typed_array(uint64_t* out, size_t size) const = 0;

  /**
   * \brief Trys to get an array of all type float out of this node.
   * \param out A pointer to the array to fill.
   * \param size The number of elements requested to fill into the array.
   * \return The number of elements filled into the array.
   */
  virtual size_t typed_array(float* out, size_t size) const = 0;

  /**
   * \brief Trys to get an array of all type double out of this node.
   * \param out A pointer to the array to fill.
   * \param size The number of elements requested to fill into the array.
   * \return The number of elements filled into the array.
   */
  virtual size_t typed_array(double* out, size_t size) const = 0;

  virtual void enumerate_array_elements(FunctionView<void(size_t i)> enumerator) = 0;

  virtual bool pull_array_element(size_t i) = 0;

  template <typename T>
  bool array_element(size_t i, T& out) {
    if (this->pull_array_element(i)) {
      sge::from_archive(*this, out);
      this->pop();
      return true;
    }

    return false;
  }

  virtual bool is_object() const = 0;

  /**
   * \brief Trys to get the number of members of the object held in the node, if this node holds an object.
   * \param out The value to assign the number of members of the object to, if this node contains an object.
   * \return Whether the value was assigned.
   */
  virtual bool object_size(size_t& out) const = 0;

  /**
   * \brief Enumerates all members of the object contained in this node, if this node contains an object.
   * \param enumerator A function to call for each object member.
   */
  virtual void enumerate_object_members(FunctionView<void(const char* name)> enumerator) = 0;

  /**
   * \brief Trys to get the named object member from this node, if this node contains an object.
   * \param name The name of the object member to try to get.
   * \param func The function to call if the object member exists.
   * \return Whether the function was called. This may return 'false' if this node does not contain an object,
   * or if the member does not exist.
   */
  virtual bool pull_object_member(const char* name) = 0;

  /**
   * \brief Trys to deserialize the given value for the named object member of this node, if this node
   * contains an object. \param name The name of the object member to try to get. \param out The value to
   * deserialize if the object member exists. \return Whether the object was deserialized.
   */
  template <typename T>
  bool object_member(const char* name, T& out) {
    if (this->pull_object_member(name)) {
      sge::from_archive(out, *this);
      this->pop();
      return true;
    }

    return false;
  }
};
}  // namespace sge

#pragma once

#include <stdint.h>
#include <string>

#include "lib/base/build.h"
#include "lib/base/functional/function_view.h"

namespace sge {
class ArchiveWriter;

template <typename T>
void to_archive(const T& value, ArchiveWriter& writer);

class SGE_BASE_EXPORT ArchiveWriter {
 public:
  /**
   * \brief Pops the active node from this writer, and returns to the parent.
   */
  virtual void pop() = 0;

  /**
   * \brief Sets this archive node as null.
   */
  virtual void null() = 0;

  /**
   * \brief Sets this archive node as a boolean value.
   * \param value The value to set.
   */
  virtual void boolean(bool value) = 0;

  /**
   * \brief Sets this archive node as a numeric value of type int8_t.
   * \param value The value to set.
   */
  virtual void number(int8_t value) = 0;

  /**
   * \brief Sets this archive node as a numeric value of type uint8_t.
   * \param value The value to set.
   */
  virtual void number(uint8_t value) = 0;

  /**
   * \brief Sets this archive node as a numeric value of type int16_t.
   * \param value The value to set.
   */
  virtual void number(int16_t value) = 0;

  /**
   * \brief Sets this archive node as a numeric value of type uint16_t.
   * \param value The value to set.
   */
  virtual void number(uint16_t value) = 0;

  /**
   * \brief Sets this archive node as a numeric value of type int32_t.
   * \param value The value to set.
   */
  virtual void number(int32_t value) = 0;

  /**
   * \brief Sets this archive node as a numeric value of type uint32_t.
   * \param value The value to set.
   */
  virtual void number(uint32_t value) = 0;

  /**
   * \brief Sets this archive node as a numeric value of type int64_t.
   * \param value The value to set.
   */
  virtual void number(int64_t value) = 0;

  /**
   * \brief Sets this archive node as a numeric value of type uint64_t.
   * \param value The value to set.
   */
  virtual void number(uint64_t value) = 0;

  /**
   * \brief Sets this archive node as a numeric value of type float.
   * \param value The value to set.
   */
  virtual void number(float value) = 0;

  /**
   * \brief Sets this archive node as a numeric value of type double.
   * \param value The value to set.
   */
  virtual void number(double value) = 0;

  /**
   * \brief Sets this archive node as a string value.
   * \param str Address of the the first character of the string.
   * \param len The length of the string.
   */
  virtual void string(const char* str, size_t len) = 0;

  /**
   * \brief Sets this archive node as an array of all type bool.
   * \param array A pointer to the first element of the array.
   * \param size The number of elements in the array.
   */
  virtual void typed_array(const bool* array, size_t size) = 0;

  /**
   * \brief Sets this archive node as an array of all type int8_t.
   * \param array A pointer to the first element of the array.
   * \param size The number of elements in the array.
   */
  virtual void typed_array(const int8_t* array, size_t size) = 0;

  /**
   * \brief Sets this archive node as an array of all type uint8_t.
   * \param array A pointer to the first element of the array.
   * \param size The number of elements in the array.
   */
  virtual void typed_array(const uint8_t* array, size_t size) = 0;

  /**
   * \brief Sets this archive node as an array of all type int16_t.
   * \param array A pointer to the first element of the array.
   * \param size The number of elements in the array.
   */
  virtual void typed_array(const int16_t* array, size_t size) = 0;

  /**
   * \brief Sets this archive node as an array of all type uint16_t.
   * \param array A pointer to the first element of the array.
   * \param size The number of elements in the array.
   */
  virtual void typed_array(const uint16_t* array, size_t size) = 0;

  /**
   * \brief Sets this archive node as an array of all type int32_t.
   * \param array A pointer to the first element of the array.
   * \param size The number of elements in the array.
   */
  virtual void typed_array(const int32_t* array, size_t size) = 0;

  /**
   * \brief Sets this archive node as an array of all type uint32_t.
   * \param array A pointer to the first element of the array.
   * \param size The number of elements in the array.
   */
  virtual void typed_array(const uint32_t* array, size_t size) = 0;

  /**
   * \brief Sets this archive node as an array of all type int64_t.
   * \param array A pointer to the first element of the array.
   * \param size The number of elements in the array.
   */
  virtual void typed_array(const int64_t* array, size_t size) = 0;

  /**
   * \brief Sets this archive node as an array of all type uint64_t.
   * \param array A pointer to the first element of the array.
   * \param size The number of elements in the array.
   */
  virtual void typed_array(const uint64_t* array, size_t size) = 0;

  /**
   * \brief Sets this archive node as an array of all type float.
   * \param array A pointer to the first element of the array.
   * \param size The number of elements in the array.
   */
  virtual void typed_array(const float* array, size_t size) = 0;

  /**
   * \brief Sets this archive node as an array of all type double.
   * \param array A pointer to the first element of the array.
   * \param size The number of elements in the array.
   */
  virtual void typed_array(const double* array, size_t size) = 0;

  virtual void push_array_element() = 0;

  template <typename T>
  void array_element(const T& value) {
    this->push_array_element();
    sge::to_archive(value, *this);
    this->pop();
  }

  /**
   * \brief Coerces this node's type to an Object, even if no members are added.
   */
  virtual void as_object() = 0;

  /**
   * \brief Sets this archive node as an object, and calls the given function
   * on the writer for the node set as a member of the object.
   * \param name The name of the member.
   */
  virtual void push_object_member(const char* name) = 0;

  /**
   * \brief Sets this archive node as an object, and serializes the given value as a member of this object
   * under the given name. \param name The name of the member. \param value The value to serialize as the
   * member.
   */
  template <typename T>
  void object_member(const char* name, const T& value) {
    this->push_object_member(name);
    sge::to_archive(value, *this);
    this->pop();
  }
};
}  // namespace sge

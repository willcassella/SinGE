// ArchiveWriter.h
#pragma once

#include <string>
#include "../Functional/FunctionView.h"
#include "../config.h"

namespace sge
{
	class ArchiveWriter;

	template <typename T>
	void to_archive(const T& value, ArchiveWriter& writer);

	class SGE_CORE_API ArchiveWriter
	{
		///////////////////
		///   Methods   ///
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
		 * \brief Sets this archive node as a numeric value of type int8.
		 * \param value The value to set.
		 */
		virtual void number(int8 value) = 0;

		/**
		 * \brief Sets this archive node as a numeric value of type uint8.
		 * \param value The value to set.
		 */
		virtual void number(uint8 value) = 0;

		/**
		 * \brief Sets this archive node as a numeric value of type int16.
		 * \param value The value to set.
		 */
		virtual void number(int16 value) = 0;

		/**
		 * \brief Sets this archive node as a numeric value of type uint16.
		 * \param value The value to set.
		 */
		virtual void number(uint16 value) = 0;

		/**
		 * \brief Sets this archive node as a numeric value of type int32.
		 * \param value The value to set.
		 */
		virtual void number(int32 value) = 0;

		/**
		 * \brief Sets this archive node as a numeric value of type uint32.
		 * \param value The value to set.
		 */
		virtual void number(uint32 value) = 0;

		/**
		 * \brief Sets this archive node as a numeric value of type int64.
		 * \param value The value to set.
		 */
		virtual void number(int64 value) = 0;

		/**
		 * \brief Sets this archive node as a numeric value of type uint64.
		 * \param value The value to set.
		 */
		virtual void number(uint64 value) = 0;

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
		virtual void string(const char* str, std::size_t len) = 0;

		/**
		 * \brief Sets this archive node as an array of all type bool.
		 * \param array A pointer to the first element of the array.
		 * \param size The number of elements in the array.
		 */
		virtual void typed_array(const bool* array, std::size_t size) = 0;

		/**
		 * \brief Sets this archive node as an array of all type int8.
		 * \param array A pointer to the first element of the array.
		 * \param size The number of elements in the array.
		 */
		virtual void typed_array(const int8* array, std::size_t size) = 0;

		/**
		 * \brief Sets this archive node as an array of all type uint8.
		 * \param array A pointer to the first element of the array.
		 * \param size The number of elements in the array.
		 */
		virtual void typed_array(const uint8* array, std::size_t size) = 0;

		/**
		 * \brief Sets this archive node as an array of all type int16.
		 * \param array A pointer to the first element of the array.
		 * \param size The number of elements in the array.
		 */
		virtual void typed_array(const int16* array, std::size_t size) = 0;

		/**
		 * \brief Sets this archive node as an array of all type uint16.
		 * \param array A pointer to the first element of the array.
		 * \param size The number of elements in the array.
		 */
		virtual void typed_array(const uint16* array, std::size_t size) = 0;

		/**
		 * \brief Sets this archive node as an array of all type int32.
		 * \param array A pointer to the first element of the array.
		 * \param size The number of elements in the array.
		 */
		virtual void typed_array(const int32* array, std::size_t size) = 0;

		/**
		 * \brief Sets this archive node as an array of all type uint32.
		 * \param array A pointer to the first element of the array.
		 * \param size The number of elements in the array.
		 */
		virtual void typed_array(const uint32* array, std::size_t size) = 0;

		/**
		 * \brief Sets this archive node as an array of all type int64.
		 * \param array A pointer to the first element of the array.
		 * \param size The number of elements in the array.
		 */
		virtual void typed_array(const int64* array, std::size_t size) = 0;

		/**
		 * \brief Sets this archive node as an array of all type uint64.
		 * \param array A pointer to the first element of the array.
		 * \param size The number of elements in the array.
		 */
		virtual void typed_array(const uint64* array, std::size_t size) = 0;

		/**
		 * \brief Sets this archive node as an array of all type float.
		 * \param array A pointer to the first element of the array.
		 * \param size The number of elements in the array.
		 */
		virtual void typed_array(const float* array, std::size_t size) = 0;

		/**
		 * \brief Sets this archive node as an array of all type double.
		 * \param array A pointer to the first element of the array.
		 * \param size The number of elements in the array.
		 */
		virtual void typed_array(const double* array, std::size_t size) = 0;

		virtual void push_array_element() = 0;

		template <typename T>
		void array_element(const T& value)
		{
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
		 * \brief Sets this archive node as an object, and serializes the given value as a member of this object under the given name.
		 * \param name The name of the member.
		 * \param value The value to serialize as the member.
		 */
		template <typename T>
		void object_member(const char* name, const T& value)
		{
			this->push_object_member(name);
			sge::to_archive(value, *this);
			this->pop();
		}
	};
}

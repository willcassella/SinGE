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
		 * \brief Sets this archive node as null.
		 */
		virtual void null() = 0;

		/**
		 * \brief Sets this archive node as a value of type boolean.
		 * \param value The value to set.
		 */
		virtual void value(bool value) = 0;

		/**
		 * \brief Sets this archive node as a value of type int8.
		 * \param value The value to set.
		 */
		virtual void value(int8 value) = 0;

		/**
		 * \brief Sets this archive node as a value of type uint8.
		 * \param value The value to set.
		 */
		virtual void value(uint8 value) = 0;

		/**
		 * \brief Sets this archive node as a value of type int16.
		 * \param value The value to set.
		 */
		virtual void value(int16 value) = 0;

		/**
		 * \brief Sets this archive node as a value of type uint16.
		 * \param value The value to set.
		 */
		virtual void value(uint16 value) = 0;

		/**
		 * \brief Sets this archive node as a value of type int32.
		 * \param value The value to set.
		 */
		virtual void value(int32 value) = 0;

		/**
		 * \brief Sets this archive node as a value of type uint32.
		 * \param value The value to set.
		 */
		virtual void value(uint32 value) = 0;

		/**
		 * \brief Sets this archive node as a value of type int64.
		 * \param value The value to set.
		 */
		virtual void value(int64 value) = 0;

		/**
		 * \brief Sets this archive node as a value of type uint64.
		 * \param value The value to set.
		 */
		virtual void value(uint64 value) = 0;

		/**
		 * \brief Sets this archive node as a value of type float.
		 * \param value The value to set.
		 */
		virtual void value(float value) = 0;

		/**
		 * \brief Sets this archive node as a value of type double.
		 * \param value The value to set.
		 */
		virtual void value(double value) = 0;

		/**
		 * \brief Sets this archive node as a value of type string.
		 * \param str The string value.
		 * \param len The length of the string.
		 */
		virtual void value(const char* str, std::size_t len) = 0;

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

		/**
		 * \brief Sets this archive node as an array of untyped elements, and calls the given function
		 * on the writer for the node appended to the array.
		 * \param func The function to call with the writer for the node at the end of the array.
		 */
		virtual void add_array_element(FunctionView<void(ArchiveWriter&)> func) = 0;

		/**
		* \brief Sets this archive node as an array, and serializes the given value as the last element of this array.
		* \param value The value to serialize as the last array element.
		*/
		template <typename T>
		void push_array_element(const T& value)
		{
			this->add_array_element([&](ArchiveWriter& elementWriter) {
				to_archive(value, elementWriter);
			});
		}

		/**
		 * \brief Sets this archive node as an object, and calls the given function
		 * on the writer for the node set as a member of the object.
		 * \param name The name of the member.
		 * \param func The function to call with the writer for the node.
		 */
		virtual void add_object_member(const char* name, FunctionView<void(ArchiveWriter&)> func) = 0;

		/**
		 * \brief Sets this archive node as an object, and serializes the given value as a member of this object under the given name.
		 * \param name The name of the member.
		 * \param value The value to serialize as the member.
		 */
		template <typename T>
		void push_object_member(const char* name, const T& value)
		{
			this->add_object_member(name, [&](ArchiveWriter& childWriter) {
				to_archive(value, childWriter);
			});
		}
	};
}

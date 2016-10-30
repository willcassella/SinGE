// ArchiveReader.h
#pragma once

#include "../Functional/FunctionView.h"
#include "../config.h"

namespace sge
{
	class ArchiveReader;

	template <typename T>
	void from_archive(T& value, const ArchiveReader& reader);

	class SGE_CORE_API ArchiveReader
	{
		///////////////////
		///   Methods   ///
	public:

		/**
		 * \breif Returns whether this node contains 'null'.
		 */
		virtual bool null() const = 0;

		/**
		 * \brief Trys to get a value of type bool out of this node.
		 * \param out The value to assign to, if this node holds a value of type bool.
		 * \return Whether the given value was assigned to.
		 */
		virtual bool value(bool& out) const = 0;

		/**
		 * \brief Trys to get a value of type int8 out of this node.
		 * \param out The value to assign to, if this node holds a value of type int8.
		 * \return Whether the given value as assigned to.
		 */
		virtual bool value(int8& out) const = 0;

		/**
		 * \brief Trys to get a value of type uint8 out of this node.
		 * \param out The value to assign to, if this node holds a value of type uint8.
		 * \return Whether the given value was assigned to.
		 */
		virtual bool value(uint8& out) const = 0;

		/**
		 * \brief Trys to get a value of type int16 out of this node.
		 * \param out The value to assign to, if this node holds a value of type int16.
		 * \return Whether the given value was assigned to.
		 */
		virtual bool value(int16& out) const = 0;

		/**
		 * \brief Trys to get a value of type uint16 out of this node.
		 * \param out The value to assign to, if this node holds a value of type uint16.
		 * \return Whether the given value was assigned to.
		 */
		virtual bool value(uint16& out) const = 0;

		/**
		 * \brief Trys to get a value of type int32 out of this node.
		 * \param out The value to assign to, if this node holds a value of type int32.
		 * \return Whether the given value was assigned to.
		 */
		virtual bool value(int32& out) const = 0;

		/**
		 * \brief Trys to get a value of type uint32 out of this node.
		 * \param out The value to assign to, if this node holds a value of type uint32.
		 * \return Whether the given value was assigned to.
		 */
		virtual bool value(uint32& out) const = 0;

		/**
		 * \brief Trys to get a value of type int64 out of this node.
		 * \param out The value to assign to, if this node holds a value of type int64.
		 * \return Whether the given value was assigned to.
		 */
		virtual bool value(int64& out) const = 0;

		/**
		 * \brief Trys to get a value of type uint64 out of this node.
		 * \param out The value to assign to, if this node holds a value of type uint64.
		 * \return Whether the given value was assigned to.
		 */
		virtual bool value(uint64& out) const = 0;

		/**
		 * \brief Trys to get a value of type float out of this node.
		 * \param out The value to assign to, if this node holds a value of type float.
		 * \return Whether the given value was assigned to.
		 */
		virtual bool value(float& out) const = 0;

		/**
		 * \brief Trys to get a value of type double out of this node.
		 * \param out The value to assign to, if this node holds a value of type double.
		 * \return Whether the given value was assigned to.
		 */
		virtual bool value(double& out) const = 0;

		/**
		 * \brief Trys to get a string value out of this node.
		 * \param outStr A pointer to assign the address of the first character of the string to, if this node contains a string.
		 * \param outLen A value to assign the length of the string to, if this node contains a string.
		 * \return Whether the given values were assigned to.
		 */
		virtual bool value(const char*& outStr, std::size_t& outLen) const = 0;

		/**
		 * \brief Trys to get the size of the array this node holds.
		 * \param out The value to assign the size of the array to, if this node holds an array.
		 * \return Whether the given value was assigned to.
		 */
		virtual bool array_size(std::size_t& out) const = 0;

		/**
		 * \brief Trys to get an array of all type bool out of this node.
		 * \param out A pointer to the array to fill.
		 * \param size The number of elements requested to fill into the array.
		 * \return The number of elements filled into the array.
		 */
		virtual std::size_t typed_array(bool* out, std::size_t size) const = 0;

		/**
		 * \brief Trys to get an array of all type int8 out of this node.
		 * \param out A pointer to the array to fill.
		 * \param size The number of elements requested to fill into the array.
		 * \return The number of elements filled into the array.
		 */
		virtual std::size_t typed_array(int8* out, std::size_t size) const = 0;

		/**
		* \brief Trys to get an array of all type uint8 out of this node.
		* \param out A pointer to the array to fill.
		* \param size The number of elements requested to fill into the array.
		* \return The number of elements filled into the array.
		*/
		virtual std::size_t typed_array(uint8* out, std::size_t size) const = 0;

		/**
		* \brief Trys to get an array of all type int16 out of this node.
		* \param out A pointer to the array to fill.
		* \param size The number of elements requested to fill into the array.
		* \return The number of elements filled into the array.
		*/
		virtual std::size_t typed_array(int16* out, std::size_t size) const = 0;

		/**
		* \brief Trys to get an array of all type uint16 out of this node.
		* \param out A pointer to the array to fill.
		* \param size The number of elements requested to fill into the array.
		* \return The number of elements filled into the array.
		*/
		virtual std::size_t typed_array(uint16* out, std::size_t size) const = 0;

		/**
		* \brief Trys to get an array of all type int32 out of this node.
		* \param out A pointer to the array to fill.
		* \param size The number of elements requested to fill into the array.
		* \return The number of elements filled into the array.
		*/
		virtual std::size_t typed_array(int32* out, std::size_t size) const = 0;

		/**
		* \brief Trys to get an array of all type uint32 out of this node.
		* \param out A pointer to the array to fill.
		* \param size The number of elements requested to fill into the array.
		* \return The number of elements filled into the array.
		*/
		virtual std::size_t typed_array(uint32* out, std::size_t size) const = 0;

		/**
		* \brief Trys to get an array of all type int64 out of this node.
		* \param out A pointer to the array to fill.
		* \param size The number of elements requested to fill into the array.
		* \return The number of elements filled into the array.
		*/
		virtual std::size_t typed_array(int64* out, std::size_t size) const = 0;

		/**
		* \brief Trys to get an array of all type uint64 out of this node.
		* \param out A pointer to the array to fill.
		* \param size The number of elements requested to fill into the array.
		* \return The number of elements filled into the array.
		*/
		virtual std::size_t typed_array(uint64* out, std::size_t size) const = 0;

		/**
		* \brief Trys to get an array of all type float out of this node.
		* \param out A pointer to the array to fill.
		* \param size The number of elements requested to fill into the array.
		* \return The number of elements filled into the array.
		*/
		virtual std::size_t typed_array(float* out, std::size_t size) const = 0;

		/**
		* \brief Trys to get an array of all type double out of this node.
		* \param out A pointer to the array to fill.
		* \param size The number of elements requested to fill into the array.
		* \return The number of elements filled into the array.
		*/
		virtual std::size_t typed_array(double* out, std::size_t size) const = 0;

		/**
		 * \brief Enumerates all array elements of this node, regardless of type, if this node contains an array.
		 * \param enumerator A function to call for each element.
		 */
		virtual void enumerate_array_elements(FunctionView<void(std::size_t i, const ArchiveReader& elementReader)> enumerator) const = 0;

		/**
		 * \brief Trys to get the indexed array element from this node, if this node holds an array.
		 * \param i The index of the array element to try to get.
		 * \param func The function to call if the array element exists.
		 * \return Whether the function was called. This may return 'false' if this node does not contain an array, or the index was out of bounds.
		 */
		virtual bool array_element(std::size_t i, FunctionView<void(const ArchiveReader& elementReader)> func) const = 0;

		/**
		 * \brief Trys to deserialze the given value for the indexed array element of this node, if this node holds an array.
		 * \param i The index of the array element to try to get.
		 * \param out The value to deserialize if the array element exists.
		 * \return Whether the object was deserialized.
		 */
		template <typename T>
		bool pull_array_element(std::size_t i, T& out) const
		{
			return this->get_array_element(i, [&](const ArchiveReader& elementReader) {
				from_archive(out, elementReader);
			});
		}

		/**
		 * \brief Trys to get the number of members of the object held in the node, if this node holds an object.
		 * \param out The value to assign the number of members of the object to, if this node contains an object.
		 * \return Whether the value was assigned.
		 */
		virtual bool object_num_members(std::size_t& out) const = 0;

		/**
		 * \brief Enumerates all members of the object contained in this node, if this node contains an object.
		 * \param enumerator A function to call for each object member.
		 */
		virtual void enumerate_object_members(FunctionView<void(const char* name, const ArchiveReader& memberReader)> enumerator) const = 0;

		/**
		 * \brief Trys to get the named object member from this node, if this node contains an object.
		 * \param name The name of the object member to try to get.
		 * \param func The function to call if the object member exists.
		 * \return Whether the function was called. This may return 'false' if this node does not contain an object, or if the member does not exist.
		 */
		virtual bool object_member(const char* name, FunctionView<void(const ArchiveReader& memberReader)> func) const = 0;

		/**
		 * \brief Trys to deserialize the given value for the named object member of this node, if this node contains an object.
		 * \param name The name of the object member to try to get.
		 * \param out The value to deserialize if the object member exists.
		 * \return Whether the object was deserialized.
		 */
		template <typename T>
		bool pull_object_member(const char* name, T& out) const
		{
			return this->get_object_member(name, [&](const ArchiveReader& memberReader) {
				from_archive(out, memberReader);
			});
		}
	};
}

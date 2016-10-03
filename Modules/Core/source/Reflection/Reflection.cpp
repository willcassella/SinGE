// Reflection.cpp

#include "../../include/Core/Reflection/Reflection.h"

namespace sge
{
	/* Builds the TypeInfo object for primitive types, since TypeInfoBuilder doesn't work. */
	template <typename T>
	NativeTypeInfo build_primitive_type_info(std::string name)
	{
		TypeInfo::Data base_type_data{ std::move(name) };
		base_type_data.size = sizeof(T);
		base_type_data.alignment = alignof(T);

		NativeTypeInfo::Data type_data;
		type_data.init = [](void* addr) {
			*static_cast<T*>(addr) = 0;
		};
		type_data.copy_init = [](void* addr, const void* copy) {
			*static_cast<T*>(addr) = *static_cast<const T*>(copy);
		};
		type_data.move_init = [](void* addr, void* move) {
			*static_cast<T*>(addr) = *static_cast<T*>(move);
		};
		type_data.copy_assign = type_data.copy_init;
		type_data.move_assign = type_data.move_init;
		type_data.drop = [](void* /*self*/) {};

		return{ base_type_data, type_data };
	}

	NativeTypeInfo build_void_type_info()
	{
		return{ TypeInfo::Data{ "void" }, NativeTypeInfo::Data{} };
	}

	namespace specialized
	{
		const NativeTypeInfo GetType<void>::type_info = build_void_type_info();
		const NativeTypeInfo GetType<bool>::type_info = build_primitive_type_info<bool>("bool");
		const NativeTypeInfo GetType<char>::type_info = build_primitive_type_info<char>("char");
		const NativeTypeInfo GetType<byte>::type_info = build_primitive_type_info<byte>("byte");
		const NativeTypeInfo GetType<int16>::type_info = build_primitive_type_info<int16>("int16");
		const NativeTypeInfo GetType<uint16>::type_info = build_primitive_type_info<uint16>("uint16");
		const NativeTypeInfo GetType<int32>::type_info = build_primitive_type_info<int32>("int32");
		const NativeTypeInfo GetType<uint32>::type_info = build_primitive_type_info<uint32>("uint32");
		const NativeTypeInfo GetType<int64>::type_info = build_primitive_type_info<int64>("int64");
		const NativeTypeInfo GetType<uint64>::type_info = build_primitive_type_info<uint64>("uint64");
		const NativeTypeInfo GetType<float>::type_info = build_primitive_type_info<float>("float");
		const NativeTypeInfo GetType<double>::type_info = build_primitive_type_info<double>("double");
		const NativeTypeInfo GetType<long double>::type_info = build_primitive_type_info<long double>("long double");

		const NativeTypeInfo GetType<std::string>::type_info = NativeTypeInfoBuilder<std::string>("sge::String");
	}
}

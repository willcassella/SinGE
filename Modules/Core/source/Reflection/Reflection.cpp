// Reflection.cpp

#include "../../include/Core/Reflection/Reflection.h"

namespace sge
{
	/* Builds the TypeInfo object for primitive types, since TypeInfoBuilder doesn't work. */
	template <typename T>
	TypeInfo::Data build_primitive_type_info(std::string name)
	{
		TypeInfo::Data result{ std::move(name) };
		result.size = sizeof(T);
		result.alignment = alignof(T);
		result.init = [](void* self) { *static_cast<T*>(self) = 0; };
		result.copy_init = [](void* self, const void* copy) { *static_cast<T*>(self) = *static_cast<const T*>(copy); };
		result.move_init = [](void* self, void* move) { *static_cast<T*>(self) = *static_cast<T*>(move); };
		result.copy_assign = result.copy_init;
		result.move_assign = result.move_init;
		result.drop = [](void* /*self*/) {};

		return result;
	}

	TypeInfo::Data build_void_type_info()
	{
		return{ "void" };
	}

	namespace specialized
	{
		const TypeInfo GetType<void>::type_info = build_void_type_info();
		const TypeInfo GetType<bool>::type_info = build_primitive_type_info<bool>("bool");
		const TypeInfo GetType<char>::type_info = build_primitive_type_info<char>("char");
		const TypeInfo GetType<byte>::type_info = build_primitive_type_info<byte>("byte");
		const TypeInfo GetType<int16>::type_info = build_primitive_type_info<int16>("int16");
		const TypeInfo GetType<uint16>::type_info = build_primitive_type_info<uint16>("uint16");
		const TypeInfo GetType<int32>::type_info = build_primitive_type_info<int32>("int32");
		const TypeInfo GetType<uint32>::type_info = build_primitive_type_info<uint32>("uint32");
		const TypeInfo GetType<int64>::type_info = build_primitive_type_info<int64>("int64");
		const TypeInfo GetType<uint64>::type_info = build_primitive_type_info<uint64>("uint64");
		const TypeInfo GetType<float>::type_info = build_primitive_type_info<float>("float");
		const TypeInfo GetType<double>::type_info = build_primitive_type_info<double>("double");
		const TypeInfo GetType<long double>::type_info = build_primitive_type_info<long double>("long double");
	}
}

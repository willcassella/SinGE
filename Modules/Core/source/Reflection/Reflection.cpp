// Reflection.cpp

#include "../../include/Core/Reflection/Reflection.h"

namespace sge
{
	/* Builds the TypeInfo object for primitive types, since TypeInfoBuilder doesn't work. */
	template <typename T>
	TypeInfo build_primitive_TypeInfo(std::string name)
	{
		TypeInfo result;
		result.name = std::move(name);
		result.size = sizeof(T);
		result.alignment = alignof(T);
		result.init = [](void* self) { *static_cast<T*>(self) = 0; };
		result.copy_init = [](void* self, const void* copy) { *static_cast<T*>(self) = *static_cast<const T*>(copy); };
		result.move_init = [](void* self, void* move) { *static_cast<T*>(self) = *static_cast<T*>(move); };
		result.copy_assign = result.copy_init;
		result.move_assign = result.move_init;

		return result;
	}

	namespace specialized
	{
		const TypeInfo GetType<char>::type_info = build_primitive_TypeInfo<char>("char");
		const TypeInfo GetType<byte>::type_info = build_primitive_TypeInfo<byte>("byte");
		const TypeInfo GetType<int16>::type_info = build_primitive_TypeInfo<int16>("int16");
		const TypeInfo GetType<uint16>::type_info = build_primitive_TypeInfo<uint16>("uint16");
		const TypeInfo GetType<int32>::type_info = build_primitive_TypeInfo<int32>("int32");
		const TypeInfo GetType<uint32>::type_info = build_primitive_TypeInfo<uint32>("uint32");
		const TypeInfo GetType<int64>::type_info = build_primitive_TypeInfo<int64>("int64");
		const TypeInfo GetType<uint64>::type_info = build_primitive_TypeInfo<uint64>("uint64");
		const TypeInfo GetType<float>::type_info = build_primitive_TypeInfo<float>("float");
		const TypeInfo GetType<double>::type_info = build_primitive_TypeInfo<double>("double");
		const TypeInfo GetType<long double>::type_info = build_primitive_TypeInfo<long double>("long double");
	}
}

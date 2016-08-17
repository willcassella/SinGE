// Reflection.cpp

#include "../../include/Core/Reflection/Reflection.h"

namespace singe
{
	namespace specialized
	{
		const TypeInfo GetType<char>::type_info = TypeInfoBuilder<char>("char");
		const TypeInfo GetType<byte>::type_info = TypeInfoBuilder<byte>("byte");
		const TypeInfo GetType<int16>::type_info = TypeInfoBuilder<int16>("int16");
		const TypeInfo GetType<uint16>::type_info = TypeInfoBuilder<uint16>("uint16");
		const TypeInfo GetType<int32>::type_info = TypeInfoBuilder<int32>("int32");
		const TypeInfo GetType<uint32>::type_info = TypeInfoBuilder<uint32>("uint32");
		const TypeInfo GetType<int64>::type_info = TypeInfoBuilder<int64>("int64");
		const TypeInfo GetType<uint64>::type_info = TypeInfoBuilder<uint64>("uint64");
		const TypeInfo GetType<float>::type_info = TypeInfoBuilder<float>("float");
		const TypeInfo GetType<double>::type_info = TypeInfoBuilder<double>("double");
		const TypeInfo GetType<long double>::type_info = TypeInfoBuilder<long double>("long double");
	}
}

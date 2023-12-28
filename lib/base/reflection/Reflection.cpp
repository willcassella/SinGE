#include <stdint.h>

#include "lib/base/reflection/reflection_builder.h"

namespace sge
{
    /* Builds the TypeInfo object for primitive types, since TypeInfoBuilder doesn't work. */
    template <typename T>
    NativeTypeInfo build_primitive_type_info(std::string name)
    {
        TypeInfo::Data base_type_data{std::move(name)};
        base_type_data.flags = TF_PRIMITIVE;
        base_type_data.size = sizeof(T);
        base_type_data.alignment = alignof(T);

        NativeTypeInfo::Data type_data;
        type_data.init = [](void *addr)
        {
            *static_cast<T *>(addr) = 0;
        };
        type_data.copy_init = [](void *addr, const void *copy)
        {
            *static_cast<T *>(addr) = *static_cast<const T *>(copy);
        };
        type_data.move_init = [](void *addr, void *move)
        {
            *static_cast<T *>(addr) = *static_cast<T *>(move);
        };
        type_data.copy_assign = type_data.copy_init;
        type_data.move_assign = type_data.move_init;
        type_data.drop = [](void * /*self*/) {};

        return {base_type_data, type_data};
    }

    NativeTypeInfo build_void_type_info()
    {
        return {TypeInfo::Data{"void"}, NativeTypeInfo::Data{}};
    }

    namespace specialized
    {
        const NativeTypeInfo GetType<void>::type_info = build_void_type_info();
        const NativeTypeInfo GetType<bool>::type_info = build_primitive_type_info<bool>("bool");
        const NativeTypeInfo GetType<int8_t>::type_info = build_primitive_type_info<int8_t>("int8");
        const NativeTypeInfo GetType<uint8_t>::type_info = build_primitive_type_info<uint8_t>("uint8");
        const NativeTypeInfo GetType<int16_t>::type_info = build_primitive_type_info<int16_t>("int16");
        const NativeTypeInfo GetType<uint16_t>::type_info = build_primitive_type_info<uint16_t>("uint16");
        const NativeTypeInfo GetType<int32_t>::type_info = build_primitive_type_info<int32_t>("int32");
        const NativeTypeInfo GetType<uint32_t>::type_info = build_primitive_type_info<uint32_t>("uint32");
        const NativeTypeInfo GetType<int64_t>::type_info = build_primitive_type_info<int64_t>("int64");
        const NativeTypeInfo GetType<uint64_t>::type_info = build_primitive_type_info<uint64_t>("uint64");
        const NativeTypeInfo GetType<float>::type_info = build_primitive_type_info<float>("float");
        const NativeTypeInfo GetType<double>::type_info = build_primitive_type_info<double>("double");

        const NativeTypeInfo GetType<std::string>::type_info = NativeTypeInfoBuilder<std::string>("sge::String")
                                                                   .flags(TF_RECURSE_TERMINAL);
    }
}

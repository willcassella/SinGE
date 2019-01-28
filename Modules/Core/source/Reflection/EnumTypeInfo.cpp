// EnumTypeInfo.cpp

#include "../../include/Core/Reflection/EnumTypeInfo.h"
#include "../../include/Core/Reflection/Reflection.h"
#include "../../include/Core/Functional/FunctionView.h"
#include "../../include/Core/Reflection/ConstructorInfo.h"

namespace sge
{
    bool EnumPropertyInfo::is_read_only() const
    {
        return false;
    }

    void EnumPropertyInfo::get(const void* self, GetterOutFn out) const
    {
        const bool value = enum_get(*static_cast<const int*>(self));
        out(Any<>{ sge::get_type<bool>(), &value });
    }

    void EnumPropertyInfo::set(void* self, const void* value) const
    {
        const int enum_value = enum_set(*static_cast<const int*>(self), *static_cast<const bool*>(value));
        *static_cast<int*>(self) = enum_value;
    }

    void EnumPropertyInfo::mutate(void* self, MutatorFn mutator) const
    {
        bool value = enum_get(*static_cast<const int*>(self));
        mutator(AnyMut<>{ sge::get_type<bool>(), &value });
        const int enum_value = enum_set(*static_cast<const int*>(self), value);
        *static_cast<int*>(self) = enum_value;
    }

    bool EnumPropertyInfo::enum_get(int enum_value) const
    {
        if (_data.is_bit_flag)
        {
            return (enum_value & _data.value) != 0;
        }
        else
        {
            return enum_value == _data.value;
        }
    }

    int EnumPropertyInfo::enum_set(int enum_value, bool value) const
    {
        if (enum_get(enum_value) == value)
        {
            return enum_value;
        }

        if (!_data.is_bit_flag)
        {
            enum_value = 0;
        }

        if (value)
        {
            enum_value |= _data.value;
        }
        else
        {
            enum_value &= ~_data.value;
        }

        return enum_value;
    }

    bool EnumTypeInfo::has_init() const
    {
        return true;
    }

    void EnumTypeInfo::init(void* addr) const
    {
        *static_cast<int*>(addr) = 0;
    }

    bool EnumTypeInfo::has_copy_init() const
    {
        return true;
    }

    void EnumTypeInfo::copy_init(void* addr, const void* copy) const
    {
        *static_cast<int*>(addr) = *static_cast<const int*>(copy);
    }

    bool EnumTypeInfo::has_move_init() const
    {
        return true;
    }

    void EnumTypeInfo::move_init(void* addr, void* move) const
    {
        copy_init(addr, move);
    }

    bool EnumTypeInfo::has_copy_assign() const
    {
        return true;
    }

    void EnumTypeInfo::copy_assign(void* self, const void* copy) const
    {
        copy_init(self, copy);
    }

    bool EnumTypeInfo::has_move_assign() const
    {
        return true;
    }

    void EnumTypeInfo::move_assign(void* self, void* move) const
    {
        copy_init(self, move);
    }

    bool EnumTypeInfo::has_drop() const
    {
        return true;
    }

    void EnumTypeInfo::drop(void* /*self*/) const
    {
    }

    bool EnumTypeInfo::has_equality_compare() const
    {
        return true;
    }

    bool EnumTypeInfo::equality_compare(const void* lhs, const void* rhs) const
    {
        return *static_cast<const int*>(lhs) == *static_cast<const int*>(rhs);
    }

    std::size_t EnumTypeInfo::num_constructors() const
    {
        return 0;
    }

    const ConstructorInfo* EnumTypeInfo::find_constructor(std::size_t /*argc*/) const
    {
        return nullptr;
    }

    void EnumTypeInfo::enumerate_constructors(FunctionView<ConstructorEnumeratorFn> /*enumerator*/) const
    {
    }

    std::size_t EnumTypeInfo::num_named_constructors() const
    {
        return 0;
    }

    const ConstructorInfo* EnumTypeInfo::find_named_constructor(const char* /*name*/) const
    {
        return nullptr;
    }

    void EnumTypeInfo::enumerate_named_constructors(FunctionView<NamedConstructorEnumeratorFn> /*enumerator*/) const
    {
    }

    std::size_t EnumTypeInfo::num_properties() const
    {
        return _data.values.size();
    }

    const PropertyInfo* EnumTypeInfo::find_property(const char* name) const
    {
        const auto iter = _data.values.find(name);
        return iter != _data.values.end() ? &iter->second : nullptr;
    }

    void EnumTypeInfo::enumerate_properties(FunctionView<PropertyEnumeratorFn> enumerator) const
    {
        for (const auto& value : _data.values)
        {
            enumerator(value.first.c_str(), value.second);
        }
    }

    std::size_t EnumTypeInfo::num_fields() const
    {
        return 0;
    }

    const FieldInfo* EnumTypeInfo::find_field(const char* /*name*/) const
    {
        return nullptr;
    }

    void EnumTypeInfo::enumerate_fields(FunctionView<FieldEnumeratorFn> /*enumerator*/) const
    {
    }

    const void* EnumTypeInfo::get_implementation(const InterfaceInfo& interf) const
    {
        const auto iter = _data.interfaces.find(&interf);
        return iter != _data.interfaces.end() ? iter->second : nullptr;
    }
}

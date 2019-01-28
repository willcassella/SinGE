// EnumTypeInfo.h
#pragma once

#include <map>
#include "../config.h"
#include "PropertyInfo.h"
#include "TypeInfo.h"

namespace sge
{
    struct SGE_CORE_API EnumPropertyInfo final : PropertyInfo
    {
        struct Data
        {
            int value = 0;
            bool is_bit_flag = false;
        };

        ////////////////////////
        ///   Constructors   ///
    public:

        EnumPropertyInfo(PropertyInfo::Data base_data, Data data)
            : PropertyInfo(std::move(base_data)), _data(std::move(data))
        {
        }

        ///////////////////
        ///   Methods   ///
    public:

        bool is_read_only() const override;

        void get(const void* self, GetterOutFn out) const override;

        void set(void* self, const void* value) const override;

        void mutate(void* self, MutatorFn mutator) const override;

    private:

        bool enum_get(int enum_value) const;

        int enum_set(int enum_value, bool value) const;

        //////////////////
        ///   Fields   ///
    private:

        Data _data;
    };

    template <typename E>
    struct EnumInfoBuilder;

    struct SGE_CORE_API EnumTypeInfo final : TypeInfo
    {
        struct Data
        {
            std::map<std::string, EnumPropertyInfo> values;
            std::map<const InterfaceInfo*, const void*> interfaces;
        };

        ////////////////////////
        ///   Constructors   ///
    public:

        EnumTypeInfo(TypeInfo::Data base_data, Data data)
            : TypeInfo(std::move(base_data)), _data(std::move(data))
        {
        }

        template <typename E>
        EnumTypeInfo(EnumInfoBuilder<E>&& builder)
            : EnumTypeInfo(std::move(builder).base_type_data, std::move(builder).enum_data)
        {
        }

        ///////////////////
        ///   Methods   ///
    public:

        bool has_init() const override;

        void init(void* addr) const override;

        bool has_copy_init() const override;

        void copy_init(void* addr, const void* copy) const override;

        bool has_move_init() const override;

        void move_init(void* addr, void* move) const override;

        bool has_copy_assign() const override;

        void copy_assign(void* self, const void* copy) const override;

        bool has_move_assign() const override;

        void move_assign(void* self, void* move) const override;

        bool has_drop() const override;

        void drop(void* self) const override;

        bool has_equality_compare() const override;

        bool equality_compare(const void* lhs, const void* rhs) const override;

        std::size_t num_constructors() const override;

        const ConstructorInfo* find_constructor(std::size_t argc) const override;

        void enumerate_constructors(FunctionView<ConstructorEnumeratorFn> enumerator) const override;

        std::size_t num_named_constructors() const override;

        const ConstructorInfo* find_named_constructor(const char* name) const override;

        void enumerate_named_constructors(FunctionView<NamedConstructorEnumeratorFn> enumerator) const override;

        std::size_t num_properties() const override;

        const PropertyInfo* find_property(const char* name) const override;

        void enumerate_properties(FunctionView<PropertyEnumeratorFn> enumerator) const override;

        std::size_t num_fields() const override;

        const FieldInfo* find_field(const char* name) const override;

        void enumerate_fields(FunctionView<FieldEnumeratorFn> enumerator) const override;

        const void* get_implementation(const InterfaceInfo& interf) const override;

        //////////////////
        ///   Fields   ///
    private:

        Data _data;
    };
}

// NativeTypeInfo.h
#pragma once

#include <unordered_map>
#include "base/reflection/TypeInfo.h"
#include "base/reflection/NativeConstructorInfo.h"
#include "base/reflection/NativePropertyInfo.h"
#include "base/reflection/FieldInfo.h"

namespace sge
{
    struct InterfaceInfo;

    template <typename T>
    struct NativeTypeInfoBuilder;

    struct SGE_BASE_EXPORT NativeTypeInfo final : TypeInfo
    {
        using InitFn = void(void *addr);
        using CopyInitFn = void(void *addr, const void *copy);
        using MoveInitFn = void(void *addr, void *move);
        using CopyAssignFn = void(void *self, const void *copy);
        using MoveAssignFn = void(void *self, void *move);
        using DropFn = void(void *self);
        using EqualityCompareFn = bool(const void *lhs, const void *rhs);

        struct Data
        {
            ////////////////////////
            ///   Constructors   ///
        public:
            Data()
                : init(nullptr),
                  copy_init(nullptr),
                  move_init(nullptr),
                  copy_assign(nullptr),
                  move_assign(nullptr),
                  drop(nullptr),
                  equality_compare(nullptr)
            {
            }

            //////////////////
            ///   Fields   ///
        public:
            InitFn *init;
            CopyInitFn *copy_init;
            MoveInitFn *move_init;
            CopyAssignFn *copy_assign;
            MoveAssignFn *move_assign;
            DropFn *drop;
            EqualityCompareFn *equality_compare;
            std::unordered_map<const InterfaceInfo *, const void *> interfaces;
            std::unordered_map<std::size_t, NativeConstructorInfo> constructors;
            std::unordered_map<std::string, NativeConstructorInfo> named_constructors;
            std::unordered_map<std::string, NativePropertyInfo> properties;
            std::unordered_map<std::string, FieldInfo> fields;
        };

        ////////////////////////
        ///   Constructors   ///
    public:
        NativeTypeInfo(TypeInfo::Data baseData, Data data)
            : TypeInfo(std::move(baseData)), _data(std::move(data))
        {
        }

        template <typename T>
        NativeTypeInfo(NativeTypeInfoBuilder<T> &&builder)
            : NativeTypeInfo(std::move(builder).base_type_data, std::move(builder).type_data)
        {
        }

        ///////////////////
        ///   Methods   ///
    public:
        bool has_init() const override
        {
            return _data.init != nullptr;
        }

        void init(void *addr) const override
        {
            _data.init(addr);
        }

        bool has_copy_init() const override
        {
            return _data.copy_init != nullptr;
        }

        void copy_init(void *addr, const void *copy) const override
        {
            return _data.copy_init(addr, copy);
        }

        bool has_move_init() const override
        {
            return _data.move_init != nullptr;
        }

        void move_init(void *addr, void *move) const override
        {
            _data.move_init(addr, move);
        }

        bool has_copy_assign() const override
        {
            return _data.copy_assign != nullptr;
        }

        void copy_assign(void *self, const void *copy) const override
        {
            _data.copy_assign(self, copy);
        }

        bool has_move_assign() const override
        {
            return _data.move_assign != nullptr;
        }

        void move_assign(void *self, void *move) const override
        {
            return _data.move_assign(self, move);
        }

        bool has_drop() const override
        {
            return _data.drop != nullptr;
        }

        void drop(void *self) const override
        {
            _data.drop(self);
        }

        bool has_equality_compare() const override
        {
            return _data.equality_compare != nullptr;
        }

        bool equality_compare(const void *lhs, const void *rhs) const override
        {
            return _data.equality_compare(lhs, rhs);
        }

        std::size_t num_constructors() const override
        {
            return _data.constructors.size();
        }

        const NativeConstructorInfo *find_constructor(std::size_t argc) const override
        {
            auto ctor = _data.constructors.find(argc);
            return ctor == _data.constructors.end() ? nullptr : &ctor->second;
        }

        void enumerate_constructors(FunctionView<ConstructorEnumeratorFn> enumerator) const override
        {
            for (const auto &ctor : _data.constructors)
            {
                enumerator(ctor.first, ctor.second);
            }
        }

        std::size_t num_named_constructors() const override
        {
            return _data.named_constructors.size();
        }

        const NativeConstructorInfo *find_named_constructor(const char *name) const override
        {
            auto ctor = _data.named_constructors.find(name);
            return ctor == _data.named_constructors.end() ? nullptr : &ctor->second;
        }

        void enumerate_named_constructors(FunctionView<NamedConstructorEnumeratorFn> enumerator) const override
        {
            for (const auto &ctor : _data.named_constructors)
            {
                enumerator(ctor.first.c_str(), ctor.second);
            }
        }

        std::size_t num_properties() const override
        {
            return _data.properties.size();
        }

        const NativePropertyInfo *find_property(const char *name) const override
        {
            auto prop = _data.properties.find(name);
            return prop == _data.properties.end() ? nullptr : &prop->second;
        }

        void enumerate_properties(FunctionView<PropertyEnumeratorFn> enumerator) const override
        {
            for (const auto &prop : _data.properties)
            {
                enumerator(prop.first.c_str(), prop.second);
            }
        }

        std::size_t num_fields() const override
        {
            return _data.fields.size();
        }

        const FieldInfo *find_field(const char *name) const override
        {
            auto field = _data.fields.find(name);
            return field == _data.fields.end() ? nullptr : &field->second;
        }

        void enumerate_fields(FunctionView<FieldEnumeratorFn> enumerator) const override
        {
            for (const auto &field : _data.fields)
            {
                enumerator(field.first.c_str(), field.second);
            }
        }

        const void *get_implementation(const InterfaceInfo &interf) const override
        {
            auto iter = _data.interfaces.find(&interf);
            return iter == _data.interfaces.end() ? nullptr : iter->second;
        }

        //////////////////
        ///   Fields   ///
    private:
        Data _data;
    };
}

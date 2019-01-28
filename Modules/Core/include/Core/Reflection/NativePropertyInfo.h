// NativePropertyInfo.h
#pragma once

#include <functional>
#include "PropertyInfo.h"

namespace sge
{
    struct SGE_CORE_API NativePropertyInfo final : PropertyInfo
    {
        struct Data
        {
            //////////////////
            ///   Fields   ///
        public:

            std::function<void(const void* self, GetterOutFn out)> getter;
            std::function<void(void* self, const void* value)> setter;
            std::function<void(void* self, MutatorFn mutator)> mutate;
        };

        ////////////////////////
        ///   Constructors   ///
    public:

        NativePropertyInfo(PropertyInfo::Data baseData, Data data)
            : PropertyInfo(std::move(baseData)), _data(std::move(data))
        {
        }

        ///////////////////
        ///   Methods   ///
    public:

        bool is_read_only() const override
        {
            return _data.setter == nullptr;
        }

        void get(const void* self, GetterOutFn out) const override
        {
            _data.getter(self, out);
        }

        void set(void* self, const void* value) const override
        {
            _data.setter(self, value);
        }

        void mutate(void* self, MutatorFn mutator) const override
        {
            _data.mutate(self, mutator);
        }

        //////////////////
        ///   Fields   ///
    private:

        Data _data;
    };
}

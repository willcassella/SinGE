#pragma once

#include <functional>

#include "lib/base/reflection/property_info.h"

namespace sge {
struct SGE_BASE_EXPORT NativePropertyInfo final : PropertyInfo {
  struct Data {
    std::function<void(const void* self, GetterOutFn out)> getter;
    std::function<void(void* self, const void* value)> setter;
    std::function<void(void* self, MutatorFn mutator)> mutate;
  };

  NativePropertyInfo(PropertyInfo::Data baseData, Data data)
      : PropertyInfo(std::move(baseData)), _data(std::move(data)) {}

  bool is_read_only() const override { return _data.setter == nullptr; }

  void get(const void* self, GetterOutFn out) const override { _data.getter(self, out); }

  void set(void* self, const void* value) const override { _data.setter(self, value); }

  void mutate(void* self, MutatorFn mutator) const override { _data.mutate(self, mutator); }

 private:
  Data _data;
};
}  // namespace sge

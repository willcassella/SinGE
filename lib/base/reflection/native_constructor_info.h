#pragma once

#include <functional>

#include "lib/base/reflection/constructor_info.h"

namespace sge {
struct SGE_BASE_EXPORT NativeConstructorInfo final : ConstructorInfo {
  struct Data {
    Data() : constructor(nullptr) {}

    std::function<void(void* addr, const ArgAny* args)> constructor;
  };

  NativeConstructorInfo(ConstructorInfo::Data baseData, Data data)
      : ConstructorInfo(std::move(baseData)), _data(std::move(data)) {}

  void invoke(void* addr, const ArgAny* args) const override { _data.constructor(addr, args); }

 private:
  Data _data;
};
}  // namespace sge

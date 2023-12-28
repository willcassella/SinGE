#pragma once

#include <string>
#include <unordered_map>

namespace sge {
struct TypeInfo;

template <class I>
struct InterfaceInfoBuilder;

struct SGE_BASE_EXPORT InterfaceInfo {
  struct Data {
    std::string name;
    std::unordered_map<const TypeInfo*, const void*> implementations;
  };

  InterfaceInfo(Data data) : _data(std::move(data)) {}

  template <class I>
  InterfaceInfo(InterfaceInfoBuilder<I>&& builder) : InterfaceInfo(std::move(builder).interface_data) {}

  const std::string& name() const { return _data.name; }

  const void* get_implementation(const TypeInfo& type) const {
    auto iter = _data.implementations.find(&type);
    return iter == _data.implementations.end() ? nullptr : iter->second;
  }

 private:
  Data _data;
};
}  // namespace sge

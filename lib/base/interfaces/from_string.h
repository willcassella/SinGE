#pragma once

#include <assert.h>
#include <stdint.h>
#include <string>

#include "lib/base/reflection/reflection.h"
#include "lib/base/util/interface_utils.h"

namespace sge {
struct SGE_BASE_EXPORT IFromString {
  SGE_REFLECTED_INTERFACE;
  SGE_INTERFACE_1(IFromString, from_string)

  size_t (*from_string)(SelfMut self, const char* str, size_t len);
};

template <typename T>
struct Impl<IFromString, T> {
  static size_t from_string(SelfMut self, const char* str, size_t len) {
    return self.as<T>()->from_string(str, len);
  }
};

template <>
struct Impl<IFromString, int32_t> {
  static size_t from_string(SelfMut self, const char* str, size_t /*len*/) {
    assert(!self.null());
    char* end;
    *self.as<int32_t>() = strtol(str, &end, 10);
    return end - str;
  }
};

template <>
struct Impl<IFromString, uint32_t> {
  static size_t from_string(SelfMut self, const char* str, size_t /*len*/) {
    assert(!self.null());
    char* end;
    *self.as<uint32_t>() = strtoul(str, &end, 10);
    return end - str;
  }
};

template <>
struct Impl<IFromString, int64_t> {
  static size_t from_string(SelfMut self, const char* str, size_t /*len*/) {
    assert(!self.null());
    char* end;
    *self.as<int64_t>() = strtoll(str, &end, 10);
    return end - str;
  }
};

template <>
struct Impl<IFromString, uint64_t> {
  static size_t from_string(SelfMut self, const char* str, size_t /*len*/) {
    assert(!self.null());
    char* end;
    *self.as<uint64_t>() = strtoull(str, &end, 10);
    return end - str;
  }
};

template <>
struct Impl<IFromString, float> {
  static size_t from_string(SelfMut self, const char* str, size_t /*len*/) {
    assert(!self.null());
    char* end;
    *self.as<float>() = strtof(str, &end);
    return end - str;
  }
};

template <>
struct Impl<IFromString, double> {
  static size_t from_string(SelfMut self, const char* str, size_t /*len*/) {
    assert(!self.null());
    char* end;
    *self.as<double>() = strtod(str, &end);
    return str - end;
  }
};

template <typename T>
size_t from_string(T& self, const char* str, size_t len) {
  return Impl<IFromString, T>::from_string(&self, str, len);
}
}  // namespace sge

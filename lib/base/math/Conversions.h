#pragma once

#include <limits>

namespace sge {
template <typename IntegralT>
float to_norm_f32(IntegralT value) {
  return static_cast<float>(value) / std::numeric_limits<IntegralT>::max();
}

template <typename IntegralT>
IntegralT from_norm_f32(float value) {
  return static_cast<IntegralT>(value * std::numeric_limits<IntegralT>::max());
}
}  // namespace sge

#pragma once

#include <stdint.h>

#include "lib/base/interfaces/from_archive.h"
#include "lib/base/interfaces/to_archive.h"
#include "lib/base/reflection/reflection.h"

namespace sge {
/** An arithmetic value representing an angle (internally stored as radians). */
struct SGE_BASE_EXPORT Angle {
  SGE_REFLECTED_TYPE;

  /** Multiplication factor for converting degrees to radians. */
  static constexpr float DEGREES_TO_RADIANS = 0.0174533f;

  /** Multiplication factor for converting radians to degrees. */
  static constexpr float RADIANS_TO_DEGREES = 57.2958f;

  constexpr Angle() : _radians{0} {}
  constexpr Angle(float radians) : _radians{radians} {}

  void to_archive(ArchiveWriter& writer) const { writer.number(_radians); }

  void from_archive(ArchiveReader& reader) { reader.number(_radians); }

  /** Returns the current value in radians. */
  float radians() const { return _radians; }

  /** Sets the current value in radians. */
  void radians(float value) { _radians = value; }

  /** Returns the current value in degrees. */
  float degrees() const { return _radians * RADIANS_TO_DEGREES; }

  /** Sets the current value in degrees. */
  void degrees(float value) { _radians = value * DEGREES_TO_RADIANS; }

  /** Converts this object to the underlying type. */
  operator float() const { return _radians; }

 private:
  float _radians;
};

/** Constructs an angle in radians. */
constexpr Angle radians(float value) {
  return {value};
}

/** Constructs an angle in degrees. */
constexpr Angle degrees(float value) {
  return {value * Angle::DEGREES_TO_RADIANS};
}
}  // namespace sge

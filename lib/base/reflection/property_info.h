#pragma once

#include <stdint.h>
#include <string>
#include <utility>

#include "lib/base/functional/function_view.h"
#include "lib/base/reflection/any.h"

namespace sge {
struct TypeInfo;

using PropertyFlags_t = uint32_t;
enum PropertyFlags : PropertyFlags_t {
  /**
   * \brief This property has no special flags.
   */
  PF_NONE = 0,

  /**
   * \brief This property is intended exclusively for use by the editor, and should not be exposed to
   * scripting.
   */
  PF_EDITOR_ONLY = (1 << 0),

  /**
   * \brief This property is not intended to be exposed to the editor.
   */
  PF_EDITOR_HIDDEN = (1 << 1),

  /**
   * \brief By default, the value of this property is not visible in the editor unless expanded.
   */
  PF_EDITOR_DEFAULT_COLLAPSED = (1 << 2)
};

struct SGE_BASE_EXPORT PropertyInfo {
  using GetterOutFn = FunctionView<void(Any<> value)>;
  using MutatorFn = FunctionView<void(AnyMut<> value)>;

  struct Data {
    PropertyFlags_t flags = PF_NONE;
    uint32_t index = 0;
    std::string category;
    std::string description;
    const TypeInfo* type = nullptr;
  };

  PropertyInfo(Data data) : _data(std::move(data)) {}

  /**
   * \brief The type of this property.
   */
  const TypeInfo& type() const { return *_data.type; }

  /**
   * \brief Any special flags for this property.
   */
  PropertyFlags_t flags() const { return _data.flags; }

  /**
   * \brief Returns the property registration index (used to order properties).
   */
  uint32_t index() const { return _data.index; }

  /**
   * \brief Returns the categorry for this property.
   */
  const std::string& category() const { return _data.category; }

  /**
   * \brief Returns the description for this property.
   */
  const std::string& description() const { return _data.description; }

  /**
   * \brief Whether this property may be read from, but not written to ('set' or 'mutate').
   */
  virtual bool is_read_only() const = 0;

  /**
   * \brief Acesses the value of this property.
   * \param self The object to access the property on.
   * \param out A function to call with the value and type of the property.
   */
  virtual void get(const void* self, GetterOutFn out) const = 0;

  /**
   * \brief Sets the value of this property.
   * \param self The object to set the property on.
   * \param value The new value of the property.
   */
  virtual void set(void* self, const void* value) const = 0;

  /**
   * \brief Mutates the value of this property.
   * \param self The object to mutate the property on.
   * \param mutator A function to call that will mutate the value of the property.
   */
  virtual void mutate(void* self, MutatorFn mutator) const = 0;

 private:
  Data _data;
};
}  // namespace sge

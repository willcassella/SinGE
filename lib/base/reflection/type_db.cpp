#include "lib/base/reflection/type_db.h"
#include "lib/base/reflection/type_info.h"

namespace sge {
void TypeDB::push_new_type_callback(std::function<TypeEntryCallbackFn> callback) {
  _new_type_callbacks.push(std::move(callback));
}

void TypeDB::push_remove_type_callback(std::function<TypeEntryCallbackFn> callback) {
  _remove_type_callbacks.push(std::move(callback));
}

void TypeDB::new_type(const TypeInfo& type) {
  auto name = type.name();

  // See if an entry exists for the current name
  auto iter = _registered_types.find(name);
  if (iter != _registered_types.end()) {
    // If we've already registered this type
    if (type == *iter->second) {
      return;
    }

    // We've got to remove the current type
    remove_type(*iter->second);
  }

  // Add the type to the database
  _registered_types.insert(std::make_pair(std::move(name), &type));

  // Set up callbacks
  decltype(_new_type_callbacks) callbacks;
  std::swap(callbacks, _new_type_callbacks);

  // Run all callbacks
  while (!callbacks.empty()) {
    callbacks.front()(*this, type);
    callbacks.pop();
  }
}

void TypeDB::remove_type(const TypeInfo& type) {
  // Search for the type in the database
  auto iter = _registered_types.find(type.name());
  if (iter == _registered_types.end()) {
    return;
  }

  // Remove the type
  _registered_types.erase(iter);

  // Set up callbacks
  decltype(_remove_type_callbacks) callbacks;
  std::swap(callbacks, _remove_type_callbacks);

  // Run all callbacks
  while (!callbacks.empty()) {
    callbacks.front()(*this, type);
    callbacks.pop();
  }
}

const TypeInfo* TypeDB::find_type(const char* name) const {
  auto iter = _registered_types.find(name);
  return iter != _registered_types.end() ? iter->second : nullptr;
}

bool TypeDB::has_type(const TypeInfo& type) const {
  auto iter = _registered_types.find(type.name());
  return iter != _registered_types.end() && *iter->second == type;
}
}  // namespace sge

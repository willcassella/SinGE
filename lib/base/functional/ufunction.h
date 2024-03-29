#pragma once

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <functional>
#include <type_traits>

#include "lib/base/env.h"
#include "lib/base/memory/functions.h"

namespace sge {
template <typename Fn>
struct UFunction;

/**
 * \brief A 'Unique Function' object.
 * This is a replacement for std::function, with greater restrictions: It cannot be copied (great for
 * move-only function types though, which std::function doesn't support), and the call operator must be
 * 'const'.
 */
template <typename RetT, typename... ArgTs>
struct UFunction<RetT(ArgTs...)> {
  static constexpr size_t LOCAL_BUFFER_SIZE = sizeof(void*) * 2;

  union Data {
    void (*erased_fptr)();
    void* erased_heap_fobj;
    uint8_t erased_local_fobj[LOCAL_BUFFER_SIZE];
  };

  using InvokerFn = RetT(const Data& data, ArgTs... args);
  using DestructorFn = void(Data& data);

  UFunction() : _invoker(nullptr), _destructor(nullptr) {}
  ~UFunction() { destroy(); }
  UFunction(std::nullptr_t) : UFunction() {}
  UFunction(UFunction& copy) = delete;
  UFunction(const UFunction& copy) = delete;
  UFunction& operator=(UFunction& copy) = delete;
  UFunction& operator=(const UFunction& copy) = delete;
  UFunction& operator=(std::nullptr_t) {
    destroy();
    _invoker = nullptr;
    _destructor = nullptr;
    return *this;
  }
  UFunction(UFunction&& move) : _data(move._data), _invoker(move._invoker), _destructor(move._destructor) {
    move._invoker = nullptr;
    move._destructor = nullptr;
  }
  UFunction& operator=(UFunction&& move) {
    if (this != &move) {
      destroy();
      _data = move._data;
      _invoker = move._invoker;
      _destructor = move._destructor;
      move._invoker = nullptr;
      move._destructor = nullptr;
    }

    return *this;
  }

  template <typename FnT>
  UFunction(FnT&& fn) {
    using ValueFnT = std::remove_reference_t<FnT>;
    UFunction::assign<ValueFnT>(std::forward<FnT>(fn), 0);
  }

  template <typename FnT>
  UFunction& operator=(FnT&& fn) {
    using ValueFnT = std::remove_reference_t<FnT>;
    destroy();
    UFunction::assign<ValueFnT>(std::forward<FnT>(fn), 0);
    return *this;
  }

  bool operator==(std::nullptr_t) const { return _invoker == nullptr; }
  bool operator!=(std::nullptr_t) const { return _invoker != nullptr; }
  operator bool() const { return _invoker != nullptr; }
  RetT operator()(ArgTs... args) const {
    assert(_invoker != nullptr);
    return _invoker(_data, std::forward<ArgTs>(args)...);
  }

 private:
  template <typename FnT>
  static RetT invoke_erased_fptr(const Data& data, ArgTs... args) {
    return reinterpret_cast<FnT*>(data.erased_fptr)(std::forward<ArgTs>(args)...);
  }

  template <typename FnT>
  static RetT invoke_erased_heap_fobj(const Data& data, ArgTs... args) {
    return (*reinterpret_cast<const FnT*>(data.erased_heap_fobj))(std::forward<ArgTs>(args)...);
  }

  template <typename FnT>
  static void destroy_erased_heap_fobj(Data& data) {
    reinterpret_cast<FnT*>(data.erased_heap_fobj)->~FnT();
    sge::free(data.erased_heap_fobj);
  }

  template <typename FnT>
  static RetT invoke_erased_local_fobj(const Data& data, ArgTs... args) {
    return (*reinterpret_cast<const FnT*>(data.erased_local_fobj))(std::forward<ArgTs>(args)...);
  }

  void destroy() {
    if (_destructor) {
      _destructor(_data);
    }
  }

  template <typename ValueFnT, typename FnObjectT>
  auto assign(FnObjectT&& fn, int) -> std::enable_if_t<
      sizeof(ValueFnT) <= LOCAL_BUFFER_SIZE && alignof(ValueFnT) <= alignof(Data) &&
      std::is_trivially_destructible<ValueFnT>::value> {
    new (_data.erased_local_fobj) ValueFnT(std::forward<FnObjectT>(fn));
    _invoker = &invoke_erased_local_fobj<ValueFnT>;
    _destructor = nullptr;
  }

  template <typename ValueFnT, typename FnObjectT>
  void assign(FnObjectT&& fn, char) {
    _data.erased_heap_fobj = sge::malloc(sizeof(ValueFnT));
    new (_data.erased_heap_fobj) ValueFnT(std::forward<FnObjectT>(fn));
    _invoker = &invoke_erased_heap_fobj<ValueFnT>;
    _destructor = &destroy_erased_heap_fobj<ValueFnT>;
  }

  template <typename RetT2, typename... ArgT2s>
  void assign(RetT2 (&fn_ref)(ArgT2s...)) {
    _data.erased_fptr = reinterpret_cast<void(*)>(&fn_ref);
    _invoker = &invoke_erased_fptr<RetT2(ArgT2s...)>;
    _destructor = nullptr;
  }

  template <typename RetT2, typename... ArgT2s>
  void assign(RetT2 (*fn_ptr)(ArgT2s...)) {
    _data.erased_fptr = reinterpret_cast<void(*)>(fn_ptr);
    _invoker = &invoke_erased_fptr<RetT2(ArgT2s...)>;
    _destructor = nullptr;
  }

 private:
  Data _data;
  InvokerFn* _invoker;
  DestructorFn* _destructor;
};
}  // namespace sge

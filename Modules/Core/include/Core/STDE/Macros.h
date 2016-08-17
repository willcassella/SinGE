// Macros.h
#pragma once

#include <type_traits>

/* Macro which resolves to the current type of 'this', assuming it's used in the context of a member function. */
#define SELF_TYPE std::remove_pointer_t<decltype(this)>

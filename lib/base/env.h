/** All macros for determining compiler and build environment go here */
#pragma once

#include <stddef.h>

/** Detect Unix (OSX/Linux) */
#if defined(unix) || defined(__unix__) || defined(__unix)
#define SGE_OS_UNIX
#endif

/** Detect Linux. */
#if defined(__linux__)
#define SGE_OS_LINUX
#endif

/** Detect OSX. */
#if defined(__APPLE__)
#define SGE_OS_OSX
#endif

/** Detect Windows. */
#if defined(_WIN32)
#define SGE_OS_WINDOWS
#endif

/** Determine linkage */
#if defined SGE_LINK_STATIC
/** Modules are being linked statically (as .lib or .ar files) */
#define SGE_IMPORT
#define SGE_EXPORT
#else
/** Modules are being linked dynamically (as .dll or .so files) */
#if defined _MSC_VER
/** The engine is being compiled on MSVC. */
#define SGE_IMPORT __declspec(dllimport)
#define SGE_EXPORT __declspec(dllexport)
#elif defined __clang__ || __GNUC__
/** The engine is being compiled on OSX or Linux. */
#define SGE_IMPORT __attribute__((visibility("default")))
#define SGE_EXPORT __attribute__((visibility("default")))
#else
/** Unknown compiler, cannot proceed. */
#error Non-static linking used on unknown compiler
#endif
#endif

/** Inline macro */
#if defined _MSC_VER
/** We're on MSVC, so use the __forceinline specifier */
#define SGE_FORCEINLINE __forceinline
#elif defined __clang__ || defined __GNUC__
/** We're on clang or gcc, so use the __attribute__((always_inline)) attribute */
#define SGE_FORCEINLINE inline __attribute__((always_inline))
#else
/** We're on some unknown compiler, so just use normal inline */
#define SGE_FORCEINLINE inline
#endif

/** Enables optimizations where pointer-aliasing does not occur. */
#define SGE_RESTRICT __restrict

/** Macro useful for macros that expect to be given an API specification, but can't be given one. */
#define SGE_NO_API

/*
 * Macro useful for maintaining alignment with buffer headers.
 * NOTE: I use 'alignas(alignof(...))' instead of the 'alignas(type-id)' expression, because that doesn't
 * appear to work on MSVC (as usual).
 */
#define SGE_ALIGNED_BUFFER_HEADER alignas(alignof(max_align_t))

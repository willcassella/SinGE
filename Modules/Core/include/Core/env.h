// env.h - Copyright 2013-2016 Will Cassella, All Rights Reserved
/** All macros for determining compiler and build environment go here */
#pragma once

#include <cstddef>
#include <cstdint>

////////////////////////////
///   Operating System   ///

/** Detect Unix (OSX/Linux) */
#if defined(unix) || defined(__unix__) || defined(__unix)
#	define SGE_OS_UNIX
#endif

/** Detect Linux. */
#if defined (__linux__)
#	define SGE_OS_LINUX
#endif

/** Detect OSX. */
#if defined (__APPLE__)
#	define SGE_OS_OSX
#endif

/** Detect Windows. */
#if defined (_WIN32)
#	define SGE_OS_WINDOWS
#endif

/////////////////////////////
///   Build Information   ///

/** Determine linkage */
#if defined SGE_LINK_STATIC
/** Modules are being linked statically (as .lib or .ar files) */
#	define SGE_IMPORT
#	define SGE_EXPORT
#else
/** Modules are being linked dynamically (as .dll or .so files) */
#	if defined _MSC_VER
/** The engine is being compiled on MSVC. */
#		define SGE_IMPORT __declspec(dllimport)
#		define SGE_EXPORT __declspec(dllexport)
#	elif defined __clang__ || __GNUC__
/** The engine is being compiled on OSX or Linux. */
#		define SGE_IMPORT
#		define SGE_EXPORT __attribute__((visibility("default")))
#	else
/** Unknown compiler, cannot proceed. */
#		error Non-static linking used on unknown compiler
#	endif
#endif

//////////////////////
///   Primitives   ///

namespace sge
{
	// bool is still bool
	// char is still char (ONLY used for characters, prefer 'Char')
	using int8 = std::int8_t;
	using uint8 = std::uint8_t;
	using int16 = std::int16_t;
	using uint16 = std::uint16_t;
	using int32 = std::int32_t;
	using uint32 = std::uint32_t;
	using int64 = std::int64_t;
	using uint64 = std::uint64_t;
	using byte = uint8;
	// float is still float (prefer 'Scalar')
	// double is still double (prefer 'Scalar')

	/** The default type used for scalar values. */
	using Scalar = float;
}

//////////////////
///   Macros   ///

/** Inline macro */
#if defined _MSC_VER
/** We're on MSVC, so use the __forceinline specifier */
#	define SGE_FORCEINLINE __forceinline
#elif defined __clang__ || defined __GNUC__
/** We're on clang or gcc, so use the __attribute__((always_inline)) attribute */
#	define SGE_FORCEINLINE inline __attribute__((always_inline))
#else
/** We're on some unknown compiler, so just use normal inline */
#	define SGE_FORCEINLINE inline
#endif

/** Macro useful for macros that expect to be given an API specification, but can't be given one. */
#define SGE_NO_API

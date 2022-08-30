#pragma once

// msvc
#ifdef _MSC_VER
#define FORCEINLINE __forceinline

// clang
#elif defined(__clang__)
#define FORCEINLINE inline

// g++
#elif defined(__GNUG__)
#define FORCEINLINE __attribute__((always_inline))

// everything else
#else
#error "Unsupported platform!"
#endif
#pragma once

// msvc
#ifdef _MSC_VER
#define G_FORCEINLINE __forceinline

// clang
#elif defined(__clang__)
#define G_FORCEINLINE inline

// g++
#elif defined(__GNUG__)
#define G_FORCEINLINE __attribute__((always_inline))

// everything else
#else
#error "Unsupported platform!"
#endif

#if __has_include("immintrin.h")
#include <immintrin.h>
#define G_PAUSE() _mm_pause()
#else
#define G_PAUSE()
#endif
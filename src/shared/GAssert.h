#pragma once
//#include <source_location>
#include <cassert>

#ifndef NDEBUG
#define G_ASSERT(x) assert(x)
#else
#define G_ASSERT(x) (void)(x)
#endif

//namespace g
//{
//  void Assert(bool condition, std::source_location = std::source_location::current());
//}
#pragma once
//#include <source_location>
#include <cassert>

#ifndef NDEBUG
#define G_ASSERT(x) assert(x)
#define G_ASSERT_MSG(x, msg) assert(x)
#else
#define G_ASSERT(x)
#define G_ASSERT_MSG(x, msg)
#endif

//namespace g
//{
//  void Assert(bool condition, std::source_location = std::source_location::current());
//}
#include "Platform.h"
#include <numeric>

// required for serialization
static_assert(std::numeric_limits<float>::is_iec559 && "Platform must support IEEE 754 floats");
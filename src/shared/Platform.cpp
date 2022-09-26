#include "Platform.h"
#include <climits>
#include <limits>

// required for serialization
static_assert(std::numeric_limits<float>::is_iec559 && 
              sizeof(float) * CHAR_BIT == 32, 
              "Platform must support IEEE 754 floats");
#pragma once

// Single source of truth: do NOT override this macro in sketches.
// Change it here if you want an optimized build.
#define DHT_OPTIMIZED_BUILD 0 // 0 = normal, 1 = optimized

namespace mydht
{
    constexpr bool optimizedBuild()
    {
        return DHT_OPTIMIZED_BUILD == 1;
    }
}
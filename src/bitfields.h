#pragma once

#define BIT(x) (1u << x)
#define BITS(value, x, w) ((value & ((1 << w) - 1)) << x)
#define EXTRACT_BITS(value, x, w) ((value >> x) & ((1 << w) - 1))

#pragma once

#include "default_logger.h"

#define ASSERT(x, msg) if(!(x))getDefaultLogger()->error("ASSERTION FAILED: {}", msg);

#include <pch.h>
#include "default_logger.h"

namespace x86a{
    static Logger s_DefaultLogger;
    Logger* getDefaultLogger(){
        return &s_DefaultLogger;
    }
}


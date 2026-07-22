#pragma once

#include "section_container.h"

namespace x86a{
    class CodeGenerator{
    public:
        std::optional<std::string> generate(const SectionContainer& section_container, std::filesystem::path output);
    };
}

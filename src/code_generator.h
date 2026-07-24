#pragma once

#include "instruction_set.h"
#include "section_container.h"

namespace x86a{
    class CodeGenerator{
    public:
        CodeGenerator(const InstructionSet& instruction_set);

        std::optional<std::string> generate(const SectionContainer& sectionsContainer, std::filesystem::path output);
    private:
        const InstructionSet* m_InstructionSet;
    };
}

#include "pch.h"
#include "section_generator.h"
#include "default_logger.h"

namespace x86a{
    SectionGenerator::SectionGenerator(const InstructionSet& instruction_set): m_InstructionSet(&instruction_set){
        getDefaultLogger()->debug("Symbol generator using instruction set {}", instruction_set.getName());
    }
    std::optional<SectionGenerator::Error> SectionGenerator::generate(const Tokenizer& tokenizer){
    
        return std::nullopt;
    }
}

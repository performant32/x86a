#pragma once

#include "tokenizer.h"
#include "instruction_set.h"

namespace x86a{
    // validates instruction set along with section/symbol generation
    class SectionGenerator{
    public:
        enum class ErrorType{
            None=0,
            BadInstruction,
        };
        struct Error{
            const Token& token;
            std::string message;
        };
    public:
        SectionGenerator(const InstructionSet& instruction_set);
        std::optional<Error> generate(const Tokenizer& tokenizer);
    private:
        const InstructionSet* m_InstructionSet=nullptr;
        const Tokenizer* m_Tokenizer=nullptr;
        std::unordered_map<std::string_view, uint64_t> m_Sections;
    };
}

#pragma once

#include "tokenizer.h"
#include "instruction_set.h"
#include "section_container.h"

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
        /// @brief returns true if success and false if failed. Output contains sections with their respective data
        bool generate(const Tokenizer& tokenizer, SectionContainer& output);
    private:
        std::optional<OperandValue> parseOperand();

        size_t m_At=0;

        const InstructionSet* m_InstructionSet=nullptr;
        const Tokenizer* m_Tokenizer=nullptr;
        std::unordered_map<std::string_view, std::vector<Section>> m_Sections;
    };
}

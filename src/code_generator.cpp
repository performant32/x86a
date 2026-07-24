#include "default_logger.h"
#include "pch.h"
#include "code_generator.h"

namespace x86a{
    CodeGenerator::CodeGenerator(const InstructionSet& instruction_set):
        m_InstructionSet(&instruction_set){
    }
    std::optional<std::string> CodeGenerator::generate(const SectionContainer& sectionsContainer, std::filesystem::path output){
        const auto& path = sectionsContainer.getFile()->getPath();
        std::ofstream file(output, std::ios::out | std::ios::binary);
        if(!file){
            return std::format("Failed to open file ", output.string());
        }

        for(const auto& [name, section] : sectionsContainer.getSections()){
            for(const auto& instruction : section.getInstructions()){
                // 15 bytes is the max for an instruction
                uint8_t bytes[15];

                int bytesWritten =0;
                if(auto error = m_InstructionSet->writeInstructionBytes(bytes, &bytesWritten, instruction.getInstruction(), instruction.getOperandValues())){
                    return std::format("Failed to generate code for {}", path.string());
                }
                getDefaultLogger()->log("Instruction {} has {} arguments, wrote {} bytes", instruction.getMnemonic(), instruction.getOperandValues().size(), bytesWritten);
                std::string values;
                for(size_t i = 0; i < bytesWritten; i++){
                    values += std::format("{:>5}: {:X}\n", i, bytes[i]);
                }
                getDefaultLogger()->log("Instruction bytes\n{}", values);
            }
        }
        return std::nullopt;
    }
}

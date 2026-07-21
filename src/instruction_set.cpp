#include "default_logger.h"
#include "pch.h"
#include <vector>
#include "instruction_set.h"


namespace x86a{
    Instruction::Instruction(uint8_t opcode, std::string_view mnemonic, std::vector<Operand>&& operands):
        m_Opcode(opcode), m_Mnemonic(mnemonic), m_Operands(operands){
    }
    I8086::I8086(){
        getDefaultLogger()->debug("Initializing I8086 instruction set");

        Instruction mov_r8_r8{0x88, "mov", 
            std::vector{
                Operand{false, 8},
                Operand{false, 8}
            }
        };
        m_Instructions.insert(std::make_pair("mov", mov_r8_r8));

    }
    std::vector<Instruction> InstructionSet::getInstructionsFromMnemonic(std::string_view mnemonic){
        return {};
    }
}

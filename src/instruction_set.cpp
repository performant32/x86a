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

        m_Registers.insert(std::make_pair("di", (Register){Register::Type::GeneralPurpose, 16}));
        m_Registers.insert(std::make_pair("ax", (Register){Register::Type::GeneralPurpose, 16}));
        Instruction mov_r8_r8{0x88, "mov", 
            std::vector{
                Operand{AddressingMode::Register, 16},
                Operand{AddressingMode::Immediate, 8}
            }
        };
        m_Instructions.insert(std::make_pair("mov", mov_r8_r8));

    }
    InstructionSet::InstructionIterator InstructionSet::getInstructionsFromMnemonic(std::string_view mnemonic)const{
        auto it = m_Instructions.find(mnemonic);
        if(it == m_Instructions.end())return {};
        return it;
    }
    const Register* InstructionSet::getRegister(std::string_view name)const noexcept{
        auto it = m_Registers.find(name);
        if(it == m_Registers.cend())return nullptr;
        return &it->second;
    }
}

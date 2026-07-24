#include "default_logger.h"
#include "pch.h"
#include <optional>
#include <vector>
#include "instruction_set.h"


namespace x86a{
    Instruction::Instruction(InstructionPrefix prefix, uint32_t opcode, std::string_view mnemonic, Encoding encoding, std::vector<Operand>&& operands):
        m_InstructionPrefix(prefix), m_Opcode(opcode), m_Encoding(encoding), m_Mnemonic(mnemonic), m_Operands(operands){
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
    std::optional<uint8_t> InstructionSet::getRegisterId(std::string_view mnemonic)const{
        for(const auto& [name, value] : m_Registers){
            if(mnemonic == name){
                return value.register_id;
            }
        }
        return std::nullopt;
    }
}

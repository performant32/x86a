#include "default_logger.h"
#include "pch.h"
#include <optional>
#include <type_traits>
#include <vector>
#include "instruction_set.h"


namespace x86a{
    Instruction::Instruction(InstructionPrefix prefix, uint32_t opcode, std::string_view mnemonic, Encoding encoding, std::vector<Operand>&& operands):
        m_InstructionPrefix(prefix), m_Opcode(opcode), m_Encoding((uint32_t)encoding), m_Mnemonic(mnemonic), m_Operands(operands){
    }
    Instruction::Instruction(InstructionPrefix prefix, uint32_t opcode, std::string_view mnemonic, int32_t encoding, std::vector<Operand>&& operands):
        m_InstructionPrefix(prefix), m_Opcode(opcode), m_Encoding((uint32_t)encoding), m_Mnemonic(mnemonic), m_Operands(operands){
    }
    InstructionSet::InstructionIterator InstructionSet::getInstructionsFromMnemonic(std::string_view mnemonic)const{
        auto it = m_Instructions.find(mnemonic);
        if(it == m_Instructions.end())return {};
        return it;
    }
    std::string Instruction::getEncodingName(int encoding){
        std::string output;
        int temp = encoding;
        for(size_t i = 0; i < sizeof(encoding) * 8; i++){
            int en = (encoding & 1);
            if(en)output+=std::string(getEncodingName(encoding & (1 << i))) + ",";
            temp>>=1;
        }
        if(output == "")output = "None";
        return output;
    }
    const char* Instruction::getEncodingName(Encoding encoding){
        switch(encoding){
        case Encoding::None:return "None";
        case Encoding::Immediate:return "Immediate";
        case Encoding::ModMI:return "ModMI";
        case Encoding::ModMR:return "ModMR";
        case Encoding::ModRM:return "ModRM";
        case Encoding::ModRMPlusSIB:return "ModRMPlusSIB";
        default:return "Invalid";
        }
    }
    std::string_view InstructionSet::getAddressingModeName(AddressingMode addressingMode)noexcept{
        switch(addressingMode){
        case AddressingMode::Immediate:return "Immediate";
        case AddressingMode::Register:return "Register";
        case AddressingMode::DirectMemory:return "DirectMemory";
        case AddressingMode::IndirectMemory:return "IndirectMemory";
        case AddressingMode::RM:return "RM";
        case AddressingMode::SIB:return "SIB";
        default:
            return "Invalid Addressing Mode";
        }
    }
    const Register* InstructionSet::getRegister(std::string_view name)const noexcept{
        auto it = m_Registers.find(name);
        if(it == m_Registers.cend())return nullptr;
        return &it->second;
    }
    std::optional<uint8_t> InstructionSet::getRegisterId(std::string_view mnemonic)const{
        const auto& it = m_Registers.find(mnemonic);
        if(it == m_Registers.end())return std::nullopt;
        return it->second.register_id;
    }
}

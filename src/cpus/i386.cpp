#include "i386.h"
#include "default_logger.h"
#include "instruction_set.h"
#include "bitfields.h"
#include <cstdlib>
#include <ratio>
#include <span>

namespace x86a {
    std::span<const char* const, std::dynamic_extent> I386::getKeywords()const noexcept{
        auto keywords = (const char* const[]){
            "section"
        };
        return std::span<const char*const>{keywords,sizeof(keywords)};
    }
    I386::I386(){
        getDefaultLogger()->debug("Initializing I386 instruction set");

        m_Registers.insert(std::make_pair("di", (Register){0b111, Register::Type::GeneralPurpose, 16}));
        m_Registers.insert(std::make_pair("bh", (Register){0b111, Register::Type::GeneralPurpose, 8}));

        m_Registers.insert(std::make_pair("ax",  (Register){0b000, Register::Type::GeneralPurpose, 16}));
        m_Registers.insert(std::make_pair("eax", (Register){0b000, Register::Type::GeneralPurpose, 32}));

        m_Registers.insert(std::make_pair("bx",  (Register){0b011, Register::Type::GeneralPurpose, 16}));
        m_Registers.insert(std::make_pair("ebx", (Register){0b011, Register::Type::GeneralPurpose, 32}));

        m_Registers.insert(std::make_pair("di",  (Register){0b111, Register::Type::GeneralPurpose, 16}));
        m_Registers.insert(std::make_pair("edi", (Register){0b111, Register::Type::GeneralPurpose, 32}));

#define CREATE_INSTRUCTION(name, ...)m_Instructions.insert(std::make_pair(name, Instruction(__VA_ARGS__)))

        CREATE_INSTRUCTION("mov",
            InstructionPrefix::None, 0x8B, "mov", Instruction::Encoding::ModRM,
                std::vector{
                    Operand{AddressingMode::Register, 32},
                    Operand{AddressingMode::Register, 32}
                }
        );

        CREATE_INSTRUCTION("mov",
            InstructionPrefix::None, 0x8B, "mov", Instruction::Encoding::ModRM,
                std::vector{
                    Operand{AddressingMode::Register, 32},
                    Operand{AddressingMode::DirectMemory, 32}
                }
        );

        CREATE_INSTRUCTION("mov",
            InstructionPrefix::None, 0xC7, "mov", Instruction::createEncoding(Instruction::Encoding::Immediate, Instruction::Encoding::ModMI),
                std::vector{
                    Operand{AddressingMode::RM,   32},
                    Operand{AddressingMode::Immediate,      32}
                }
        );

        CREATE_INSTRUCTION("int",
            InstructionPrefix::None, 0xcd, "int", Instruction::Encoding::Immediate,
                std::vector{
                    Operand{AddressingMode::Immediate, 8},
                }
        );

#undef CREATE_INSTRUCTION
    }
    uint8_t I386::createModRMByte(EffectiveAddress address, uint8_t register_opcode)const noexcept{
        return BITS(register_opcode, 3, 5) | BITS((int)address, 0, 3);
    }

    std::optional<std::string> I386::writeInstructionBytes(uint8_t* output, int* bytesWritten, const Instruction& instruction, const std::vector<OperandValue>& arguments)const{
        const auto& operands = instruction.getOperands();
        const auto& mnemonic = instruction.getMnemonic();

        /// TODO: support multi byte opcode
        const auto& opcode = instruction.getOpcode();
        size_t at = 0;

        auto write = [output, &at](uint8_t data){
            output[at++] = data;
        };
        auto writeWord = [output, &at](uint32_t data){
            output[at++] = data & 0xFF;
            output[at++] = (data >> 8) & 0xFF;
        };
        auto writeDWord = [output, &at](uint32_t data){
            output[at++] = data & 0xFF;
            output[at++] = (data >> 8)  & 0xFF;
            output[at++] = (data >> 16) & 0xFF;
            output[at++] = (data >> 24) & 0xFF;
        };

        getDefaultLogger()->log("Count {} {}", operands.size(), arguments.size());
        InstructionPrefix prefix = instruction.getInstructionPrefix();
        if(prefix != InstructionPrefix::None){
            write((uint8_t)prefix);
        }
        write(opcode);
        auto encoding = instruction.getEncoding();
        if(encoding & (int)Instruction::Encoding::Immediate){
            for(size_t i = 0; i < arguments.size(); i++){
                const Operand& operand = operands[i];
                if(operand.mode != AddressingMode::Immediate)continue;
                const OperandValue& value = arguments[i];
                switch(operand.width){
                    case 8:{
                        write(value.u8);
                    }break;
                    case 16:{
                        writeWord(value.u16);
                    }break;
                    case 32:{
                        writeDWord(value.u32);
                    }break;
                    default:{
                        getDefaultLogger()->error("");
                    }break;
                };
            }
        }
        if(encoding & (int)Instruction::Encoding::ModMI){
            uint8_t register_id = arguments[1].u8;
            AddressingMode mode = operands[0].mode;
            switch(mode){
            case AddressingMode::Immediate:{
                write(createModRMByte(EffectiveAddress::Disp32, register_id));
                uint32_t value = arguments[0].u32;
                writeDWord(value);
                break;
            }
            default:
                getDefaultLogger()->error("Unsupported addressing mode {} for instruction + encoding! {} {}", (int)mode, instruction.getMnemonic(), (int)instruction.getEncoding());
                std::abort();

            }
        }
        if(encoding & (int)Instruction::Encoding::ModRM){
            uint8_t register_id = arguments[0].u8;
            AddressingMode mode = operands[1].mode;
            switch(mode){
            case AddressingMode::Register:{
                write(createModRMByte((EffectiveAddress)(BITS(11, 3, 2) | register_id), arguments[1].u8));
                break;
            }
            default:
                getDefaultLogger()->error("Unsupported addressing mode {} for instruction + encoding! {} {}", (int)mode, instruction.getMnemonic(), (int)instruction.getEncoding());
                std::abort();

            }
        }
        *bytesWritten = at;
        return std::nullopt;
    }
}

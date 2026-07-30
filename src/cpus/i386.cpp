#include "i386.h"
#include "default_logger.h"
#include "instruction_set.h"
#include "bitfields.h"
#include "macros.h"
#include <complex>

namespace x86a {
    std::span<const char* const, std::dynamic_extent> I386::getKeywords()const noexcept{
        auto keywords = (const char* const[]){
            "section"
        };
        return std::span<const char*const>{keywords,sizeof(keywords)};
    }
    I386::I386(){
        getDefaultLogger()->debug("Initializing I386 instruction set");


        m_Registers.insert(std::make_pair("ax",  (Register){0b000, Register::Type::GeneralPurpose, 16}));
        m_Registers.insert(std::make_pair("eax", (Register){0b000, Register::Type::GeneralPurpose, 32}));

        m_Registers.insert(std::make_pair("bx",  (Register){0b011, Register::Type::GeneralPurpose, 16}));
        m_Registers.insert(std::make_pair("ebx", (Register){0b011, Register::Type::GeneralPurpose, 32}));

        m_Registers.insert(std::make_pair("di",  (Register){0b111, Register::Type::GeneralPurpose, 16}));
        m_Registers.insert(std::make_pair("edi", (Register){0b111, Register::Type::GeneralPurpose, 32}));
        m_Registers.insert(std::make_pair("bh",  (Register){0b111, Register::Type::GeneralPurpose, 8}));

        m_Registers.insert(std::make_pair("cx",  (Register){0b1, Register::Type::GeneralPurpose, 16}));
        m_Registers.insert(std::make_pair("ecx", (Register){0b1, Register::Type::GeneralPurpose, 32}));

#define CREATE_INSTRUCTION(name, ...)m_Instructions.insert(std::make_pair(name, Instruction(__VA_ARGS__)))

        CREATE_INSTRUCTION("mov",
            InstructionPrefix::None, 0x8B, "mov", Instruction::createEncoding(Instruction::Encoding::Register, Instruction::Encoding::ModRM),
                std::vector{
                    Operand{AddressingMode::Register, 32},
                    Operand{AddressingMode::RM, 32}
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
    std::string_view I386::getEffectiveAddressName(EffectiveAddress address)const noexcept{
        switch(address){
            case EffectiveAddress::EAXPtr:return "EAXPtr";
            case EffectiveAddress::ECXPtr:return "ECXPtr";
            case EffectiveAddress::EDXPtr:return "EDXPtr";
            case EffectiveAddress::EBXPtr:return "EBXPtr";
            case EffectiveAddress::ESIPtr:return "ESIPtr";
            case EffectiveAddress::EDIPtr:return "EDIPtr";
            case EffectiveAddress::EAX:return "EAX";
            case EffectiveAddress::EBX:return "EBX";       
            case EffectiveAddress::ECX:return "ECX";

            case EffectiveAddress::SIB:return "SIB";
            case EffectiveAddress::Disp32:return "Disp32";
            default:
                return "Invalid Effective Address";
        }
    }
    uint8_t I386::createModRMByte(EffectiveAddress address, uint8_t register_opcode)const noexcept{
        return BITS(EXTRACT_BITS((int)address, 3, 2), 6, 2) | BITS(register_opcode, 3, 3) | BITS(EXTRACT_BITS((int)address, 0, 3), 0, 3);

        //return BITS((int)address, 3, 5) | BITS(register_opcode, 0, 3);
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
        int opcode_at = at;
        write(opcode);

        auto encoding = instruction.getEncoding();
        /// e.g. mov r/32 immediate
        bool write_register_opcode = true;
        if(encoding & (int)Instruction::Encoding::Register){
            // uint8_t output_register = 0;
            // for(const auto& argument : arguments){
            //     if(argument.mode != AddressingMode::Register)continue;
            //     output_register = argument.u8;
            // }
            // write_register_opcode = false;
            //output[opcode_at] |= output_register;
        }
        if(encoding & (int)Instruction::Encoding::ModMI){
            const OperandValue& immediate = arguments[1];
            const OperandValue& rm = arguments[0];

            uint8_t rm_byte = 0;
            switch(rm.mode){
                case AddressingMode::Register:{
                    //rm_byte = BITS(11, 3, 2) | BITS(0, 3, 3) | BITS(rm.u8, 3, 0);
                    rm_byte = createModRMByte((EffectiveAddress)(BITS(0b11, 3, 2) | BITS(rm.u8, 0, 3)), 0);
                }break;
                default:
                    getDefaultLogger()->error("Unsupported RM operand for instruction {}", instruction.getMnemonic());
                    std::abort();
            }

            write(rm_byte);
        }
        if(encoding & (int)Instruction::Encoding::ModRM){
            uint8_t register_id = 0;
            if(write_register_opcode){
                const OperandValue& r = arguments[0];
                if(r.mode != AddressingMode::Register){
                    getDefaultLogger()->error("Expected operand 2 to be of type Register, instead got {}", getAddressingModeName(r.mode));
                    return std::nullopt;
                }
                register_id = r.u8;
                output[opcode_at] += BITS(register_id, 0, 3);
            }

            const OperandValue& rm = arguments[1];
            switch(rm.mode){
            //case AddressingMode::RM:{
                // some sort of [eax]
            //}break;
            case x86a::AddressingMode::Register:{
                write(createModRMByte((EffectiveAddress)(BITS(0b11, 3, 2) | BITS(rm.u8, 0, 3)), 0));
            }break;
            default:{
                getDefaultLogger()->error("Unsupported addressing mode for instruction {1}, mode {0}", getAddressingModeName(rm.mode), instruction.getMnemonic());
            }break;
            }
        }
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
        *bytesWritten = at;
        return std::nullopt;
    }
}

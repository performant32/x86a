#include "i8086.h"
#include "default_logger.h"
#include "instruction_set.h"
#include "bitfields.h"
#include <ratio>

namespace x86a {
    I8086::I8086(){
        getDefaultLogger()->debug("Initializing I8086 instruction set");

        m_Registers.insert(std::make_pair("di", (Register){0b111, Register::Type::GeneralPurpose, 16}));
        m_Registers.insert(std::make_pair("bh", (Register){0b111, Register::Type::GeneralPurpose, 8}));

        m_Registers.insert(std::make_pair("ax",  (Register){0b000, Register::Type::GeneralPurpose, 16}));
        m_Registers.insert(std::make_pair("eax", (Register){0b000, Register::Type::GeneralPurpose, 32}));

        m_Registers.insert(std::make_pair("bx",  (Register){0b011, Register::Type::GeneralPurpose, 16}));
        m_Registers.insert(std::make_pair("ebx", (Register){0b011, Register::Type::GeneralPurpose, 32}));

        m_Registers.insert(std::make_pair("di",  (Register){0b111, Register::Type::GeneralPurpose, 16}));
        m_Registers.insert(std::make_pair("edi", (Register){0b111, Register::Type::GeneralPurpose, 32}));

        m_Instructions.insert(std::make_pair("mov", Instruction(
            InstructionPrefix::None, 0x89, "mov", Instruction::Encoding::ModRM,
                std::vector{
                    Operand{AddressingMode::Register, 32},
                    Operand{AddressingMode::Register, 32}
                }
            )
        ));
    }
    uint8_t I8086::createModRMByte(uint8_t mod, uint8_t rm, uint8_t reg_or_opcode)const noexcept{
        return BITS(mod, 6, 2) | BITS(reg_or_opcode, 3, 3) | BITS(rm, 0, 3);
    }
    std::optional<std::string> I8086::writeInstructionBytes(uint8_t* output, int* bytesWritten, const Instruction& instruction, const std::vector<OperandValue>& arguments)const{
        const auto& operands = instruction.getOperands();
        const auto& mnemonic = instruction.getMnemonic();

        /// TODO: support multi byte opcode
        const auto& opcode = instruction.getOpcode();
        size_t at = 0;

        auto write = [output, &at](uint8_t data){
            getDefaultLogger()->log("At {}", at);
            output[at++] = data;
        };

        getDefaultLogger()->log("Count {} {}", operands.size(), arguments.size());
        switch (opcode) {
        case 0x89:{
            getDefaultLogger()->log("Writing mov");
            write(0x89);
            getDefaultLogger()->log("done Writing mov");

            switch(operands[1].mode){
                case AddressingMode::Register:{
                    uint8_t destination_operand = arguments[0].register_id;
                    uint8_t source_operand = arguments[1].register_id;
                    write(createModRMByte(11, source_operand, destination_operand));
                    getDefaultLogger()->log("Wrote mov instruction args {}, {}", destination_operand, source_operand);
                }break;
                case x86a::AddressingMode::Immediate:{
                };
            }
        }break;
        }
        *bytesWritten = at;
        return std::nullopt;
    }
}

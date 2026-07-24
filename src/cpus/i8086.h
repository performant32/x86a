#pragma once

#include "instruction_set.h"

namespace x86a {
    class I8086: public InstructionSet{
    public:
        I8086();

        uint8_t createModRMByte(uint8_t mod, uint8_t rm, uint8_t reg_or_opcode=0)const noexcept;
        std::optional<std::string> writeInstructionBytes(uint8_t* output, int* bytesWritten, const Instruction& instruction, const std::vector<OperandValue>& arguments)const override;
        const char* getName() const noexcept override {return "I8086";}
    };
}

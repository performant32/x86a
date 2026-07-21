#pragma once

#include "pch.h"

namespace x86a{
    enum class AddressingMode{
        Implied,
    };
    struct Operand{
        // Either memory or register
        bool is_memory;
        uint8_t width;
    };

    class Instruction{
    public:
        Instruction(uint8_t opcode, std::string_view mnemonic, std::vector<Operand>&& operands);

        uint8_t getOpcode() const noexcept{return m_Opcode;}
        std::string_view getMnemonic() const noexcept{return m_Mnemonic;}
        const std::vector<Operand>& getOperands() const noexcept{return m_Operands;}
    private:
        uint8_t m_Opcode;
        std::string_view m_Mnemonic;
        std::vector<Operand> m_Operands;
    };
    class InstructionSet{
    public:
        std::vector<Instruction> getInstructionsFromMnemonic(std::string_view mnemonic);
        virtual const char* getName() const noexcept = 0;
    protected:
        std::unordered_multimap<std::string_view, Instruction> m_Instructions;
    };
    class I8086: public InstructionSet{
    public:
        I8086();
        const char* getName() const noexcept override {return "I8086";}
    };
}

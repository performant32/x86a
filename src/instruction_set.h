#pragma once

#include "pch.h"
#include <unordered_map>

namespace x86a{
    // Addressing mode per operand
    // http://www.csc.villanova.edu/~mdamian/Past/csc2400fa16/notes/AssemblyAddressing.pdf
    enum class AddressingMode{
        Immediate,
        Register,
        DirectMemory,
        IndirectMemory,
    };

    struct Operand{
        AddressingMode mode;
        uint8_t width;
    };

    struct Register{
        enum Type{
            GeneralPurpose,
            Segment,
            IndexOrPointer,
            Indicator,
        };
        Type type;
        int width;
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
        using InstructionMap = std::unordered_multimap<std::string_view, Instruction>;
        using InstructionIterator = InstructionMap::const_iterator;

        const Register* getRegister(std::string_view name)const noexcept;
        InstructionIterator getInstructionsFromMnemonic(std::string_view mnemonic) const;
        inline InstructionIterator getInstructionsEndIterator()const noexcept{return m_Instructions.cend();}
        virtual const char* getName() const noexcept = 0;
    protected:
        std::unordered_map<std::string_view, Register> m_Registers;
        InstructionMap m_Instructions;
    };
    class I8086: public InstructionSet{
    public:
        I8086();
        const char* getName() const noexcept override {return "I8086";}
    };
}

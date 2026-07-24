#pragma once

#include "pch.h"
#include <string_view>
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

    enum class InstructionPrefix{
        None=0
    };

    struct Operand{
        AddressingMode mode;
        uint8_t width;
    };

    struct OperandValue{
        union{
            uint8_t register_id;
            uint8_t u8;
            uint16_t u16;
            uint32_t u32;
            uint8_t* mu8;
            uint16_t* mu16;
            uint32_t* mu32;
        };
    };

    struct Register{
        enum Type{
            GeneralPurpose,
            Segment,
            IndexOrPointer,
            Indicator,
        };
        uint8_t register_id;
        Type type;
        int width;
    };

    class Instruction{
    public:
        enum class Encoding{
            RegisterField,
            ModRM,
            ModRMPlusSIB
        };

    public:
        Instruction(InstructionPrefix prefix, uint32_t opcode, std::string_view mnemonic, Encoding encoding, std::vector<Operand>&& operands);

        InstructionPrefix getInstructionPrefix()const noexcept{return m_InstructionPrefix;}

        uint32_t getOpcode() const noexcept{return m_Opcode;}
        Encoding getEncoding()const noexcept{return m_Encoding;}
        std::string_view getMnemonic() const noexcept{return m_Mnemonic;}
        const std::vector<Operand>& getOperands() const noexcept{return m_Operands;}
    private:
        InstructionPrefix m_InstructionPrefix;
        uint32_t m_Opcode;
        Encoding m_Encoding;
        std::string_view m_Mnemonic;
        std::vector<Operand> m_Operands;
    };
    class InstructionSet{
    public:
        using InstructionMap = std::unordered_multimap<std::string_view, Instruction>;
        using InstructionIterator = InstructionMap::const_iterator;

        const Register* getRegister(std::string_view name)const noexcept;
        std::optional<uint8_t> getRegisterId(std::string_view mnemonic)const;

        /// @brief writes the instruction and operands to the output and returns bytes written
        virtual std::optional<std::string> writeInstructionBytes(uint8_t* output, int* bytesWritten, const Instruction& instruction, const std::vector<OperandValue>& arguments)const =0;

        InstructionIterator getInstructionsFromMnemonic(std::string_view mnemonic) const;
        inline InstructionIterator getInstructionsEndIterator()const noexcept{return m_Instructions.cend();}
        virtual const char* getName() const noexcept = 0;
    protected:
        std::unordered_map<std::string_view, Register> m_Registers;
        InstructionMap m_Instructions;
    };
}

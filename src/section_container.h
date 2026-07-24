#pragma once

#include "asm_file.h"
#include "instruction_set.h"

namespace x86a{

    class InstructionInstance : public Instruction{
    public:
        InstructionInstance(const Instruction& instruction, std::vector<OperandValue>&& operandValues);
        const Instruction& getInstruction()const noexcept{return *this;}
        const std::vector<OperandValue>& getOperandValues()const noexcept{return m_OperandValues;}
    private:
        std::vector<OperandValue> m_OperandValues;
    };

    class Section{
    public:
    public:
        Section(std::string_view name);

        void addInstruction(const Instruction& instruction, std::vector<OperandValue>&& values);

        const std::vector<InstructionInstance>& getInstructions()const noexcept{return m_Instructions;}
        const std::string& getName()const noexcept{return m_Name;}
    private:
        std::vector<InstructionInstance> m_Instructions;
        std::string m_Name;
    };
    class SectionContainer{
    public:
        SectionContainer(const ASMFile* file);
        Section& getSection(std::string_view name);

        const ASMFile* getFile() const noexcept{return m_File;}
        const std::unordered_map<std::string_view, Section>& getSections() const noexcept{return m_Sections;}
    private:
        const ASMFile* m_File = nullptr;
        std::unordered_map<std::string_view, Section> m_Sections;
    }; 
}

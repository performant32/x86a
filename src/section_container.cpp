#include "instruction_set.h"
#include "pch.h"
#include "section_container.h"

namespace x86a{
    InstructionInstance::InstructionInstance(const Instruction& instruction, std::vector<OperandValue>&& operandValues):
         m_OperandValues(std::move(operandValues)), Instruction(instruction){
    }
    void Section::addInstruction(const Instruction& instruction, std::vector<OperandValue>&& values){
        m_Instructions.emplace_back(instruction, std::move(values));
    }
    Section::Section(std::string_view name) : m_Name(name){}

    SectionContainer::SectionContainer(const ASMFile* file) : m_File(file){}

    Section& SectionContainer::getSection(std::string_view name){
        const auto& it = m_Sections.find(name);
        if(it != m_Sections.end())return it->second;
        m_Sections.insert(std::make_pair(name, name));
        return m_Sections.find(name)->second;
    }
}

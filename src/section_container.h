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

        void addData(char data)noexcept;

        const std::vector<char>& getData()const noexcept{return m_Data;}
        const std::string& getName()const noexcept{return m_Name;}
    private:
        std::vector<char> m_Data;
        std::string m_Name;
    };
    class SectionContainer{
    public:
        struct StringHash {
            using is_transparent = void;
            [[nodiscard]] size_t operator()(const char *txt) const {
                return std::hash<std::string_view>{}(txt);
            }
            [[nodiscard]] size_t operator()(std::string_view txt) const {
                return std::hash<std::string_view>{}(txt);
            }
            [[nodiscard]] size_t operator()(const std::string &txt) const {
                return std::hash<std::string>{}(txt);
            }
        };
        using SectionMap = std::unordered_map<std::string, Section,StringHash, std::equal_to<>>;
        SectionContainer(const ASMFile* file);
        Section& getSection(std::string_view name);

        const ASMFile* getFile() const noexcept{return m_File;}
        const SectionMap& getSections() const noexcept{return m_Sections;}
    private:
        const ASMFile* m_File = nullptr;
        SectionMap m_Sections;
    }; 
}

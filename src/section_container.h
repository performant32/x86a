#pragma once

#include "asm_file.h"

namespace x86a{
    class Section{
    public:
        Section(std::string name);

        const std::string& getName()const noexcept{return m_Name;}
    private:
        std::string m_Name;
    };
    class SectionContainer{
    public:
        SectionContainer(const ASMFile* file);
        const ASMFile* getFile() const noexcept{return m_File;}
        const std::unordered_map<std::string_view, Section>& getSections() const noexcept{return m_Sections;}
    private:
        const ASMFile* m_File = nullptr;
        std::unordered_map<std::string_view, Section> m_Sections;
    }; 
}

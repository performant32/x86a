#include "instruction_set.h"
#include "pch.h"
#include "section_container.h"

namespace x86a{
    void Section::addData(char data)noexcept{
        m_Data.push_back(data);
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

#include "pch.h"
#include "section_container.h"
namespace x86a{
    Section::Section(std::string name) : m_Name(name){}

    SectionContainer::SectionContainer(const ASMFile* file) : m_File(file){}
}

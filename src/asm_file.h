#pragma once

#include "pch.h"

class ASMFile{
public:
    ASMFile(const char* file);
    const std::string& getPath()const noexcept{return m_Path;}
    const std::vector<char>& getData()const noexcept{return m_Data;}
private:
    std::string m_Path;
    std::vector<char> m_Data;
};

#pragma once

#include "pch.h"

class ASMFile{
public:
    std::optional<std::string> open(std::string_view path);

    const std::filesystem::path& getPath()const noexcept{return m_Path;}
    const std::vector<char>& getData()const noexcept{return m_Data;}
private:
    std::filesystem::path m_Path;
    std::vector<char> m_Data;
};
